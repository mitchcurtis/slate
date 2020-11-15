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
    mProject->doSetImageSize(mPreviousImage);
}

void ChangeImageSizeCommand::redo()
{
    qCDebug(lcChangeImageSizeCommand) << "redoing" << this;
    mProject->doSetImageSize(mNewImage);
}

int ChangeImageSizeCommand::id() const
{
    return -1;
}

bool ChangeImageSizeCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const ChangeImageSizeCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ChangeImageSizeCommand(0x0)";

    debug.nospace() << "(ChangeImageSizeCommand new size=" << command->mNewImage.size()
        << "previous size=" << command->mPreviousImage.size()
        << ")";
    return debug;
}
