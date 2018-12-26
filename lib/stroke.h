#ifndef STROKE_H
#define STROKE_H

#include <QPointF>
#include <QVector>
#include <QPainter>
#include <QtMath>

#include "brush.h"

struct StrokePoint {
    bool operator==(const StrokePoint &other) const {
        return pos == other.pos && qFuzzyCompare(pressure, other.pressure);
    }
    bool operator!=(const StrokePoint &other) const {
        return !(*this == other);
    }

    StrokePoint snapped(const QPointF snapOffset = {0.0, 0.0}) const {
        return StrokePoint{{qRound(pos.x() + snapOffset.x()) - snapOffset.x(), qRound(pos.y() + snapOffset.y()) - snapOffset.y()}, pressure};
    }

    QPoint pixel() const {
        return QPoint{qFloor(pos.x()), qFloor(pos.y())};
    }

    QPointF pos;
    qreal pressure;
};

inline QDebug operator<<(QDebug debug, const StrokePoint &point)
{
    debug.nospace() << "StrokePoint(" << point.pos << ", " << point.pressure << ")";
    return debug.space();
}

class Stroke : public QVector<StrokePoint> {
public:
    using QVector::QVector;
    Stroke(const QVector<StrokePoint> &vector) : QVector<StrokePoint>(vector) {}

    static qreal strokeSegment(QPainter *const painter, const Brush &brush, const QColor &colour, const StrokePoint &point0, const StrokePoint &point1, const qreal scaleMin, const qreal scaleMax, const qreal stepOffset = 0.0);

    StrokePoint snapped(const int index, const QPointF snapOffset = {0.0, 0.0}, const bool snapToPixel = true) {
        if (!snapToPixel) return at(index);
        else return at(index).snapped(snapOffset);
    }

    void draw(QPainter *const painter, const Brush &brush, const qreal scaleMin, const qreal scaleMax, const QColor &colour, const QPainter::CompositionMode mode, const bool snapToPixel = false);

    QRect bounds(const Brush &brush, const qreal scaleMin, const qreal scaleMax) {
        QRectF bounds;
        for (auto point : *this) {
            bounds = bounds.united(brush.bounds(scaleMin + point.pressure * (scaleMax - scaleMin)).translated(point.pos));
        }
        return bounds.toAlignedRect();
    }
};

#endif // STROKE_H
