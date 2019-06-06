/*
    Copyright 2017, Mitch Curtis

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

#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>
#include <QRegularExpression>

#include "addlayercommand.h"
#include "changelayeredimagesizecommand.h"
#include "changelayeredimagecanvassizecommand.h"
#include "changelayernamecommand.h"
#include "changelayeropacitycommand.h"
#include "changelayerordercommand.h"
#include "changelayervisiblecommand.h"
#include "deletelayercommand.h"
#include "duplicatelayercommand.h"
#include "imagelayer.h"
#include "jsonutils.h"
#include "mergelayerscommand.h"
#include "movelayeredimagecontentscommand.h"

LayeredImageProject::LayeredImageProject() :
    mCurrentLayerIndex(0),
    mLayersCreated(0),
    mAutoExportEnabled(false),
    mUsingAnimation(false),
    mHasUsedAnimation(false),
    mLayerListViewContentY(0.0)
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

void LayeredImageProject::setCurrentLayerIndex(int index)
{
    const int adjustedIndex = qBound(0, index, mLayers.size() - 1);
    if (adjustedIndex == mCurrentLayerIndex)
        return;

    emit preCurrentLayerChanged();

    mCurrentLayerIndex = adjustedIndex;
    emit currentLayerIndexChanged();
    emit postCurrentLayerChanged();
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

QImage LayeredImageProject::flattenedImage(std::function<QImage(int)> layerSubstituteFunction) const
{
    return flattenedImage(0, layerCount() - 1, layerSubstituteFunction);
}

QImage LayeredImageProject::flattenedImage(int fromIndex, int toIndex, std::function<QImage(int)> layerSubstituteFunction) const
{
    Q_ASSERT(isValidIndex(fromIndex));
    Q_ASSERT(isValidIndex(toIndex));
    // If there's only one layer, the from and to indices will be the same.
    if (fromIndex != 0 && toIndex != 0)
        Q_ASSERT(fromIndex < toIndex);

    QImage finalImage(size(), QImage::Format_ARGB32_Premultiplied);
    finalImage.fill(Qt::transparent);

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
        QImage finalImage(size(), QImage::Format_ARGB32_Premultiplied);
        finalImage.fill(Qt::transparent);

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
            const QSize imageSize = size();
            mAnimationPlayback.setFrameCount(imageSize.width() >= 8 ? 4 : 1);
            mAnimationPlayback.setFrameWidth(imageSize.width() / mAnimationPlayback.frameCount());
            mAnimationPlayback.setFrameHeight(imageSize.height());

            mHasUsedAnimation = true;
        }
    }

    emit usingAnimationChanged();
}

qreal LayeredImageProject::layerListViewContentY() const
{
    return mLayerListViewContentY;
}

void LayeredImageProject::setLayerListViewContentY(qreal contentY)
{
    if (qFuzzyCompare(contentY, mLayerListViewContentY))
        return;

    mLayerListViewContentY = contentY;
    emit layerListViewContentYChanged();
}

AnimationPlayback *LayeredImageProject::animationPlayback()
{
    return &mAnimationPlayback;
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

    if (projectObject.contains("currentLayerIndex"))
        setCurrentLayerIndex(projectObject.value("currentLayerIndex").toInt());

    readGuides(projectObject);
    // Allow older project files without swatch support (saved with version <= 0.2.1) to still be loaded.
    if (!readJsonSwatch(projectObject, IgnoreSerialisationFailures))
        return;

    mAutoExportEnabled = projectObject.value("autoExportEnabled").toBool(false);

    mUsingAnimation = projectObject.value("usingAnimation").toBool(false);
    mHasUsedAnimation = projectObject.value("hasUsedAnimation").toBool(false);
    if (mHasUsedAnimation) {
        mAnimationPlayback.read(projectObject.value("animationPlayback").toObject());
    }

    mLayerListViewContentY = projectObject.value("layerListViewContentY").toDouble();

    mCachedProjectJson = projectObject;

    setUrl(url);
    emit projectLoaded();
}

void LayeredImageProject::doClose()
{
    setNewProject(false);

    // Workaround for QTBUG-62946; when it's fixed we can remove the new code
    // and go back to the old code
    emit preLayersCleared();
    while (!mLayers.isEmpty()) {
        delete mLayers.takeAt(0);
    }
    emit layerCountChanged();
    emit postLayersCleared();
    setCurrentLayerIndex(0);

    // Old code:
//    while (!mLayers.isEmpty()) {
//        delete takeLayer(0);
//    }

    setUrl(QUrl());
    mUndoStack.clear();
    mLayersCreated = 0;
    mAutoExportEnabled = false;
    mUsingAnimation = false;
    mHasUsedAnimation = false;
    mAnimationPlayback.reset();
    mLayerListViewContentY = 0.0;
    emit projectClosed();
}

void LayeredImageProject::doSaveAs(const QUrl &url)
{
    if (!hasLoaded())
        return;

    if (url.isEmpty())
        return;

    const QString filePath = url.toLocalFile();
    const QFileInfo projectSaveFileInfo(filePath);
    if (mTempDir.isValid()) {
        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
            error(QLatin1String("Cannot save project in internal temporary directory"));
            return;
        }
    }

    QFile jsonFile;
    if (QFile::exists(filePath)) {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to open project's JSON file:\n\n%1").arg(filePath));
            return;
        }
    } else {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to create project's JSON file:\n\n%1").arg(filePath));
            return;
        }
    }

    QJsonObject rootJson;

    QJsonObject projectObject;

    QJsonArray layersArray;
    foreach (ImageLayer *layer, mLayers) {
        QJsonObject layerObject;
        layer->write(layerObject);
        layersArray.prepend(layerObject);
    }

    projectObject.insert("layers", layersArray);
    projectObject.insert("currentLayerIndex", mCurrentLayerIndex);

    writeGuides(projectObject);
    writeJsonSwatch(projectObject);
    emit readyForWritingToJson(&projectObject);

    if (mAutoExportEnabled) {
        projectObject.insert("autoExportEnabled", true);

        if (!exportImage(QUrl::fromLocalFile(autoExportFilePath(url))))
            return;
    }

    if (mUsingAnimation)
        projectObject.insert("usingAnimation", true);

    if (mHasUsedAnimation) {
        projectObject.insert("hasUsedAnimation", true);

        QJsonObject animationObject;
        mAnimationPlayback.write(animationObject);
        projectObject.insert("animationPlayback", animationObject);
    }

    projectObject.insert("layerListViewContentY", mLayerListViewContentY);

    rootJson.insert("project", projectObject);

    QJsonDocument jsonDoc(rootJson);
    const qint64 bytesWritten = jsonFile.write(jsonDoc.toJson());
    if (bytesWritten == -1) {
        error(QString::fromLatin1("Failed to save project - couldn't write to JSON project file:\n\n%1")
            .arg(jsonFile.errorString()));
        return;
    }

    if (bytesWritten == 0) {
        error(QString::fromLatin1("Failed to save project - wrote zero bytes to JSON project file"));
        return;
    }

    if (mFromNew) {
        // The project was successfully saved, so it can now save
        // to the same URL by default from now on.
        setNewProject(false);
    }
    setUrl(url);
    mUndoStack.setClean();
    mHadUnsavedChangesBeforeMacroBegan = false;
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

void LayeredImageProject::resize(int width, int height)
{
    const QSize newSize(width, height);
    if (newSize == size())
        return;

    QVector<QImage> previousImages;
    QVector<QImage> newImages;
    foreach (ImageLayer *layer, mLayers) {
        previousImages.append(*layer->image());

        const QImage resized = layer->image()->scaled(newSize, Qt::IgnoreAspectRatio, Qt::FastTransformation);
        newImages.append(resized);
    }

    beginMacro(QLatin1String("ChangeLayeredImageSize"));
    addChange(new ChangeLayeredImageSizeCommand(this, previousImages, newImages));
    endMacro();
}

void LayeredImageProject::moveContents(int x, int y, bool onlyVisibleContents)
{
    if (x == 0 && y == 0)
        return;

    QVector<QImage> previousImages;
    QVector<QImage> newImages;
    foreach (ImageLayer *layer, mLayers) {
        const QImage oldImage = *layer->image();
        previousImages.append(oldImage);

        if (onlyVisibleContents && !layer->isVisible()) {
            // This is lazy and inefficient (we could e.g. store a null QImage),
            // but it avoids adding more code paths elsewhere.
            newImages.append(oldImage);
        } else {
            QImage translated(size(), QImage::Format_ARGB32_Premultiplied);
            translated.fill(Qt::transparent);

            QPainter painter(&translated);
            painter.drawImage(x, y, oldImage);
            painter.end();

            newImages.append(translated);
        }
    }

    beginMacro(QLatin1String("MoveLayeredImageContents"));
    addChange(new MoveLayeredImageContentsCommand(this, previousImages, newImages));
    endMacro();
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

bool LayeredImageProject::isValidIndex(int index) const
{
    return index >= 0 && index < mLayers.size();
}

Project::Type LayeredImageProject::type() const
{
    return LayeredImageType;
}

void LayeredImageProject::doSetCanvasSize(const QVector<QImage> &newImages)
{
    doSetImageSize(newImages);
}

void LayeredImageProject::doSetImageSize(const QVector<QImage> &newImages)
{
    Q_ASSERT(newImages.size() == mLayers.size());

    const QSize previousSize = size();

    for (int i = 0; i < newImages.size(); ++i) {
        const QImage newImage = newImages.at(i);
        Q_ASSERT(!newImage.isNull());
        Q_ASSERT(newImage.size() != previousSize);

        ImageLayer *layer = mLayers.at(i);
        *layer->image() = newImage;
    }

    emit sizeChanged();
}

void LayeredImageProject::doMoveContents(const QVector<QImage> &newImages)
{
    Q_ASSERT(newImages.size() == mLayers.size());

    for (int i = 0; i < newImages.size(); ++i) {
        const QImage newImage = newImages.at(i);
        Q_ASSERT(!newImage.isNull());

        ImageLayer *layer = mLayers.at(i);
        *layer->image() = newImage;
    }

    emit contentsMoved();
}

void LayeredImageProject::addNewLayer(int imageWidth, int imageHeight, bool transparent, bool undoable)
{
    QImage emptyImage(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
    emptyImage.fill(transparent ? Qt::transparent : Qt::white);

    QScopedPointer<ImageLayer> imageLayer(new ImageLayer(nullptr, emptyImage));
    imageLayer->setName(QString::fromLatin1("Layer %1").arg(++mLayersCreated));

    if (undoable) {
        beginMacro(QLatin1String("AddLayerCommand"));
        addChange(new AddLayerCommand(this, imageLayer.data(), 0));
        endMacro();
    } else {
        addLayer(imageLayer.data(), 0);
    }

    imageLayer.take();
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
