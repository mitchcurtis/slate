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

#include "addlayercommand.h"
#include "changelayeredimagesizecommand.h"
#include "changelayeredimagecanvassizecommand.h"
#include "changelayernamecommand.h"
#include "changelayeropacitycommand.h"
#include "changelayerordercommand.h"
#include "changelayervisiblecommand.h"
#include "deletelayercommand.h"
#include "imagelayer.h"
#include "jsonutils.h"

LayeredImageProject::LayeredImageProject() :
    mCurrentLayerIndex(0),
    mLayersCreated(0),
    mAutoExportEnabled(false),
    mUsingAnimation(false),
    mHasUsedAnimation(false)
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

    beginMacro(QLatin1String("ChangeLayeredImageCanvasSize"));
    addChange(new ChangeLayeredImageCanvasSizeCommand(this, size(), newSize));
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

QImage LayeredImageProject::flattenedImage(std::function<QImage(int)> layerSubstituteFunction) const
{
    QImage finalImage(size(), QImage::Format_ARGB32_Premultiplied);
    finalImage.fill(Qt::transparent);

    QPainter painter(&finalImage);
    // Work backwards from the last layer so that it gets drawn at the "bottom".
    for (int i = layerCount() - 1; i >= 0; --i) {
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

    if (mUsingAnimation)
        mHasUsedAnimation = true;

    emit usingAnimationChanged();
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

void LayeredImageProject::load(const QUrl &url)
{
    qCDebug(lcProject) << "loading project:" << url;

    const QString filePath = url.toLocalFile();
    if (!QFileInfo::exists(filePath)) {
        error(QString::fromLatin1("Layered image project does not exist:\n\n%1").arg(filePath));
        return;
    }

    QFile jsonFile(filePath);
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        error(QString::fromLatin1("Layered image project files must have a .slp extension:\n\n%1").arg(filePath));
        return;
    }

    if (QFileInfo(jsonFile).suffix() != "slp") {
        error(QString::fromLatin1("Failed to open layered image project's .slp file:\n\n%1").arg(filePath));
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject rootJson = jsonDoc.object();
    QJsonObject projectObject = JsonUtils::strictValue(rootJson, "project").toObject();
    QJsonArray layerArray = JsonUtils::strictValue(projectObject, "layers").toArray();
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

    mAutoExportEnabled = projectObject.value("autoExportEnabled").toBool(false);

    mUsingAnimation = projectObject.value("usingAnimation").toBool(false);
    mHasUsedAnimation = projectObject.value("hasUsedAnimation").toBool(false);
    if (mHasUsedAnimation) {
        mAnimationPlayback.read(projectObject.value("animationPlayback").toObject());
    }

    mCachedProjectJson = projectObject;

    setUrl(url);
    emit projectLoaded();

    qCDebug(lcProject) << "loaded project";
}

void LayeredImageProject::close()
{
    qCDebug(lcProject) << "closing project...";

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
    emit projectClosed();

    qCDebug(lcProject) << "... closed project";
}

void LayeredImageProject::saveAs(const QUrl &url)
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

    const QString filePath = url.toLocalFile();
    const QFileInfo projectSaveFileInfo(filePath);
    if (mTempDir.isValid()) {
        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
            error(QLatin1String("Cannot save project in internal temporary directory"));
            return false;
        }
    }

    if (!flattenedImage().save(filePath)) {
        error(QString::fromLatin1("Failed to save project's image to:\n\n%1").arg(filePath));
        return false;
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

void LayeredImageProject::doSetSize(const QSize &newSize)
{
    if (newSize.width() <= 0 || newSize.height() <= 0) {
        error(QString::fromLatin1("Cannot set project size to: %1 x %2").arg(newSize.width(), newSize.height()));
        return;
    }

    Q_ASSERT(newSize != size());
    Q_ASSERT(!mLayers.isEmpty());

    foreach (ImageLayer *layer, mLayers) {
        layer->setSize(newSize);
    }

    emit sizeChanged();
}

void LayeredImageProject::doResize(const QVector<QImage> &newImages)
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

ImageLayer *LayeredImageProject::takeLayer(int index)
{
    preLayerRemoved(index);

    ImageLayer *layer = mLayers.takeAt(index);

    postLayerRemoved(index);

    emit layerCountChanged();

    // Keep the same index to select the layer that was above us.
    setCurrentLayerIndex(index);

    layer->setParent(nullptr);

    return layer;
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
