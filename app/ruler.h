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

#ifndef RULER_H
#define RULER_H

#include <QQuickPaintedItem>

class Ruler : public QQuickPaintedItem
{
    Q_OBJECT
//    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(QColor foregroundColour READ foregroundColour WRITE setForegroundColour NOTIFY foregroundColourChanged)
    Q_PROPERTY(QColor backgroundColour READ backgroundColour WRITE setBackgroundColour NOTIFY backgroundColourChanged)

public:
    Ruler(Qt::Orientation orientation, QQuickItem *parentItem);

    int zoomLevel() const;
    void setZoomLevel(int zoomLevel);

    QColor foregroundColour() const;
    void setForegroundColour(const QColor &foregroundColour);

    QColor backgroundColour() const;
    void setBackgroundColour(const QColor &backgroundColour);

    void paint(QPainter *painter) override;

signals:
//    void zoomLevelChanged();
    void foregroundColourChanged();
    void backgroundColourChanged();

private:
    Qt::Orientation mOrientation;
    int mZoomLevel;
    QColor mForegroundColour;
    QColor mBackgroundColour;
};

#endif // RULER_H
