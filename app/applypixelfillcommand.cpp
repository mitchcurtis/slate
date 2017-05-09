/*
    Copyright 2016, Mitch Curtis

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

#include "applypixelfillcommand.h"

#include <QLoggingCategory>

#include "commands.h"
#include "imagecanvas.h"

Q_LOGGING_CATEGORY(lcApplyPixelFillCommand, "app.undo.applyPixelFillCommand")

ApplyPixelFillCommand::ApplyPixelFillCommand(ImageCanvas *canvas, const QVector<QPoint> &scenePositions,
    const QColor &previousColour, const QColor &colour, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mColour(colour)
{
    mScenePositions = scenePositions;
    mPreviousColour = previousColour;

    qCDebug(lcApplyPixelFillCommand) << "constructed" << this;
}

void ApplyPixelFillCommand::undo()
{
    qCDebug(lcApplyPixelFillCommand) << "undoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mPreviousColour);
    }
}

void ApplyPixelFillCommand::redo()
{
    qCDebug(lcApplyPixelFillCommand) << "redoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mColour);
    }
}

int ApplyPixelFillCommand::id() const
{
    return ApplyPixelFillCommandId;
}

bool ApplyPixelFillCommand::mergeWith(const UndoCommand *other)
{
    const ApplyPixelFillCommand *otherCommand = qobject_cast<const ApplyPixelFillCommand*>(other);
    if (!otherCommand) {
        return false;
    }

    if (otherCommand->mColour != mColour) {
        return false;
    }

    // Duplicate pixel; we can just discard the other command.
    if (otherCommand->mScenePositions.size() == 1 && mScenePositions.contains(otherCommand->mScenePositions.first())) {
        return true;
    }

    // A unique pixel that we haven't touched yet; add it.
    qCDebug(lcApplyPixelFillCommand) << "\nmerging:\n    " << *otherCommand << "\nwith:\n    " << *this;
    mScenePositions.append(otherCommand->mScenePositions);
    return true;
}

QDebug operator<<(QDebug debug, const ApplyPixelFillCommand &command)
{
    debug.nospace() << "(ApplyPixelFillCommand scenePositions=" << command.mScenePositions
        << ", previousColours=" << command.mPreviousColour
        << ", colour=" << command.mColour
        << ")";
    return debug.space();
}
