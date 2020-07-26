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

#include "animationplayback.h"

#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>

#include "animation.h"

Q_LOGGING_CATEGORY(lcAnimationPlayback, "app.animationPlayback")

AnimationPlayback::AnimationPlayback(QObject *parent) :
    QObject(parent)
{
    reset();
}

Animation *AnimationPlayback::animation() const
{
    return mAnimation;
}

void AnimationPlayback::setAnimation(Animation *animation)
{
    if (animation == mAnimation)
        return;

    auto oldAnimation = mAnimation;
    if (oldAnimation)
        oldAnimation->disconnect(this);

    mAnimation = animation;

    if (mAnimation)
        connect(mAnimation, &Animation::fpsChanged, this, &AnimationPlayback::fpsChanged);

    emit animationChanged(oldAnimation);
}

int AnimationPlayback::currentFrameIndex() const
{
    return mCurrentFrameIndex;
}

qreal AnimationPlayback::scale() const
{
    return mScale;
}

void AnimationPlayback::setScale(const qreal &scale)
{
    if (qFuzzyCompare(scale, mScale))
        return;

    mScale = scale;
    emit scaleChanged();
}

bool AnimationPlayback::isPlaying() const
{
    return mPlaying;
}

void AnimationPlayback::setPlaying(bool playing)
{
    if (playing == mPlaying)
        return;

    if (mPlaying) {
        qCDebug(lcAnimationPlayback) << "pausing";
        if (mTimerId != -1) {
            killTimer(mTimerId);
            mTimerId = -1;
        }
    }

    mPlaying = playing;

    if (mPlaying) {
        qCDebug(lcAnimationPlayback) << "playing";
        mTimerId = startTimer(1000 / qMax(1, mAnimation->fps()));
    }

    emit playingChanged();
}

bool AnimationPlayback::shouldLoop() const
{
    return mLoop;
}

void AnimationPlayback::setLoop(bool loop)
{
    if (loop == mLoop)
        return;

    mLoop = loop;
    emit loopChanged();
}

void AnimationPlayback::timerEvent(QTimerEvent *)
{
    Q_ASSERT(mPlaying);

    int newFrameIndex = mCurrentFrameIndex + 1;
    if (newFrameIndex >= mAnimation->frameCount()) {
        newFrameIndex = 0;

        if (!mLoop) {
            setPlaying(false);
        }
    }

    qCDebug(lcAnimationPlayback) << "timer triggered; new frame index is" << newFrameIndex;

    setCurrentFrameIndex(newFrameIndex);
}

void AnimationPlayback::read(const QJsonObject &json)
{
    setScale(json.value(QLatin1String("scale")).toDouble());
    setLoop(json.value(QLatin1String("loop")).toBool());
    setPlaying(false);
}

void AnimationPlayback::write(QJsonObject &json) const
{
    json[QLatin1String("scale")] = mScale;
    json[QLatin1String("loop")] = mLoop;
}

void AnimationPlayback::reset()
{
    setCurrentFrameIndex(0);
    setPlaying(false);
    setScale(1.0);
    setLoop(true);
    mTimerId = -1;
}

void AnimationPlayback::fpsChanged()
{
    if (!mPlaying || mTimerId == -1)
        return;

    killTimer(mTimerId);
    mTimerId = startTimer(1000 / mAnimation->fps());
}

void AnimationPlayback::setCurrentFrameIndex(int currentFrameIndex)
{
    if (currentFrameIndex == mCurrentFrameIndex)
        return;

    mCurrentFrameIndex = currentFrameIndex;
    emit currentFrameIndexChanged();
}
