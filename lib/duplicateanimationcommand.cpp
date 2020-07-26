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

#include "duplicateanimationcommand.h"

#include <QLoggingCategory>

#include "animation.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcDuplicateAnimationCommand, "app.undo.duplicateAnimationCommand")

DuplicateAnimationCommand::DuplicateAnimationCommand(LayeredImageProject *project, int targetIndex, int duplicateIndex,
    const QString &name, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mTargetIndex(targetIndex),
    mDuplicateIndex(duplicateIndex)
{
    qCDebug(lcDuplicateAnimationCommand) << "constructing" << this;

    const auto targetAnimation = project->animationSystem()->animationAt(mTargetIndex);

    mAnimationGuard.reset(new Animation);
    mAnimationGuard->setName(name);
    mAnimationGuard->setFps(targetAnimation->fps());
    mAnimationGuard->setFrameCount(targetAnimation->frameCount());
    mAnimationGuard->setFrameX(targetAnimation->frameX());
    mAnimationGuard->setFrameY(targetAnimation->frameY());
    mAnimationGuard->setFrameWidth(targetAnimation->frameWidth());
    mAnimationGuard->setFrameHeight(targetAnimation->frameHeight());
}

void DuplicateAnimationCommand::undo()
{
    qCDebug(lcDuplicateAnimationCommand) << "undoing" << this;

    mAnimationGuard.reset(mProject->animationSystem()->takeAnimation(mDuplicateIndex));
}

void DuplicateAnimationCommand::redo()
{
    qCDebug(lcDuplicateAnimationCommand) << "redoing" << this;

    mProject->animationSystem()->addAnimation(mAnimationGuard.take(), mDuplicateIndex);
}

int DuplicateAnimationCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const DuplicateAnimationCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "DuplicateAnimationCommand(0x0)";

    debug.nospace() << "(DuplicateAnimationCommand"
        << " targetIndex=" << command->mTargetIndex
        << " duplicateIndex=" << command->mDuplicateIndex
        << " name=" << (command->mAnimationGuard ? command->mAnimationGuard->name() : "N/A")
        << ")";
    return debug;
}
