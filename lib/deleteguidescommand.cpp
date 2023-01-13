/*
    Copyright 2023, Mitch Curtis

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

#include "deleteguidescommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcDeleteGuidesCommand, "app.undo.deleteGuidesCommand")

DeleteGuidesCommand::DeleteGuidesCommand(Project *project, const QVector<Guide> &guides, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mGuides(guides)
{
    qCDebug(lcDeleteGuidesCommand) << "constructed" << this;
}

void DeleteGuidesCommand::undo()
{
    qCDebug(lcDeleteGuidesCommand) << "undoing" << this;
    mProject->addGuides(mGuides);
}

void DeleteGuidesCommand::redo()
{
    qCDebug(lcDeleteGuidesCommand) << "redoing" << this;
    mProject->removeGuides(mGuides);
}

int DeleteGuidesCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const DeleteGuidesCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "DeleteGuidesCommand(0x0)";

    debug.nospace() << "(DeleteGuidesCommand guides=" << command->mGuides
        << ")";
    return debug;
}
