/*
    Copyright 2016, Mitch Curtis

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

#include "changecanvassizecommand.h"

#include <QLoggingCategory>

#include "project.h"

Q_LOGGING_CATEGORY(lcChangeCanvasSizeCommand, "app.changeCanvasSizeCommand")

ChangeCanvasSizeCommand::ChangeCanvasSizeCommand(Project *project, const QSize &previousSize,
    const QSize &size, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mPreviousSize(previousSize),
    mSize(size)
{
    mPreviousTiles = project->tiles();
}

void ChangeCanvasSizeCommand::undo()
{
    qCDebug(lcChangeCanvasSizeCommand) << "undoing change canvas size command:" << mPreviousSize;
    mProject->changeSize(mPreviousSize, mPreviousTiles);
}

void ChangeCanvasSizeCommand::redo()
{
    qCDebug(lcChangeCanvasSizeCommand) << "redoing change canvas size command:" << mSize;
    mProject->changeSize(mSize);
}

int ChangeCanvasSizeCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeCanvasSizeCommand &command)
{
    debug.nospace() << "(ChangeCanvasSizeCommand size=" << command.mSize
        << "previousSize=" << command.mPreviousSize
        << ")";
    return debug.space();
}
