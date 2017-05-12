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

#include "moveimagecanvasselectioncommand.h"

#include "imagecanvas.h"
#include "imageproject.h"

Q_LOGGING_CATEGORY(lcMoveImageCanvasSelectionCommand, "app.undo.moveImageCanvasSelectionCommand")

MoveImageCanvasSelectionCommand::MoveImageCanvasSelectionCommand(ImageCanvas *canvas, const QRect &previousArea, const QImage &previousAreaImagePortion,
    const QRect &newArea, bool fromPaste, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mPreviousArea(previousArea),
    mPreviousAreaImagePortion(previousAreaImagePortion),
    mNewArea(newArea),
    mNewAreaImagePortion(canvas->mImageProject->image()->copy(newArea)),
    mFromPaste(fromPaste)
{
    qCDebug(lcMoveImageCanvasSelectionCommand) << "constructed" << this;
}

void MoveImageCanvasSelectionCommand::undo()
{
    qCDebug(lcMoveImageCanvasSelectionCommand) << "undoing" << this;
    mCanvas->paintImageOntoPortionOfImage(mPreviousArea, mPreviousAreaImagePortion);
    mCanvas->paintImageOntoPortionOfImage(mNewArea, mNewAreaImagePortion);
    // This matches what mspaint does; undoing a selection move causes the selection to be cleared.
    mCanvas->clearSelection();
}

void MoveImageCanvasSelectionCommand::redo()
{
    qCDebug(lcMoveImageCanvasSelectionCommand) << "redoing" << this;
    if (!mFromPaste)
        mCanvas->erasePortionOfImage(mPreviousArea);
    mCanvas->paintImageOntoPortionOfImage(mNewArea, mPreviousAreaImagePortion);
}

int MoveImageCanvasSelectionCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const MoveImageCanvasSelectionCommand &command)
{
    debug.nospace() << "(MoveImageCanvasSelectionCommand size=" << command.mNewArea
        << "previousSize=" << command.mPreviousArea
        << "fromPaste=" << command.mFromPaste
        << ")";
    return debug.space();
}
