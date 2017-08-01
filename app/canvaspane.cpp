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

#include "canvaspane.h"

#include <QJsonObject>

CanvasPane::CanvasPane(QObject *parent) :
    QObject(parent),
    mSize(0.5),
    mZoomLevel(1),
    mMaxZoomLevel(30),
    mSceneCentered(true)
{
}

qreal CanvasPane::size() const
{
    return mSize;
}

void CanvasPane::setSize(const qreal &size)
{
    if (size == mSize) {
        return;
    }

    mSize = size;
    emit sizeChanged();
}

int CanvasPane::zoomLevel() const
{
    return mZoomLevel;
}

void CanvasPane::setZoomLevel(int zoomLevel)
{
    const int adjustedLevel = qBound(1, zoomLevel, mMaxZoomLevel);
    if (adjustedLevel == mZoomLevel)
        return;

    mZoomLevel = adjustedLevel;
    emit zoomLevelChanged();
}

int CanvasPane::maxZoomLevel() const
{
    return mMaxZoomLevel;
}

QSize CanvasPane::zoomedSize(const QSize &size) const
{
    return size * mZoomLevel;
}

QPoint CanvasPane::offset() const
{
    return mOffset;
}

void CanvasPane::setOffset(const QPoint &offset)
{
    if (offset == mOffset)
        return;

    mOffset = offset;
    emit offsetChanged();
}

QPoint CanvasPane::zoomedOffset() const
{
    return mOffset * zoomLevel();
}

bool CanvasPane::isSceneCentered() const
{
    return mSceneCentered;
}

void CanvasPane::setSceneCentered(bool centreScene)
{
    mSceneCentered = centreScene;
}

void CanvasPane::read(const QJsonObject &json)
{
    setSize(json.value(QLatin1String("size")).toDouble());
    setZoomLevel(json.value(QLatin1String("zoomLevel")).toInt());
    setOffset(QPoint(json.value(QLatin1String("offsetX")).toInt(), json.value(QLatin1String("offsetY")).toInt()));
    setSceneCentered(json.value(QLatin1String("sceneCentered")).toBool());
}

void CanvasPane::write(QJsonObject &json) const
{
    json[QLatin1String("size")] = mSize;
    json[QLatin1String("zoomLevel")] = mZoomLevel;
    json[QLatin1String("offsetX")] = mOffset.x();
    json[QLatin1String("offsetY")] = mOffset.y();
    json[QLatin1String("sceneCentered")] = mSceneCentered;
}

void CanvasPane::reset()
{
    setSize(0.5);
    setZoomLevel(1);
    setOffset(QPoint(0, 0));
    setSceneCentered(true);
}
