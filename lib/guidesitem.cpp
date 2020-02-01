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

#include "guidesitem.h"

#include "canvaspane.h"
#include "guide.h"
#include "imagecanvas.h"
#include "panedrawinghelper.h"
#include "project.h"

#include <QPainter>

GuidesItem::GuidesItem(ImageCanvas *canvas) :
    QQuickPaintedItem(canvas),
    mCanvas(canvas)
{
    setObjectName("GuidesItem");
    setRenderTarget(FramebufferObject);
}

GuidesItem::~GuidesItem()
{
}

void GuidesItem::paint(QPainter *painter)
{
    if (mCanvas->isSplitScreen()) {
        drawPane(painter, mCanvas->secondPane(), 1);
    }

    drawPane(painter, mCanvas->firstPane(), 0);
}

void GuidesItem::drawPane(QPainter *painter, const CanvasPane *pane, int paneIndex)
{
    if (!mCanvas->project()->hasLoaded())
        return;

    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, pane, paneIndex);

    QPen pen;
    pen.setColor(Qt::gray);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    // Draw the guides.
    QVector<Guide> guides = mCanvas->project()->guides();
    for (int i = 0; i < guides.size(); ++i) {
        const Guide guide = guides.at(i);
        drawGuide(&paneDrawingHelper, &guide, i);
    }

    // Draw the guide that's being dragged from the ruler, if any.
    if (mCanvas->pressedRuler()) {
        const bool horizontal = mCanvas->pressedRuler()->orientation() == Qt::Horizontal;
        const Guide guide(horizontal ? mCanvas->cursorSceneY() : mCanvas->cursorSceneX(), mCanvas->pressedRuler()->orientation());
        drawGuide(&paneDrawingHelper, &guide, -1);
    }
}

void GuidesItem::drawGuide(PaneDrawingHelper *paneDrawingHelper, const Guide *guide, int guideIndex)
{
    QPainter *painter = paneDrawingHelper->painter();
    const CanvasPane *pane = paneDrawingHelper->pane();
    const int paneIndex = paneDrawingHelper->paneIndex();

    // If this is an existing guide that is currently being dragged, draw it in its dragged position.
    const bool draggingExistingGuide = mCanvas->pressedGuideIndex() != -1 && mCanvas->pressedGuideIndex() == guideIndex;
    const bool vertical = guide->orientation() == Qt::Vertical;
    const int guidePosition = draggingExistingGuide ? (vertical ? mCanvas->cursorSceneX() : mCanvas->cursorSceneY()) : guide->position();
    const qreal zoomedGuidePosition = guidePosition * pane->integerZoomLevel() + painter->pen().widthF() / 2.0;

    painter->save();
    painter->setPen(Qt::cyan);

    const QRect visibleSceneArea = mCanvas->paneVisibleSceneArea(paneIndex);

    if (vertical) {
        // Don't bother drawing it if it's not visible within the scene.
        // Use visibleSceneArea.x()/y() to ensure that that coordinate is within the
        // scene area, as we don't care about it and are only testing the other coordinate.
        if (visibleSceneArea.contains(QPoint(guidePosition, visibleSceneArea.y()))) {
            // Don't need to account for the vertical offset anymore, as vertical guides go across the whole height of the pane.
            painter->translate(0, -pane->integerOffset().y());
            painter->drawLine(QLineF(zoomedGuidePosition, 0, zoomedGuidePosition, height()));
        }
    } else {
        if (visibleSceneArea.contains(QPoint(visibleSceneArea.x(), guidePosition))) {
            // Don't need to account for the horizontal offset anymore, as horizontal guides go across the whole width of the pane.
            painter->translate(-pane->integerOffset().x(), 0);
            painter->drawLine(QLineF(0, zoomedGuidePosition, mCanvas->paneWidth(paneIndex), zoomedGuidePosition));
        }
    }
    painter->restore();
}
