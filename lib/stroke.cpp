#include "stroke.h"

bool StrokePoint::operator==(const StrokePoint &other) const
{
    return pos == other.pos && qFuzzyCompare(pressure, other.pressure);
}

bool StrokePoint::operator!=(const StrokePoint &other) const
{
    return !(*this == other);
}

StrokePoint StrokePoint::snapped(const QPointF snapOffset) const
{
    return StrokePoint{{qRound(pos.x() + snapOffset.x()) - snapOffset.x(), qRound(pos.y() + snapOffset.y()) - snapOffset.y()}, pressure};
}

QPoint StrokePoint::pixel() const
{
    return QPoint{qFloor(pos.x()), qFloor(pos.y())};
}

qreal Stroke::strokeSegment(QPainter *const painter, const Brush &brush, const QColor &colour, const StrokePoint &point0, const StrokePoint &point1, const qreal scaleMin, const qreal scaleMax, const qreal stepOffset, const bool stepOffsetOnly)
{
    const QPointF posDelta = {point1.pos.x() - point0.pos.x(), point1.pos.y() - point0.pos.y()};
    const qreal pressureDelta = point1.pressure - point0.pressure;
    const qreal steps = qMax(qMax(qAbs(posDelta.x()), qAbs(posDelta.y())), 1.0);
    const qreal step = 1.0 / steps;
    qreal pos = stepOffset * step;
    while (pos < 1.0 || qFuzzyCompare(pos, 1.0)) {
        const QPointF point = point0.pos + pos * posDelta;
        if (!stepOffsetOnly) {
            const qreal pressure = point0.pressure + pos * pressureDelta;
            const qreal scale = scaleMin + pressure * (scaleMax - scaleMin);
            brush.draw(painter, colour, point, scale);
        }
        pos += step;
    }
    return (pos - 1.0) * steps;
}

StrokePoint Stroke::snapped(const int index, const QPointF snapOffset, const bool snapToPixel)
{
    if (!snapToPixel) return at(index);
    else return at(index).snapped(snapOffset);
}

void Stroke::draw(QPainter *const painter, const Brush &brush, const qreal scaleMin, const qreal scaleMax, const QColor &colour, const QPainter::CompositionMode mode, const bool snapToPixel)
{
    painter->save();
    painter->setCompositionMode(mode);
    if (length() == 1) {
        strokeSegment(painter, brush, colour, snapped(0, brush.handle, snapToPixel), snapped(0, brush.handle, snapToPixel), scaleMin, scaleMax);
    } else {
        qreal stepOffset = 0.0;
        for (int i = 1; i < length(); ++i) {
            const QRect clipRect = painter->clipBoundingRect().toAlignedRect();
            const QRect segmentBounds = Stroke{at(i - 1), at(i)}.bounds(brush, scaleMin, scaleMax);
            const bool drawSegment = clipRect.isValid() && clipRect.intersects(segmentBounds);
            stepOffset = strokeSegment(painter, brush, colour, snapped(i - 1, brush.handle, snapToPixel), snapped(i, brush.handle, snapToPixel), scaleMin, scaleMax, stepOffset, !drawSegment);
        }
    }
    painter->restore();
}

QRect Stroke::bounds(const Brush &brush, const qreal scaleMin, const qreal scaleMax)
{
    QRectF bounds;
    for (auto point : *this) {
        bounds = bounds.united(brush.bounds(point.pos, scaleMin + point.pressure * (scaleMax - scaleMin)));
    }
    return bounds.toAlignedRect();
}
