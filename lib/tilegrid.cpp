/*
    Copyright 2019, Mitch Curtis

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

#include "tilegrid.h"

#include <QtMath>
#include <QPainter>

TileGrid::TileGrid() :
    mTileWidth(32),
    mTileHeight(32),
    mTilesWide(1),
    mTilesHigh(1),
    mHighlightedIndex(-1),
    mColour(QColor::fromRgba(0x55000000)),
    mHighlightColour(Qt::red)
{
}

void TileGrid::paint(QPainter *painter)
{
    QPen pen(mColour);
    painter->setPen(pen);

    // http://stackoverflow.com/a/28262815/904422
    painter->setCompositionMode(QPainter::CompositionMode_Source);

    const qreal halfPenWidth = pen.widthF() / 2.0;

    for (int y = 0; y < mTilesHigh; ++y) {
        for (int x = 0; x < mTilesWide; ++x) {
            const QRectF rect(x * mTileWidth + halfPenWidth, y * mTileHeight + halfPenWidth, mTileWidth, mTileHeight);
            painter->drawLine(QPointF(rect.x(), rect.y()), QPointF(rect.x(), rect.y() + rect.height()));

            if (x == mTilesWide - 1) {
                // If this is the right-most edge tile, draw a line on the outside of it.
                // Note that we have to subtract the width of the pen so that the line is
                // drawn within our bounds.
                painter->drawLine(QPointF(rect.x() + rect.width() - pen.width(), rect.y()),
                    QPointF(rect.x() + rect.width() - pen.width(), rect.y() + rect.height()));
            }

            painter->drawLine(QPointF(rect.x(), rect.y()), QPointF(rect.x() + rect.width(), rect.y()));

            if (y == mTilesHigh - 1) {
                // If this is the bottom-most edge tile, draw a line on the outside of it.
                painter->drawLine(QPointF(rect.x(), rect.y() + rect.height() - pen.width()),
                    QPointF(rect.x() + rect.width() - pen.width(), rect.y() + rect.height() - pen.width()));
            }
        }
    }

    if (mHighlightedIndex != -1) {
        const int x = mHighlightedIndex % mTilesWide;
        const int y = mHighlightedIndex / mTilesWide;
        const int widthReduction = x == mTilesWide - 1 ? pen.width() : 0;
        const int heightReduction = y == mTilesHigh - 1 ? pen.width() : 0;
        const QRectF rect(x * mTileWidth + halfPenWidth, y * mTileHeight + halfPenWidth,
            mTileWidth - widthReduction, mTileHeight - heightReduction);
        QPainterPath path;
        path.addRect(rect);

        QPen strokePen(pen);
        strokePen.setJoinStyle(Qt::MiterJoin);
        strokePen.setColor(mHighlightColour);
        painter->strokePath(path, strokePen);
    }
}

int TileGrid::tileWidth() const
{
    return mTileWidth;
}

void TileGrid::setTileWidth(int width)
{
    if (width == mTileWidth)
        return;

    mTileWidth = width;
    update();
    emit tileWidthChanged();
}

int TileGrid::tileHeight() const
{
    return mTileHeight;
}

void TileGrid::setTileHeight(int height)
{
    if (height == mTileHeight)
        return;

    mTileHeight = height;
    update();
    emit tileHeightChanged();
}

int TileGrid::tilesWide() const
{
    return mTilesWide;
}

void TileGrid::setTilesWide(int tilesWide)
{
    if (tilesWide == mTilesWide)
        return;

    mTilesWide = tilesWide;
    update();
    emit tilesWideChanged();
}

int TileGrid::tilesHigh() const
{
    return mTilesHigh;
}

void TileGrid::setTilesHigh(int tilesHigh)
{
    if (tilesHigh == mTilesHigh)
        return;

    mTilesHigh = tilesHigh;
    update();
    emit tilesHighChanged();
}

int TileGrid::highlightedIndex() const
{
    return mHighlightedIndex;
}

void TileGrid::setHighlightedIndex(int highlightedIndex)
{
    if (highlightedIndex == mHighlightedIndex)
        return;

    mHighlightedIndex = highlightedIndex;
    update();
    emit highlightedIndexChanged();
}

QColor TileGrid::colour() const
{
    return mColour;
}

void TileGrid::setColour(const QColor &colour)
{
    if (colour == mColour)
        return;

    mColour = colour;
    update();
    emit colourChanged();
}

QColor TileGrid::highlightColour() const
{
    return mHighlightColour;
}

void TileGrid::setHighlightColour(const QColor &highlightColour)
{
    if (highlightColour == mHighlightColour)
        return;

    mHighlightColour = highlightColour;
    update();
    emit highlightColourChanged();
}
