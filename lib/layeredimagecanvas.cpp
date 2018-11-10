/*
    Copyright 2018, Mitch Curtis

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
    qCDebug(lcImageCanvasLifecycle) << "constructing LayeredImageCanvas" << this;
}

LayeredImageCanvas::~LayeredImageCanvas()
{
    qCDebug(lcImageCanvasLifecycle) << "destructing LayeredImageCanvas" << this;
}

void LayeredImageCanvas::onPostLayerAdded(int index)
{
    ImageLayer *layer = mLayeredImageProject->layerAt(index);
    // TODO: could we move these to LayeredImageProject and save a few connections?
    connect(layer, &ImageLayer::visibleChanged, this, &LayeredImageCanvas::onLayerVisibleChanged);
    connect(layer, &ImageLayer::opacityChanged, this, &LayeredImageCanvas::onLayerOpacityChanged);
    requestContentPaint();
}

void LayeredImageCanvas::onPreLayerRemoved(int index)
{
    ImageLayer *layer = mLayeredImageProject->layerAt(index);
    disconnect(layer, &ImageLayer::visibleChanged, this, &LayeredImageCanvas::onLayerVisibleChanged);
    disconnect(layer, &ImageLayer::opacityChanged, this, &LayeredImageCanvas::onLayerOpacityChanged);
}

void LayeredImageCanvas::onPostLayerRemoved()
{
    requestContentPaint();
}

void LayeredImageCanvas::onPostLayerMoved()
{
    requestContentPaint();
}

void LayeredImageCanvas::onPostLayerImageChanged()
{
    requestContentPaint();
}

void LayeredImageCanvas::onLayerVisibleChanged()
{
    requestContentPaint();

    ImageLayer *layer = qobject_cast<ImageLayer*>(sender());
    if (layer == mLayeredImageProject->currentLayer())
        updateWindowCursorShape();
}

void LayeredImageCanvas::onLayerOpacityChanged()
{
    ImageLayer *layer = qobject_cast<ImageLayer*>(sender());
    Q_ASSERT(layer);
    // We don't care about opacity changes of invisible layers.
    if (layer->isVisible())
        requestContentPaint();
}

void LayeredImageCanvas::onPreCurrentLayerChanged()
{
    // TODO: move paste branch into clearOrConfirmSelection();?
    if (mLastSelectionModification != NoSelectionModification && mLastSelectionModification != SelectionPaste)
        confirmSelectionModification();
    else if (mIsSelectionFromPaste)
        confirmPasteSelection();
    else
        clearSelection();
}

void LayeredImageCanvas::onPostCurrentLayerChanged()
{
    updateWindowCursorShape();
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
    connect(mLayeredImageProject, &LayeredImageProject::postLayerImageChanged, this, &LayeredImageCanvas::onPostLayerImageChanged);
    connect(mLayeredImageProject, &LayeredImageProject::preCurrentLayerChanged, this, &LayeredImageCanvas::onPreCurrentLayerChanged);
    connect(mLayeredImageProject, &LayeredImageProject::postCurrentLayerChanged, this, &LayeredImageCanvas::onPostCurrentLayerChanged);
    connect(mLayeredImageProject, &LayeredImageProject::contentsMoved, this, &LayeredImageCanvas::requestContentPaint);

    // Connect to all existing layers, as onPostLayerAdded() won't get called for them automatically.
    for (int i = 0; i < mLayeredImageProject->layerCount(); ++i) {
        onPostLayerAdded(i);
    }
}

void LayeredImageCanvas::disconnectSignals()
{
    ImageCanvas::disconnectSignals();

    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerAdded, this, &LayeredImageCanvas::onPostLayerAdded);
    disconnect(mLayeredImageProject, &LayeredImageProject::preLayerRemoved, this, &LayeredImageCanvas::onPreLayerRemoved);
    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerRemoved, this, &LayeredImageCanvas::onPostLayerRemoved);
    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerMoved, this, &LayeredImageCanvas::onPostLayerMoved);
    disconnect(mLayeredImageProject, &LayeredImageProject::postLayerImageChanged, this, &LayeredImageCanvas::onPostLayerImageChanged);
    disconnect(mLayeredImageProject, &LayeredImageProject::preCurrentLayerChanged, this, &LayeredImageCanvas::onPreCurrentLayerChanged);
    disconnect(mLayeredImageProject, &LayeredImageProject::postCurrentLayerChanged, this, &LayeredImageCanvas::onPostCurrentLayerChanged);
    disconnect(mLayeredImageProject, &LayeredImageProject::contentsMoved, this, &LayeredImageCanvas::requestContentPaint);

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

QImage *LayeredImageCanvas::imageForLayerAt(int layerIndex)
{
    return mLayeredImageProject->layerAt(layerIndex)->image();
}

int LayeredImageCanvas::currentLayerIndex() const
{
    return mLayeredImageProject->currentLayerIndex();
}

QImage LayeredImageCanvas::getContentImage()
{
    return mLayeredImageProject->flattenedImage([=](int index) {
        QImage layerImage;
        if (index == mLayeredImageProject->currentLayerIndex()) {
            if (shouldDrawSelectionPreviewImage()) {
                layerImage = mSelectionPreviewImage;
            } else if (isLineVisible()) {
                layerImage = *mLayeredImageProject->currentLayer()->image();
                QPainter linePainter(&layerImage);
                drawLine(&linePainter, linePoint1(), linePoint2());
            }
        }
        return layerImage;
    });
}

void LayeredImageCanvas::replaceImage(int layerIndex, const QImage &replacementImage)
{
    *mLayeredImageProject->layerAt(layerIndex)->image() = replacementImage;
    requestContentPaint();
}

bool LayeredImageCanvas::areToolsForbidden() const
{
    // For layered image projects, tools cannot be used on the current layer
    // while it is hidden.
    ImageLayer *currentLayer = mLayeredImageProject->currentLayer();
    return currentLayer && !currentLayer->isVisible();
}
