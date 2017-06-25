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
#include <QJsonObject>

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
    mLayersCreated(0)
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

    mCurrentLayerIndex = adjustedIndex;
    emit currentLayerIndexChanged();
    emit currentLayerChanged();
}

ImageLayer *LayeredImageProject::layerAt(int index)
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

    addNewLayer(imageWidth, imageHeight, transparentBackground);

    setUrl(QUrl());
    setNewProject(true);
    emit projectCreated();

    qCDebug(lcProject) << "finished creating new project";
}

void LayeredImageProject::load(const QUrl &url)
{
    qCDebug(lcProject) << "loading project:" << url;

    QFile jsonFile(url.toLocalFile());
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        error(QString::fromLatin1("Layered image project files must have a .json extension (%1)").arg(url.toLocalFile()));
        return;
    }

    if (QFileInfo(jsonFile).suffix() != "json") {
        error(QString::fromLatin1("Failed to open layered image project's JSON file at %1").arg(url.toLocalFile()));
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject rootJson = jsonDoc.object();
    QJsonObject projectObject = JsonUtils::strictValue(rootJson, "project").toObject();
    QJsonArray layerArray = JsonUtils::strictValue(projectObject, "layers").toArray();
    for (int i = 0; i < layerArray.size(); ++i) {
        QJsonObject layerObject = layerArray.at(i).toObject();
        const QString base64ImageData = layerObject.value("imageData").toString();
        QByteArray imageData = QByteArray::fromBase64(base64ImageData.toLatin1());

        QImage image;
        image.loadFromData(imageData, "png");
        if (image.isNull()) {
            error(QString::fromLatin1("Failed to load image for layer %1").arg(i));
            close();
            return;
        }

        ImageLayer *imageLayer = new ImageLayer(this, image);
        addLayerAboveAll(imageLayer);
    }

    setUrl(url);
    emit projectLoaded();

    qCDebug(lcProject) << "loaded project";
}

void LayeredImageProject::close()
{
    qCDebug(lcProject) << "closing project...";

    setNewProject(false);
    qDeleteAll(mLayers);
    mLayers.clear();
    setUrl(QUrl());
    mUndoStack.clear();
    mLayersCreated = 0;
    emit projectClosed();

    qCDebug(lcProject) << "... closed project";
}

void LayeredImageProject::saveAs(const QUrl &url)
{
    if (!hasLoaded())
        return;

    if (url.isEmpty())
        return;

//    const QString filePath = url.toLocalFile();
//    const QFileInfo projectSaveFileInfo(filePath);
//    if (mTempDir.isValid()) {
//        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
//            error(QLatin1String("Cannot save project in internal temporary directory"));
//            return;
//        }
//    }

//    if (mImage.isNull()) {
//        error(QString::fromLatin1("Failed to save project: image is null"));
//        return;
//    }

//    if (!mImage.save(filePath)) {
//        error(QString::fromLatin1("Failed to save project's image to %1").arg(filePath));
//        return;
//    }

//    if (mFromNew) {
//        // The project was successfully saved, so it can now save
//        // to the same URL by default from now on.
//        setNewProject(false);
//    }
//    setUrl(url);
//    mUndoStack.setClean();
    //    mHadUnsavedChangesBeforeMacroBegan = false;
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

void LayeredImageProject::changeSize(const QSize &newSize)
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

void LayeredImageProject::addNewLayer(int imageWidth, int imageHeight, bool transparent)
{
    QImage emptyImage(imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
    emptyImage.fill(transparent ? Qt::transparent : Qt::white);

    ImageLayer *imageLayer = new ImageLayer(this, emptyImage);
    imageLayer->setName(QString::fromLatin1("Layer %1").arg(++mLayersCreated));
    addLayerAboveAll(imageLayer);
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
        debug << "\n    name=" << layer->name() << " visible=" << layer->isVisible() << " opacity=" << layer->opacity();
    }
    return debug.space();
}
