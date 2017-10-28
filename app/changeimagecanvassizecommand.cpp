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

#include "changeimagecanvassizecommand.h"

#include <QLoggingCategory>

#include "imageproject.h"

Q_LOGGING_CATEGORY(lcChangeImageCanvasSizeCommand, "app.undo.changeImageCanvasSizeCommand")

ChangeImageCanvasSizeCommand::ChangeImageCanvasSizeCommand(ImageProject *project, const QSize &previousSize,
    const QSize &size, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mPreviousSize(previousSize),
    mSize(size)
{
    qCDebug(lcChangeImageCanvasSizeCommand) << "constructed" << this;
}

void ChangeImageCanvasSizeCommand::undo()
{
    qCDebug(lcChangeImageCanvasSizeCommand) << "undoing" << this;
    mProject->doSetSize(mPreviousSize);
}

void ChangeImageCanvasSizeCommand::redo()
{
    qCDebug(lcChangeImageCanvasSizeCommand) << "redoing" << this;
    mProject->doSetSize(mSize);
}

int ChangeImageCanvasSizeCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeImageCanvasSizeCommand *command)
{
    debug.nospace() << "(ChangeImageCanvasSizeCommand size=" << command->mSize
        << "previousSize=" << command->mPreviousSize
        << ")";
    return debug.space();
}
