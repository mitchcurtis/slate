/*
    Copyright 2020, Mitch Curtis

    This file is part of Slate.

    Slate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slate. If not, see <http://www.gnu.org/licenses/>.
*/

#include "layeredimageproject.h"

#include <memory>

#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>
#include <QRegularExpression>

#include "addanimationcommand.h"
#include "addlayercommand.h"
#include "changeanimationordercommand.h"
#include "changelayeredimagesizecommand.h"
#include "changelayeredimagecanvassizecommand.h"
#include "changelayernamecommand.h"
#include "changelayeropacitycommand.h"
#include "changelayerordercommand.h"
#include "changelayervisiblecommand.h"
#include "clipboard.h"
#include "deleteanimationcommand.h"
#include "deletelayercommand.h"
#include "duplicateanimationcommand.h"
#include "duplicatelayercommand.h"
#include "imagelayer.h"
#include "imageutils.h"
#include "jsonutils.h"
#include "mergelayerscommand.h"
#include "modifyanimationcommand.h"
#include "movelayeredimagecontentscommand.h"
#include "pasteacrosslayerscommand.h"
#include "rearrangelayeredimagecontentsintogridcommand.h"

Q_LOGGING_CATEGORY(lcLivePreview, "app.layeredimageproject.livepreview")
Q_LOGGING_CATEGORY(lcMoveContents, "app.layeredimageproject.movecontents")
Q_LOGGING_CATEGORY(lcRearrangeContentsIntoGrid, "app.layeredimageproject.rearrangecontentsintogrid")
Q_LOGGING_CATEGORY(lcPasteAcrossLayers, "app.layeredimageproject.pasteacrosslayers")

LayeredImageProject::LayeredImageProject() :
    mCurrentLayerIndex(0),
    mLayersCreated(0),
    mAutoExportEnabled(false),
    mUsingAnimation(false),
    mHasUsedAnimation(false),
    mAnimationHelper(this, &mAnimationSystem, &mUsingAnimation)
{
    setObjectName(QLatin1String("LayeredImageProject"));
    qCDebug(lcProjectLifecycle) << "constructing" << this;
}

LayeredImageProject::~LayeredImageProject()
{
    qCDebug(lcProjectLifecycle) << "destructing" << this;
}

ImageLayer *LayeredImageProject::currentLayer()
{
    return isValidIndex(mCurrentLayerIndex) ? mLayers.at(mCurrentLayerIndex) : nullptr;
}

int LayeredImageProject::currentLayerIndex() const
{
    return mCurrentLayerIndex;
}

void LayeredImageProject::setCurrentLayerIndex(int index, bool force)
{
    const int adjustedIndex = qBound(0, index, qMax(mLayers.size() - 1, 0));
    if (!force && adjustedIndex == mCurrentLayerIndex)
        return;

    emit preCurrentLayerChanged();

    mCurrentLayerIndex = adjustedIndex;
    emit currentLayerIndexChanged();
    emit postCurrentLayerChanged();
}

QVector<ImageLayer *> LayeredImageProject::layers()
{
    return mLayers;
}

ImageLayer *LayeredImageProject::layerAt(int index)
{
    return isValidIndex(index) ? mLayers.at(index) : nullptr;
}

const ImageLayer *LayeredImageProject::layerAt(int index) const
{
    return isValidIndex(index) ? mLayers.at(index) : nullptr;
}

const ImageLayer *LayeredImageProject::layerAt(const QString &name) const
{
    foreach (const auto layer, mLayers) {
        if (layer->name() == name)
            return layer;
    }
    return nullptr;
}

int LayeredImageProject::layerCount() const
{
    return mLayers.size();
}

QSize LayeredImageProject::size() const
{
    return !mLayers.isEmpty() ? mLayers.first()->size() : QSize();
}

void LayeredImageProject::setSize(const QSize &newSize)
{
    if (newSize == size())
        return;

    QVector<QImage> previousImages;
    QVector<QImage> newImages;
    foreach (ImageLayer *layer, mLayers) {
        previousImages.append(*layer->image());

        const QImage resized = layer->image()->copy(0, 0, newSize.width(), newSize.height());
        newImages.append(resized);
    }

    beginMacro(QLatin1String("ChangeLayeredImageCanvasSize"));
    addChange(new ChangeLayeredImageCanvasSizeCommand(this, previousImages, newImages));
    endMacro();
}

void LayeredImageProject::doSetImageSize(const QVector<QImage> &newImages)
{
    Q_ASSERT(newImages.size() == mLayers.size());

    assignNewImagesToLayers(newImages);

    emit sizeChanged();
}

void LayeredImageProject::doSetCanvasSize(const QVector<QImage> &newImages)
{
    doSetImageSize(newImages);
}

int LayeredImageProject::widthInPixels() const
{
    return size().width();
}

int LayeredImageProject::heightInPixels() const
{
    return size().height();
}

QRect LayeredImageProject::bounds() const
{
    const QSize ourSize(size());
    return QRect(0, 0, ourSize.width(), ourSize.height());
}

