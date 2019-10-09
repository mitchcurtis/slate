/*
    Copyright 2019, Mitch Curtis

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

#include "deletenotecommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcDeleteNoteCommand, "app.undo.deleteNoteCommand")

DeleteNoteCommand::DeleteNoteCommand(Project *project, const Note &note, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mNote(note)
{
    qCDebug(lcDeleteNoteCommand) << "constructed" << this;
}

void DeleteNoteCommand::undo()
{
    qCDebug(lcDeleteNoteCommand) << "undoing" << this;
    mProject->addNote(mNote);
}

void DeleteNoteCommand::redo()
{
    qCDebug(lcDeleteNoteCommand) << "redoing" << this;
    mProject->removeNote(mNote);
}

int DeleteNoteCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const DeleteNoteCommand *command)
{
    debug.nospace() << "(DeleteNoteCommand note.position=" << command->mNote.position()
        << " note.text=" << command->mNote.text()
        << ")";
    return debug.space();
}
