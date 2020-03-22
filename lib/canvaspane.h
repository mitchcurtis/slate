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

#ifndef CANVASPANE_H
#define CANVASPANE_H

#include <QObject>
#include <QPoint>
#include <QSize>

#include "slate-global.h"

class QJsonObject;

class SLATE_EXPORT CanvasPane : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(int integerZoomLevel READ integerZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(int maxZoomLevel READ maxZoomLevel CONSTANT)
    Q_PROPERTY(QPoint integerOffset READ integerOffset WRITE setIntegerOffset NOTIFY integerOffsetChanged)

public:
    explicit CanvasPane(QObject *parent = nullptr);

    qreal size() const;
    void setSize(const qreal &size);

    qreal zoomLevel() const;
    int integerZoomLevel() const;
    void setZoomLevel(qreal zoomLevel);
    int maxZoomLevel() const;

    QSize zoomedSize(const QSize &size) const;

    QPoint integerOffset() const;
    void setIntegerOffset(const QPoint &integerOffset);
    QPointF offset() const;
    void setOffset(const QPointF &offset);

    QPoint zoomedOffset() const;

    bool isSceneCentered() const;
    void setSceneCentered(bool sceneCentered);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void reset();

signals:
    void zoomLevelChanged();
    void sizeChanged();
    void integerOffsetChanged();

private:
    qreal mSize;
    qreal mZoomLevel;
    int mMaxZoomLevel;
    // From the top left of the canvas.
    QPointF mOffset;
    // Scenes are centered in each pane until the view is panned or zoomed.
    bool mSceneCentered;
};

SLATE_EXPORT QDebug operator<<(QDebug debug, const CanvasPane *pane);

#endif // CANVASPANE_H
