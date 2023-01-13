/*
    Copyright 2023, Mitch Curtis

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
    qCDebug(lcAnimationPlayback) << "setAnimation called on" << objectName()
        << "with" << animation << "- current animation is" << mAnimation;
    if (animation == mAnimation)
        return;

    auto oldAnimation = mAnimation;
    if (oldAnimation) {
        oldAnimation->disconnect(this);

        const bool wasPlaying = mPlaying;
        setPlaying(false);
        mWasPlayingBeforeAnimationChanged = wasPlaying;
    }

    mAnimation = animation;
    mPauseIndex = -1;

    if (mAnimation) {
        connect(mAnimation, &Animation::fpsChanged, this, &AnimationPlayback::onFpsChanged);
        connect(mAnimation, &Animation::reverseChanged, this, &AnimationPlayback::setCurrentIndexToStart);

        if (mWasPlayingBeforeAnimationChanged)
            setPlaying(true);
    }

    emit animationChanged(oldAnimation);

    setCurrentIndexToStart();
}

int AnimationPlayback::currentFrameIndex() const
{
    return mCurrentFrameIndex;
}

qreal AnimationPlayback::progress() const
{
    if (!mAnimation)
        return 0;

    const qreal nonReversedProgress = qreal(mCurrentFrameIndex) / (mAnimation->frameCount() - 1);
    if (!mAnimation->isReverse())
        return nonReversedProgress;

    return 1.0 - nonReversedProgress;
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
    if (playing && !mAnimation) {
        qWarning() << "Can't play without an animation";
        return;
    }

    qCDebug(lcAnimationPlayback) << "setPlaying called on" << objectName()
        << "with" << playing << "- current value is" << mPlaying;
    if (playing == mPlaying)
        return;

    if (mPlaying) {
        qCDebug(lcAnimationPlayback) << "killing timer on" << objectName();
        if (mTimerId != -1) {
            killTimer(mTimerId);
            mTimerId = -1;
        }
    }

    mPlaying = playing;
    mWasPlayingBeforeAnimationChanged = mPlaying;

    if (mPlaying) {
        qCDebug(lcAnimationPlayback) << "starting timer on" << objectName();

        if (mPauseIndex != -1) {
            // The animation hasn't changed since we stopped playing, so resume from where it was stopped.
            qCDebug(lcAnimationPlayback) << "resuming animation on" << objectName() << "from" << mPauseIndex;
            setCurrentFrameIndex(mPauseIndex);
            mPauseIndex = -1;
        }

        mTimerId = startTimer(1000 / qMax(1, mAnimation->fps()));
    } else {
        // We paused, so store the index at which we paused so we can resume later.
        mPauseIndex = mCurrentFrameIndex;
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

int AnimationPlayback::pauseIndex() const
{
    return mPauseIndex;
}

void AnimationPlayback::timerEvent(QTimerEvent *)
{
    Q_ASSERT(mPlaying);

    int newFrameIndex = !mAnimation->isReverse() ? mCurrentFrameIndex + 1 : mCurrentFrameIndex - 1;
    const bool finished = !mAnimation->isReverse() ? newFrameIndex >= mAnimation->frameCount() : newFrameIndex < 0;
    qCDebug(lcAnimationPlayback) << "timer triggered on" << objectName() << "- finished" << finished;
    if (finished) {
        newFrameIndex = startFrameIndex();

        if (!mLoop) {
            setPlaying(false);
            // setPlaying(false) tells it to pause, which causes the pause index to be set,
            // which we don't want for the natural end of an animation.
            mPauseIndex = -1;
        }
    }

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
    mPauseIndex = -1;
    setPlaying(false);
    mWasPlayingBeforeAnimationChanged = false;
    setScale(1.0);
    setLoop(true);
    mTimerId = -1;
}

void AnimationPlayback::onFpsChanged()
{
    if (!mPlaying || mTimerId == -1)
        return;

    killTimer(mTimerId);
    mTimerId = startTimer(1000 / mAnimation->fps());
}

void AnimationPlayback::setCurrentIndexToStart()
{
    // Start from the beginning.
    setCurrentFrameIndex(startFrameIndex());
    // This function is called when the animation is reversed (amongst other things),
    // so clear the pause index since it would no longer make sense.
    mPauseIndex = -1;
}

int AnimationPlayback::startFrameIndex() const
{
    return !mAnimation->isReverse() ? 0 : mAnimation->frameCount() - 1;
}

void AnimationPlayback::setCurrentFrameIndex(int currentFrameIndex)
{
    if (currentFrameIndex == mCurrentFrameIndex)
        return;

    qCDebug(lcAnimationPlayback) << "currentFrameIndex of" << objectName() << "was" << mCurrentFrameIndex
        << "and is now" << currentFrameIndex;
    mCurrentFrameIndex = currentFrameIndex;
    emit currentFrameIndexChanged();
}
