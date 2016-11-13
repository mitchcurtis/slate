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

#include "tilecanvaspane.h"

TileCanvasPane::TileCanvasPane(QObject *parent) :
    QObject(parent),
    mSize(0.5),
    mZoomLevel(1),
    mMaxZoomLevel(30),
    mSceneCentered(true)
{
}

qreal TileCanvasPane::size() const
{
    return mSize;
}

void TileCanvasPane::setSize(const qreal &size)
{
    if (size == mSize) {
        return;
    }

    mSize = size;
    emit sizeChanged();
}

int TileCanvasPane::zoomLevel() const
{
    return mZoomLevel;
}

void TileCanvasPane::setZoomLevel(int zoomLevel)
{
    const int adjustedLevel = qBound(1, zoomLevel, mMaxZoomLevel);
    if (adjustedLevel == mZoomLevel)
        return;

    mZoomLevel = adjustedLevel;
    emit zoomLevelChanged();
}

int TileCanvasPane::maxZoomLevel() const
{
    return mMaxZoomLevel;
}

QSize TileCanvasPane::zoomedSize(const QSize &size) const
{
    return size * mZoomLevel;
}

QPoint TileCanvasPane::offset() const
{
    return mOffset;
}

void TileCanvasPane::setOffset(const QPoint &offset)
{
    if (offset == mOffset)
        return;

    mOffset = offset;
    emit offsetChanged();
}

QPoint TileCanvasPane::zoomedOffset() const
{
    return mOffset * zoomLevel();
}

bool TileCanvasPane::isSceneCentered() const
{
    return mSceneCentered;
}

void TileCanvasPane::setSceneCentered(bool centreScene)
{
    mSceneCentered = centreScene;
}

void TileCanvasPane::reset()
{
    setSize(0.5);
    setZoomLevel(1);
    setOffset(QPoint(0, 0));
    setSceneCentered(true);
}