QImage LayeredImageProject::flattenedImage(const std::function<QImage(int)> &layerSubstituteFunction) const
{
    return flattenedImage(0, layerCount() - 1, layerSubstituteFunction);
}

QImage LayeredImageProject::flattenedImage(int fromIndex, int toIndex, const std::function<QImage(int)> &layerSubstituteFunction) const
{
    Q_ASSERT(isValidIndex(fromIndex));
    Q_ASSERT(isValidIndex(toIndex));
    // If there's only one layer, the from and to indices will be the same.
    if (fromIndex != 0 && toIndex != 0)
        Q_ASSERT(fromIndex < toIndex);

    QImage finalImage = ImageUtils::filledImage(size());

    QPainter painter(&finalImage);
    // Work backwards from the last layer so that it gets drawn at the "bottom".
    for (int i = toIndex; i >= fromIndex; --i) {
        const ImageLayer *layer = layerAt(i);
        if (!layer->isVisible() || qFuzzyIsNull(layer->opacity()))
            continue;

        QImage layerImage;
        if (layerSubstituteFunction) {
            layerImage = layerSubstituteFunction(i);
        }
        if (layerImage.isNull()) {
            layerImage = *layer->image();
        }
        painter.drawImage(0, 0, layerImage);
    }

    return finalImage;
}

QString LayeredImageProject::expandLayerNameVariables(const QString &layerFileNamePrefix) const
{
    static const QString projectBaseNameIndicator(QLatin1String("%p"));

    // Currently we only have one variable, %p, which refers to the project's
    // base file name. It doesn't make sense for that to be repeated,
    // so we only find and replace once.
    QString expanded = layerFileNamePrefix;
    expanded.replace(projectBaseNameIndicator, fileBaseName());
    return expanded;
}

static bool shouldDraw(const ImageLayer *layer, const QString &layerFileName)
{
    // Only regular, non-file-named layers respect visibility.
    if (layerFileName.isEmpty() && !layer->isVisible()) {
        qCDebug(lcProject) << "  - layer" << layer->name()
            << "is a regular layer that's not visible; removing from remaining layers";
        return false;
    }

    // On the other hand, all types of layer respect opacity (if/when we add support for it).
    if (qFuzzyIsNull(layer->opacity())) {
        qCDebug(lcProject) << "  - layer" << layer->name() << "has 0 opacity; removing from remaining layers";
        return false;
    }

    return true;
}

/*
    By default, all layers are combined into one image when exported,
    with the highest (lowest index) layers in the list having the highest "Z order".

    If one or more layers contain the same [prefix], however, they will be combined
    into a separate image which is saved as that prefix with a .png extension,
    in the same directory as the save URL.

    For example, if the exported URL is house.png, the following images will be saved:

    [house-roof] left roof     // house-roof.png
    [house-roof] right roof    // house-roof.png
    [house-walls] left wall    // house-walls.png
    [house-walls] right wall   // house-walls.png
    background                 // house.png

    This allows related assets to be saved in the same project file, which avoids
    the need to have separate projects for them.

    Any variables (e.g. %p) are expanded.
*/
QHash<QString, QImage> LayeredImageProject::flattenedImages() const
{
    qCDebug(lcProject) << "flattening" << mLayers.size() << "layers";

    static const QRegularExpression fileNameRegex("^\\[.*\\]");
    static const QString noExportString("[no-export]");
    QHash<QString, QImage> images;

    QVector<ImageLayer*> remainingLayers = mLayers;
    while (!remainingLayers.isEmpty()) {
        // Take the last layer and try to find "[file-name]" or "[no-export]" in its layer name.
        QString targetFileName;
        qCDebug(lcProject) << "- taking last layer" << remainingLayers.last();
        ImageLayer *layer = remainingLayers.takeLast();
        if (layer->name().startsWith(noExportString)) {
            // Don't export this layer.
            continue;
        }

        QRegularExpressionMatch match;
        if (layer->name().contains(fileNameRegex, &match))
            targetFileName = match.captured();

        qCDebug(lcProject) << "- searching for layers with fileName" << targetFileName;

        // The final image that contains all of the matching layers combined.
        QImage finalImage = ImageUtils::filledImage(size());

        QPainter painter(&finalImage);
        if (shouldDraw(layer, targetFileName)) {
            // Draw the last layer's image.
            qCDebug(lcProject) << "  - drawing bottom layer" << layer->name();
            QImage layerImage = *layer->image();
            painter.drawImage(0, 0, layerImage);
        }

        // Now we're going to go through every layer looking for that file name.
        // If there was no file name, we'll look for layers without file names.
        // When a matching layer is found, it gets painted onto the final image.
        //
        // Work backwards from the last layer so that it gets drawn at the "bottom".
        for (int i = remainingLayers.size() - 1; i >= 0; --i) {
            QRegularExpressionMatch ourMatch;
            QString fileName;
            const ImageLayer *layer = remainingLayers.at(i);
            if (layer->name().contains(fileNameRegex, &ourMatch)) {
                // This is a file-named layer.
                fileName = ourMatch.captured();
            }

            if (!shouldDraw(layer, fileName)) {
                remainingLayers.removeAt(i);
                continue;
            }

            if (fileName != targetFileName) {
                qCDebug(lcProject) << "  - layer" << layer->name() << "has the file name" << fileName
                    << "which is not equal to" << targetFileName << "; skipping";
                continue;
            }

            qCDebug(lcProject) << "  - drawing layer" << layer->name();
            QImage layerImage = *layer->image();
            painter.drawImage(0, 0, layerImage);

            remainingLayers.removeAt(i);
        }

        qCDebug(lcProject) << "- completed drawing of layers with file name" << targetFileName;

        if (!targetFileName.isEmpty()) {
            // The file name is in brackets so remove them so we save it under the correct name.
            targetFileName = targetFileName.chopped(1).remove(0, 1);
            // Expand any variables that might be in there.
            targetFileName = expandLayerNameVariables(targetFileName);
        }
        images[targetFileName] = finalImage;
    }

    return images;
}

