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

#include "changetilecanvassizecommand.h"

#include <QLoggingCategory>

#include "tilesetproject.h"

Q_LOGGING_CATEGORY(lcChangeTileCanvasSizeCommand, "app.undo.changeTileCanvasSizeCommand")

ChangeTileCanvasSizeCommand::ChangeTileCanvasSizeCommand(TilesetProject *project, const QSize &previousSize,
    const QSize &size, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mPreviousSize(previousSize),
    mSize(size)
{
    mPreviousTiles = project->tiles();

    qCDebug(lcChangeTileCanvasSizeCommand) << "constructed" << this;
}

void ChangeTileCanvasSizeCommand::undo()
{
    qCDebug(lcChangeTileCanvasSizeCommand) << "undoing" << this;
    mProject->changeSize(mPreviousSize, mPreviousTiles);
}

void ChangeTileCanvasSizeCommand::redo()
{
    qCDebug(lcChangeTileCanvasSizeCommand) << "redoing" << this;
    mProject->changeSize(mSize);
}

int ChangeTileCanvasSizeCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeTileCanvasSizeCommand *command)
{
    debug.nospace() << "(ChangeTileCanvasSizeCommand size=" << command->mSize
        << "previousSize=" << command->mPreviousSize
        << ")";
    return debug.space();
}
