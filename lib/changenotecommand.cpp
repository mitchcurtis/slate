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

#include "changenotecommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcChangeNoteCommand, "app.undo.changeNoteCommand")

ChangeNoteCommand::ChangeNoteCommand(Project *project, int noteIndex, const Note &oldNote, const Note &newNote, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mNoteIndex(noteIndex),
    mOldNote(oldNote),
    mNewNote(newNote)
{
    qCDebug(lcChangeNoteCommand) << "constructed" << this;
}

void ChangeNoteCommand::undo()
{
    qCDebug(lcChangeNoteCommand) << "undoing" << this;
    mProject->modifyNote(mNoteIndex, mOldNote);
}

void ChangeNoteCommand::redo()
{
    qCDebug(lcChangeNoteCommand) << "redoing" << this;
    mProject->modifyNote(mNoteIndex, mNewNote);
}

int ChangeNoteCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeNoteCommand *command)
{
    debug.nospace() << "(MoveGuideCommand"
        << " note index=" << command->mNoteIndex
        << " old note=" << command->mOldNote
        << " new note" << command->mNewNote
        << ")";
    return debug.space();
}
