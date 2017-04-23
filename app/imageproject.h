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

#ifndef IMAGEPROJECT_H
#define IMAGEPROJECT_H

#include <QImage>

#include "project.h"

class ImageProject : public Project
{
    Q_OBJECT

public:
    ImageProject();
    ~ImageProject();

    QImage *image();
    const QImage *image() const;

    Type type() const override;
    QSize size() const override;
    void setSize(const QSize &size) override;
    int widthInPixels() const override;
    int heightInPixels() const override;

public slots:
    void createNew(int imageWidth, int imageHeight, bool transparentBackground);
    void close();

private:
    friend class ChangeImageCanvasSizeCommand;

    void changeSize(const QSize &size);

    QUrl mImageUrl;
    QImage mImage;
};

#endif // IMAGEPROJECT_H
