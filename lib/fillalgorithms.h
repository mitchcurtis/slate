/*
    Copyright 2017, Mitch Curtis

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

#ifndef FILLALGORITHMS_H
#define FILLALGORITHMS_H

class QColor;
class QImage;
class QPoint;
template <typename T>
class QVector;

class TexturedFillParameters;
class TilesetProject;
class Tile;

class FillColourProvider
{
public:
    virtual QColor colour(const QColor &baseColour) const;
};

QImage imagePixelFloodFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const FillColourProvider &fillColourProvider = FillColourProvider());

QImage imageGreedyPixelFill(const QImage *image, const QPoint &startPos, const QColor &targetColour,
    const QColor &replacementColour, const FillColourProvider &fillColourProvider = FillColourProvider());

QImage texturedFill(const QImage *image, const QPoint &startPos,
    const QColor &targetColour, const QColor &replacementColour, const TexturedFillParameters &parameters);

QImage greedyTexturedFill(const QImage *image, const QPoint &startPos,
    const QColor &targetColour, const QColor &replacementColour, const TexturedFillParameters &parameters);

void tilesetPixelFloodFill(const Tile *tile, const QPoint &pos, const QColor &targetColour,
    const QColor &replacementColour, QVector<QPoint> &filledPositions);

void tilesetTileFloodFill(const TilesetProject *project, const Tile *tile, const QPoint &tilePos, int targetTile,
    int replacementTile, QVector<QPoint> &filledTilePositions);

#endif // FILLALGORITHMS_H
