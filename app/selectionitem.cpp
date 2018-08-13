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

#include "selectionitem.h"

#include "canvaspane.h"
#include "imagecanvas.h"
#include "panedrawinghelper.h"
#include "utils.h"

#include <QPainter>

SelectionItem::SelectionItem(ImageCanvas *canvas) :
    QQuickPaintedItem(canvas),
    mCanvas(canvas)
{
    setObjectName("SelectionItem");
    setVisible(false);

    connect(canvas, &ImageCanvas::hasSelectionChanged, [=]() { setVisible(mCanvas->hasSelection()); });
}

void SelectionItem::paint(QPainter *painter)
{
    if (mCanvas->isSplitScreen()) {
        drawPane(painter, mCanvas->secondPane(), 1);
    }

    drawPane(painter, mCanvas->firstPane(), 0);
}

void SelectionItem::drawPane(QPainter *painter, const CanvasPane *pane, int paneIndex)
{
    if (!mCanvas->hasSelection())
        return;

    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, pane, paneIndex);
    const QRect zoomedSelectionArea(mCanvas->selectionArea().topLeft() * pane->integerZoomLevel(),
        pane->zoomedSize(mCanvas->selectionArea().size()));
    Utils::strokeRectWithDashes(painter, zoomedSelectionArea);
}
