/*
    Copyright 2018, Mitch Curtis

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

#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <QImage>
#include <QRect>

namespace Utils {
    QImage paintImageOntoPortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage);

    QImage replacePortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage);

    QImage erasePortionOfImage(const QImage &image, const QRect &portion);

    QImage rotate(const QImage &image, int angle);
    QImage rotateAreaWithinImage(const QImage &image, const QRect &area, int angle, QRect &inRotatedArea);

    void modifyHsl(QImage &image, qreal hue, qreal saturation, qreal lightness);

    void strokeRectWithDashes(QPainter *painter, const QRect &rect);

    QRect ensureWithinArea(const QRect &rect, const QSize &boundsSize);

    template<typename T>
    QString enumToString(T enumValue)
    {
        QString string;
        QDebug debug(&string);
        debug << enumValue;
        return string;
    }
}

#endif // UTILS_H
