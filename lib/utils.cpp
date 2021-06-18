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

#include "utils.h"

#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QPainter>
#include <QPainterPath>
#include <QScopeGuard>
#include <QThread>
#include <QTransform>

// Need this otherwise we get linker errors.
extern "C" {
#include "bitmap/bmp.h"
#include "bitmap/misc/gif.h"
}

#include "gif-h/qt-cpp/gifwriter.h"

#include "animation.h"
#include "animationplayback.h"

Q_LOGGING_CATEGORY(lcUtils, "app.utils")

QImage Utils::paintImageOntoPortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage)
{
    QImage newImage = image;
    QPainter painter(&newImage);
    painter.drawImage(portion.topLeft(), replacementImage);
    return newImage;
}

QImage Utils::replacePortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage)
{
    QImage newImage = image;
    QPainter painter(&newImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(portion.topLeft(), replacementImage);
    return newImage;
}

QImage Utils::erasePortionOfImage(const QImage &image, const QRect &portion)
{
    QImage newImage = image;
    QPainter painter(&newImage);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(portion, Qt::transparent);
    return newImage;
}

QImage Utils::rotate(const QImage &image, int angle)
{
    const QPoint center = image.rect().center();
    QTransform transform;
    transform.translate(center.x(), center.y());
    transform.rotate(angle);
    return image.transformed(transform);
}

/*!
    1. Copies \a area in \a image.
    2. Erases \a area in \a image, filling it with transparency.
    3. Pastes the rotated image from step 1 at the centre of \a area.
    4. Tries to move the rotated area within the bounds of the image if it's outside.
    5. Crops the the rotated image if it's too large.
    6. Returns the final rotated image portion and sets \a inRotatedArea as the area
       representing the newly rotated \a area.
*/
QImage Utils::rotateAreaWithinImage(const QImage &image, const QRect &area, int angle, QRect &inRotatedArea)
{
    QImage result = image;
    const QPoint areaCentre = area.center();

    // Create an image from the target area and then rotate it.
    // The resulting image will be big enough to contain the rotation.
    QImage rotatedImagePortion = image.copy(area);
    QTransform transform;
    transform.translate(areaCentre.x(), areaCentre.y());
    transform.rotate(angle);
    rotatedImagePortion = rotatedImagePortion.transformed(transform);

    // Remove what was behind the area and replace it with transparency.
    result = erasePortionOfImage(result, area);

    // Centre the rotated image over the target area's centre...
    QRect rotatedArea = rotatedImagePortion.rect();
    rotatedArea.moveCenter(areaCentre);

    // Move the rotated area if it's outside the bounds.
    rotatedArea = ensureWithinArea(rotatedArea, image.size());

    // If it's still out of bounds, it's probably too big
    // (e.g. image width is not equal to height so rotating makes it too large).
    // In that case, just crop it.
    bool cropped = false;
    if (rotatedArea.width() > image.width()) {
        rotatedArea.setWidth(image.width());
        cropped = true;
    }
    if (rotatedArea.height() > image.height()) {
        rotatedArea.setHeight(image.height());
        cropped = true;
    }
    if (cropped)
        rotatedImagePortion = rotatedImagePortion.copy(0, 0, rotatedArea.width(), rotatedArea.height());

    inRotatedArea = rotatedArea;

    return rotatedImagePortion;
}

void Utils::strokeRectWithDashes(QPainter *painter, const QRect &rect)
{
    static const QColor greyColour(0, 0, 0, 180);
    static const QColor whiteColour(255, 255, 255, 180);

    painter->save();

    QPainterPathStroker stroker;
    stroker.setWidth(1);
    stroker.setJoinStyle(Qt::BevelJoin);
    stroker.setCapStyle(Qt::FlatCap);

    QVector<qreal> dashes;
    const qreal dash = 4;
    const qreal space = 4;
    dashes << dash << space;
    stroker.setDashPattern(dashes);

    QPainterPath path;
    path.addRect(rect.x() + 0.5, rect.y() + 0.5, rect.width() - 1.0, rect.height() - 1.0);

    // Stroke with grey.
    QPainterPath stroke = stroker.createStroke(path);
    painter->fillPath(stroke, greyColour);

    // Stroke with white.
    stroker.setDashOffset(4);
    stroke = stroker.createStroke(path);
    painter->fillPath(stroke, whiteColour);

    painter->restore();
}

QRect Utils::ensureWithinArea(const QRect &rect, const QSize &boundsSize)
{
    QRect newArea = rect;

    if (rect.x() + rect.width() > boundsSize.width())
        newArea.moveLeft(boundsSize.width() - rect.width());
    else if (rect.x() < 0)
        newArea.moveLeft(0);

    if (rect.y() + rect.height() > boundsSize.height())
        newArea.moveTop(boundsSize.height() - rect.height());
    else if (rect.y() < 0)
        newArea.moveTop(0);

    return newArea;
}

void Utils::modifyHsl(QImage &image, qreal hue, qreal saturation, qreal lightness, qreal alpha,
    ImageCanvas::AlphaAdjustmentFlags alphaAdjustmentFlags)
{
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QColor rgb = image.pixelColor(x, y);
            QColor hsl = rgb.toHsl();

            const bool doNotModifyFullyTransparentPixels = alphaAdjustmentFlags.testFlag(ImageCanvas::DoNotModifyFullyTransparentPixels);
            const bool doNotModifyFullyOpaquePixels = alphaAdjustmentFlags.testFlag(ImageCanvas::DoNotModifyFullyOpaquePixels);
            // By default, modify the alpha.
            bool modifyAlpha = !doNotModifyFullyTransparentPixels && !doNotModifyFullyOpaquePixels;
            qreal finalAlpha = hsl.alphaF();
            if (!modifyAlpha) {
                // At least one of the flags was set, so check further if we should modify.
                const bool isFullyTransparent = qFuzzyCompare(hsl.alphaF(), 0.0f);
                const bool isFullyOpaque = qFuzzyCompare(hsl.alphaF(), 1.0f);

                if (doNotModifyFullyTransparentPixels && doNotModifyFullyOpaquePixels)
                    modifyAlpha = !isFullyTransparent && !isFullyOpaque;
                else if (doNotModifyFullyTransparentPixels)
                    modifyAlpha = !isFullyTransparent;
                else if (doNotModifyFullyOpaquePixels)
                    modifyAlpha = !isFullyOpaque;
            }
            if (modifyAlpha)
                finalAlpha = hsl.alphaF() + alpha;

            hsl.setHslF(
                qBound(0.0, hsl.hslHueF() + hue, 1.0),
                qBound(0.0, hsl.hslSaturationF() + saturation, 1.0),
                qBound(0.0, hsl.lightnessF() + lightness, 1.0),
                // Only increase the alpha if it's non-zero to prevent fully transparent
                // pixels (#00000000) becoming black (#FF000000).
                qBound(0.0, finalAlpha, 1.0));
            image.setPixelColor(x, y, hsl.toRgb());
        }
    }
}

