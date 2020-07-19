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

#include "moveguidecommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcMoveGuideCommand, "app.undo.moveGuideCommand")

MoveGuideCommand::MoveGuideCommand(Project *project, const Guide &guide, int newPosition, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mGuide(guide),
    mPreviousPosition(guide.position()),
    mNewPosition(newPosition)
{
    qCDebug(lcMoveGuideCommand) << "constructed" << this;
}

void MoveGuideCommand::undo()
{
    qCDebug(lcMoveGuideCommand) << "undoing" << this;
    mProject->moveGuide(Guide(mNewPosition, mGuide.orientation()), mPreviousPosition);
}

void MoveGuideCommand::redo()
{
    qCDebug(lcMoveGuideCommand) << "redoing" << this;
    mProject->moveGuide(mGuide, mNewPosition);
}

int MoveGuideCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const MoveGuideCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "MoveGuideCommand(0x0)";

    debug.nospace() << "(MoveGuideCommand guide.position=" << command->mGuide.position()
        << " guide.orientation=" << command->mGuide.orientation()
        << " newPosition" << command->mNewPosition
        << ")";
    return debug;
}
