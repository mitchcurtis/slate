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

#include "animationplayback.h"

#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcAnimationPlayback, "app.animationPlayback")

AnimationPlayback::AnimationPlayback(QObject *parent) :
    QObject(parent),
    mTimerId(-1)
{
    reset();
}

int AnimationPlayback::fps() const
{
    return mFps;
}

void AnimationPlayback::setFps(int fps)
{
    if (fps == mFps)
        return;

    mFps = fps;
    emit fpsChanged();
}

int AnimationPlayback::frameCount() const
{
    return mFrameCount;
}

void AnimationPlayback::setFrameCount(int frameCount)
{
    if (frameCount == mFrameCount)
        return;

    mFrameCount = frameCount;
    emit frameCountChanged();
}

int AnimationPlayback::frameX() const
{
    return mFrameX;
}

void AnimationPlayback::setFrameX(int frameX)
{
    if (frameX == mFrameX)
        return;

    mFrameX = frameX;
    emit frameXChanged();
}

int AnimationPlayback::frameY() const
{
    return mFrameY;
}

void AnimationPlayback::setFrameY(int frameY)
{
    if (frameY == mFrameY)
        return;

    mFrameY = frameY;
    emit frameYChanged();
}

int AnimationPlayback::frameWidth() const
{
    return mFrameWidth;
}

void AnimationPlayback::setFrameWidth(int frameWidth)
{
    if (frameWidth == mFrameWidth)
        return;

    mFrameWidth = frameWidth;
    emit frameWidthChanged();
}

int AnimationPlayback::frameHeight() const
{
    return mFrameHeight;
}

void AnimationPlayback::setFrameHeight(int frameHeight)
{
    if (frameHeight == mFrameHeight)
        return;

    mFrameHeight = frameHeight;
    emit frameHeightChanged();
}

int AnimationPlayback::currentFrameIndex() const
{
    return mCurrentFrameIndex;
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
        mTimerId = startTimer(1000 / mFps);
    }

    emit playingChanged();
}

void AnimationPlayback::setCurrentFrameIndex(int currentFrameIndex)
{
    if (currentFrameIndex == mCurrentFrameIndex)
        return;

    mCurrentFrameIndex = currentFrameIndex;
    emit currentFrameIndexChanged();
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
    if (newFrameIndex >= mFrameCount) {
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
    setFps(json.value(QLatin1String("fps")).toInt());
    setFrameCount(json.value(QLatin1String("frameCount")).toInt());
    setFrameX(json.value(QLatin1String("frameX")).toInt());
    setFrameY(json.value(QLatin1String("frameY")).toInt());
    setFrameWidth(json.value(QLatin1String("frameWidth")).toInt());
    setFrameHeight(json.value(QLatin1String("frameHeight")).toInt());
    setScale(json.value(QLatin1String("scale")).toDouble());
    setLoop(json.value(QLatin1String("loop")).toBool());
    setPlaying(false);
}

void AnimationPlayback::write(QJsonObject &json) const
{
    json[QLatin1String("fps")] = mFps;
    json[QLatin1String("frameX")] = mFrameX;
    json[QLatin1String("frameY")] = mFrameY;
    json[QLatin1String("frameCount")] = mFrameCount;
    json[QLatin1String("frameWidth")] = mFrameWidth;
    json[QLatin1String("frameHeight")] = mFrameHeight;
    json[QLatin1String("scale")] = mScale;
    json[QLatin1String("loop")] = mLoop;
}

void AnimationPlayback::reset()
{
    setFps(4);
    setFrameCount(4);
    setFrameX(0);
    setFrameY(0);
    setFrameWidth(32);
    setFrameHeight(32);
    setCurrentFrameIndex(0);
    setPlaying(false);
    setScale(1.0);
    setLoop(true);
    mTimerId = -1;
}