bool Utils::exportGif(const QImage &gifSourceImage, const QUrl &url, const AnimationPlayback &playback, QString &errorMessage)
{
    const QString path = url.toLocalFile();
    if (!path.endsWith(QLatin1String(".gif"))) {
        errorMessage = QObject::tr("Failed to export GIF: path must end with .gif");
        return false;
    }

    if (gifSourceImage.size().isEmpty()) {
        errorMessage = QObject::tr("Failed to export GIF: the width and/or height of the exported image is zero");
        return false;
    }

    qCDebug(lcUtils).nospace() << "exporting gif to: " << path << "...";

    const Animation *animation = playback.animation();
    const int width = animation->frameWidth() * playback.scale();
    const int height = animation->frameHeight() * playback.scale();

    // The GIF format expects centiseconds (hundredths of a second):
    // http://giflib.sourceforge.net/gifstandard/GIF89a.html
    const int frameDelayInCentiseconds = 100.0 / animation->fps();
    static const bool dither = true;

    qCDebug(lcUtils) << "original width:" << animation->frameWidth()
        << "original height:" << animation->frameHeight()
        << "width:" << width << "height:" << height << "scale:" << playback.scale()
        << "frames per second:" << animation->fps()
        << "frame delay in centiseconds:" << frameDelayInCentiseconds
        << "dither:" << dither;

    // Convert it to an 8 bit image so that the byte order is as we expect.
    const QImage eightBitImage = gifSourceImage.convertToFormat(QImage::Format_RGBA8888);

    std::vector<QImage> frameImages;

    const int frameStartIndex = playback.animation()->startIndex(gifSourceImage.width());
    for (int frameIndex = frameStartIndex; frameIndex < frameStartIndex + animation->frameCount(); ++frameIndex) {
        const QImage frameSourceImage = imageForAnimationFrame(eightBitImage, playback, frameIndex - frameStartIndex);
        const QImage scaledFrameSourceImage = frameSourceImage.scaled(frameSourceImage.size() * playback.scale());
        frameImages.push_back(scaledFrameSourceImage);
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        errorMessage = QObject::tr("Failed to export GIF: can't open %1").arg(path);
        return false;
    }

    const auto data = GifH::GifWriter::encode(frameImages, frameDelayInCentiseconds, dither);
    file.write(data);

    qCDebug(lcUtils) << "... successfully exported gif";

    return true;
}