QImage LayeredImageProject::exportedImage() const
{
    return flattenedImage();
}

QVector<QImage> LayeredImageProject::layerImages() const
{
    QVector<QImage> images;
    images.reserve(mLayers.size());
    for (const ImageLayer *layer : qAsConst(mLayers))
        images.append(*layer->image());
    return images;
}

QVector<QImage> LayeredImageProject::layerImagesBeforeLivePreview() const
{
    return mLayerImagesBeforeLivePreview;
}

bool LayeredImageProject::isAutoExportEnabled() const
{
    return mAutoExportEnabled;
}

void LayeredImageProject::setAutoExportEnabled(bool autoExportEnabled)
{
    if (autoExportEnabled == mAutoExportEnabled)
        return;

    mAutoExportEnabled = autoExportEnabled;
    emit autoExportEnabledChanged();
}

QString LayeredImageProject::autoExportFilePath(const QUrl &projectUrl)
{
    const QString filePath = projectUrl.toLocalFile();
    QString path = filePath;
    const int lastPeriodIndex = filePath.lastIndexOf(QLatin1Char('.'));
    if (lastPeriodIndex != -1)
        path.replace(lastPeriodIndex + 1, filePath.size() - lastPeriodIndex - 1, QLatin1String("png"));
    else
        path.append(QLatin1String("png"));
    return path;
}

bool LayeredImageProject::isUsingAnimation() const
{
    return mUsingAnimation;
}

void LayeredImageProject::setUsingAnimation(bool isUsingAnimation)
{
    if (isUsingAnimation == mUsingAnimation)
        return;

    mUsingAnimation = isUsingAnimation;

    if (mUsingAnimation) {
        if (!mHasUsedAnimation) {
            mAnimationSystem.addNewAnimation(size());

            mHasUsedAnimation = true;
        }
    }

    emit usingAnimationChanged();
}

AnimationSystem *LayeredImageProject::animationSystem()
{
    return &mAnimationSystem;
}

const AnimationSystem *LayeredImageProject::animationSystem() const
{
    return &mAnimationSystem;
}

void LayeredImageProject::exportGif(const QUrl &url)
{
    if (!mUsingAnimation) {
        error(tr("Cannot export as GIF because the project isn't using animation"));
        return;
    }

    QString errorMessage;
    if (!ImageUtils::exportGif(exportedImage(), url, *mAnimationSystem.currentAnimationPlayback(), errorMessage))
        error(errorMessage);
}

void LayeredImageProject::createNew(int imageWidth, int imageHeight, bool transparentBackground)
{
    if (hasLoaded()) {
        close();
    }

    qCDebug(lcProject) << "creating new project:"
        << "imageWidth =" << imageWidth
        << "imageHeight =" << imageHeight;

    Q_ASSERT(mUndoStack.count() == 0);
    Q_ASSERT(mLayers.isEmpty());

    addNewLayer(imageWidth, imageHeight, transparentBackground, false);

    setUrl(QUrl());
    setNewProject(true);
    emit projectCreated();

    qCDebug(lcProject) << "finished creating new project";
}

void LayeredImageProject::beginLivePreview()
{
    qCDebug(lcLivePreview) << "beginLivePreview called";

    if (mLivePreviewActive) {
        qWarning() << "Live preview already active";
        return;
    }

    // We could do this stuff lazily (when the first modification is made),
    // but it's better to have any delay from copying happen when
    // the dialog opens instead of as the sliders etc. are being interacted with.

    mLayerImagesBeforeLivePreview = layerImages();

    mLivePreviewActive = true;

    // Note that mCurrentLivePreviewModification isn't set yet; that happens
    // when the first modification is made.
}

