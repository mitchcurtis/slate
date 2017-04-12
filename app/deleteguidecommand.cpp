/*
    Copyright 2017, Mitch Curtis

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

#include "deleteguidecommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcDeleteGuideCommand, "app.undo.deleteGuideCommand")

DeleteGuideCommand::DeleteGuideCommand(Project *project, const Guide &guide, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mGuide(guide)
{
    qCDebug(lcDeleteGuideCommand) << "constructed" << this;
}

void DeleteGuideCommand::undo()
{
    qCDebug(lcDeleteGuideCommand) << "undoing" << this;
    mProject->addGuide(mGuide);
}

void DeleteGuideCommand::redo()
{
    qCDebug(lcDeleteGuideCommand) << "redoing" << this;
    mProject->removeGuide(mGuide);
}

int DeleteGuideCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const DeleteGuideCommand *command)
{
    debug.nospace() << "(DeleteGuideCommand guide.position=" << command->mGuide.position()
        << " guide.orientation=" << command->mGuide.orientation()
        << ")";
    return debug.space();
}
