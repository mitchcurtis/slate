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

#include "utils.h"

#include <QPainter>

QImage Utils::rotate(const QImage &image, int angle)
{
    const QPoint center = image.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(angle);
    return image.transformed(matrix);
}

QImage Utils::paintImageOntoPortionOfImage(const QImage &image, const QRect &portion, const QImage &replacementImage)
{
    QImage newImage = image;
    QPainter painter(&newImage);
    painter.drawImage(portion.topLeft(), replacementImage);
    return newImage;
}

QImage Utils::erasePortionOfImage(const QImage &image, const QRect &portion)
{
    QImage newImage = image;
    QPainter painter(&newImage);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(portion, Qt::transparent);
    return newImage;
}
