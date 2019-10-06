/*
    Copyright 2019, Mitch Curtis

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

#include "canvaspaneitem.h"

#include "canvaspane.h"
#include "guide.h"
#include "imagecanvas.h"
#include "panedrawinghelper.h"
#include "project.h"
#include "utils.h"

#include <QPainter>

/*
    This class is a purely visual respresentation of a canvas pane;
    ImageCanvas contains all of the state that will be painted, and this class paints it.
*/

CanvasPaneItem::CanvasPaneItem(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setObjectName("CanvasPaneItem");
    setRenderTarget(FramebufferObject);
}

CanvasPaneItem::~CanvasPaneItem()
{
}

ImageCanvas *CanvasPaneItem::canvas() const
{
    return mCanvas;
}

void CanvasPaneItem::setCanvas(ImageCanvas *canvas)
{
    if (canvas == mCanvas)
        return;

    if (mCanvas)
        disconnectFromCanvas();

    mCanvas = canvas;

    if (mCanvas)
        connectToCanvas();

    emit canvasChanged();
}

CanvasPane *CanvasPaneItem::pane() const
{
    return mPane;
}

void CanvasPaneItem::setPane(CanvasPane *pane)
{
    if (pane == mPane)
        return;

    mPane = pane;
    emit paneChanged();
}

int CanvasPaneItem::paneIndex() const
{
    return mPaneIndex;
}

void CanvasPaneItem::setPaneIndex(int paneIndex)
{
    if (paneIndex == mPaneIndex)
        return;

    mPaneIndex = paneIndex;
    emit paneIndexChanged();
}

void CanvasPaneItem::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value)
{
    if (change == ItemVisibleHasChanged) {
        if (value.boolValue)
            connectToCanvas();
        else
            disconnectFromCanvas();
    }

    QQuickPaintedItem::itemChange(change, value);
}

void CanvasPaneItem::connectToCanvas()
{
    connect(mCanvas, &ImageCanvas::contentPaintRequested, this, &CanvasPaneItem::onContentPaintRequested);
}

void CanvasPaneItem::disconnectFromCanvas()
{
    mCanvas->disconnect(this);
}

void CanvasPaneItem::onContentPaintRequested(int paneIndex)
{
    if (paneIndex == -1 || paneIndex == mPaneIndex) {
        // Only schedule a re-paint if we were the pane it was requested for.
        update();
    }
}

void CanvasPaneItem::paint(QPainter *painter)
{
    if (!mCanvas->project() || !mCanvas->project()->hasLoaded())
        return;

    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, mPane, mPaneIndex);

    // Draw the checkered pixmap that acts as an indicator for transparency.
    // We use the unbounded canvas size here, otherwise the drawn area is too small past a certain zoom level.
    const QSize zoomedCanvasSize = mPane->zoomedSize(mCanvas->currentProjectImage()->size());
    painter->drawTiledPixmap(0, 0, zoomedCanvasSize.width(), zoomedCanvasSize.height(), mCanvas->mCheckerPixmap);

    const QImage image = mCanvas->contentImage();
    const QSize zoomedImageSize = mPane->zoomedSize(image.size());
    painter->drawImage(QRectF(QPointF(0, 0), zoomedImageSize), image, QRectF(0, 0, image.width(), image.height()));
}
