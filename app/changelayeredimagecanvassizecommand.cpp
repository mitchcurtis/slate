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

#include "changelayeredimagecanvassizecommand.h"

#include <QLoggingCategory>

#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcChangeLayeredImageCanvasSizeCommand, "app.undo.changeLayeredImageCanvasSizeCommand")

ChangeLayeredImageCanvasSizeCommand::ChangeLayeredImageCanvasSizeCommand(LayeredImageProject *project, const QSize &previousSize,
    const QSize &size, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mPreviousSize(previousSize),
    mSize(size)
{
    qCDebug(lcChangeLayeredImageCanvasSizeCommand) << "constructed" << this;
}

void ChangeLayeredImageCanvasSizeCommand::undo()
{
    qCDebug(lcChangeLayeredImageCanvasSizeCommand) << "undoing" << this;
    mProject->changeSize(mPreviousSize);
}

void ChangeLayeredImageCanvasSizeCommand::redo()
{
    qCDebug(lcChangeLayeredImageCanvasSizeCommand) << "redoing" << this;
    mProject->changeSize(mSize);
}

int ChangeLayeredImageCanvasSizeCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeLayeredImageCanvasSizeCommand *command)
{
    debug.nospace() << "(ChangeLayeredImageCanvasSizeCommand size=" << command->mSize
        << "previousSize=" << command->mPreviousSize
        << ")";
    return debug.space();
}
