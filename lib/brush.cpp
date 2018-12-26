#include "brush.h"

#include <QPainter>
#include <QtMath>
#include <QSet>

Brush::Brush(const Brush::Type type, const QSize &size, const QPointF handle, const bool relativeHandle) :
    type(type), size(size), pixmap(), handle(relativeHandle ? QPointF(handle.x() * qreal(size.width()), handle.y() * qreal(size.height())) : handle)
{
    if (type == ImageType) {
        pixmap = QPixmap::fromImage(QImage(size, QImage::Format_ARGB32));
    } else {
        QImage image = QImage(size, QImage::Format_ARGB32);
        image.fill(qRgba(0, 0, 0, 0));
        const QRgb colour = qRgba(255, 255, 255, 255);
        if (type == SquareType) {
            fillRectangle(image, image.rect(), image.rect(), colour);
        }
        else {
            fillEllipse(image, image.rect(), image.rect(), colour);
        }
        pixmap = QBitmap::fromImage(image.createMaskFromColor(colour, Qt::MaskOutColor));
    }
}

Brush::Brush(const QImage &image, const QPointF handle, const bool relativeHandle) :
    Brush(ImageType, image.size(), handle, relativeHandle)
{
    pixmap = QPixmap::fromImage(image);
}

bool Brush::operator==(const Brush &other) const
{
    return type == other.type && size == other.size && pixmap.toImage() == other.pixmap.toImage() && handle == other.handle;
}

bool Brush::operator!=(const Brush &other) const
{
    return !(*this == other);
}

Brush &Brush::operator=(const Brush &other)
{
    type = other.type;
    size = other.size;
    pixmap = other.pixmap;
    handle = other.handle;
    return *this;
}

QTransform Brush::transform() const
{
    QTransform transform;
    transform.translate(-handle.x(), -handle.y());
    return transform;
}

QRectF Brush::bounds(const qreal scale) const
{
    return QRectF(-handle * scale, size * scale);
}

inline void Brush::drawPixel(QImage &image, const QRect &clip, const QPoint point, const QRgb colour)
{
    Q_ASSERT(image.format() == QImage::Format_ARGB32);

    const QRect clipped = clip & image.rect();
    if (clipped.contains(point))
        *(reinterpret_cast<QRgb *>(image.scanLine(point.y())) + point.x()) = colour;
}

inline void Brush::drawSpan(QImage &image, const QRect &clip, const int x0, const int x1, const int y, const QRgb colour)
{
    Q_ASSERT(image.format() == QImage::Format_ARGB32);

    const QRect clipped = clip & image.rect();
    if (y >= clipped.y() && y < clipped.y() + clipped.height()) {
        const int startX = qMax(clipped.x(), x0);
        const int endX = qMin(clipped.x() + clipped.width(), x1);
        QRgb *const startPixel = reinterpret_cast<QRgb *>(image.scanLine(y)) + startX;
        QRgb *const endPixel = reinterpret_cast<QRgb *>(image.scanLine(y)) + endX;
        for (QRgb *pixel = startPixel; pixel < endPixel; ++pixel)
            *pixel = colour;
    }
}

void Brush::fillRectangle(QImage &image, const QRect &clip, const QRectF &rect, const QRgb colour)
{
    const QRectF clipped = rect & clip & image.rect();
    for (int y = qFloor(clipped.y() + 0.5), end = qFloor(clipped.y() + clipped.height() + 0.5); y < end; ++y) {
        const int startX = qFloor(clipped.x() + 0.5);
        const int endX = qFloor(clipped.x() + clipped.width() + 0.5);
        drawSpan(image, clip, startX, endX, y, colour);
    }
}

void Brush::fillEllipse(QImage &image, const QRect &clip, const QRectF &rect, const QRgb colour)
{
    const QSizeF radius = rect.size() / 2.0;
    const QPointF origin = rect.topLeft() + QPointF(radius.width(), radius.height());
    const qreal ratio = radius.width() / radius.height();
    const qreal heightRadiusSquared = radius.height() * radius.height();
    const QRectF clipped = rect & clip & image.rect();
    for (int y = qFloor(clipped.y() + 0.5), end = qFloor(clipped.y() + clipped.height() + 0.5); y < end; ++y) {
        const qreal yOffset = y + 0.5 - origin.y();
        const qreal halfSpanWidth = sqrt(heightRadiusSquared - yOffset * yOffset) * ratio;
        const int startX = qMax(qFloor(clipped.x() + 0.5), qFloor(origin.x() - halfSpanWidth + 0.5));
        const int endX = qMin(qFloor(clipped.x() + clipped.width() + 0.5), qFloor(origin.x() + halfSpanWidth + 0.5));
        drawSpan(image, clip, startX, endX, y, colour);
    }
}

void Brush::draw(QPainter *const painter, const QColor &colour, const QPointF pos, const qreal scale, const qreal rotation) const
{
    QTransform brushTransform;
    brushTransform.rotate(rotation);
    brushTransform.scale(scale, scale);

    painter->save();
    // Transform to cursor position
    painter->translate(pos);
    painter->setTransform(brushTransform, true);
    // Draw brush image
    // Apply brush transfom
    painter->setTransform(transform(), true);
    // Draw brush image
    if (QSet<QImage::Format>{QImage::Format_Mono, QImage::Format_MonoLSB}.contains(pixmap.toImage().format())) {
        painter->setPen(colour);
        painter->setBackgroundMode(Qt::TransparentMode);
        painter->drawPixmap(QPointF(0.0, 0.0), pixmap);
    }
    else {
        painter->drawPixmap(QPointF(0.0, 0.0), pixmap);
    }
    painter->restore();
}
