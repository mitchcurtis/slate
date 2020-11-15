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

#include "modifyanimationcommand.h"

#include <QLoggingCategory>

#include "animation.h"
#include "animationsystem.h"

Q_LOGGING_CATEGORY(lcModifyAnimationCommand, "app.undo.modifyAnimationCommand")

ModifyAnimationCommand::ModifyAnimationCommand(AnimationSystem *animationSystem, int index,
    const QString &name, int fps, int frameCount, int frameX, int frameY, int frameWidth, int frameHeight, bool reverse, UndoCommand *parent) :
    UndoCommand(parent),
    mAnimationSystem(animationSystem),
    mAnimation(mAnimationSystem->animationAt(index)),
    mIndex(index),
    mNewName(name),
    mNewFps(fps),
    mNewFrameCount(frameCount),
    mNewFrameX(frameX),
    mNewFrameY(frameY),
    mNewFrameWidth(frameWidth),
    mNewFrameHeight(frameHeight),
    mNewReverse(reverse),
    mOldName(mAnimation->name()),
    mOldFps(mAnimation->fps()),
    mOldFrameCount(mAnimation->frameCount()),
    mOldFrameX(mAnimation->frameX()),
    mOldFrameY(mAnimation->frameY()),
    mOldFrameWidth(mAnimation->frameWidth()),
    mOldFrameHeight(mAnimation->frameHeight()),
    mOldReverse(mAnimation->isReverse())

{
    qCDebug(lcModifyAnimationCommand) << "constructed" << this;
}

void ModifyAnimationCommand::undo()
{
    qCDebug(lcModifyAnimationCommand) << "undoing" << this;

    auto animation = mAnimationSystem->animationAt(mIndex);
    animation->setName(mOldName);
    animation->setFps(mOldFps);
    animation->setFrameCount(mOldFrameCount);
    animation->setFrameX(mOldFrameX);
    animation->setFrameY(mOldFrameY);
    animation->setFrameWidth(mOldFrameWidth);
    animation->setFrameHeight(mOldFrameHeight);
    animation->setReverse(mOldReverse);
    emit mAnimationSystem->animationModified(mIndex);
}

void ModifyAnimationCommand::redo()
{
    qCDebug(lcModifyAnimationCommand) << "redoing" << this;

    auto animation = mAnimationSystem->animationAt(mIndex);
    animation->setName(mNewName);
    animation->setFps(mNewFps);
    animation->setFrameCount(mNewFrameCount);
    animation->setFrameX(mNewFrameX);
    animation->setFrameY(mNewFrameY);
    animation->setFrameWidth(mNewFrameWidth);
    animation->setFrameHeight(mNewFrameHeight);
    animation->setReverse(mNewReverse);
    emit mAnimationSystem->animationModified(mIndex);
}

int ModifyAnimationCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ModifyAnimationCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ModifyAnimationCommand(0x0)";

    debug.nospace() << "(ModifyAnimationCommand index=" << command->mIndex << ")";
    return debug;
}
