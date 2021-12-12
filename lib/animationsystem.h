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

#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include <QObject>
#include <QPoint>
#include <QQmlEngine>
#include <QSize>

#include "animation.h"
#include "animationplayback.h"
#include "slate-global.h"

class QJsonObject;

class SLATE_EXPORT AnimationSystem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(AnimationPlayback *currentAnimationPlayback READ currentAnimationPlayback CONSTANT FINAL)
    Q_PROPERTY(int currentAnimationIndex READ currentAnimationIndex WRITE setCurrentAnimationIndex NOTIFY currentAnimationIndexChanged)
    Q_PROPERTY(Animation *currentAnimation READ currentAnimation NOTIFY currentAnimationIndexChanged FINAL)
    Q_PROPERTY(Animation *editAnimation READ editAnimation CONSTANT FINAL)
    Q_PROPERTY(int animationCount READ animationCount NOTIFY animationCountChanged)
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit AnimationSystem(QObject *parent = nullptr);

    int currentAnimationIndex() const;
    void setCurrentAnimationIndex(int index);

    Animation *currentAnimation();
    Animation *editAnimation();

    AnimationPlayback *currentAnimationPlayback();

    bool containsAnimation(const QString &name) const;
    int indexOfAnimation(const QString &name) const;
    int animationCount() const;
    Animation *animationAt(int index);
    /*! Adds a new animation using default values, returning the generated name. */
    QString addNewAnimation(const QSize &canvasSize);
    /*! Adds the given animation, taking ownership of it. */
    void addAnimation(Animation *animation, int index);
    void moveAnimation(int fromIndex, int toIndex);
    Animation *takeAnimation(int index);
    Animation *findAnimationWithName(const QString &name);

    QString generateDuplicateName(const Animation *animation) const;

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void reset();

signals:
    void currentAnimationIndexChanged();
    void scaleChanged();
    void loopChanged();
    void playingChanged();
    void animationCountChanged();

    void preAnimationAdded(int index);
    void postAnimationAdded(int index);
    void animationModified(int index);
    void preAnimationMoved(int fromIndex, int toIndex);
    void postAnimationMoved(int fromIndex, int toIndex);
    void preAnimationRemoved(int index);
    void postAnimationRemoved(int index);

private:
    bool isValidIndexOrWarn(int index) const;
    bool isValidCurrentIndexOrWarn(int currentIndex) const;

    QString peekNextGeneratedName() const;
    QString takeNextGeneratedName();

    QVector<Animation*>::iterator findAnimationItWithName(const QString &name);
    QVector<Animation*>::const_iterator findAnimationItWithName(const QString &name) const;
    Animation *animationAtNameOrWarn(const QString &name);

    QVector<Animation*> mAnimations;
    int mCurrentAnimationIndex = -1;
    AnimationPlayback mCurrentAnimationPlayback;
    Animation mEditAnimation;
    int mAnimationsCreated = 0;
};

#endif // ANIMATIONSYSTEM_H