QImage Utils::imageForAnimationFrame(const QImage &sourceImage, const AnimationPlayback &playback, int relativeFrameIndex)
{
    const Animation *animation = playback.animation();
    const int frameWidth = animation->frameWidth();
    const int frameHeight = animation->frameHeight();
    const int framesWide = animation->framesWide(sourceImage.width());
    const int startIndex = animation->startIndex(sourceImage.width());

    const int absoluteCurrentIndex = startIndex + relativeFrameIndex;
    const int frameX = (absoluteCurrentIndex % framesWide) * frameWidth;
    const int frameY = (absoluteCurrentIndex / framesWide) * frameHeight;

    const QImage image = sourceImage.copy(frameX, frameY, frameWidth, frameHeight);
    qCDebug(lcUtils).nospace() << "returning image for animation:"
        << " frameX=" << animation->frameX()
        << " frameY=" << animation->frameY()
        << " currentFrameIndex=" << playback.currentFrameIndex()
        << " x=" << frameX
        << " y=" << frameY
        << " w=" << frameWidth
        << " h=" << frameHeight;
    return image;
}

Utils::FindUniqueColoursResult Utils::findUniqueColours(const QImage &image,
    int maximumUniqueColours, QVector<QColor> &uniqueColoursFound)
{
    for (int y = 0; y < image.height(); ++y) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qCDebug(lcUtils) << "Interrupt requested on the current thread; bailing out of finding unique colours";
            return ThreadInterrupted;
        }

        if (uniqueColoursFound.size() > maximumUniqueColours) {
            qCDebug(lcUtils).nospace() << "Exceeded maxium unique colours ("
                << maximumUniqueColours << "); bailing out of finding unique colours";
            return MaximumUniqueColoursExceeded;
        }

        for (int x = 0; x < image.width(); ++x) {
            const QColor colour = image.pixelColor(x, y);
            if (!uniqueColoursFound.contains(colour))
                uniqueColoursFound.append(colour);
        }
    }
    return FindUniqueColoursSucceeded;
}

Utils::FindUniqueColoursResult Utils::findUniqueColoursAndProbabilities(const QImage &image,
    int maximumUniqueColours, QVector<QColor> &uniqueColoursFound, QVector<qreal> &probabilities)
{
    const int imageWidth = image.width();
    const int imageHeight = image.height();
    const int totalPixels = imageWidth * imageHeight;

    for (int y = 0; y < imageHeight; ++y) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            qCDebug(lcUtils) << "Interrupt requested on the current thread; bailing out of finding unique colours";
            return ThreadInterrupted;
        }

        if (uniqueColoursFound.size() > maximumUniqueColours) {
            qCDebug(lcUtils).nospace() << "Exceeded maxium unique colours ("
                << maximumUniqueColours << "); bailing out of finding unique colours";
            return MaximumUniqueColoursExceeded;
        }

        for (int x = 0; x < imageWidth; ++x) {
            const QColor colour = image.pixelColor(x, y);
            const int index = uniqueColoursFound.indexOf(colour);
            if (index == -1) {
                // This colour is unique (so far).
                uniqueColoursFound.append(colour);
                probabilities.append(1.0 / totalPixels);
            } else {
                // We already have it.
                probabilities[index] += 1.0 / totalPixels;
            }
        }
    }
    return FindUniqueColoursSucceeded;
}

QVarLengthArray<unsigned int> Utils::findMax256UniqueArgbColours(const QImage &image)
{
    QVarLengthArray<unsigned int> colours;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QColor colour = image.pixelColor(x, y);
            // Used the same approach as in https://stackoverflow.com/a/4801397/904422.
            unsigned int argb = colour.alpha();
            argb = (argb << 8) + colour.red();
            argb = (argb << 8) + colour.green();
            argb = (argb << 8) + colour.blue();
            if (!colours.contains(argb))
                colours.append(argb);

            if (colours.size() == 256)
                return colours;
        }
    }
    return colours;
}
