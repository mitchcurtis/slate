#include "stroke.h"

qreal Stroke::strokeSegment(QPainter *const painter, const Brush &brush, const QColor &colour, const StrokePoint &point0, const StrokePoint &point1, const qreal scaleMin, const qreal scaleMax, const qreal stepOffset)
{
    const QPointF posDelta = {point1.pos.x() - point0.pos.x(), point1.pos.y() - point0.pos.y()};
    const qreal pressureDelta = point1.pressure - point0.pressure;
    const qreal steps = qMax(qMax(qAbs(posDelta.x()), qAbs(posDelta.y())), 1.0);
    const qreal step = 1.0 / steps;
    qreal pos = stepOffset * step;
    while (pos < 1.0 || qFuzzyCompare(pos, 1.0)) {
        const QPointF point = point0.pos + pos * posDelta;
        const qreal pressure = point0.pressure + pos * pressureDelta;
        const qreal scale = scaleMin + pressure * (scaleMax - scaleMin);
        brush.draw(painter, colour, point, scale);
        pos += step;
    }
    return (pos - 1.0) * steps;
}

void Stroke::draw(QPainter *const painter, const Brush &brush, const qreal scaleMin, const qreal scaleMax, const QColor &colour, const QPainter::CompositionMode mode, const bool snapToPixel)
{
    painter->save();
    painter->setCompositionMode(mode);
    if (length() == 1) {
        strokeSegment(painter, brush, colour, snapped(0, brush.handle, snapToPixel), snapped(0, brush.handle, snapToPixel), scaleMin, scaleMax);
    }
    else {
        qreal stepOffset = 0.0;
        for (int i = 1; i < length(); ++i) {
            stepOffset = strokeSegment(painter, brush, colour, snapped(i - 1, brush.handle, snapToPixel), snapped(i, brush.handle, snapToPixel), scaleMin, scaleMax, stepOffset);
        }
    }
    painter->restore();
}
