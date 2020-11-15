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

#include "changeanimationordercommand.h"

#include <QLoggingCategory>

#include "animation.h"
#include "animationsystem.h"

Q_LOGGING_CATEGORY(lcChangeAnimationOrderCommand, "app.undo.changeAnimationOrderCommand")

ChangeAnimationOrderCommand::ChangeAnimationOrderCommand(AnimationSystem *animationSystem,
        int oldIndex, int newIndex, UndoCommand *parent) :
    UndoCommand(parent),
    mAnimationSystem(animationSystem),
    mOldIndex(oldIndex),
    mNewIndex(newIndex)
{
    qCDebug(lcChangeAnimationOrderCommand) << "constructing" << this;
}

void ChangeAnimationOrderCommand::undo()
{
    qCDebug(lcChangeAnimationOrderCommand) << "undoing" << this;

    mAnimationSystem->moveAnimation(mNewIndex, mOldIndex);
}

void ChangeAnimationOrderCommand::redo()
{
    qCDebug(lcChangeAnimationOrderCommand) << "redoing" << this;

    mAnimationSystem->moveAnimation(mOldIndex, mNewIndex);
}

int ChangeAnimationOrderCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeAnimationOrderCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ChangeAnimationOrderCommand(0x0)";

    debug.nospace() << "(ChangeAnimationOrderCommand"
        << " oldIndex=" << command->mOldIndex
        << " newIndex=" << command->mNewIndex
        << ")";
    return debug;
}
