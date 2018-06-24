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

#include "selectioncursorguide.h"

#include "canvaspane.h"
#include "imagecanvas.h"
#include "panedrawinghelper.h"

#include <QPainter>

SelectionCursorGuide::SelectionCursorGuide() :
    mCanvas(nullptr)
{
}

SelectionCursorGuide::~SelectionCursorGuide()
{
}

void SelectionCursorGuide::setCanvas(ImageCanvas *canvas)
{
    mCanvas = canvas;
}

void SelectionCursorGuide::paint(QPainter *painter)
{
    if (mCanvas->isSplitScreen()) {
        drawPane(painter, mCanvas->secondPane(), 1);
    }

    drawPane(painter, mCanvas->firstPane(), 0);
}

void SelectionCursorGuide::drawPane(QPainter *painter, const CanvasPane *pane, int paneIndex)
{
    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, pane, paneIndex);

    painter->save();

    QPen pen;
    pen.setColor(Qt::gray);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    // Draw the vertical cursor selection guide.
    painter->save();

    int guidePosition = mCanvas->cursorSceneX();
    qreal zoomedGuidePosition = (guidePosition * pane->integerZoomLevel()) + (painter->pen().widthF() / 2.0);
    painter->translate(0, -pane->offset().y());
    painter->drawLine(QLineF(zoomedGuidePosition, 0, zoomedGuidePosition, height()));

    painter->restore();

    // Draw the horizontal cursor selection guide.
    guidePosition = mCanvas->cursorSceneY();
    zoomedGuidePosition = (guidePosition * pane->integerZoomLevel()) + (painter->pen().widthF() / 2.0);
    painter->translate(-pane->offset().x(), 0);
    painter->drawLine(QLineF(0, zoomedGuidePosition, mCanvas->paneWidth(paneIndex), zoomedGuidePosition));

    painter->restore();
}
