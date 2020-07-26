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
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcChangeAnimationOrderCommand, "app.undo.changeAnimationOrderCommand")

ChangeAnimationOrderCommand::ChangeAnimationOrderCommand(LayeredImageProject *project, int oldIndex, int newIndex, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mOldIndex(oldIndex),
    mNewIndex(newIndex)
{
    qCDebug(lcChangeAnimationOrderCommand) << "constructing" << this;
}

void ChangeAnimationOrderCommand::undo()
{
    qCDebug(lcChangeAnimationOrderCommand) << "undoing" << this;

    mProject->animationSystem()->moveAnimation(mNewIndex, mOldIndex);
}

void ChangeAnimationOrderCommand::redo()
{
    qCDebug(lcChangeAnimationOrderCommand) << "redoing" << this;

    mProject->animationSystem()->moveAnimation(mOldIndex, mNewIndex);
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
