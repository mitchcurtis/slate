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

#include "spriteimage.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QPainter>

#include "animation.h"
#include "animationplayback.h"
#include "project.h"
#include "utils.h"

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

    const QImage copy = Utils::imageForAnimationFrame(exportedImage,
        *mAnimationPlayback, mAnimationPlayback->currentFrameIndex());
    Q_ASSERT(!copy.isNull());

    qCDebug(lcSpriteImage).nospace() << "painting sprite animation starting at"
        << " frameX=" << mAnimationPlayback->animation()->frameX()
        << " frameY=" << mAnimationPlayback->animation()->frameY()
        << " currentFrameIndex=" << mAnimationPlayback->currentFrameIndex();

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

    if (mProject)
        disconnect(mProject, &Project::contentsModified, this, &SpriteImage::onNeedsUpdate);

    mProject = project;

    if (mProject)
        connect(mProject, &Project::contentsModified, this, &SpriteImage::onNeedsUpdate);

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
        connect(mAnimationPlayback, &AnimationPlayback::animationChanged, this, &SpriteImage::onAnimationChanged);
        connect(mAnimationPlayback, &AnimationPlayback::currentFrameIndexChanged, this, &SpriteImage::onNeedsUpdate);
    }

    // Force implicit size change & repaint.
    onFrameSizeChanged();
    // Make sure we're listening to the animation if it's already set on the playback object.
    onAnimationChanged(nullptr);

    emit animationPlaybackChanged();
}

void SpriteImage::onNeedsUpdate()
{
    update();
}

void SpriteImage::onFrameSizeChanged()
{
    const auto animation = mAnimationPlayback ? mAnimationPlayback->animation() : nullptr;
    setImplicitWidth(animation ? animation->frameWidth() : 0);
    setImplicitHeight(animation ? animation->frameHeight() : 0);
    update();
}

void SpriteImage::onAnimationChanged(Animation *oldAnimation)
{
    if (oldAnimation)
        oldAnimation->disconnect(this);

    onFrameSizeChanged();

    if (mAnimationPlayback && mAnimationPlayback->animation()) {
        connect(mAnimationPlayback->animation(), &Animation::frameXChanged, this, &SpriteImage::onNeedsUpdate);
        connect(mAnimationPlayback->animation(), &Animation::frameYChanged, this, &SpriteImage::onNeedsUpdate);
        connect(mAnimationPlayback->animation(), &Animation::frameWidthChanged, this, &SpriteImage::onFrameSizeChanged);
        connect(mAnimationPlayback->animation(), &Animation::frameHeightChanged, this, &SpriteImage::onFrameSizeChanged);
    }
}
