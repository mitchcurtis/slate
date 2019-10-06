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

#include "movelayeredimagecontentscommand.h"

#include <QLoggingCategory>

#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcMoveLayeredImageContentsCommand, "app.undo.moveLayeredImageContentsCommand")

MoveLayeredImageContentsCommand::MoveLayeredImageContentsCommand(LayeredImageProject *project,
    const QVector<QImage> &previousImages, const QVector<QImage> &newImages, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mPreviousImages(previousImages),
    mNewImages(newImages)
{
    qCDebug(lcMoveLayeredImageContentsCommand) << "constructed" << this;
}

void MoveLayeredImageContentsCommand::undo()
{
    qCDebug(lcMoveLayeredImageContentsCommand) << "undoing" << this;
    mProject->doMoveContents(mPreviousImages);
}

void MoveLayeredImageContentsCommand::redo()
{
    qCDebug(lcMoveLayeredImageContentsCommand) << "redoing" << this;
    mProject->doMoveContents(mNewImages);
}

int MoveLayeredImageContentsCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const MoveLayeredImageContentsCommand *)
{
    debug.nospace() << "(MoveLayeredImageContentsCommand)";
    return debug.space();
}
