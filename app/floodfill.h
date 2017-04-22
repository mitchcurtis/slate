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

#ifndef FLOODFILL_H
#define FLOODFILL_H

class QColor;
class QImage;
class QPoint;
template <typename T>
class QVector;

class TilesetProject;
class Tile;

void imagePixelFloodFill(const QImage *image, const QPoint &pos, const QColor &targetColour,
    const QColor &replacementColour, QVector<QPoint> &filledPositions);

void tilesetPixelFloodFill(const Tile *tile, const QPoint &pos, const QColor &targetColour,
    const QColor &replacementColour, QVector<QPoint> &filledPositions);

void tilesetTileFloodFill(const TilesetProject *project, const Tile *tile, const QPoint &tilePos, int targetTile,
    int replacementTile, QVector<QPoint> &filledTilePositions);

#endif // FLOODFILL_H
