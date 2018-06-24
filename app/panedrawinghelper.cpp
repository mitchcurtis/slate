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

#include "panedrawinghelper.h"

#include "canvaspane.h"
#include "imagecanvas.h"

PaneDrawingHelper::PaneDrawingHelper(const ImageCanvas *canvas, QPainter *painter, const CanvasPane *pane, int paneIndex) :
    mCanvas(canvas),
    mPainter(painter),
    mPane(pane),
    mPaneIndex(paneIndex)
{
    mPainter->save();

    QPoint translateDistance;
    if (mPaneIndex == 1) {
        translateDistance.rx() = mCanvas->width() * mCanvas->firstPane()->size();
    }
    translateDistance += mPane->offset();
    painter->translate(translateDistance);

    // Can't set two clip regions on the same painting, so we only clip the left-hand pane
    // and simply paint it over the top of any out-of-bounds drawing the right-hand one has done.
    const int paneWidth = mCanvas->width() * mPane->size();
    if (mPaneIndex == 0)
        painter->setClipRegion(QRegion(-translateDistance.x(), -translateDistance.y(), paneWidth, canvas->height()));
}

PaneDrawingHelper::~PaneDrawingHelper()
{
    mPainter->restore();
}
