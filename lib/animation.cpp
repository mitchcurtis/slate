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

#include "animation.h"

#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcAnimation, "app.animation")

Animation::Animation(QObject *parent) :
    QObject(parent)
{
}

QString Animation::name() const
{
    return mName;
}

void Animation::setName(const QString &name)
{
    if (name == mName)
        return;

    mName = name;
    emit nameChanged();
}

int Animation::fps() const
{
    return mFps;
}

void Animation::setFps(int fps)
{
    if (fps == mFps)
        return;

    mFps = fps;
    emit fpsChanged();
}

int Animation::frameCount() const
{
    return mFrameCount;
}

void Animation::setFrameCount(int frameCount)
{
    if (frameCount == mFrameCount)
        return;

    mFrameCount = frameCount;
    emit frameCountChanged();
}

int Animation::frameX() const
{
    return mFrameX;
}

void Animation::setFrameX(int frameX)
{
    if (frameX == mFrameX)
        return;

    mFrameX = frameX;
    emit frameXChanged();
}

int Animation::frameY() const
{
    return mFrameY;
}

void Animation::setFrameY(int frameY)
{
    if (frameY == mFrameY)
        return;

    mFrameY = frameY;
    emit frameYChanged();
}

int Animation::frameWidth() const
{
    return mFrameWidth;
}

int Animation::framesWide(int sourceImageWidth) const
{
    return sourceImageWidth / mFrameWidth;
}

void Animation::setFrameWidth(int frameWidth)
{
    if (frameWidth == mFrameWidth)
        return;

    mFrameWidth = frameWidth;
    emit frameWidthChanged();
}

int Animation::frameHeight() const
{
    return mFrameHeight;
}

void Animation::setFrameHeight(int frameHeight)
{
    if (frameHeight == mFrameHeight)
        return;

    mFrameHeight = frameHeight;
    emit frameHeightChanged();
}

int Animation::startColumn() const
{
    return mFrameX / mFrameWidth;
}

int Animation::startRow() const
{
    return mFrameY / mFrameHeight;
}

int Animation::startIndex(int sourceImageWidth) const
{
    return startRow() * framesWide(sourceImageWidth) + startColumn();
}

void Animation::read(const QJsonObject &json)
{
    setName(json.value(QLatin1String("name")).toString());
    setFps(json.value(QLatin1String("fps")).toInt());
    setFrameX(json.value(QLatin1String("frameX")).toInt());
    setFrameY(json.value(QLatin1String("frameY")).toInt());
    setFrameCount(json.value(QLatin1String("frameCount")).toInt());
    setFrameWidth(json.value(QLatin1String("frameWidth")).toInt());
    setFrameHeight(json.value(QLatin1String("frameHeight")).toInt());
}

void Animation::write(QJsonObject &json) const
{
    json[QLatin1String("name")] = mName;
    json[QLatin1String("fps")] = mFps;
    json[QLatin1String("frameX")] = mFrameX;
    json[QLatin1String("frameY")] = mFrameY;
    json[QLatin1String("frameCount")] = mFrameCount;
    json[QLatin1String("frameWidth")] = mFrameWidth;
    json[QLatin1String("frameHeight")] = mFrameHeight;
}

bool operator==(const Animation &lhs, const Animation &rhs)
{
    return lhs.fps() == rhs.fps()
        && lhs.name() == rhs.name()
        && lhs.frameX() == rhs.frameY()
        && lhs.frameY() == rhs.frameY()
        && lhs.frameCount() == rhs.frameCount()
        && lhs.frameWidth() == rhs.frameWidth()
        && lhs.frameHeight() == rhs.frameHeight();
}

QDebug operator<<(QDebug debug, const Animation *animation)
{
    QDebugStateSaver saver(debug);
    if (!animation)
        return debug << "Animation(0x0)";

    debug.nospace() << "(Animation name=" << animation->name()
        << " fps=" << animation->fps()
        << " frameX=" << animation->frameX()
        << " frameY=" << animation->frameY()
        << " frameWidth=" << animation->frameWidth()
        << " frameHeight=" << animation->frameHeight()
        << " frameCount=" << animation->frameCount() << ")";
    return debug;
}
