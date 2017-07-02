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

#include "applygreedypixelfillcommand.h"

#include <QLoggingCategory>

#include "commands.h"
#include "imagecanvas.h"

Q_LOGGING_CATEGORY(lcApplyGreedyPixelFillCommand, "app.undo.applyGreedyPixelFillCommand")

ApplyGreedyPixelFillCommand::ApplyGreedyPixelFillCommand(ImageCanvas *canvas, const QVector<QPoint> &scenePositions,
    const QColor &previousColour, const QColor &colour, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mColour(colour)
{
    mScenePositions = scenePositions;
    mPreviousColour = previousColour;

    qCDebug(lcApplyGreedyPixelFillCommand) << "constructed" << this;
}

void ApplyGreedyPixelFillCommand::undo()
{
    qCDebug(lcApplyGreedyPixelFillCommand) << "undoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mPreviousColour);
    }
}

void ApplyGreedyPixelFillCommand::redo()
{
    qCDebug(lcApplyGreedyPixelFillCommand) << "redoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mColour);
    }
}

int ApplyGreedyPixelFillCommand::id() const
{
    return -1;
}

bool ApplyGreedyPixelFillCommand::mergeWith(const UndoCommand *)
{
    return false;
}

QDebug operator<<(QDebug debug, const ApplyGreedyPixelFillCommand *command)
{
    debug.nospace() << "(ApplyGreedyPixelFillCommand scenePositions=" << command->mScenePositions
        << ", previousColours=" << command->mPreviousColour
        << ", colour=" << command->mColour
        << ")";
    return debug.space();
}
