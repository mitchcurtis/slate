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
    if (!isValidIndexOrWarn(index))
        return;

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

bool AnimationSystem::containsAnimation(const QString &name) const
{
    return findAnimationWithName(name) != mAnimations.end();
}

int AnimationSystem::indexOfAnimation(const QString &name) const
{
    const auto animationIt = findAnimationWithName(name);
    if (animationIt == mAnimations.end())
        return -1;

    return std::distance(mAnimations.begin(), animationIt);
}

int AnimationSystem::animationCount() const
{
    return mAnimations.size();
}

QString AnimationSystem::addNewAnimation(const QSize &canvasSize)
{
    const QString name = peekNextGeneratedName();
    auto existingAnimationIt = findAnimationWithName(name);
    if (existingAnimationIt != mAnimations.end()) {
        qWarning().nospace() << "Animation named \"" << name << "\" already exists";
        return QString();
    }

    ++mAnimationsCreated;

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

    return name;
}

void AnimationSystem::addAnimation(Animation *animation, int index)
{
    const int existingIndex = mAnimations.indexOf(animation);
    if (existingIndex != -1) {
        qWarning().nospace() << "Animation named \"" << animation->name()
            << "\" already exists (at index " << existingIndex << ")";
        return;
    }

    if (!isValidIndexOrWarn(index))
        return;

    animation->setParent(this);

    mAnimations.insert(index, animation);

    if (mAnimations.size() == 1)
        setCurrentAnimationIndex(0);
    else if (index <= mCurrentAnimationIndex) {
        setCurrentAnimationIndex(mCurrentAnimationIndex + 1);
    }
}

void AnimationSystem::takeAnimation(const QString &name)
{
    auto animationIt = findAnimationWithName(name);
    if (animationIt == mAnimations.end()) {
        qWarning().nospace() << "Animation named \"" << name << "\" doesn't exist";
        return;
    }

    if (mAnimations.size() == 1)
        setCurrentAnimationIndex(-1);

    const int removedIndex = std::distance(mAnimations.begin(), animationIt);
    if (removedIndex <= mCurrentAnimationIndex)
        setCurrentAnimationIndex(mCurrentAnimationIndex - 1);

    mAnimations.erase(animationIt);
}

Animation *AnimationSystem::animationAt(int index)
{
    if (!isValidIndexOrWarn(index))
        return nullptr;

    return mAnimations.at(index);
}

Animation *AnimationSystem::takeAnimation(int index)
{
    if (!isValidIndexOrWarn(index))
        return nullptr;

    Animation *animation = mAnimations.takeAt(index);
    animation->setParent(nullptr);
    return animation;
}

void AnimationSystem::read(const QJsonObject &json)
{
    // Pre-0.10.0 projects don't support multiple animations, so we
    // create an animation for them (and then later save it using the new format).
    if (json.contains("animationPlayback")) {
        QScopedPointer<Animation> animation(new Animation);
        animation->setName(takeNextGeneratedName());
        animation->setFps(json.value(QLatin1String("fps")).toInt());
        animation->setFrameCount(json.value(QLatin1String("frameCount")).toInt());
        animation->setFrameX(json.value(QLatin1String("frameX")).toInt());
        animation->setFrameY(json.value(QLatin1String("frameY")).toInt());
        animation->setFrameWidth(json.value(QLatin1String("frameWidth")).toInt());
        animation->setFrameHeight(json.value(QLatin1String("frameHeight")).toInt());

        addAnimation(animation.take(), 0);

        mCurrentAnimationPlayback.setScale(json.value(QLatin1String("scale")).toDouble());
        mCurrentAnimationPlayback.setLoop(json.value(QLatin1String("loop")).toBool());
        mCurrentAnimationPlayback.setPlaying(false);
    }

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

bool AnimationSystem::isValidIndexOrWarn(int index) const
{
    if (index < 0 || index > mAnimations.size()) {
        qWarning() << "Animation index" << index << "is invalid";
        return false;
    }
    return true;
}

QString AnimationSystem::peekNextGeneratedName() const
{
    const QString name = QString::fromLatin1("Animation %1").arg(mAnimationsCreated + 1);
    return name;
}

QString AnimationSystem::takeNextGeneratedName()
{
    const QString name = peekNextGeneratedName();
    ++mAnimationsCreated;
    return name;
}

QVector<Animation*>::iterator AnimationSystem::findAnimationWithName(const QString &name)
{
    return std::find_if(mAnimations.begin(), mAnimations.end(), [name](Animation *animation) {
        return animation->name() == name;
    });
}

QVector<Animation*>::const_iterator AnimationSystem::findAnimationWithName(const QString &name) const
{
    return std::find_if(mAnimations.begin(), mAnimations.end(), [name](Animation *animation) {
        return animation->name() == name;
    });
}

Animation *AnimationSystem::animationAtNameOrWarn(const QString &name)
{
    auto animationIt = findAnimationWithName(name);
    if (animationIt == mAnimations.end())
        qWarning().nospace() << "Animation named \"" << name << "\" doesn't exist";

    return *animationIt;
}
