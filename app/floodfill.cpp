/*
    Copyright 2016, Mitch Curtis

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

#include "floodfill.h"

#include <QDebug>
#include <QColor>
#include <QImage>
#include <QLoggingCategory>
#include <QQueue>

#include "tile.h"
#include "tilesetproject.h"

Q_LOGGING_CATEGORY(lcPixelFloodFill, "app.pixelFloodFill")
Q_LOGGING_CATEGORY(lcTileFloodFill, "app.tileFloodFill")

// https://en.wikipedia.org/wiki/Flood_fill#Alternative_implementations
// TODO: this is too slow
QVector<QPoint> imagePixelFloodFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour)
{
    QVector<QPoint> filledPositions;
    const QRect imageBounds(0, 0, image->width(), image->height());
    if (!imageBounds.contains(startPos)) {
        return filledPositions;
    }

    if (image->pixelColor(startPos) == replacementColour) {
        return filledPositions;
    }

    if (image->pixelColor(startPos) != targetColour) {
        return filledPositions;
    }

    QQueue<QPoint> queue;
    queue.append(startPos);
    filledPositions.append(startPos);

    while (!queue.isEmpty()) {
        QPoint node = queue.takeFirst();

        const QPoint north = node - QPoint(0, 1);
        const QPoint south = node + QPoint(0, 1);
        const QPoint east = node + QPoint(1, 0);
        const QPoint west = node - QPoint(1, 0);

        if (imageBounds.contains(west) && !filledPositions.contains(west) && image->pixelColor(west) == targetColour) {
            queue.append(west);
            filledPositions.append(west);
        }
        if (imageBounds.contains(east) && !filledPositions.contains(east) && image->pixelColor(east) == targetColour) {
            queue.append(east);
            filledPositions.append(east);
        }
        if (imageBounds.contains(north) && !filledPositions.contains(north) && image->pixelColor(north) == targetColour) {
            queue.append(north);
            filledPositions.append(north);
        }
        if (imageBounds.contains(south) && !filledPositions.contains(south) && image->pixelColor(south) == targetColour) {
            queue.append(south);
            filledPositions.append(south);
        }
    }

    return filledPositions;
}

// TODO: convert these to non-recursive algorithms as above
void tilesetPixelFloodFill(const Tile *tile, const QPoint &pos, const QColor &targetColour,
    const QColor &replacementColour, QVector<QPoint> &filledPositions)
{
    qCDebug(lcPixelFloodFill) << "attempting to fill pixel at" << pos << "...";

    if (filledPositions.contains(pos)) {
        // This tile has already been filled (would this ever happen?)
        qCDebug(lcPixelFloodFill) << "already filled";
        return;
    }

    if (tile->pixelColor(pos) == replacementColour) {
        qCDebug(lcPixelFloodFill) << "hit the same colour as replacement colour; returning";
        return;
    }

    if (tile->pixelColor(pos) != targetColour) {
        qCDebug(lcPixelFloodFill) << "hit a different colour; returning";
        return;
    }

    qCDebug(lcPixelFloodFill) << "filling!";
    filledPositions.append(pos);

    const QRect tileBounds(0, 0, tile->sourceRect().width(), tile->sourceRect().height());
    const QPoint north = pos - QPoint(0, 1);
    const QPoint south = pos + QPoint(0, 1);
    const QPoint east = pos + QPoint(1, 0);
    const QPoint west = pos - QPoint(1, 0);

    if (tileBounds.contains(north)) {
        tilesetPixelFloodFill(tile, north, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << north << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
    if (tileBounds.contains(south)) {
        tilesetPixelFloodFill(tile, south, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << south << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
    if (tileBounds.contains(east)) {
        tilesetPixelFloodFill(tile, east, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << east << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
    if (tileBounds.contains(west)) {
        tilesetPixelFloodFill(tile, west, targetColour, replacementColour, filledPositions);
    } else {
        qCDebug(lcPixelFloodFill) << west << "is out of bounds" << "( tileBounds =" << tileBounds << ")";
    }
}

void tilesetTileFloodFill(const TilesetProject *project, const Tile *tile, const QPoint &tilePos,
    int targetTile, int replacementTile, QVector<QPoint> &filledTilePositions)
{
    qCDebug(lcTileFloodFill) << "attempting to fill pixel at" << tilePos << "...";

    if (filledTilePositions.contains(tilePos)) {
        // This tile has already been filled (would this ever happen?)
        qCDebug(lcTileFloodFill) << "already filled";
        return;
    }

    const int tileIdAtTilePos = project->tileIdAtTilePos(tilePos);
    if (tileIdAtTilePos == replacementTile) {
        qCDebug(lcTileFloodFill) << "hit the same tile as replacement tile; returning";
        return;
    }

    if (tileIdAtTilePos != targetTile) {
        qCDebug(lcTileFloodFill) << "hit a different colour; returning";
        return;
    }

    qCDebug(lcTileFloodFill) << "filling!";
    filledTilePositions.append(tilePos);

    const QPoint north = tilePos - QPoint(0, 1);
    const QPoint south = tilePos + QPoint(0, 1);
    const QPoint east = tilePos + QPoint(1, 0);
    const QPoint west = tilePos - QPoint(1, 0);

    if (project->isTilePosWithinBounds(north)) {
        tilesetTileFloodFill(project, tile, north, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << north << "is out of bounds";
    }
    if (project->isTilePosWithinBounds(south)) {
        tilesetTileFloodFill(project, tile, south, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << south << "is out of bounds";
    }
    if (project->isTilePosWithinBounds(east)) {
        tilesetTileFloodFill(project, tile, east, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << east << "is out of bounds";
    }
    if (project->isTilePosWithinBounds(west)) {
        tilesetTileFloodFill(project, tile, west, targetTile, replacementTile, filledTilePositions);
    } else {
        qCDebug(lcTileFloodFill) << west << "is out of bounds";
    }
}