void LayeredImageProject::endLivePreview(LivePreviewModificationAction modificationAction)
{
    qCDebug(lcLivePreview) << "endLivePreview called with modificationAction" << modificationAction;

    if (!mLivePreviewActive) {
        qWarning() << "Can't end live preview as it isn't active";
        return;
    }

    auto cleanup = [&](){
        mLayerImagesBeforeLivePreview.clear();
        mLivePreviewActive = false;
    };

    if (mCurrentLivePreviewModification == LivePreviewModification::None) {
        // A dialog was opened but no changes were made.
        cleanup();
        return;
    }

    if (modificationAction == CommitModificaton) {
        // The dialog was accepted.

        // Live preview does modifications directly to the project's images,
        // bypassing the undo framework until the changes are confirmed.
        // The undo commands still need to know the old and new images, though,
        // so we gather them here.
        const QVector<QImage> newImages = layerImages();

        switch (mCurrentLivePreviewModification) {
        case LivePreviewModification::Resize:
            beginMacro(QLatin1String("ChangeLayeredImageSize"));
            addChange(new ChangeLayeredImageSizeCommand(this, mLayerImagesBeforeLivePreview, newImages));
            endMacro();
            break;
        case LivePreviewModification::MoveContents:
            beginMacro(QLatin1String("MoveLayeredImageContents"));
            addChange(new MoveLayeredImageContentsCommand(this, mLayerImagesBeforeLivePreview, newImages));
            endMacro();
            break;
        case LivePreviewModification::RearrangeContentsIntoGrid:
            beginMacro(QLatin1String("RearrangeLayeredImageContentsIntoGrid"));
            addChange(new RearrangeLayeredImageContentsIntoGridCommand(this, mLayerImagesBeforeLivePreview, newImages));
            endMacro();
            break;
        case LivePreviewModification::PasteAcrossLayers:
            beginMacro(QLatin1String("PasteAcrossLayers"));
            addChange(new PasteAcrossLayersCommand(this, mLayerImagesBeforeLivePreview, newImages));
            endMacro();
            break;
        case LivePreviewModification::None:
            qFatal("mCurrentLivePreviewModification is LivePreviewModification::None where it shouldn't be");
            break;
        }
    } else {
        // The dialog was cancelled.
        for (int i = 0; i < mLayers.size(); ++i) {
            ImageLayer *layer = mLayers.at(i);
            *layer->image() = mLayerImagesBeforeLivePreview.at(i);
        }

        // The canvas needs to repaint if the dialog was cancelled, since
        // we're modifying the contents directly.
        emit contentsModified();
    }

    cleanup();
}

#define CONTAINS_KEY_OR_ERROR(jsonObject, key, filePath) \
    if (!(jsonObject).contains(key)) { \
        error(QString::fromLatin1("Layered image project file is missing a \"%1\" key:\n\n%2").arg(key, filePath)); \
        return; \
    }

void LayeredImageProject::doLoad(const QUrl &url)
{
    const QString filePath = url.toLocalFile();
    if (!QFileInfo::exists(filePath)) {
        error(QString::fromLatin1("Layered image project does not exist:\n\n%1").arg(filePath));
        return;
    }

    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        error(QString::fromLatin1("Failed to open layered image project's .slp file:\n\n%1").arg(filePath));
        return;
    }

    if (QFileInfo(jsonFile).suffix() != "slp") {
        error(QString::fromLatin1("Layered image project files must have a .slp extension:\n\n%1").arg(filePath));
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject rootJson = jsonDoc.object();
    CONTAINS_KEY_OR_ERROR(rootJson, "project", filePath);
    QJsonObject projectObject = rootJson.value("project").toObject();

    readVersionNumbers(projectObject);

    CONTAINS_KEY_OR_ERROR(projectObject, "layers", filePath);
    QJsonArray layerArray = projectObject.value("layers").toArray();
    for (int i = 0; i < layerArray.size(); ++i) {
        QJsonObject layerObject = layerArray.at(i).toObject();
        ImageLayer *imageLayer = new ImageLayer(this);
        imageLayer->read(layerObject);
        if (imageLayer->image()->isNull()) {
            error(QString::fromLatin1("Failed to load image for layer:\n\n%1").arg(i));
            close();
            return;
        }
        addLayerAboveAll(imageLayer);
    }
    mCurrentLayerIndex = projectObject.value("currentLayerIndex").toInt(0);

    mAutoExportEnabled = projectObject.value("autoExportEnabled").toBool(false);

    mUsingAnimation = projectObject.value("usingAnimation").toBool(false);
    mHasUsedAnimation = projectObject.value("hasUsedAnimation").toBool(false);
    if (mHasUsedAnimation) {
        if (projectObject.contains("animationPlayback")) {
            // Pre-0.10 format.
            mAnimationSystem.read(projectObject.value("animationPlayback").toObject());
        } else {
            // >= 0.10 format.
            mAnimationSystem.read(projectObject.value("animationSystem").toObject());
        }
    }

    readUiState(projectObject);

    readGuides(projectObject);
    readNotes(projectObject);
    // Allow older project files without swatch support (saved with version <= 0.2.1) to still be loaded.
    if (!readJsonSwatch(projectObject, IgnoreSerialisationFailures))
        return;

    setUrl(url);
    emit projectLoaded();
}

