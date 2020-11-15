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

#include "addanimationcommand.h"

#include <QLoggingCategory>

#include "animation.h"
#include "animationsystem.h"

Q_LOGGING_CATEGORY(lcAddAnimationCommand, "app.undo.addAnimationCommand")

AddAnimationCommand::AddAnimationCommand(AnimationSystem *animationSystem, const QSize &projectSize, UndoCommand *parent) :
    UndoCommand(parent),
    mAnimationSystem(animationSystem),
    mIndex(animationSystem->animationCount()),
    mProjectSize(projectSize)
{
    qCDebug(lcAddAnimationCommand) << "constructed" << this;
}

void AddAnimationCommand::undo()
{
    qCDebug(lcAddAnimationCommand) << "undoing" << this;
    delete mAnimationSystem->takeAnimation(mIndex);
}

void AddAnimationCommand::redo()
{
    qCDebug(lcAddAnimationCommand) << "redoing" << this;
    mName = mAnimationSystem->addNewAnimation(mProjectSize);
}

int AddAnimationCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const AddAnimationCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "AddAnimationCommand(0x0)";

    debug.nospace() << "(AddAnimationCommand index=" << command->mIndex
        << " name=" << command->mName
        << " index= " << command->mIndex
        << " projectSize=" << command->mProjectSize << ")";
    return debug;
}
