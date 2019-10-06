/*
    Copyright 2019, Mitch Curtis

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

#include "spriteimageprovider.h"

#include <QImage>
#include <QDebug>

SpriteImageProvider::SpriteImageProvider() :
    QQuickImageProvider(QQmlImageProviderBase::Image)
{
}

QImage SpriteImageProvider::requestImage(const QString &id, QSize *size, const QSize &)
{
    QStringList args = id.split(QLatin1String(","));
    if (args.length() < 1) {
        qWarning() << "Must pass at least the file name as arguments for SpriteImageProvider source";
        return QImage();
    } else if (args.length() != 1 && args.length() != 2 && args.length() != 4) {
        qWarning() << "Must pass either (fileName), (fileName, frameIndex) or"
                   << "(fileName, frameWidth, frameHeight, frameIndex) as arguments for SpriteImageProvider source";
        return QImage();
    }

    const QString imageFilename = ":/" + args.first();

    int frameIndex = -2;
    int frameWidth = -2;
    int frameHeight = -2;

    if (args.length() > 1) {
        frameIndex = parseFrameIndex(args.last());
        if (frameIndex == -1)
            return QImage();
    }

    if (args.length() == 4) {
        frameWidth = parseFrameIndex(args.at(1));
        if (frameWidth == -1)
            return QImage();

        frameHeight = parseFrameIndex(args.at(2));
        if (frameHeight == -1)
            return QImage();
    }

    QHash<QString, QImage>::const_iterator it = mImages.constFind(imageFilename);
    if (it == mImages.constEnd()) {
        QImage image(imageFilename);
        if (image.isNull()) {
            qWarning() << "Failed to load image at" << imageFilename;
            return image;
        }

        mImages.insert(imageFilename, image);
        it = mImages.constFind(imageFilename);
    }

    if (frameWidth == -2 || frameHeight == -2) {
        if (frameIndex == -2) {
            // Use the whole image.
            frameWidth = it.value().width();
            frameHeight = it.value().height();
            frameIndex = 0;
        } else {
            frameWidth = 64;
            frameHeight = 64;
        }
    }

    // Copy an individual frame out of the larger image.
    const int framesWide = it.value().width() / frameWidth;
    const QRect subRect((frameIndex % framesWide) * frameWidth,
                        (frameIndex / framesWide) * frameHeight,
                        frameWidth,
                        frameHeight);
    const QImage frameImage = it.value().copy(subRect);
    *size = frameImage.size();
    return frameImage;
}


int SpriteImageProvider::parseFrameIndex(const QString &frameIndexStr) const
{
    bool convertedToIntSuccessfully = false;
    const int frameIndex = frameIndexStr.toInt(&convertedToIntSuccessfully);
    if (!convertedToIntSuccessfully) {
        qWarning() << "Failed to convert frame index" << frameIndexStr << "to an int";
        return -1;
    }
    return frameIndex;
}

int SpriteImageProvider::parseFrameWidth(const QString &frameWidthStr) const
{
    bool convertedToIntSuccessfully = false;
    const int frameWidth = frameWidthStr.toInt(&convertedToIntSuccessfully);
    if (!convertedToIntSuccessfully) {
        qWarning() << "Failed to convert frame width" << frameWidthStr << "to an int";
        return -1;
    }
    return frameWidth;
}

int SpriteImageProvider::parseFrameHeight(const QString &frameHeightStr) const
{
    bool convertedToIntSuccessfully = false;
    const int frameHeight = frameHeightStr.toInt(&convertedToIntSuccessfully);
    if (!convertedToIntSuccessfully) {
        qWarning() << "Failed to convert frame height" << frameHeightStr << "to an int";
        return -1;
    }
    return frameHeight;
}
