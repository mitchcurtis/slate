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
    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, mPane, mPaneIndex);

    TileCanvas *tileCanvas = qobject_cast<TileCanvas*>(mCanvas);
    Q_ASSERT(tileCanvas);

    TilesetProject *tilesetProject = qobject_cast<TilesetProject*>(tileCanvas->project());
    Q_ASSERT(tilesetProject);

    const QSize zoomedTileSize = mPane->zoomedSize(tilesetProject->tileSize());
    // Try to only draw as much as could possibly fit within the pane.
//    const int horizontalStartTile = pane.offset().x() < 0 ? qFloor(qAbs(pane.offset().x()) / zoomedTileSize.width()) : 0;
//    const int verticalStartTile = pane.offset().y() < 0 ? qFloor(qAbs(pane.offset().y()) / zoomedTileSize.height()) : 0;
    const int tilesAcross = tilesetProject->tilesWide();//qMin(mProject->tilesWide(), qCeil(paneWidth / zoomedTileSize.width()) + 1);
    const int tilesDown = tilesetProject->tilesHigh();//qMin(mProject->tilesHigh(), qCeil(height() / zoomedTileSize.width()) + 1);

    for (int y = 0; y < tilesDown; ++y) {
        for (int x = 0; x < tilesAcross; ++x) {
            const QPoint topLeftInScene(x * tilesetProject->tileWidth(), y * tilesetProject->tileHeight());
            const QRect rect(x * zoomedTileSize.width(), y * zoomedTileSize.height(),
                zoomedTileSize.width(), zoomedTileSize.height());

            // If the tile pen is in use and it's over this tile, draw it, otherwise draw the current tile
            // at that location as usual.
            bool previewTile = false;
            if (tileCanvas->mTilePenPreview) {
                QRect tileSceneRect(topLeftInScene.x(), topLeftInScene.y(), tilesetProject->tileWidth(), tilesetProject->tileHeight());
                previewTile = tileSceneRect.contains(tileCanvas->cursorSceneX(), tileCanvas->cursorSceneY());
            }

            if (previewTile) {
                painter->drawImage(rect, *tileCanvas->mPenTile->tileset()->image(), tileCanvas->mPenTile->sourceRect());
            } else {
                const Tile *tile = tilesetProject->tileAt(topLeftInScene);
                if (tile) {
                    painter->drawImage(rect, *tile->tileset()->image(), tile->sourceRect());
                }
            }

            if (tileCanvas->mGridVisible) {
                QPen pen(tileCanvas->mGridColour);
                painter->setPen(pen);

                painter->drawLine(rect.x(), rect.y(), rect.x(), rect.y() + rect.height() - 1);

                if (x == tilesAcross - 1) {
                    // If this is the right-most edge tile, draw a line on the outside of it.
                    painter->drawLine(rect.x() + zoomedTileSize.width(), rect.y(),
                        rect.x() + zoomedTileSize.width(), rect.y() + rect.height() - 1);
                }

                painter->drawLine(rect.x() + 1, rect.y(), rect.x() + rect.width() - 1, rect.y());

                if (y == tilesDown - 1) {
                    // If this is the bottom-most edge tile, draw a line on the outside of it.
                    painter->drawLine(rect.x(), rect.y() + zoomedTileSize.height(),
                        rect.x() + rect.width(), rect.y() + zoomedTileSize.width());
                }
            }
        }
    }
}
