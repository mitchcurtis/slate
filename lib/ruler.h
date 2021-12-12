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

#ifndef RULER_H
#define RULER_H

#include <QQuickPaintedItem>

#include "slate-global.h"

class SLATE_EXPORT Ruler : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(int from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QColor foregroundColour READ foregroundColour WRITE setForegroundColour NOTIFY foregroundColourChanged)
    Q_PROPERTY(QColor backgroundColour READ backgroundColour WRITE setBackgroundColour NOTIFY backgroundColourChanged)
    Q_PROPERTY(bool drawCorner READ drawCorner WRITE setDrawCorner NOTIFY drawCornerChanged)
    QML_ELEMENT

public:
    Ruler(QQuickItem *parentItem = nullptr);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    int zoomLevel() const;
    void setZoomLevel(int zoomLevel);

    int from() const;
    void setFrom(int from);

    QColor foregroundColour() const;
    void setForegroundColour(const QColor &foregroundColour);

    QColor backgroundColour() const;
    void setBackgroundColour(const QColor &backgroundColour);

    void paint(QPainter *painter) override;

    bool drawCorner() const;
    void setDrawCorner(bool drawCorner);

signals:
    void orientationChanged();
    void zoomLevelChanged();
    void fromChanged();
    void foregroundColourChanged();
    void backgroundColourChanged();
    void drawCornerChanged();

private:
    Qt::Orientation mOrientation = Qt::Horizontal;
    int mZoomLevel = 1;
    int mFrom = 0;
    QColor mForegroundColour = QColor(170, 170, 170, 255);
    QColor mBackgroundColour = QColor(70, 70, 70, 255);
    bool mDrawCorner = false;
};

#endif // RULER_H
