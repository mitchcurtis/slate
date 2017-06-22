/*
    Copyright 2017, Mitch Curtis

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

#include "imagelayer.h"

ImageLayer::ImageLayer(QObject *parent, const QImage &image) :
    QObject(parent),
    mImage(image)
{
    setObjectName(QLatin1String("ImageLayer"));
}

ImageLayer::~ImageLayer()
{
}

QSize ImageLayer::size() const
{
    return !mImage.isNull() ? mImage.size() : QSize();
}

void ImageLayer::setSize(const QSize &newSize)
{
    if (newSize == size())
        return;

    mImage = mImage.copy(0, 0, newSize.width(), newSize.height());
}

QImage *ImageLayer::image()
{
    return &mImage;
}

const QImage *ImageLayer::image() const
{
    return &mImage;
}
