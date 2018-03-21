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

ChangeImageCanvasSizeCommand::ChangeImageCanvasSizeCommand(ImageProject *project, const QImage &previousImage,
    const QImage &newImage, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mPreviousImage(previousImage),
    mNewImage(newImage)
{
    qCDebug(lcChangeImageCanvasSizeCommand) << "constructed" << this;
}

void ChangeImageCanvasSizeCommand::undo()
{
    qCDebug(lcChangeImageCanvasSizeCommand) << "undoing" << this;
    mProject->doSetCanvasSize(mPreviousImage);
}

void ChangeImageCanvasSizeCommand::redo()
{
    qCDebug(lcChangeImageCanvasSizeCommand) << "redoing" << this;
    mProject->doSetCanvasSize(mNewImage);
}

int ChangeImageCanvasSizeCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeImageCanvasSizeCommand *command)
{
    debug.nospace() << "(ChangeImageCanvasSizeCommand new size=" << command->mNewImage.size()
        << "previous size=" << command->mPreviousImage.size()
        << ")";
    return debug.space();
}
