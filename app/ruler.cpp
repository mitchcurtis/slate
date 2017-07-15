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

#include "ruler.h"

#include <QPainter>
#include <QtMath>
#include <QQuickWindow>

Ruler::Ruler(Qt::Orientation orientation, QQuickItem *parentItem) :
    QQuickPaintedItem(parentItem),
    mOrientation(orientation),
    mZoomLevel(1),
    mForegroundColour(QColor(170, 170, 170, 255)),
    mBackgroundColour(QColor(70, 70, 70, 255))
{
}

int Ruler::zoomLevel() const
{
    return mZoomLevel;
}

void Ruler::setZoomLevel(int zoomLevel)
{
    if (zoomLevel == mZoomLevel)
        return;

    mZoomLevel = zoomLevel;
    update();
    //    emit zoomLevelChanged();
}

QColor Ruler::foregroundColour() const
{
    return mForegroundColour;
}

void Ruler::setForegroundColour(const QColor &foregroundColour)
{
    if (foregroundColour == mForegroundColour)
        return;

    mForegroundColour = foregroundColour;
    update();
    emit foregroundColourChanged();
}

QColor Ruler::backgroundColour() const
{
    return mBackgroundColour;
}

void Ruler::setBackgroundColour(const QColor &backgroundColour)
{
    if (backgroundColour == mBackgroundColour)
        return;

    mBackgroundColour = backgroundColour;
    update();
    emit backgroundColourChanged();
}

void Ruler::paint(QPainter *painter)
{
    painter->fillRect(0, 0, width(), height(), mBackgroundColour);

    static const int minorSpacing = 8;
    static const int minorLength = 4;
    static const int minorThickness = 1;

    static const int majorLength = 8;
    static const int majorThickness = 1;

    painter->setPen(mForegroundColour);

    // Translate to account for the corner; we don't want to draw there.
    if (mOrientation == Qt::Horizontal) {
        const qreal rulerHeight = height();
        painter->translate(rulerHeight, 0);

        for (int x = 0, i = 0; x < width(); x += minorSpacing, ++i) {
            if (i % 5 == 0) {
                painter->fillRect(x, rulerHeight - majorLength, majorThickness, majorLength, mForegroundColour);
                painter->drawText(x, height() - majorLength, QString::number(qRound(x / window()->devicePixelRatio())));
            } else {
                painter->fillRect(x, rulerHeight - minorLength, minorThickness, minorLength, mForegroundColour);
            }
        }
    } else {
        painter->translate(0, width());
    }
}
