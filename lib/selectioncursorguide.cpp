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

#include "selectioncursorguide.h"

#include "canvaspane.h"
#include "imagecanvas.h"
#include "panedrawinghelper.h"

#include <QPainter>

// TODO: see if we can turn this into two thin items rather than one big one.
// could even just try having the two items as a child of this one

SelectionCursorGuide::SelectionCursorGuide(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setObjectName("selectionCursorGuide");
}

SelectionCursorGuide::~SelectionCursorGuide()
{
}

ImageCanvas *SelectionCursorGuide::canvas() const
{
    return mCanvas;
}

void SelectionCursorGuide::setCanvas(ImageCanvas *newCanvas)
{
    if (mCanvas == newCanvas)
        return;

    if (mCanvas)
        mCanvas->disconnect(this);

    mCanvas = newCanvas;

    if (mCanvas) {
        connect(mCanvas, &ImageCanvas::cursorScenePosChanged, this, [=]() {
            // TODO: disconnect when hidden instead of always being called
            if (isVisible())
                update();
        });
        // Apparently setVisible isn't enough to ensure that we're rendered after
        // a selection is cleared, so we have do it ourselves here.
        connect(mCanvas, &ImageCanvas::hasSelectionChanged, this, [=]() { update(); });
    }

    emit canvasChanged();
}

CanvasPane *SelectionCursorGuide::pane() const
{
    return mPane;
}

void SelectionCursorGuide::setPane(CanvasPane *newPane)
{
    if (mPane == newPane)
        return;

    mPane = newPane;
    emit paneChanged();
}

int SelectionCursorGuide::paneIndex() const
{
    return mPaneIndex;
}

void SelectionCursorGuide::setPaneIndex(int paneIndex)
{
    if (mPaneIndex == paneIndex)
        return;

    mPaneIndex = paneIndex;
    emit paneIndexChanged();
}

void SelectionCursorGuide::paint(QPainter *painter)
{
    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, mPane, mPaneIndex);

    painter->save();

    QPen pen;
    pen.setColor(Qt::gray);
    pen.setStyle(Qt::DotLine);
    painter->setPen(pen);

    // Draw the vertical cursor selection guide.
    painter->save();

    int guidePosition = mCanvas->cursorSceneX();
    qreal zoomedGuidePosition = (guidePosition * mPane->integerZoomLevel()) + (painter->pen().widthF() / 2.0);
    painter->translate(0, -mPane->integerOffset().y());
    painter->drawLine(QLineF(zoomedGuidePosition, 0, zoomedGuidePosition, height()));

    painter->restore();

    // Draw the horizontal cursor selection guide.
    guidePosition = mCanvas->cursorSceneY();
    zoomedGuidePosition = (guidePosition * mPane->integerZoomLevel()) + (painter->pen().widthF() / 2.0);
    painter->translate(-mPane->integerOffset().x(), 0);
    painter->drawLine(QLineF(0, zoomedGuidePosition, mCanvas->paneWidth(mPaneIndex), zoomedGuidePosition));

    painter->restore();
}
