/*
    Copyright 2023, Mitch Curtis

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

#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QDebug>
#include <QImage>
#include <QRect>

#include "imagecanvas.h"

class AnimationPlayback;
class ImageLayer;

namespace ImageUtils {
    SLATE_EXPORT QImage filledImage(int width, int height, const QColor &colour = Qt::transparent);
    SLATE_EXPORT QImage filledImage(const QSize &size, const QColor &colour = Qt::transparent);

    QImage paintImageOntoPortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage);

    QImage replacePortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage);

    QImage erasePortionOfImage(const QImage &image, const QRect &portion);

    QImage rotate(const QImage &image, int angle);
    QImage rotateAreaWithinImage(const QImage &image, const QRect &area, int angle, QRect &inRotatedArea);

    SLATE_EXPORT QImage moveContents(const QImage &image, int xDistance, int yDistance);
    SLATE_EXPORT QImage resizeContents(const QImage &image, int newWidth, int newHeight, bool smooth = false);
    SLATE_EXPORT QImage resizeContents(const QImage &image, const QSize &newSize, bool smooth = false);
    SLATE_EXPORT QVector<QImage> rearrangeContentsIntoGrid(const QVector<QImage> &images,
        uint cellWidth, uint cellHeight, uint columns, uint rows);
    SLATE_EXPORT QVector<QImage> pasteAcrossLayers(const QVector<ImageLayer*> &layers,
        const QVector<QImage> &layerImagesBeforeLivePreview, int pasteX, int pasteY, bool onlyPasteIntoVisibleLayers);

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
}

#endif // IMAGEUTILS_H
