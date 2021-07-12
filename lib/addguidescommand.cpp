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

#include "addguidescommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcAddGuidesCommand, "app.undo.addGuidesCommand")

AddGuidesCommand::AddGuidesCommand(Project *project, const QVector<Guide> &guides, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mGuides(guides)
{
    qCDebug(lcAddGuidesCommand) << "constructed" << this;
}

void AddGuidesCommand::undo()
{
    qCDebug(lcAddGuidesCommand) << "undoing" << this;
    mProject->removeGuides(mGuides);
}

void AddGuidesCommand::redo()
{
    qCDebug(lcAddGuidesCommand) << "redoing" << this;
    mProject->addGuides(mGuides);
}

int AddGuidesCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const AddGuidesCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "AddGuidesCommand(0x0)";

    debug.nospace() << "(AddGuidesCommand guides=" << command->mGuides
        << ")";
    return debug;
}
