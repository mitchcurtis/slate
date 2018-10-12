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

RotateImageCanvasSelectionCommand::RotateImageCanvasSelectionCommand(ImageCanvas *canvas,
        const QRect &area, int angle, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas),
    mAngle(angle),
    mArea(area)
{
    qCDebug(lcRotateImageCanvasSelectionCommand) << "constructed" << this;
}

void RotateImageCanvasSelectionCommand::undo()
{
    qCDebug(lcRotateImageCanvasSelectionCommand) << "undoing" << this;
    mCanvas->doRotateSelection(mCanvas->currentLayerIndex(), mArea, -mAngle);
}

void RotateImageCanvasSelectionCommand::redo()
{
    qCDebug(lcRotateImageCanvasSelectionCommand) << "redoing" << this;
    mCanvas->doRotateSelection(mCanvas->currentLayerIndex(), mArea, mAngle);
}

int RotateImageCanvasSelectionCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const RotateImageCanvasSelectionCommand *command)
{
    debug.nospace() << "(RotateImageCanvasSelectionCommand area=" << command->mArea
        << "angle =" << command->mAngle
        << ")";
    return debug.space();
}
