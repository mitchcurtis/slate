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

#include "changeimagesizecommand.h"

#include <QLoggingCategory>

#include "imageproject.h"

Q_LOGGING_CATEGORY(lcChangeImageSizeCommand, "app.undo.ChangeImageSizeCommand")

ChangeImageSizeCommand::ChangeImageSizeCommand(ImageProject *project, const QImage &previousImage,
    const QImage &newImage, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mPreviousImage(previousImage),
    mNewImage(newImage)
{
    qCDebug(lcChangeImageSizeCommand) << "constructed" << this;
}

void ChangeImageSizeCommand::undo()
{
    qCDebug(lcChangeImageSizeCommand) << "undoing" << this;
    mProject->doResize(mPreviousImage);
}

void ChangeImageSizeCommand::redo()
{
    qCDebug(lcChangeImageSizeCommand) << "redoing" << this;
    mProject->doResize(mNewImage);
}

int ChangeImageSizeCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeImageSizeCommand *command)
{
    debug.nospace() << "(ChangeImageSizeCommand size=" << command->mNewImage.size()
        << "previousSize=" << command->mPreviousImage.size()
        << ")";
    return debug.space();
}
