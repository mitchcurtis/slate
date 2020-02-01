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

#ifndef TILEGRID_H
#define TILEGRID_H

#include <QQuickPaintedItem>

#include "slate-global.h"

class SLATE_EXPORT TileGrid : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int tileWidth READ tileWidth WRITE setTileWidth NOTIFY tileWidthChanged)
    Q_PROPERTY(int tileHeight READ tileHeight WRITE setTileHeight NOTIFY tileHeightChanged)
    Q_PROPERTY(int tilesWide READ tilesWide WRITE setTilesWide NOTIFY tilesWideChanged)
    Q_PROPERTY(int tilesHigh READ tilesHigh WRITE setTilesHigh NOTIFY tilesHighChanged)
    Q_PROPERTY(int highlightedIndex READ highlightedIndex WRITE setHighlightedIndex NOTIFY highlightedIndexChanged)
    Q_PROPERTY(QColor colour READ colour WRITE setColour NOTIFY colourChanged)
    Q_PROPERTY(QColor highlightColour READ highlightColour WRITE setHighlightColour NOTIFY highlightColourChanged)

public:
    TileGrid();

    void paint(QPainter *painter) override;

    int tileWidth() const;
    void setTileWidth(int tileWidth);

    int tileHeight() const;
    void setTileHeight(int tileHeight);

    int tilesWide() const;
    void setTilesWide(int tilesWide);

    int tilesHigh() const;
    void setTilesHigh(int tilesHigh);

    int highlightedIndex() const;
    void setHighlightedIndex(int highlightedIndex);

    QColor colour() const;
    void setColour(const QColor &colour);

    QColor highlightColour() const;
    void setHighlightColour(const QColor &highlightColour);

signals:
    void tilesWideChanged();
    void tilesHighChanged();
    void tileWidthChanged();
    void tileHeightChanged();
    void highlightedIndexChanged();
    void colourChanged();
    void highlightColourChanged();

private:
    int mTileWidth;
    int mTileHeight;
    int mTilesWide;
    int mTilesHigh;
    int mHighlightedIndex;
    QColor mColour;
    QColor mHighlightColour;
};

#endif // TILEGRID_H
