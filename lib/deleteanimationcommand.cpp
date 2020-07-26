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

#include "deleteanimationcommand.h"

#include <QLoggingCategory>

#include "animation.h"
#include "animationsystem.h"

Q_LOGGING_CATEGORY(lcDeleteAnimationCommand, "app.undo.deleteAnimationCommand")

DeleteAnimationCommand::DeleteAnimationCommand(AnimationSystem *animationSystem, int index, QUndoCommand *parent) :
    QUndoCommand(parent),
    mAnimationSystem(animationSystem),
    mIndex(index)
{
    qCDebug(lcDeleteAnimationCommand) << "constructed" << this;
}

void DeleteAnimationCommand::undo()
{
    qCDebug(lcDeleteAnimationCommand) << "undoing" << this;
    mAnimationSystem->addAnimation(mAnimationGuard.take(), mIndex);
}

void DeleteAnimationCommand::redo()
{
    qCDebug(lcDeleteAnimationCommand) << "redoing" << this;
    mAnimationGuard.reset(mAnimationSystem->takeAnimation(mIndex));
}

int DeleteAnimationCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const DeleteAnimationCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "DeleteAnimationCommand(0x0)";

    debug.nospace() << "(DeleteAnimationCommand index=" << command->mIndex
        << " index= " << command->mIndex
        << " animation=" << command->mAnimationGuard.data() << ")";
    return debug;
}
