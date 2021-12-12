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

#ifndef FILLALGORITHMS_H
#define FILLALGORITHMS_H

#include <QString>
#include <QtContainerFwd>

class QColor;
class QImage;
class QPoint;

class TexturedFillParameters;
class TilesetProject;
class Tile;

class FillColourProvider
{
public:
    virtual QColor colour(const QColor &baseColour) const;

    // True if this provider allows filling a starting pixel whose
    // colour is equal to the replacement colour. Regular fill providers
    // will return false because they only fill with one colour, but textured fill
    // providers will want to return true because they fill with random colours,
    // so it's rarely a no-op.
    virtual bool allowsNoOpFills() const;

    // A general "can we do our job given our inputs" check.
    virtual bool canProvideColours() const;

    virtual QString debugName() const;
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
