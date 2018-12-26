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
    Brush(const QImage &image, const QPointF handle = {0.5, 0.5}, const bool relativeHandle = true);

    bool operator==(const Brush &other) const;
    bool operator!=(const Brush &other) const;
    Brush &operator=(const Brush &other);

    QTransform transform() const;

    QRectF bounds(const qreal scale) const;

    static void drawPixel(QImage &image, const QRect &clip, const QPoint point, const QRgb colour);
    static void drawSpan(QImage &image, const QRect &clip, const int x0, const int x1, const int y, const QRgb colour);
    static void fillRectangle(QImage &image, const QRect &clip, const QRectF &rect, const QRgb colour);
    static void fillEllipse(QImage &image, const QRect &clip, const QRectF &rect, const QRgb colour);

    void draw(QPainter *const painter, const QColor &colour, const QPointF pos = {0.0, 0.0}, const qreal scale = 1.0, const qreal rotation = 0.0) const;

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
