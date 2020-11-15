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

#include "addnotecommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcAddNoteCommand, "app.undo.addNoteCommand")

AddNoteCommand::AddNoteCommand(Project *project, const Note &note, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mNote(note)
{
    qCDebug(lcAddNoteCommand) << "constructed" << this;
}

void AddNoteCommand::undo()
{
    qCDebug(lcAddNoteCommand) << "undoing" << this;
    mProject->removeNote(mNote);
}

void AddNoteCommand::redo()
{
    qCDebug(lcAddNoteCommand) << "redoing" << this;
    mProject->addNote(mNote);
}

int AddNoteCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const AddNoteCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "AddNoteCommand(0x0)";

    debug.nospace() << "(AddNoteCommand note.position=" << command->mNote.position()
        << " note.text=" << command->mNote.text()
        << ")";
    return debug;
}
