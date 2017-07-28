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

    const bool horizontal = mOrientation == Qt::Horizontal;
    const qreal rulerThickness = horizontal ? height() : width();
    const int tickThickness = 1;

    // Largest tickmarks; always visible.
    const int lvl1BaseSpacing = 50;
    const int lvl1Spacing = lvl1BaseSpacing * mZoomLevel;
    const int lvl1Length = rulerThickness;

    // Second-largest tickmarks; also always visible.
    const int lvl2BaseSpacing = lvl1BaseSpacing / 5;
    const int lvl2Spacing = lvl1Spacing / 5;
    const int lvl2Length = rulerThickness * 0.3;

    // Third-largest tickmarks; only visible at zoom levels >= 2.
    const int lvl3Spacing = lvl2Spacing / 5;
    const int lvl3Length = rulerThickness * 0.2;
    const int lvl3VisibleAtZoomLevel = 2;

    // Fourth-largest tickmarks; only visible at zoom levels >= 5.
    // These mark individual pixels.
    const int lvl4Length = qMax(1.0, rulerThickness * 0.1);
    const int lvl4VisibleAtZoomLevel = 5;

    QFont font(painter->font());
    font.setPixelSize(10);
    painter->setFont(font);
    painter->setPen(mForegroundColour);

    QFontMetrics fontMetrics(font);
//    const int maxTextWidth = fontMetrics.width(QString::number(width()));

    // - lvl1BaseSpacing to ensure we render a bit outside of ourselves,
    // rather than leaving some gaps where stuff is missing.
    const int diffToLvl1 = (mFrom % lvl1BaseSpacing) - lvl1BaseSpacing;
    const int safeLvl1Value = -(mFrom - diffToLvl1);

    if (horizontal) {
        const int textY = fontMetrics.ascent() + 1;

        // + lvl1BaseSpacing because we start outside of the left side of item, so we have
        // to make sure that there isn't a gap on the right-hand side.
        int number = safeLvl1Value;
        int tickmarkIndex = 0;
        // No idea why this zoom level crap is necessary... but it works! :D
        const int startXValue = diffToLvl1 * mZoomLevel - (mZoomLevel > 1 ? mFrom * (mZoomLevel > 2 ? mZoomLevel - 1 : 1) : 0);
        for (int x = startXValue; x < width() + lvl1BaseSpacing; x += lvl2Spacing, ++tickmarkIndex, number += lvl2BaseSpacing) {
            if (tickmarkIndex % 5 == 0) {
                painter->fillRect(x, rulerThickness - lvl1Length, tickThickness, lvl1Length, mForegroundColour);

                // + 4 to go slightly past the tick.
                painter->drawText(x + 4, textY, QString::number(qAbs(number)));
            } else {
                painter->fillRect(x, rulerThickness - lvl2Length, tickThickness, lvl2Length, mForegroundColour);

                if (mZoomLevel >= lvl4VisibleAtZoomLevel) {
                    painter->drawText(x + 4, textY, QString::number(qAbs(number)));
                }
            }

            if (mZoomLevel >= lvl3VisibleAtZoomLevel) {
                for (int lvl3Index = 0; lvl3Index < 5; ++lvl3Index) {
                    // We don't draw at the first index, as the level 2 tickmark has that position,
                    // but including it in the loop simplifies the code for the level 4 tickmarks.
                    if (lvl3Index > 0) {
                        painter->fillRect(x + (lvl3Index * lvl3Spacing), rulerThickness - lvl3Length, tickThickness, lvl3Length, mForegroundColour);
                    }

                    if (mZoomLevel >= lvl4VisibleAtZoomLevel) {
                        painter->fillRect(x + (lvl3Index * lvl3Spacing) + lvl3Spacing / 2, rulerThickness - lvl4Length, tickThickness, lvl4Length, mForegroundColour);
                    }
                }
            }
        }
    } else {
        // TODO: use QTextLayout for more compact vertical text

        const int fontDigitWidth = fontMetrics.width(QLatin1Char('9'));
        // + lvl1BaseSpacing because we start outside of the left side of item, so we have
        // to make sure that there isn't a gap on the right-hand side.
        int number = safeLvl1Value;
        int tickmarkIndex = 0;
        // No idea why this zoom level crap is necessary... but it works! :D
        const int startYValue = diffToLvl1 * mZoomLevel - (mZoomLevel > 1 ? mFrom * (mZoomLevel > 2 ? mZoomLevel - 1 : 1) : 0);

        for (int y = startYValue; y < height() + lvl1BaseSpacing; y += lvl2Spacing, ++tickmarkIndex, number += lvl2BaseSpacing) {
            if (tickmarkIndex % 5 == 0) {
                painter->fillRect(rulerThickness - lvl1Length, y, lvl1Length, tickThickness, mForegroundColour);

                // + 4 to go slightly past the tick.
                painter->drawText(2, y + 4, fontDigitWidth, 100, Qt::TextWordWrap | Qt::TextWrapAnywhere, QString::number(qAbs(number)));
            } else {
                painter->fillRect(rulerThickness - lvl2Length, y, lvl2Length, tickThickness, mForegroundColour);

                if (mZoomLevel >= lvl4VisibleAtZoomLevel) {
                    painter->drawText(2, y + 4, fontDigitWidth, 100, Qt::TextWordWrap | Qt::TextWrapAnywhere, QString::number(number));
                }
            }

            if (mZoomLevel >= lvl3VisibleAtZoomLevel) {
                for (int lvl3Index = 0; lvl3Index < 5; ++lvl3Index) {
                    // We don't draw at the first index, as the level 2 tickmark has that position,
                    // but including it in the loop simplifies the code for the level 4 tickmarks.
                    if (lvl3Index > 0) {
                        painter->fillRect(rulerThickness - lvl3Length, y + (lvl3Index * lvl3Spacing), lvl3Length, tickThickness, mForegroundColour);
                    }

                    if (mZoomLevel >= lvl4VisibleAtZoomLevel) {
                        painter->fillRect(rulerThickness - lvl4Length, y + (lvl3Index * lvl3Spacing), lvl4Length, tickThickness, mForegroundColour);
                    }
                }
            }
        }
    }
}
