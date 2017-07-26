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

Ruler::Ruler(Qt::Orientation orientation, QQuickItem *parentItem) :
    QQuickPaintedItem(parentItem),
    mOrientation(orientation),
    mZoomLevel(1),
    mFrom(0),
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

int Ruler::from() const
{
    return mFrom;
}

void Ruler::setFrom(int from)
{
    if (from == mFrom)
        return;

    mFrom = from;
    update();
//    emit fromChanged();
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

    const qreal rulerHeight = height();
    const int thickness = 1;

    // Largest tickmarks; always visible.
    const int lvl1BaseSpacing = 50;
    const int lvl1Spacing = lvl1BaseSpacing * mZoomLevel;
    const int lvl1Length = rulerHeight;

    // Second-largest tickmarks; also always visible.
    const int lvl2Spacing = lvl1Spacing / 5;
    const int lvl2Length = rulerHeight * 0.4;

    // Third-largest tickmarks; only visible at zoom levels >= 2.
    const int lvl3Spacing = lvl2Spacing / 5;
    const int lvl3Length = rulerHeight * 0.2;
    const int lvl3VisibleAtZoomLevel = 2;

    // Fourth-largest tickmarks; only visible at zoom levels >= 5.
    // These mark individual pixels.
    const int lvl4Length = qMax(1.0, rulerHeight * 0.1);
    const int lvl4VisibleAtZoomLevel = 5;

    QFont font(painter->font());
    font.setPixelSize(10);
    painter->setFont(font);
    painter->setPen(mForegroundColour);

    QFontMetrics fontMetrics(font);
//    const int maxTextWidth = fontMetrics.width(QString::number(width()));

    // Translate to account for the corner; we don't want to draw there.
    if (mOrientation == Qt::Horizontal) {
//        painter->translate(rulerHeight, 0);

//        qDebug() << mZoomLevel << lvl2Spacing;

        // - lvl1BaseSpacing to ensure we render a bit outside of ourselves,
        // rather than leaving some gaps where stuff is missing.
        const int diffToLvl1 = (mFrom % lvl1BaseSpacing) - lvl1BaseSpacing;
        const int safeLvl1Value = -(mFrom - diffToLvl1) / mZoomLevel;
        qDebug() << "-mFrom" << -mFrom << "diffToLvl1" << diffToLvl1 << "safeLvl1Value" << safeLvl1Value;

        painter->translate(diffToLvl1, 0);

        // + lvl1BaseSpacing because we translate to the left above, so we have
        // to make sure that there isn't a gap on the right-hand side.
        for (int x = 0, i = 0; x < width() + lvl1BaseSpacing; x += lvl2Spacing, ++i) {
            if (i % 5 == 0) {
                painter->fillRect(x, rulerHeight - lvl1Length, thickness, lvl1Length, mForegroundColour);

                // + 4 to go slightly past the tick.
                painter->drawText(x + 4, fontMetrics.ascent(), QString::number((safeLvl1Value + i * lvl2Spacing) / mZoomLevel));
            } else {
                painter->fillRect(x, rulerHeight - lvl2Length, thickness, lvl2Length, mForegroundColour);

                if (mZoomLevel >= lvl4VisibleAtZoomLevel) {
                    painter->drawText(x + 4, fontMetrics.ascent(), QString::number((i * lvl2Spacing) / mZoomLevel));
                }
            }

            if (mZoomLevel >= lvl3VisibleAtZoomLevel) {
                for (int lvl3Index = 0; lvl3Index < 5; ++lvl3Index) {
                    // We don't draw at the first index, as the level 2 tickmark has that position,
                    // but including it in the loop simplifies the code for the level 4 tickmarks.
                    if (lvl3Index > 0) {
                        painter->fillRect(x + (lvl3Index * lvl3Spacing), rulerHeight - lvl3Length, thickness, lvl3Length, mForegroundColour);
                    }

                    if (mZoomLevel >= lvl4VisibleAtZoomLevel) {
                        painter->fillRect(x + (lvl3Index * lvl3Spacing) + lvl3Spacing / 2, rulerHeight - lvl4Length, thickness, lvl4Length, mForegroundColour);
                    }
                }
            }
        }
    } else {
        painter->translate(0, width());
    }
}
