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

public:
    explicit AnimationSystem(QObject *parent = nullptr);

    int currentAnimationIndex() const;
    void setCurrentAnimationIndex(int index);

    Animation *currentAnimation();

    AnimationPlayback *currentAnimationPlayback();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    void reset();

public slots:
    void addAnimation(const QSize &canvasSize);
    void removeAnimation(const QString &name);

signals:
    void currentAnimationIndexChanged();
    void scaleChanged();
    void loopChanged();
    void playingChanged();

private:
    QVector<Animation*>::iterator findAnimationWithName(const QString &name);
    Animation *animationAtNameOrWarn(const QString &name);

    QVector<Animation*> mAnimations;
    int mCurrentAnimationIndex = -1;
    AnimationPlayback mCurrentAnimationPlayback;
    int mAnimationsCreated = 0;
};

#endif // ANIMATIONSYSTEM_H
