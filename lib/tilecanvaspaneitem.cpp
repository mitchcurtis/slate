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

#include "tilecanvaspaneitem.h"

#include "canvaspane.h"
#include "panedrawinghelper.h"
#include "tilecanvas.h"
#include "tilesetproject.h"

#include <QPainter>

/*
    This class is a purely visual respresentation of a canvas pane;
    ImageCanvas contains all of the state that will be painted, and this class paints it.
*/

TileCanvasPaneItem::TileCanvasPaneItem(QQuickItem *parent) :
    CanvasPaneItem(parent)
{
    setObjectName("TileCanvasPaneItem");
}

TileCanvasPaneItem::~TileCanvasPaneItem()
{
}

void TileCanvasPaneItem::paint(QPainter *painter)
{
    CanvasPaneItem::paint(painter);

    if (!mCanvas->project() || !mCanvas->project()->hasLoaded())
        return;

    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, mPane, mPaneIndex);

    const TileCanvas *const tileCanvas = qobject_cast<TileCanvas*>(mCanvas);
    Q_ASSERT(tileCanvas);

    const TilesetProject *const tilesetProject = qobject_cast<TilesetProject*>(tileCanvas->project());
    Q_ASSERT(tilesetProject);

    const QSize zoomedTileSize = tilesetProject->tileSize() * mPane->integerZoomLevel();

    const QRect tilesRect{QPoint{0, 0}, QSize{tilesetProject->tilesWide(), tilesetProject->tilesHigh()}};

    if (tileCanvas->mGridVisible) {
        painter->setPen(tileCanvas->mGridColour);
        for (int y = tilesRect.top(); y <= tilesRect.bottom(); ++y) {
            for (int x = tilesRect.left(); x <= tilesRect.right(); ++x) {
                // Draw top edge for tile
                painter->drawLine(x * zoomedTileSize.width(), y * zoomedTileSize.height(), (x + 1) * zoomedTileSize.width(), y * zoomedTileSize.height());
                // Draw left edge for tile
                painter->drawLine(x * zoomedTileSize.width(), y * zoomedTileSize.height(), x * zoomedTileSize.width(), (y + 1) * zoomedTileSize.height());
            }
        }
        // Draw bottom-most edges
        for (int x = tilesRect.left(), y = tilesRect.bottom() + 1; x <= tilesRect.right(); ++x) {
            painter->drawLine(x * zoomedTileSize.width(), y * zoomedTileSize.height(), (x + 1) * zoomedTileSize.width(), y * zoomedTileSize.height());
        }
        // Draw right-most edges
        for (int y = tilesRect.top(), x = tilesRect.right() + 1; y <= tilesRect.bottom(); ++y) {
            painter->drawLine(x * zoomedTileSize.width(), y * zoomedTileSize.height(), x * zoomedTileSize.width(), (y + 1) * zoomedTileSize.height());
        }
    }
}
