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

#include "animationplayback.h"
#include "project.h"
#include "slate-global.h"

class SLATE_EXPORT ImageProject : public Project
{
    Q_OBJECT
    Q_PROPERTY(bool usingAnimation READ isUsingAnimation WRITE setUsingAnimation NOTIFY usingAnimationChanged FINAL)
    Q_PROPERTY(AnimationPlayback *animationPlayback READ animationPlayback CONSTANT FINAL)

public:
    ImageProject();
    ~ImageProject();

    QImage *image();
    const QImage *image() const;

    Type type() const override;
    QSize size() const override;
    void setSize(const QSize &newSize) override;
    int widthInPixels() const override;
    int heightInPixels() const override;
    virtual QRect bounds() const override;

    bool isUsingAnimation() const;
    void setUsingAnimation(bool isUsingAnimation);

    AnimationPlayback *animationPlayback();

    QImage exportedImage() const override;

signals:
    void usingAnimationChanged();

public slots:
    void createNew(int imageWidth, int imageHeight, bool transparentBackground);

    void resize(int width, int height, bool smooth);

protected:
    void doLoad(const QUrl &url) override;
    void doClose() override;
    void doSaveAs(const QUrl &url) override;

private:
    friend class ChangeImageCanvasSizeCommand;
    friend class ChangeImageSizeCommand;

    void doSetCanvasSize(const QImage &newImage);
    void doSetImageSize(const QImage &newImage);

    QUrl mImageUrl;
    QImage mImage;
    bool mUsingAnimation;
    AnimationPlayback mAnimationPlayback;
};


#endif // IMAGEPROJECT_H