void LayeredImageProject::doClose()
{
    // Workaround for QTBUG-62946; when it's fixed we can remove the new code
    // and go back to the old code
    emit preLayersCleared();
    while (!mLayers.isEmpty()) {
        delete mLayers.takeAt(0);
    }
    emit layerCountChanged();
    emit postLayersCleared();
    setCurrentLayerIndex(0);

    mLayersCreated = 0;
    mAutoExportEnabled = false;
    mUsingAnimation = false;
    mHasUsedAnimation = false;
    mAnimationSystem.reset();
    emit projectClosed();
}

bool LayeredImageProject::doSaveAs(const QUrl &url)
{
    const QString filePath = url.toLocalFile();
    const QFileInfo projectSaveFileInfo(filePath);
    if (mTempDir.isValid()) {
        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
            error(QLatin1String("Cannot save project in internal temporary directory"));
            return false;
        }
    }

    QFile jsonFile;
    if (QFile::exists(filePath)) {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to open project's JSON file:\n\n%1").arg(filePath));
            return false;
        }
    } else {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to create project's JSON file:\n\n%1").arg(filePath));
            return false;
        }
    }

    QJsonObject rootJson;

    QJsonObject projectObject;

    writeVersionNumbers(projectObject);

    QJsonArray layersArray;
    foreach (ImageLayer *layer, mLayers) {
        QJsonObject layerObject;
        layer->write(layerObject);
        layersArray.prepend(layerObject);
    }

    projectObject.insert("layers", layersArray);
    projectObject.insert("currentLayerIndex", mCurrentLayerIndex);

    writeGuides(projectObject);
    writeNotes(projectObject);
    writeJsonSwatch(projectObject);
    writeUiState(projectObject);

    if (mAutoExportEnabled) {
        projectObject.insert("autoExportEnabled", true);

        if (!exportImage(QUrl::fromLocalFile(autoExportFilePath(url))))
            return false;
    }

    if (mUsingAnimation)
        projectObject.insert("usingAnimation", true);

    if (mHasUsedAnimation) {
        projectObject.insert("hasUsedAnimation", true);

        QJsonObject animationObject;
        mAnimationSystem.write(animationObject);
        projectObject.insert("animationSystem", animationObject);
    }

    rootJson.insert("project", projectObject);

    QJsonDocument jsonDoc(rootJson);
    const qint64 bytesWritten = jsonFile.write(jsonDoc.toJson());
    if (bytesWritten == -1) {
        error(QString::fromLatin1("Failed to save project - couldn't write to JSON project file:\n\n%1")
            .arg(jsonFile.errorString()));
        return false;
    }

    if (bytesWritten == 0) {
        error(QString::fromLatin1("Failed to save project - wrote zero bytes to JSON project file"));
        return false;
    }

    if (mFromNew) {
        // The project was successfully saved, so it can now save
        // to the same URL by default from now on.
        setNewProject(false);
    }
    setUrl(url);
    mUndoStack.setClean();
    mHadUnsavedChangesBeforeMacroBegan = false;

    return true;
}

// Returns true because the auto-export feature in saveAs() needs to know whether or not it should return early.
bool LayeredImageProject::exportImage(const QUrl &url)
{
    if (!hasLoaded())
        return false;

    if (url.isEmpty())
        return false;

    const QString mainExportFilePath = url.toLocalFile();
    const QFileInfo projectSaveFileInfo(mainExportFilePath);
    if (mTempDir.isValid()) {
        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
            error(QLatin1String("Cannot save project in internal temporary directory"));
            return false;
        }
    }

    const QHash<QString, QImage> imagesToExport = flattenedImages();
    const auto keys = imagesToExport.keys();
    for (const QString &key : keys) {
        const QImage image = imagesToExport.value(key);
        const QString imageFilePath = key.isEmpty()
            ? mainExportFilePath : projectSaveFileInfo.dir().path() + "/" + key + ".png";
        if (!image.save(imageFilePath)) {
            error(QString::fromLatin1("Failed to save project's image to:\n\n%1").arg(imageFilePath));
            return false;
        }
    }

    return true;
}

void LayeredImageProject::resize(int width, int height, bool smooth)
{
    if (warnIfLivePreviewNotActive(QLatin1String("resize")))
        return;

    const QSize newSize(width, height);
    if (newSize == size())
        return;

    QVector<QImage> newImages;
    newImages.reserve(mLayers.size());

    for (const QImage &originalLayerImage : qAsConst(mLayerImagesBeforeLivePreview)) {
        const QImage resized = ImageUtils::resizeContents(originalLayerImage, newSize, smooth);
        newImages.append(resized);
    }

    makeLivePreviewModification(LivePreviewModification::Resize, newImages);
}

