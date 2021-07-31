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

#include "ruler.h"

#include <QPainter>
#include <QtMath>

Ruler::Ruler(QQuickItem *parentItem) :
    QQuickPaintedItem(parentItem)
{
    // I'm not sure if it makes a visible difference, but I decided
    // not to call setRenderTarget(FramebufferObject) here in case it affects the
    // anti-aliasing of the text.
}

Qt::Orientation Ruler::orientation() const
{
    return mOrientation;
}

void Ruler::setOrientation(Qt::Orientation orientation)
{
    if (orientation == mOrientation)
        return;

    mOrientation = orientation;
    emit orientationChanged();
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
    emit zoomLevelChanged();
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
    if (isVisible()) {
        // The docs say that redraws only happen if the item is visible, but
        // commenting out this check results in the second horizontal being drawn...
        update();
    }
    emit fromChanged();
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
    const bool horizontal = mOrientation == Qt::Horizontal;

    painter->fillRect(0, 0, width(), height(), mBackgroundColour);

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
    font.setPixelSize(8);
    painter->setFont(font);
    painter->setPen(mForegroundColour);

    QFontMetrics fontMetrics(font);

    // Find the first number that will be visible on the ruler.
    int firstVisibleLvl1Number = 0;
    if (mFrom >= 0) {
        firstVisibleLvl1Number = -mFrom / mZoomLevel;
        const int closestLvl1 = firstVisibleLvl1Number % lvl1BaseSpacing;
        firstVisibleLvl1Number += qAbs(closestLvl1);
    } else {
        firstVisibleLvl1Number = -mFrom / mZoomLevel;
        const int closestLvl1 = qAbs(firstVisibleLvl1Number % lvl1BaseSpacing);
        firstVisibleLvl1Number += lvl1BaseSpacing - closestLvl1;
    }

    // Just to be safe, go back one more.
    firstVisibleLvl1Number -= lvl1BaseSpacing;

    if (horizontal) {
        const int textY = fontMetrics.ascent() + 3;

        // + lvl1BaseSpacing because we start outside of the left side of item, so we have
        // to make sure that there isn't a gap on the right-hand side.
        int tickmarkIndex = 0;

        // This is the x position of the first number we draw. The first number is never visible, draw
        // outside the visible bounds just be ensure there are never gaps with missing ticks/text.
        int startX = (firstVisibleLvl1Number + (qreal(mFrom) / mZoomLevel)) * mZoomLevel;

        int number = firstVisibleLvl1Number;

        // Starting at the first number, increment the number's x position by 10 pixels (20 at mZoomLevel == 2, and so on...)
        for (int x = startX; x < width() + lvl1BaseSpacing; x += lvl2Spacing, ++tickmarkIndex, number += lvl2BaseSpacing) {
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

        const int fontDigitWidth = fontMetrics.horizontalAdvance(QLatin1Char('9'));
        const int textX = 3;

        int tickmarkIndex = 0;

        // This is the x position of the first number we draw. The first number is never visible, draw
        // outside the visible bounds just be ensure there are never gaps with missing ticks/text.
        int startY = (firstVisibleLvl1Number + (qreal(mFrom) / mZoomLevel)) * mZoomLevel;

        int number = firstVisibleLvl1Number;

        for (int y = startY; y < height() + lvl1BaseSpacing; y += lvl2Spacing, ++tickmarkIndex, number += lvl2BaseSpacing) {
            if (tickmarkIndex % 5 == 0) {
                painter->fillRect(rulerThickness - lvl1Length, y, lvl1Length, tickThickness, mForegroundColour);

                // + 4 to go slightly past the tick.
                painter->drawText(textX, y + 4, fontDigitWidth, 100, Qt::TextWordWrap | Qt::TextWrapAnywhere, QString::number(qAbs(number)));
            } else {
                painter->fillRect(rulerThickness - lvl2Length, y, lvl2Length, tickThickness, mForegroundColour);

                if (mZoomLevel >= lvl4VisibleAtZoomLevel) {
                    painter->drawText(textX, y + 4, fontDigitWidth, 100, Qt::TextWordWrap | Qt::TextWrapAnywhere, QString::number(qAbs(number)));
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

    // Draw lines on edges over ticks.
    const QColor lineColour = mBackgroundColour.lighter(110);
//    painter->fillRect(0, 0, horizontal ? width() : 1, horizontal ? 1 : height(), lineColour);
    painter->fillRect(horizontal ? 0 : width() - 1, horizontal ? height() - 1 : 0, horizontal ? width() : 1, horizontal ? 1 : height(), lineColour);

    if (mDrawCorner) {
        painter->fillRect(0, 0, rulerThickness, rulerThickness, mBackgroundColour);
    }
}

bool Ruler::drawCorner() const
{
    return mDrawCorner;
}

void Ruler::setDrawCorner(bool drawCorner)
{
    if (drawCorner == mDrawCorner)
        return;

    mDrawCorner = drawCorner;
    update();
    emit drawCornerChanged();
}
