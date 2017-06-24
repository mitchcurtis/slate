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
}

void LayeredImageCanvas::onPreLayerRemoved(int index)
{
    ImageLayer *layer = mLayeredImageProject->layerAt(index);
    disconnect(layer, &ImageLayer::visibleChanged, this, &LayeredImageCanvas::onLayerVisibleChanged);
    disconnect(layer, &ImageLayer::opacityChanged, this, &LayeredImageCanvas::onLayerOpacityChanged);
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

    // Connect to all existing layers, as onPostLayerAdded() won't get called for them automatically.
    for (int i = 0; i < mLayeredImageProject->layerCount(); ++i) {
        onPostLayerAdded(i);
    }
}

void LayeredImageCanvas::disconnectSignals()
{
    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerAdded, this, &LayeredImageCanvas::onPostLayerAdded);
    disconnect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this, &LayeredImageCanvas::onPreLayerRemoved);

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
    return ImageCanvas::contentImage();
}
