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

//#include "changelayeredimagecanvassizecommand.h"
#include "imagelayer.h"
#include "jsonutils.h"

LayeredImageProject::LayeredImageProject() :
    mCurrentLayerIndex(0)
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
    if (index == mCurrentLayerIndex)
        return;

    mCurrentLayerIndex = index;
    emit currentLayerIndexChanged();
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

//    beginMacro(QLatin1String("ChangeLayeredImageCanvasSize"));
//    addChange(new ChangeLayeredImageCanvasSizeCommand(this, mImage.size(), newSize));
//    endMacro();
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
        addLayer(imageLayer);
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
    imageLayer->setName(QString::fromLatin1("Layer %1").arg(mLayers.size() + 1));
    addLayer(imageLayer);
}

void LayeredImageProject::addLayer(ImageLayer *imageLayer)
{
    if (mLayers.contains(imageLayer)) {
        qWarning() << "Can't add layer" << imageLayer << "to project as we already have it";
        return;
    }

    const int layerIndex = mLayers.size();
    preLayerAdded(layerIndex);

    mLayers.append(imageLayer);

    postLayerAdded(layerIndex);
}
