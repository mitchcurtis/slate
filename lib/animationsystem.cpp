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
#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcAnimationSystem, "app.animationSystem")

AnimationSystem::AnimationSystem(QObject *parent) :
    QObject(parent)
{
    mCurrentAnimationPlayback.setObjectName("animationSystemPlayback");
    mEditAnimation.setObjectName("editAnimation");
}

int AnimationSystem::currentAnimationIndex() const
{
    return mCurrentAnimationIndex;
}

void AnimationSystem::setCurrentAnimationIndex(int index)
{
    qCDebug(lcAnimationSystem()) << "setCurrentAnimationIndex called with" << index
        << "- mCurrentAnimationIndex is" << mCurrentAnimationIndex;

    if (!isValidCurrentIndexOrWarn(index))
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

Animation *AnimationSystem::editAnimation()
{
    return &mEditAnimation;
}

AnimationPlayback *AnimationSystem::currentAnimationPlayback()
{
    return &mCurrentAnimationPlayback;
}

bool AnimationSystem::containsAnimation(const QString &name) const
{
    return findAnimationItWithName(name) != mAnimations.end();
}

int AnimationSystem::indexOfAnimation(const QString &name) const
{
    const auto animationIt = findAnimationItWithName(name);
    if (animationIt == mAnimations.end())
        return -1;

    return std::distance(mAnimations.begin(), animationIt);
}

int AnimationSystem::animationCount() const
{
    return mAnimations.size();
}

Animation *AnimationSystem::animationAt(int index)
{
    if (!isValidIndexOrWarn(index))
        return nullptr;

    return mAnimations.at(index);
}

QString AnimationSystem::addNewAnimation(const QSize &canvasSize)
{
    const QString name = peekNextGeneratedName();
    auto existingAnimationIt = findAnimationItWithName(name);
    if (existingAnimationIt != mAnimations.end()) {
        qWarning().nospace() << "Animation named \"" << name << "\" already exists";
        return QString();
    }

    qCDebug(lcAnimationSystem()) << "adding new animation" << name;

    ++mAnimationsCreated;

    auto animation = new Animation();
    animation->setName(name);
    animation->setFps(4);
    animation->setFrameCount(canvasSize.width() >= 8 ? 4 : 1);
    animation->setFrameX(0);
    animation->setFrameY(0);
    animation->setFrameWidth(canvasSize.width() / animation->frameCount());
    animation->setFrameHeight(canvasSize.height());

    const int addIndex = mAnimations.size();
    addAnimation(animation, addIndex);

    return name;
}

void AnimationSystem::addAnimation(Animation *animation, int index)
{
    // Don't try to check for existing animations if we're adding one at the end.
    if (index < mAnimations.size()) {
        const int existingIndex = mAnimations.indexOf(animation);
        if (existingIndex != -1) {
            qWarning().nospace() << "Animation named \"" << animation->name()
                << "\" already exists (at index " << existingIndex << ")";
            return;
        }
    }

    // If we're adding the only animation, don't use isValidIndexOrWarn(),
    // as that assumes that it's not empty.
    // Similarly, if we're adding an animation at the end, it would assume that it's out of bounds.
    if (!(index == 0 && mAnimations.isEmpty()) && index != mAnimations.size() && !isValidIndexOrWarn(index))
        return;

    qCDebug(lcAnimationSystem()) << "adding new animation" << animation->name() << "at index" << index;

    emit preAnimationAdded(index);

    animation->setParent(this);

    mAnimations.insert(index, animation);

    if (mAnimations.size() == 1)
        setCurrentAnimationIndex(0);
    else if (index <= mCurrentAnimationIndex)
        setCurrentAnimationIndex(mCurrentAnimationIndex + 1);

    emit postAnimationAdded(index);
    emit animationCountChanged();

    qCDebug(lcAnimationSystem) << "animations after adding:";
    for (const auto animation : mAnimations)
        qCDebug(lcAnimationSystem) << "- " << animation;
}

void AnimationSystem::moveAnimation(int fromIndex, int toIndex)
{
    if (fromIndex == toIndex) {
        qWarning() << "Cannot move animation as fromIndex and toIndex are equal";
        return;
    }

    if (!isValidIndexOrWarn(fromIndex) || !isValidIndexOrWarn(toIndex))
        return;

    qCDebug(lcAnimationSystem) << "moving animation at index" << fromIndex << "to index" << toIndex;

    auto current = currentAnimation();
    emit preAnimationMoved(fromIndex, toIndex);

    mAnimations.move(fromIndex, toIndex);

    setCurrentAnimationIndex(mAnimations.indexOf(current));
    emit postAnimationMoved(fromIndex, toIndex);

    qCDebug(lcAnimationSystem) << "animations after moving:";
    for (const auto animation : mAnimations)
        qCDebug(lcAnimationSystem) << "- " << animation;
}

Animation *AnimationSystem::takeAnimation(int index)
{
    if (!isValidIndexOrWarn(index))
        return nullptr;

    emit preAnimationRemoved(index);

    if (index <= mCurrentAnimationIndex)
        setCurrentAnimationIndex(mCurrentAnimationIndex - 1);

    Animation *animation = mAnimations.takeAt(index);
    animation->setParent(nullptr);

    emit postAnimationRemoved(index);
    emit animationCountChanged();

    return animation;
}

Animation *AnimationSystem::findAnimationWithName(const QString &name)
{
    auto it = findAnimationItWithName(name);
    return it != mAnimations.end() ? *it : nullptr;
}

QString AnimationSystem::generateDuplicateName(const Animation *animation) const
{
    static int copyLimit = 1000;

    QString generatedName;
    int copyNumber = 1;
    bool nameAlreadyTaken = false;

    do {
        Q_ASSERT(!mAnimations.isEmpty());
        nameAlreadyTaken = false;

        for (const auto otherAnimation : mAnimations) {
            generatedName = animation->name() + " Copy";
            if (copyNumber > 1)
                generatedName += QString::fromLatin1(" #%1").arg(copyNumber);

            if (otherAnimation->name() == generatedName) {
                // There's already a copy with this name; we have to bump our number
                // and start the search from the beginning with a new number.
                ++copyNumber;

                // Should never happen, but just to be safe...
                if (copyNumber == copyLimit)
                    return QString();

                nameAlreadyTaken = true;
                break;
            }
        }
    } while (nameAlreadyTaken);

    return generatedName;
}

void AnimationSystem::read(const QJsonObject &json)
{
    // Pre-0.10.0 projects don't support multiple animations, so we
    // create an animation for them (and then later save it using the new format).
    if (json.contains("fps")) {
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
    } else {
        QJsonArray animationArray = json.value("animations").toArray();
        for (int i = 0; i < animationArray.size(); ++i) {
            QJsonObject layerObject = animationArray.at(i).toObject();
            QScopedPointer<Animation> animation(new Animation(this));
            animation->read(layerObject);
            mAnimations.append(animation.take());
        }

        mCurrentAnimationPlayback.read(json.value("currentAnimationPlayback").toObject());

        setCurrentAnimationIndex(json.value("currentAnimationIndex").toInt(-1));
    }
}

void AnimationSystem::write(QJsonObject &json) const
{
    QJsonObject playbackJson;
    mCurrentAnimationPlayback.write(playbackJson);
    json["currentAnimationPlayback"] = playbackJson;

    QJsonArray animationsArray;
    for (Animation *animation : qAsConst(mAnimations)) {
        QJsonObject animationObject;
        animation->write(animationObject);
        animationsArray.append(animationObject);
    }
    json["animations"] = animationsArray;

    json["currentAnimationIndex"] = mCurrentAnimationIndex;
}

void AnimationSystem::reset()
{
    mAnimations.clear();
    mCurrentAnimationIndex = -1;
    mCurrentAnimationPlayback.reset();
    mAnimationsCreated = 0;
}

bool AnimationSystem::isValidIndexOrWarn(int index) const
{
    if (index < 0 || index >= mAnimations.size()) {
        qWarning() << "Animation index" << index << "is invalid";
        return false;
    }
    return true;
}

bool AnimationSystem::isValidCurrentIndexOrWarn(int currentIndex) const
{
    // If there are no animations, allow setting -1 as the current index.
    if (mAnimations.isEmpty() && currentIndex == -1)
        return true;

    if (currentIndex < 0  || currentIndex >= mAnimations.size()) {
        qWarning() << "Index" << currentIndex << "is not a valid current index";
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

QVector<Animation*>::iterator AnimationSystem::findAnimationItWithName(const QString &name)
{
    return std::find_if(mAnimations.begin(), mAnimations.end(), [name](Animation *animation) {
        return animation->name() == name;
    });
}

QVector<Animation*>::const_iterator AnimationSystem::findAnimationItWithName(const QString &name) const
{
    return std::find_if(mAnimations.begin(), mAnimations.end(), [name](Animation *animation) {
        return animation->name() == name;
    });
}

Animation *AnimationSystem::animationAtNameOrWarn(const QString &name)
{
    auto animationIt = findAnimationItWithName(name);
    if (animationIt == mAnimations.end())
        qWarning().nospace() << "Animation named \"" << name << "\" doesn't exist";

    return *animationIt;
}
