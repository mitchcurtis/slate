/*
    Copyright 2016, Mitch Curtis

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

#include "tileset.h"

#include <QDebug>
#include <QPainter>

#include "utils.h"

Tileset::Tileset(const QString &fileName, int tilesWide, int tilesHigh, QObject *parent) :
    QObject(parent),
    mFileName(fileName),
    mImage(fileName),
    mTilesWide(tilesWide),
    mTilesHigh(tilesHigh)
{
}

bool Tileset::isValid() const
{
    return !mImage.isNull();
}

QString Tileset::fileName() const
{
    return mFileName;
}

void Tileset::setFileName(const QString &fileName)
{
    if (fileName == mFileName)
        return;

    mFileName = fileName;
}

const QImage *Tileset::image() const
{
    return &mImage;
}

QImage *Tileset::image()
{
    return &mImage;
}

void Tileset::setPixelColor(int x, int y, const QColor &colour)
{
    mImage.setPixelColor(x, y, colour);
    emit imageChanged();
}

void Tileset::copy(const QPoint &sourceTopLeft, const QPoint &targetTopLeft)
{
    const int tileW = tileWidth();
    const int tileH = tileWidth();

    if (!validTopLeft(sourceTopLeft) || !validTopLeft(targetTopLeft)) {
        return;
    }

    for (int y = 0; y < tileH; ++y) {
        for (int x = 0; x < tileW; ++x) {
            const QColor sourceColour = mImage.pixelColor(sourceTopLeft + QPoint(x, y));
            mImage.setPixelColor(targetTopLeft.x() + x, targetTopLeft.y() + y, sourceColour);
        }
    }
    emit imageChanged();
}

void Tileset::rotateCounterClockwise(const QPoint &tileTopLeft)
{
    rotate(tileTopLeft, -90);
}

void Tileset::rotateClockwise(const QPoint &tileTopLeft)
{
    rotate(tileTopLeft, 90);
}

int Tileset::tilesWide() const
{
    return mTilesWide;
}

int Tileset::tilesHigh() const
{
    return mTilesHigh;
}

// TODO: this information could be set by the project
int Tileset::tileWidth() const
{
    return mImage.width() / mTilesWide;
}

int Tileset::tileHeight() const
{
    return mImage.height() / mTilesHigh;
}

bool Tileset::validTopLeft(const QPoint &topLeft) const
{
    const QRect imageRect(0, 0, mImage.width(), mImage.height());
    const int tileWidth = mImage.width() / mTilesWide;
    const int tileHeight = mImage.height() / mTilesHigh;
    if (!imageRect.contains(QRect(topLeft.x(), topLeft.y(), tileWidth, tileHeight))) {
        qWarning() << "Tileset top-left" << topLeft << "is outside of tileset bounds" << imageRect;
        return false;
    }

    if (topLeft.x() % tileWidth != 0 || topLeft.y() % tileHeight != 0) {
        qWarning() << "Tileset top-left" << topLeft << "is not a multiple of the tile size";
        return false;
    }

    return true;
}

void Tileset::rotate(const QPoint &tileTopLeft, int angle)
{
    if (!validTopLeft(tileTopLeft)) {
        return;
    }

    const QImage tileImage = mImage.copy(tileTopLeft.x(), tileTopLeft.y(), tileWidth(), tileHeight());
    const QImage rotatedImage = Utils::rotate(tileImage, angle);
    QPainter painter(&mImage);
    // Make sure that we clear the previous tile image before painting on the newly rotated one.
    for (int y = 0; y < tileImage.height(); ++y) {
        for (int x = 0; x < tileImage.width(); ++x) {
            mImage.setPixelColor(tileTopLeft.x() + x, tileTopLeft.y() + y, Qt::transparent);
        }
    }
    painter.drawImage(tileTopLeft, rotatedImage);
    emit imageChanged();
}
