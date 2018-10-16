/*
    Copyright 2018, Mitch Curtis

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

#include "rotateimagecanvasselectioncommand.h"

#include "imagecanvas.h"

Q_LOGGING_CATEGORY(lcRotateImageCanvasSelectionCommand, "app.undo.rotateImageCanvasSelectionCommand")

RotateImageCanvasSelectionCommand::RotateImageCanvasSelectionCommand(ImageCanvas *canvas, int layerIndex,
        const QRect &area, int angle, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas),
    mLayerIndex(layerIndex),
    mAngle(angle),
    mArea(area)
{
    qCDebug(lcRotateImageCanvasSelectionCommand) << "constructed" << this;
}

void RotateImageCanvasSelectionCommand::undo()
{
    qCDebug(lcRotateImageCanvasSelectionCommand) << "undoing" << this;
    mCanvas->replacePortionOfImage(mLayerIndex, mTotalAffectedArea, mPreviousImagePortion);
    // This matches what mspaint does; undoing a selection rotation causes the selection to be cleared.
    mCanvas->clearSelection();
}

void RotateImageCanvasSelectionCommand::redo()
{
    qCDebug(lcRotateImageCanvasSelectionCommand) << "redoing" << this;
    // If rotating on a non-transparent background, we can't just call doRotateSelection()
    // with a reversed angle, because it won't clean up the now-transparent areas.
    // So, we store the whole affected area of the image: the rect that
    // contains both mArea and the rotated area.
    const QImage previousImage = *mCanvas->currentProjectImage();
    mTotalAffectedArea = mCanvas->doRotateSelection(mLayerIndex, mArea, mAngle);
    mPreviousImagePortion = previousImage.copy(mTotalAffectedArea);
    qCDebug(lcRotateImageCanvasSelectionCommand) << "redid" << this;
}

int RotateImageCanvasSelectionCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const RotateImageCanvasSelectionCommand *command)
{
    debug.nospace() << "(RotateImageCanvasSelectionCommand area=" << command->mArea
        << " totalAffectedArea=" << command->mTotalAffectedArea
        << " angle =" << command->mAngle
        << ")";
    return debug.space();
}
