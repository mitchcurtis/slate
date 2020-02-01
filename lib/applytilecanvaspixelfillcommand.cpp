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

#include "applytilecanvaspixelfillcommand.h"

#include <QLoggingCategory>

#include "commands.h"
#include "tilecanvas.h"

Q_LOGGING_CATEGORY(lcApplyTileCanvasPixelFillCommand, "app.undo.applyTileCanvasPixelFillCommand")

ApplyTileCanvasPixelFillCommand::ApplyTileCanvasPixelFillCommand(TileCanvas *canvas, const QVector<QPoint> &scenePositions,
    const QColor &previousColour, const QColor &colour, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas),
    mColour(colour)
{
    mScenePositions = scenePositions;
    mPreviousColour = previousColour;

    qCDebug(lcApplyTileCanvasPixelFillCommand) << "constructed" << this;
}

void ApplyTileCanvasPixelFillCommand::undo()
{
    qCDebug(lcApplyTileCanvasPixelFillCommand) << "undoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(-1, mScenePositions.at(i), mPreviousColour);
    }
}

void ApplyTileCanvasPixelFillCommand::redo()
{
    qCDebug(lcApplyTileCanvasPixelFillCommand) << "redoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(-1, mScenePositions.at(i), mColour);
    }
}

int ApplyTileCanvasPixelFillCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ApplyTileCanvasPixelFillCommand *command)
{
    debug.nospace() << "(ApplyTileCanvasPixelFillCommand scenePositions=" << command->mScenePositions
        << ", previousColours=" << command->mPreviousColour
        << ", colour=" << command->mColour
        << ")";
    return debug.space();
}
