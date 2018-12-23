#ifndef BRUSH_H
#define BRUSH_H

#include <QObject>
#include <QBitmap>
#include <QPixmap>
#include <QTransform>
#include <QDebug>

#include "slate-global.h"

class QPainter;

class SLATE_EXPORT Brush {
    Q_GADGET

public:
    enum Type {
        SquareType,
        CircleType,
        ImageType,
    };
    Q_ENUM(Type)

    Brush() : type(), size(), pixmap(), handle() {}
    Brush(const Brush &other) : type(other.type), size(other.size), pixmap(other.pixmap), handle(other.handle) {}
    Brush(const Type type, const QSize &size, const QPointF handle = {0.5, 0.5}, const bool relativeHandle = true);
    Brush(const QImage &image, const QPointF handle = {0.5, 0.5}, const bool relativeHandle = true) :
        Brush(ImageType, image.size(), handle, relativeHandle)
    {
        pixmap = QPixmap::fromImage(image);
    }

    bool operator==(const Brush &other) const {
        return type == other.type && size == other.size && pixmap.toImage() == other.pixmap.toImage() && handle == other.handle;
    }
    bool operator!=(const Brush &other) const {
        return !(*this == other);
    }
    Brush &operator=(const Brush &other) {
        type = other.type;
        size = other.size;
        pixmap = other.pixmap;
        handle = other.handle;
        return *this;
    }

    QTransform transform() const {
        QTransform transform;
        transform.translate(-handle.x(), -handle.y());
        return transform;
    }

    QRectF bounds(const qreal scale)
    {
        return QRectF(-handle * scale, size * scale);
    }

    static void drawPixel(QImage &image, const QRect &clip, const QPoint point, const QRgb colour);
    static void drawSpan(QImage &image, const QRect &clip, const int x0, const int x1, const int y, const QRgb colour);
    static void fillRectangle(QImage &image, const QRect &clip, const QRectF &rect, const QRgb colour);
    static void fillEllipse(QImage &image, const QRect &clip, const QRectF &rect, const QRgb colour);

    void draw(QPainter *const painter, const QPointF &point, const QColor &colour) const;

    Type type;
    QSize size;
    QPixmap pixmap;
    QPointF handle;
};

inline QDebug operator<<(QDebug debug, const Brush &brush)
{
    debug.nospace() << "Brush(" << brush.type << ", " << brush.size << ", " << brush.pixmap << ", " << brush.handle << ")";
    return debug.space();
}

#endif // BRUSH_H
