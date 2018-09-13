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

#include "ApplyLayeredImagePixelFillCommand.h"

#include <QLoggingCategory>

#include "imagecanvas.h"

Q_LOGGING_CATEGORY(lcApplyLayeredImagePixelFillCommand, "app.undo.applyLayeredImagePixelFillCommand")

ApplyLayeredImagePixelFillCommand::ApplyLayeredImagePixelFillCommand(LayeredImageProject *project,
    const QImage &previousImage, const QImage &newImage, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mLayerIndex(-1)
{
    qCDebug(lcApplyLayeredImagePixelFillCommand) << "constructed" << this;
}

void ApplyLayeredImagePixelFillCommand::undo()
{
    qCDebug(lcApplyLayeredImagePixelFillCommand) << "undoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mPreviousColour);
    }
}

void ApplyLayeredImagePixelFillCommand::redo()
{
    qCDebug(lcApplyLayeredImagePixelFillCommand) << "redoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mColour);
    }
}

int ApplyLayeredImagePixelFillCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ApplyLayeredImagePixelFillCommand *command)
{
    debug.nospace() << "(ApplyLayeredImagePixelFillCommand
        << " previousColour=" << command->mPreviousColour
        << ", colour=" << command->mColour
        << ")";
    return debug.space();
}
