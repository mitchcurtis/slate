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
    mCurrentAnimationPlayback.setAnimation(currentAnimation());
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
    mAnimationsCreated = 0;
}

void AnimationSystem::addAnimation(const QSize &canvasSize)
{
    const QString name = QString::fromLatin1("Animation %1").arg(++mAnimationsCreated);
    auto existingAnimationIt = findAnimationWithName(name);
    if (existingAnimationIt != mAnimations.end()) {
        qWarning() << "Animation named \"" << name << "\" already exists";
        return;
    }

    // todo: undo command
    // TODO: update currentAnimationIndex if it was added before the current

    auto animation = new Animation();
    animation->setName(name);
    animation->setFps(4);
    animation->setFrameCount(canvasSize.width() >= 8 ? 4 : 1);
    animation->setFrameX(0);
    animation->setFrameY(0);
    animation->setFrameWidth(canvasSize.width() / animation->frameCount());
    animation->setFrameHeight(canvasSize.height());
    mAnimations.append(animation);

    if (mAnimations.size() == 1)
        setCurrentAnimationIndex(0);
}

void AnimationSystem::removeAnimation(const QString &name)
{
    auto animationIt = findAnimationWithName(name);
    if (animationIt == mAnimations.end()) {
        qWarning() << "Animation named \"" << name << "\" doesn't exist";
        return;
    }

    if (mAnimations.size() == 1)
        setCurrentAnimationIndex(0);

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