void LayeredImageProject::crop(const QRect &rect)
{
    if (rect.x() == 0 && rect.y() == 0 && rect.size() == size()) {
        // No change.
        return;
    }

    QVector<QImage> previousImages;
    previousImages.reserve(mLayers.size());
    QVector<QImage> newImages;
    newImages.reserve(mLayers.size());

    for (const ImageLayer *layer : qAsConst(mLayers)) {
        previousImages.append(*layer->image());

        const QImage cropped = layer->image()->copy(rect);
        newImages.append(cropped);
    }

    beginMacro(QLatin1String("CropLayeredImageCanvas"));
    addChange(new ChangeLayeredImageCanvasSizeCommand(this, previousImages, newImages));
    endMacro();
}

void LayeredImageProject::moveContents(int xDistance, int yDistance, bool onlyVisibleContents)
{
    qCDebug(lcMoveContents) << "moveContents called with xDistance" << xDistance
        << "yDistance" << yDistance << "onlyVisibleContents" << onlyVisibleContents;

    if (warnIfLivePreviewNotActive(QLatin1String("move contents")))
        return;

    if (xDistance == 0 && yDistance == 0)
        return;

    QVector<QImage> newImages;
    for (int layerIndex = 0; layerIndex < mLayers.size(); ++layerIndex) {
        const bool layerVisible = mLayers.at(layerIndex)->isVisible();
        const QImage oldImage = mLayerImagesBeforeLivePreview.at(layerIndex);
        if (onlyVisibleContents && !layerVisible) {
            // This is lazy and inefficient (we could e.g. store a null QImage),
            // but it avoids adding more code paths elsewhere.
            newImages.append(oldImage);
        } else {
            newImages.append(ImageUtils::moveContents(oldImage, xDistance, yDistance));
        }
    }

    makeLivePreviewModification(LivePreviewModification::MoveContents, newImages);
}

void LayeredImageProject::rearrangeContentsIntoGrid(int cellWidth, int cellHeight, int columns, int rows)
{
    qCDebug(lcRearrangeContentsIntoGrid) << "moveContents called with cellWidth" << cellWidth
        << "cellHeight" << cellHeight << "columns" << columns << "rows" << rows;

    if (warnIfLivePreviewNotActive(QLatin1String("rearrange contents into grid")))
        return;

    const QVector<QImage> newImages = ImageUtils::rearrangeContentsIntoGrid(
        mLayerImagesBeforeLivePreview, cellWidth, cellHeight, columns, rows);
    makeLivePreviewModification(LivePreviewModification::MoveContents, newImages);
}

void LayeredImageProject::doMoveContents(const QVector<QImage> &newImages)
{
    Q_ASSERT(newImages.size() == mLayers.size());

    assignNewImagesToLayers(newImages);

    emit contentsMoved();
}

void LayeredImageProject::doRearrangeContentsIntoGrid(const QVector<QImage> &newImages)
{
    Q_ASSERT(newImages.size() == mLayers.size());

    assignNewImagesToLayers(newImages);

    // This may be necessary for undos.
    emit contentsModified();
}

void LayeredImageProject::doPasteAcrossLayers(const QVector<QImage> &newImages)
{
    assignNewImagesToLayers(newImages);

    emit contentsModified();
}

void LayeredImageProject::addNewLayer()
{
    addNewLayer(widthInPixels(), heightInPixels(), true);
}

void LayeredImageProject::deleteCurrentLayer()
{
    if (mLayers.size() <= 1 || !isValidIndex(mCurrentLayerIndex))
        return;

    beginMacro(QLatin1String("DeleteLayerCommand"));
    addChange(new DeleteLayerCommand(this, mCurrentLayerIndex));
    endMacro();
}

void LayeredImageProject::moveCurrentLayerUp()
{
    if (!isValidIndex(mCurrentLayerIndex) || !isValidIndex(mCurrentLayerIndex - 1))
        return;

    beginMacro(QLatin1String("ChangeLayerOrderCommand"));
    addChange(new ChangeLayerOrderCommand(this, mCurrentLayerIndex, mCurrentLayerIndex - 1));
    endMacro();
}

void LayeredImageProject::moveCurrentLayerDown()
{
    if (!isValidIndex(mCurrentLayerIndex) || !isValidIndex(mCurrentLayerIndex + 1))
        return;

    beginMacro(QLatin1String("ChangeLayerOrderCommand"));
    addChange(new ChangeLayerOrderCommand(this, mCurrentLayerIndex, mCurrentLayerIndex + 1));
    endMacro();
}

void LayeredImageProject::mergeCurrentLayerUp()
{
    if (!isValidIndex(mCurrentLayerIndex) || !isValidIndex(mCurrentLayerIndex - 1))
        return;

    beginMacro(QLatin1String("MergeLayersCommand"));
    addChange(new MergeLayersCommand(this, mCurrentLayerIndex, layerAt(mCurrentLayerIndex),
        mCurrentLayerIndex - 1, layerAt(mCurrentLayerIndex - 1)));
    endMacro();
}

void LayeredImageProject::mergeCurrentLayerDown()
{
    if (!isValidIndex(mCurrentLayerIndex) || !isValidIndex(mCurrentLayerIndex + 1))
        return;

    beginMacro(QLatin1String("MergeLayersCommand"));
    addChange(new MergeLayersCommand(this, mCurrentLayerIndex, layerAt(mCurrentLayerIndex),
        mCurrentLayerIndex + 1, layerAt(mCurrentLayerIndex + 1)));
    endMacro();
}

