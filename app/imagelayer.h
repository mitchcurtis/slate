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

#ifndef IMAGELAYER_H
#define IMAGELAYER_H

#include <QImage>
#include <QObject>

class ImageLayer : public QObject
{
    Q_OBJECT

public:
    ImageLayer();
    explicit ImageLayer(QObject *parent, const QImage &image);
    ~ImageLayer();

    QImage *image();
    const QImage *image() const;

    QSize size() const;
    void setSize(const QSize &newSize);

private:
    QImage mImage;
};

#endif // IMAGELAYER_H
