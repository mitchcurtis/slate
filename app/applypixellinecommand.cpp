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

#include "applypixellinecommand.h"

#include <QLoggingCategory>

#include "commands.h"

Q_LOGGING_CATEGORY(lcApplyPixelLineCommand, "app.undo.applyPixelLineCommand")

ApplyPixelLineCommand::ApplyPixelLineCommand(ImageCanvas *canvas, const QImage &imageWithLine,
    const QImage &imageWithoutLine, const QPoint &newLastPixelPenReleaseScenePos, const QPoint &oldLastPixelPenReleaseScenePos,
    UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mImageWithLine(imageWithLine),
    mImageWithoutLine(imageWithoutLine),
    mNewLastPixelPenReleaseScenePos(newLastPixelPenReleaseScenePos),
    mOldLastPixelPenReleaseScenePos(oldLastPixelPenReleaseScenePos)
{
    qCDebug(lcApplyPixelLineCommand) << "constructed" << this;
}

ApplyPixelLineCommand::~ApplyPixelLineCommand()
{
    qCDebug(lcApplyPixelLineCommand) << "destructed" << this;
}

void ApplyPixelLineCommand::undo()
{
    qCDebug(lcApplyPixelLineCommand) << "undoing" << this;
    mCanvas->applyPixelLineTool(mImageWithoutLine, mOldLastPixelPenReleaseScenePos);
}

void ApplyPixelLineCommand::redo()
{
    qCDebug(lcApplyPixelLineCommand) << "redoing" << this;
    mCanvas->applyPixelLineTool(mImageWithLine, mNewLastPixelPenReleaseScenePos);
}

int ApplyPixelLineCommand::id() const
{
    return ApplyPixelLineCommandId;
}

bool ApplyPixelLineCommand::mergeWith(const UndoCommand *)
{
    return false;
}

QDebug operator<<(QDebug debug, const ApplyPixelLineCommand *command)
{
    debug.nospace() << "(ApplyPixelLineCommand"
        << " newLastPixelPenReleaseScenePos=" << command->mNewLastPixelPenReleaseScenePos
        << ", oldLastPixelPenReleaseScenePos=" << command->mOldLastPixelPenReleaseScenePos
        << ")";
    return debug.space();
}
