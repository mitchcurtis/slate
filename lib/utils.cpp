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

#include "utils.h"

#include <QDebug>
#include <QPainter>

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
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(angle);
    return image.transformed(matrix);
}

/*!
    1. Copies \a area in \a image.
    2. Erases \a area in \a image, filling it with transparency.
    3. Pastes the rotated image from step 1 at the centre of \a area.
    4. Returns the final image and sets \a inRotatedArea as the area
       representing the newly rotated \a area.
*/
QImage Utils::rotateArea(const QImage &image, const QRect &area, int angle, QRect &inRotatedArea)
{
    QImage result = image;
    const QPoint areaCentre = area.center();

    // Create an image from the target area and then rotate it.
    // The resulting image will be big enough to contain the rotation.
    QImage rotatedImagePortion = image.copy(area);
    QMatrix matrix;
    matrix.translate(areaCentre.x(), areaCentre.y());
    matrix.rotate(angle);
    rotatedImagePortion = rotatedImagePortion.transformed(matrix);

    // Remove what was behind the area and replace it with transparency.
    result = erasePortionOfImage(result, area);

    // Centre the rotated image over the target area's centre...
    QRect rotatedArea = rotatedImagePortion.rect();
    rotatedArea.moveCenter(areaCentre);

    // ...and paint it onto the result.
    QPainter painter(&result);
    painter.drawImage(rotatedArea, rotatedImagePortion);

    inRotatedArea = rotatedArea;

    return result;
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
