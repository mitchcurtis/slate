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

#include "applypixelpencommand.h"

#include <QLoggingCategory>

#include "commands.h"

Q_LOGGING_CATEGORY(lcApplyPixelPenCommand, "app.undo.applyPixelPenCommand")

ApplyPixelPenCommand::ApplyPixelPenCommand(ImageCanvas *canvas, const QVector<QPoint> &scenePositions,
    const QVector<QColor> &previousColours, const QColor &colour, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mColour(colour)
{
    mScenePositions = scenePositions;
    mPreviousColours = previousColours;
}

void ApplyPixelPenCommand::undo()
{
    qCDebug(lcApplyPixelPenCommand) << "undoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mPreviousColours.at(i));
    }
}

void ApplyPixelPenCommand::redo()
{
    qCDebug(lcApplyPixelPenCommand) << "redoing" << this;
    for (int i = 0; i < mScenePositions.size(); ++i) {
        mCanvas->applyPixelPenTool(mScenePositions.at(i), mColour);
    }
}

int ApplyPixelPenCommand::id() const
{
    return ApplyPixelPenCommandId;
}

bool ApplyPixelPenCommand::mergeWith(const UndoCommand *other)
{
    const ApplyPixelPenCommand *otherCommand = qobject_cast<const ApplyPixelPenCommand*>(other);
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
    qCDebug(lcApplyPixelPenCommand) << "\nmerging:\n    " << *otherCommand << "\nwith:\n    " << *this;
    mScenePositions.append(otherCommand->mScenePositions);
    mPreviousColours.append(otherCommand->mPreviousColours);
    return true;
}

QDebug operator<<(QDebug debug, const ApplyPixelPenCommand &command)
{
    debug.nospace() << "(ApplyPixelPenCommand scenePositions=" << command.mScenePositions
        << ", previousColours=" << command.mPreviousColours
        << ", colour=" << command.mColour
        << ")";
    return debug.space();
}
