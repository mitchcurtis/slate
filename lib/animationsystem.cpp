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

#include "animationsystem.h"

#include <QDebug>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcAnimationSystem, "app.AnimationSystem")

AnimationSystem::AnimationSystem(QObject *parent) :
    QObject(parent)
{
}

int AnimationSystem::currentAnimationIndex() const
{
    return mCurrentAnimationIndex;
}

void AnimationSystem::setCurrentAnimationIndex(int index)
{
    if (index < 0 || index > mAnimations.size()) {
        qWarning() << "Animation index" << index << "is invalid";
        return;
    }

    if (index == mCurrentAnimationIndex)
        return;

    mCurrentAnimationIndex = index;
    emit currentAnimationIndexChanged();
}

Animation *AnimationSystem::currentAnimation()
{
    if (mCurrentAnimationIndex == -1)
        return nullptr;
    return mAnimations.at(mCurrentAnimationIndex);
}

AnimationPlayback *AnimationSystem::currentAnimationPlayback()
{
    return &mCurrentAnimationPlayback;
}

void AnimationSystem::read(const QJsonObject &json)
{
    // TODO: account for old AnimationPlayback data here

    mCurrentAnimationPlayback.read(json.value("currentAnimationPlayback").toObject());
}

void AnimationSystem::write(QJsonObject &json) const
{
    QJsonObject playbackJson;
    mCurrentAnimationPlayback.write(playbackJson);
    json["currentAnimationPlayback"] = playbackJson;
}

void AnimationSystem::reset()
{
    mAnimations.clear();
    mCurrentAnimationPlayback.reset();
}

void AnimationSystem::addAnimation(const QSize &canvasSize)
{
    const int frameX = 0;
    const int frameY = 0;
    const int frameCount = canvasSize.width() >= 8 ? 4 : 1;
    const int frameWidth = canvasSize.width() / frameCount;
    const int frameHeight = canvasSize.height();

    auto existingAnimationIt = findAnimationWithName(name);
    if (existingAnimationIt != mAnimations.end()) {
        qWarning() << "Animation named \"" << name << "\" already exists";
        return;
    }

    // todo: undo command
    // TODO: update currentAnimationIndex if it was added before the current

    auto animation = new Animation();
    animation->setName(name);
    animation->setFps(fps);
    animation->setFrameCount(frameCount);
    animation->setFrameX(frameX);
    animation->setFrameY(frameY);
    animation->setFrameWidth(frameWidth);
    animation->setFrameHeight(frameHeight);
    mAnimations.append(animation);
}

void AnimationSystem::removeAnimation(const QString &name)
{
    auto animationIt = findAnimationWithName(name);
    if (animationIt == mAnimations.end()) {
        qWarning() << "Animation named \"" << name << "\" doesn't exist";
        return;
    }

    // todo: undo command
    // TODO: update currentAnimationIndex if it was removed before the current

    mAnimations.erase(animationIt);
}

QVector<Animation*>::iterator AnimationSystem::findAnimationWithName(const QString &name)
{
    return std::find_if(mAnimations.begin(), mAnimations.end(), [name](Animation *animation) {
        return animation->name() == name;
    });
}

Animation *AnimationSystem::animationAtNameOrWarn(const QString &name)
{
    auto animationIt = findAnimationWithName(name);
    if (animationIt == mAnimations.end())
        qWarning() << "Animation named \"" << name << "\" doesn't exist";

    return *animationIt;
}
