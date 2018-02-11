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

#include <QJsonObject>

AnimationPlayback::AnimationPlayback(QObject *parent) :
    QObject(parent)
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

    mPlaying = playing;
    emit playingChanged();
}

void AnimationPlayback::setCurrentFrameIndex(int currentFrameIndex)
{
    if (currentFrameIndex == mCurrentFrameIndex)
        return;

    mCurrentFrameIndex = currentFrameIndex;
    emit currentFrameIndexChanged();
}

void AnimationPlayback::read(const QJsonObject &json)
{
    setFps(json.value(QLatin1String("fps")).toInt());
    setFrameCount(json.value(QLatin1String("frameCount")).toInt());
    setFrameWidth(json.value(QLatin1String("frameWidth")).toInt());
    setFrameHeight(json.value(QLatin1String("frameHeight")).toInt());
    setPlaying(false);
}

void AnimationPlayback::write(QJsonObject &json) const
{
    json[QLatin1String("fps")] = mFps;
    json[QLatin1String("frameCount")] = mFrameCount;
    json[QLatin1String("frameWidth")] = mFrameWidth;
    json[QLatin1String("frameHeight")] = mFrameHeight;
}

void AnimationPlayback::reset()
{
    setFps(4);
    setFrameCount(0);
    setFrameWidth(32);
    setFrameHeight(32);
    setCurrentFrameIndex(-1);
    setPlaying(false);
}
