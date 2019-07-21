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

#include "imagecanvas.h"

class AnimationPlayback;

namespace Utils {
    QImage paintImageOntoPortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage);

    QImage replacePortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage);

    QImage erasePortionOfImage(const QImage &image, const QRect &portion);

    QImage rotate(const QImage &image, int angle);
    QImage rotateAreaWithinImage(const QImage &image, const QRect &area, int angle, QRect &inRotatedArea);

    void modifyHsl(QImage &image, qreal hue, qreal saturation, qreal lightness, qreal alpha,
        ImageCanvas::AlphaAdjustmentFlags alphaAdjustmentFlags);

    void strokeRectWithDashes(QPainter *painter, const QRect &rect);

    QRect ensureWithinArea(const QRect &rect, const QSize &boundsSize);

    QVector<QColor> findUniqueColours(const QImage &image);
    QVarLengthArray<unsigned int> findMax256UniqueArgbColours(const QImage &image);

    // relativeFrameIndex is the index of the animation relative to animation.startIndex()
    QImage imageForAnimationFrame(const QImage &sourceImage, const AnimationPlayback &animation, int relativeFrameIndex);
    bool exportGif(const QImage &gifSourceImage, const QUrl &url, const AnimationPlayback &animation, QString &errorMessage);

    template<typename T>
    QString enumToString(T enumValue)
    {
        QString string;
        QDebug debug(&string);
        debug << enumValue;
        return string;
    }

    template<typename T>
    inline T divFloor(const T dividend, const T divisor) {
        T quotient = dividend / divisor;
        const T remainder = dividend % divisor;
        if ((remainder != 0) && ((remainder < 0) != (divisor < 0))) --quotient;
        return quotient;
    }
    template<typename T>
    inline T divCeil(const T dividend, const T divisor) {
        return divFloor(dividend + (divisor - 1), divisor);
    }
    template<typename T>
    inline T modFloor(const T dividend, const T divisor) {
        T remainder = dividend % divisor;
        if ((remainder != 0) && ((remainder < 0) != (divisor < 0))) remainder += divisor;
        return remainder;
    }
    template<typename T>
    inline T modCeil(const T dividend, const T divisor) {
        return modFloor(dividend + (divisor - 1), divisor);
    }

    // Based on https://stackoverflow.com/a/28413370/904422.
    class ScopeGuard {
    public:
        template<class Callable>
        ScopeGuard(Callable && restoreFunc) : restoreFunc(std::forward<Callable>(restoreFunc)) {}

        ~ScopeGuard() {
            if(restoreFunc)
                restoreFunc();
        }

        ScopeGuard(const ScopeGuard&) = delete;
        void operator=(const ScopeGuard&) = delete;

    private:
        std::function<void()> restoreFunc;
    };
}

#endif // UTILS_H
