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

#include "selectionitem.h"

#include "canvaspane.h"
#include "imagecanvas.h"
#include "imageutils.h"
#include "panedrawinghelper.h"

#include <QPainter>

SelectionItem::SelectionItem(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setObjectName("SelectionItem");
    setVisible(false);
}

ImageCanvas *SelectionItem::canvas() const
{
    return mCanvas;
}

void SelectionItem::setCanvas(ImageCanvas *newCanvas)
{
    if (mCanvas == newCanvas)
        return;

    if (mCanvas)
        mCanvas->disconnect(this);

    mCanvas = newCanvas;

    if (mCanvas) {
        connect(mCanvas, &ImageCanvas::hasSelectionChanged, this, [=]() { setVisible(mCanvas->hasSelection()); });
        connect(mCanvas, &ImageCanvas::selectionAreaChanged, this, [=]() { update(); });
    }

    emit canvasChanged();
}

CanvasPane *SelectionItem::pane() const
{
    return mPane;
}

void SelectionItem::setPane(CanvasPane *newPane)
{
    if (mPane == newPane)
        return;

    if (mPane)
        mPane->disconnect(this);

    mPane = newPane;

    if (mPane) {
        connect(mPane, &CanvasPane::zoomLevelChanged, this, [=]() { update(); });
        connect(mPane, &CanvasPane::integerOffsetChanged, this, [=]() { update(); });
        connect(mPane, &CanvasPane::sizeChanged, this, [=]() { update(); });
    }

    emit paneChanged();
}

int SelectionItem::paneIndex() const
{
    return mPaneIndex;
}

void SelectionItem::setPaneIndex(int paneIndex)
{
    if (mPaneIndex == paneIndex)
        return;

    mPaneIndex = paneIndex;
    emit paneIndexChanged();
}

void SelectionItem::paint(QPainter *painter)
{
    if (!mCanvas->hasSelection())
        return;

    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, mPane, mPaneIndex);
    const QRect zoomedSelectionArea(mCanvas->selectionArea().topLeft() * mPane->integerZoomLevel(),
        mPane->zoomedSize(mCanvas->selectionArea().size()));
    ImageUtils::strokeRectWithDashes(painter, zoomedSelectionArea);
}
