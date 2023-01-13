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

#ifndef DELETENOTECOMMAND_H
#define DELETENOTECOMMAND_H

#include <QDebug>

#include "note.h"
#include "slate-global.h"
#include "undocommand.h"

class Project;

class SLATE_EXPORT DeleteNoteCommand : public UndoCommand
{
public:
    DeleteNoteCommand(Project *project, const Note &note, UndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

private:
    friend QDebug operator<<(QDebug debug, const DeleteNoteCommand *command);

    Project *mProject;
    Note mNote;
};

#endif // DELETENOTECOMMAND_H