void LayeredImageProject::duplicateCurrentLayer()
{
    if (!isValidIndex(mCurrentLayerIndex))
        return;

    beginMacro(QLatin1String("DuplicateLayerCommand"));
    addChange(new DuplicateLayerCommand(this, mCurrentLayerIndex, layerAt(mCurrentLayerIndex)->clone()));
    endMacro();
}

void LayeredImageProject::setLayerName(int layerIndex, const QString &name)
{
    if (!isValidIndex(layerIndex) || name == layerAt(layerIndex)->name())
        return;

    beginMacro(QLatin1String("ChangeLayerNameCommand"));
    addChange(new ChangeLayerNameCommand(this, layerIndex, layerAt(layerIndex)->name(), name));
    endMacro();
}

void LayeredImageProject::setLayerVisible(int layerIndex, bool visible)
{
    if (!isValidIndex(layerIndex) || visible == layerAt(layerIndex)->isVisible())
        return;

    beginMacro(QLatin1String("ChangeLayerVisibleCommand"));
    addChange(new ChangeLayerVisibleCommand(this, layerIndex, layerAt(layerIndex)->isVisible(), visible));
    endMacro();
}

void LayeredImageProject::setLayerOpacity(int layerIndex, qreal opacity)
{
    if (!isValidIndex(layerIndex) || opacity == layerAt(layerIndex)->opacity())
        return;

    beginMacro(QLatin1String("ChangeLayerOpacityCommand"));
    addChange(new ChangeLayerOpacityCommand(this, layerIndex, layerAt(layerIndex)->opacity(), opacity));
    endMacro();
}

void LayeredImageProject::copyAcrossLayers(const QRect &copyArea)
{
    QVector<QImage> copiedImages;
    const QVector<QImage> imagesToCopy = layerImages();
    copiedImages.reserve(imagesToCopy.size());
    for (auto layerImage : imagesToCopy)
        copiedImages.append(layerImage.copy(copyArea));
    Clipboard::instance()->setCopiedLayerImages(copiedImages);
}

void LayeredImageProject::pasteAcrossLayers(int pasteX, int pasteY, bool onlyPasteIntoVisibleLayers)
{
    qCDebug(lcPasteAcrossLayers) << "pasteAcrossLayers called with pasteX" << pasteX
        << "pasteY" << pasteY << "onlyPasteIntoVisibleLayers" << onlyPasteIntoVisibleLayers;

    if (warnIfLivePreviewNotActive(QLatin1String("move contents")))
        return;

    if (pasteX == 0 && pasteY == 0)
        return;

    const QVector<QImage> newImages = ImageUtils::pasteAcrossLayers(mLayers, mLayerImagesBeforeLivePreview,
        pasteX, pasteY, onlyPasteIntoVisibleLayers);
    makeLivePreviewModification(LivePreviewModification::PasteAcrossLayers, newImages);
}

void LayeredImageProject::addAnimation()
{
    mAnimationHelper.addAnimation();
}

void LayeredImageProject::duplicateAnimation(int index)
{
    mAnimationHelper.duplicateAnimation(index);
}

void LayeredImageProject::modifyAnimation(int index)
{
    mAnimationHelper.modifyAnimation(index);
}

void LayeredImageProject::renameAnimation(int index)
{
    mAnimationHelper.renameAnimation(index);
}

void LayeredImageProject::moveCurrentAnimationUp()
{
    mAnimationHelper.moveCurrentAnimationUp();
}

void LayeredImageProject::moveCurrentAnimationDown()
{
    mAnimationHelper.moveCurrentAnimationDown();
}

void LayeredImageProject::removeAnimation(int index)
{
    mAnimationHelper.removeAnimation(index);
}

bool LayeredImageProject::isValidIndex(int index) const
{
    return index >= 0 && index < mLayers.size();
}

void LayeredImageProject::makeLivePreviewModification(LivePreviewModification modification, const QVector<QImage> &newImages)
{
    qCDebug(lcLivePreview) << "makeLivePreviewModification called with modification" << modification;

    if (warnIfLivePreviewNotActive(QLatin1String("make live preview modification")))
        return;

    if (mCurrentLivePreviewModification == LivePreviewModification::None)
        mCurrentLivePreviewModification = modification;

    if (modification != mCurrentLivePreviewModification) {
        qWarning() << "Cannot make live preview modification" << modification << "as it is different"
            << "to the current modification of" << mCurrentLivePreviewModification;
        return;
    }

    Q_ASSERT(newImages.size() == mLayers.size());

    assignNewImagesToLayers(newImages);

    // Let the canvas know that it should repaint.
    emit contentsModified();
}

void LayeredImageProject::assignNewImagesToLayers(const QVector<QImage> &newImages)
{
    for (int i = 0; i < newImages.size(); ++i) {
        const QImage newImage = newImages.at(i);
        Q_ASSERT(!newImage.isNull());

        ImageLayer *layer = mLayers.at(i);
        *layer->image() = newImage;
    }
}

