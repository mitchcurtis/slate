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

#include "spriteimage.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QPainter>

#include "animationplayback.h"
#include "project.h"

Q_LOGGING_CATEGORY(lcSpriteImage, "app.spriteImage")

SpriteImage::SpriteImage() :
    mProject(nullptr),
    mAnimationPlayback(nullptr)
{
}

void SpriteImage::paint(QPainter *painter)
{
    if (!mProject || !mAnimationPlayback)
        return;

    const QImage exportedImage = mProject->exportedImage();
    if (exportedImage.isNull())
        return;

    QImage copy;
    // The defaults...
    const int frameWidth = mAnimationPlayback->frameWidth();
    const int frameHeight = mAnimationPlayback->frameHeight();
    const int framesWide = exportedImage.width() / frameWidth;
    const int startColumn = mAnimationPlayback->frameX() / frameWidth;
    const int startRow = mAnimationPlayback->frameY() / frameHeight;
    const int xOffset = startColumn;
    const int yOffset = startRow * framesWide;

    const int column = (xOffset + mAnimationPlayback->currentFrameIndex()) % framesWide;
    const int row = (yOffset + mAnimationPlayback->currentFrameIndex()) / framesWide;
    copy = exportedImage.copy(column * frameWidth, row * frameHeight, frameWidth, frameHeight);
    Q_ASSERT(!copy.isNull());

    qCDebug(lcSpriteImage) << "painting sprite frame with"
        << mAnimationPlayback->frameX() << mAnimationPlayback->frameY() << mAnimationPlayback->frameWidth() << mAnimationPlayback->frameHeight()
        << "at" << column * frameWidth << row * frameHeight << frameWidth << frameHeight;

    painter->drawImage(0, 0, copy);
}

Project *SpriteImage::project() const
{
    return mProject;
}

void SpriteImage::setProject(Project *project)
{
    if (project == mProject)
        return;

    mProject = project;
    update();
    emit projectChanged();
}

AnimationPlayback *SpriteImage::animationPlayback() const
{
    return mAnimationPlayback;
}

void SpriteImage::setAnimationPlayback(AnimationPlayback *animationPlayback)
{
    if (animationPlayback == mAnimationPlayback)
        return;

    if (mAnimationPlayback)
        mAnimationPlayback->disconnect(this);

    mAnimationPlayback = animationPlayback;

    if (mAnimationPlayback) {
        connect(mAnimationPlayback, &AnimationPlayback::currentFrameIndexChanged, [=]{ update(); });
        connect(mAnimationPlayback, &AnimationPlayback::frameWidthChanged, this, &SpriteImage::onFrameSizeChanged);
        connect(mAnimationPlayback, &AnimationPlayback::frameHeightChanged, this, &SpriteImage::onFrameSizeChanged);
    }

    // Force implicit size change & repaint.
    onFrameSizeChanged();

    emit animationPlaybackChanged();
}

void SpriteImage::onFrameSizeChanged()
{
    setImplicitWidth(mAnimationPlayback ? mAnimationPlayback->frameWidth() : 0);
    setImplicitHeight(mAnimationPlayback ? mAnimationPlayback->frameHeight() : 0);
    update();
}
