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

#ifndef CANVASPANE_H
#define CANVASPANE_H

#include <QObject>
#include <QPoint>
#include <QSize>

class CanvasPane : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(int maxZoomLevel READ maxZoomLevel CONSTANT)
    Q_PROPERTY(QPoint offset READ offset WRITE setOffset NOTIFY offsetChanged)

public:
    explicit CanvasPane(QObject *parent = 0);

    qreal size() const;
    void setSize(const qreal &size);

    int zoomLevel() const;
    void setZoomLevel(int zoomLevel);
    int maxZoomLevel() const;

    QSize zoomedSize(const QSize &size) const;

    QPoint offset() const;
    void setOffset(const QPoint &offset);

    QPoint zoomedOffset() const;

    bool isSceneCentered() const;
    void setSceneCentered(bool isSceneCentered);

    void reset();

signals:
    void zoomLevelChanged();
    void sizeChanged();
    void offsetChanged();

private:
    qreal mSize;
    int mZoomLevel;
    int mMaxZoomLevel;
    // From the top left of the canvas.
    QPoint mOffset;
    // Scenes are centered in each pane until the view is panned or zoomed.
    bool mSceneCentered;

};

#endif // CANVASPANE_H