Project::Type LayeredImageProject::type() const
{
    return LayeredImageType;
}

void LayeredImageProject::addNewLayer(int imageWidth, int imageHeight, bool transparent, bool undoable)
{
    QImage emptyImage = ImageUtils::filledImage(imageWidth, imageHeight, transparent ? Qt::transparent : Qt::white);

    std::unique_ptr<ImageLayer> imageLayer(new ImageLayer(nullptr, emptyImage));
    imageLayer->setName(QString::fromLatin1("Layer %1").arg(++mLayersCreated));

    const int layerIndex = mCurrentLayerIndex;

    if (undoable) {
        beginMacro(QLatin1String("AddLayerCommand"));
        addChange(new AddLayerCommand(this, imageLayer.get(), layerIndex));
        endMacro();
    } else {
        addLayer(imageLayer.get(), layerIndex);
    }

    imageLayer.release();
}

void LayeredImageProject::addLayerAboveAll(ImageLayer *imageLayer)
{
    addLayer(imageLayer, 0);
}

void LayeredImageProject::addLayer(ImageLayer *imageLayer, int index)
{
    qCDebug(lcProject).nospace() << "adding layer " << imageLayer << " at index " << index
        << ", mCurrentLayerIndex is " << mCurrentLayerIndex;

    if (mLayers.contains(imageLayer)) {
        qWarning() << "Can't add layer" << imageLayer << "to project as we already have it";
        return;
    }

    imageLayer->setParent(this);

    emit preLayerAdded(index);

    mLayers.insert(index, imageLayer);

    emit postLayerAdded(index);

    emit layerCountChanged();

    if (index <= mCurrentLayerIndex) {
        // The index is before that of the current layer's, so we must update it.
        setCurrentLayerIndex(mCurrentLayerIndex + 1);
    }
}

void LayeredImageProject::moveLayer(int fromIndex, int toIndex)
{
    if (!isValidIndex(fromIndex) || !isValidIndex(toIndex))
        return;

    ImageLayer *current = currentLayer();

    emit preLayerMoved(fromIndex, toIndex);

    mLayers.move(fromIndex, toIndex);

    emit postLayerMoved(fromIndex, toIndex);

    const int newCurrentIndex = mLayers.indexOf(current);
    setCurrentLayerIndex(newCurrentIndex);
}

/*!
    Merges the layers \a sourceIndex and \a targetIndex together,
    transferring ownership of the layer at \a sourceIndex to the caller.
*/
void LayeredImageProject::mergeLayers(int sourceIndex, int targetIndex)
{
    if (!isValidIndex(sourceIndex) || !isValidIndex(targetIndex))
        return;

    const int fromIndex = sourceIndex < targetIndex ? sourceIndex : targetIndex;
    const int toIndex = sourceIndex < targetIndex ? targetIndex : sourceIndex;

    // flattenedImage() merges the layers' images.
    setLayerImage(targetIndex, flattenedImage(fromIndex, toIndex));

    // Remove the source layer as it has been merged into the target layer.
    takeLayer(sourceIndex);

    /*
        takeLayer() does set the currentLayerIndex, but it's not always
        what we want, which is why we set it ourselves here.

        Merging down with the following layers results in Layer 3 (sourceIndex = 0)
        being merged into Layer 2 (targetIndex = 1), so the currentIndex should be 0.

        - Layer 3 (current)
        - Layer 2
        - Layer 1

        Merging up with the following layers results in Layer 2 (sourceIndex = 1)
        being merged into Layer 3 (targetIndex = 0), so the currentIndex should be 0.

        - Layer 3
        - Layer 2 (current)
        - Layer 1

        That's why we always use the smaller of the two indices.

        Force the change to avoid the current layer not being updated in some cases.
    */
    setCurrentLayerIndex(qMin(sourceIndex, targetIndex), true);
}

ImageLayer *LayeredImageProject::takeLayer(int index)
{
    emit preLayerRemoved(index);

    ImageLayer *layer = mLayers.takeAt(index);

    emit postLayerRemoved(index);

    emit layerCountChanged();

    // Keep the same index to select the layer that was above us.
    setCurrentLayerIndex(index);

    layer->setParent(nullptr);

    return layer;
}

void LayeredImageProject::setLayerImage(int index, const QImage &image)
{
    Q_ASSERT(isValidIndex(index));

    *mLayers[index]->image() = image;

    emit postLayerImageChanged();
}

QDebug operator<<(QDebug debug, const LayeredImageProject *project)
{
    debug.nospace() << "LayeredImageProject currentLayerIndex=" << project->mCurrentLayerIndex
                    << ", layers:";
    foreach (ImageLayer *layer, project->mLayers) {
        debug << "\n    name=" << layer->name()
              << " visible=" << layer->isVisible()
              << " opacity=" << layer->opacity()
              << " image=" << *layer->image();
    }
    return debug.space();
}
