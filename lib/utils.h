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

    SLATE_EXPORT QImage moveContents(const QImage &image, int xDistance, int yDistance);
    SLATE_EXPORT QImage resizeContents(const QImage &image, int newWidth, int newHeight);

    void modifyHsl(QImage &image, qreal hue, qreal saturation, qreal lightness, qreal alpha,
        ImageCanvas::AlphaAdjustmentFlags alphaAdjustmentFlags);

    void strokeRectWithDashes(QPainter *painter, const QRect &rect);

    SLATE_EXPORT QRect ensureWithinArea(const QRect &rect, const QSize &boundsSize);

    enum FindUniqueColoursResult {
        ThreadInterrupted,
        MaximumUniqueColoursExceeded,
        FindUniqueColoursSucceeded
    };

    FindUniqueColoursResult findUniqueColours(const QImage &image, int maximumUniqueColours, QVector<QColor> &uniqueColoursFound);
    FindUniqueColoursResult findUniqueColoursAndProbabilities(const QImage &image, int maximumUniqueColours,
        QVector<QColor> &uniqueColoursFound, QVector<qreal> &probabilities);
    QVarLengthArray<unsigned int> findMax256UniqueArgbColours(const QImage &image);

    // relativeFrameIndex is the index of the animation relative to animation.startIndex()
    QImage imageForAnimationFrame(const QImage &sourceImage, const AnimationPlayback &playback, int relativeFrameIndex);
    bool exportGif(const QImage &gifSourceImage, const QUrl &url, const AnimationPlayback &playback, QString &errorMessage);

    template<typename T>
    QString enumToString(T enumValue)
    {
        QString string;
        QDebug debug(&string);
        debug << enumValue;
        return string;
    }

    template <typename T>
    QString toString(const T &object) {
        QString buffer;
        QDebug stream(&buffer);
        stream.nospace() << object;
        return buffer;
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

    // TODO: use logging category
//#define FIND_CHILD_ITEMS_DEBUG

    template<typename T>
    void doFindChildItems(QQuickItem *item, const QString &objectName, QList<T> &childItemsFound, int nestLevel = 0)
    {
        const auto childItems = item->childItems();

        ++nestLevel;

        for (QQuickItem *child : childItems) {
            auto childAsT = qobject_cast<T>(child);
            bool found = false;
            if (childAsT && (objectName.isEmpty() || child->objectName() == objectName)) {
                found = true;
                childItemsFound.append(childAsT);
            }

#ifdef FIND_CHILD_ITEMS_DEBUG
            const QString indentation(nestLevel, QLatin1Char('.'));
            qDebug().noquote() << indentation << (found ? 'Y' : 'N') << child << "visible:" << child->isVisible();
#endif

            doFindChildItems(child, objectName, childItemsFound, nestLevel);
        }
    }

    template<typename T>
    QList<T> findChildItems(QQuickItem *item, const QString &objectName = QString())
    {
#ifdef FIND_CHILD_ITEMS_DEBUG
        qDebug().nospace() << "Looking for child items of " << item << " with optional objectName" << objectName << ":";
#endif
        QList<T> childItems;
        doFindChildItems(item, objectName, childItems);
        return childItems;
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
