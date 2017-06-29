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

#include "layeredimagecanvas.h"

#include <QLoggingCategory>
#include <QPainter>

#include "imagelayer.h"
#include "layeredimageproject.h"

LayeredImageCanvas::LayeredImageCanvas() :
    mLayeredImageProject(nullptr)
{
    qCDebug(lcCanvasLifecycle) << "constructing" << this;
}

LayeredImageCanvas::~LayeredImageCanvas()
{
    qCDebug(lcCanvasLifecycle) << "destructing" << this;
}

void LayeredImageCanvas::onPostLayerAdded(int index)
{
    ImageLayer *layer = mLayeredImageProject->layerAt(index);
    connect(layer, &ImageLayer::visibleChanged, this, &LayeredImageCanvas::onLayerVisibleChanged);
    connect(layer, &ImageLayer::opacityChanged, this, &LayeredImageCanvas::onLayerOpacityChanged);
    update();
}

void LayeredImageCanvas::onPreLayerRemoved(int index)
{
    ImageLayer *layer = mLayeredImageProject->layerAt(index);
    disconnect(layer, &ImageLayer::visibleChanged, this, &LayeredImageCanvas::onLayerVisibleChanged);
    disconnect(layer, &ImageLayer::opacityChanged, this, &LayeredImageCanvas::onLayerOpacityChanged);
}

void LayeredImageCanvas::onPostLayerRemoved()
{
    update();
}

void LayeredImageCanvas::onPostLayerMoved()
{
    update();
}

void LayeredImageCanvas::onLayerVisibleChanged()
{
    update();
}

void LayeredImageCanvas::onLayerOpacityChanged()
{
    ImageLayer *layer = qobject_cast<ImageLayer*>(sender());
    Q_ASSERT(layer);
    // We don't care about opacity changes of invisible layers.
    if (layer->isVisible())
        update();
}

void LayeredImageCanvas::connectSignals()
{
    ImageCanvas::connectSignals();

    mLayeredImageProject = qobject_cast<LayeredImageProject*>(mProject);
    Q_ASSERT_X(mLayeredImageProject, Q_FUNC_INFO, "Non-layered image project set on LayeredImageCanvas");

    connect(mLayeredImageProject, &LayeredImageProject::postLayerAdded, this, &LayeredImageCanvas::onPostLayerAdded);
    connect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this, &LayeredImageCanvas::onPreLayerRemoved);
    connect(mLayeredImageProject, &LayeredImageProject::postLayerRemoved, this, &LayeredImageCanvas::onPostLayerRemoved);
    connect(mLayeredImageProject, &LayeredImageProject::postLayerMoved, this, &LayeredImageCanvas::onPostLayerMoved);

    // Connect to all existing layers, as onPostLayerAdded() won't get called for them automatically.
    for (int i = 0; i < mLayeredImageProject->layerCount(); ++i) {
        onPostLayerAdded(i);
    }
}

void LayeredImageCanvas::disconnectSignals()
{
    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerAdded, this, &LayeredImageCanvas::onPostLayerAdded);
    disconnect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this, &LayeredImageCanvas::onPreLayerRemoved);
    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerRemoved, this, &LayeredImageCanvas::onPostLayerRemoved);
    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerMoved, this, &LayeredImageCanvas::onPostLayerMoved);

    mLayeredImageProject = nullptr;
}

QImage *LayeredImageCanvas::currentProjectImage()
{
    return mLayeredImageProject->currentLayer()->image();
}

const QImage *LayeredImageCanvas::currentProjectImage() const
{
    return mLayeredImageProject->currentLayer()->image();
}

QImage LayeredImageCanvas::contentImage() const
{
    QImage finalImage(mLayeredImageProject->size(), QImage::Format_ARGB32_Premultiplied);
    finalImage.fill(Qt::transparent);

    QPainter painter(&finalImage);
    // Work backwards from the last layer so that it gets drawn at the "bottom".
    for (int i = mLayeredImageProject->layerCount() - 1; i >= 0; --i) {
        ImageLayer *layer = mLayeredImageProject->layerAt(i);
        if (!layer->isVisible() || qFuzzyIsNull(layer->opacity()))
            continue;

        QImage layerImage;
        if (i == mLayeredImageProject->currentLayerIndex() && shouldDrawSelectionPreviewImage()) {
            layerImage = mSelectionPreviewImage;
        } else {
            layerImage = *layer->image();
        }
        painter.drawImage(0, 0, layerImage);
    }

    return finalImage;
}
