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

#include "flipimagecanvasselectioncommand.h"

#include "imagecanvas.h"

Q_LOGGING_CATEGORY(lcFlipImageCanvasSelectionCommand, "app.undo.flipImageCanvasSelectionCommand")

FlipImageCanvasSelectionCommand::FlipImageCanvasSelectionCommand(ImageCanvas *canvas,
        const QRect &area, Qt::Orientation orientation, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas),
    mOrientation(orientation),
    mArea(area)
{
    qCDebug(lcFlipImageCanvasSelectionCommand) << "constructed" << this;
}

void FlipImageCanvasSelectionCommand::undo()
{
    qCDebug(lcFlipImageCanvasSelectionCommand) << "undoing" << this;
    mCanvas->doFlipSelection(mCanvas->currentLayerIndex(), mArea, mOrientation);
}

void FlipImageCanvasSelectionCommand::redo()
{
    qCDebug(lcFlipImageCanvasSelectionCommand) << "redoing" << this;
    mCanvas->doFlipSelection(mCanvas->currentLayerIndex(), mArea, mOrientation);
}

int FlipImageCanvasSelectionCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const FlipImageCanvasSelectionCommand *command)
{
    debug.nospace() << "(FlipImageCanvasSelectionCommand area=" << command->mArea
        << "orientation =" << command->mOrientation
        << ")";
    return debug.space();
}
