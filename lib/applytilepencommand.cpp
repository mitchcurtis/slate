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

#include "applytilepencommand.h"

#include <QLoggingCategory>

#include "commands.h"

Q_LOGGING_CATEGORY(lcApplyTilePenCommand, "app.undo.applyTilePenCommand")

ApplyTilePenCommand::ApplyTilePenCommand(TileCanvas *canvas, const QPoint &tilePos,
    int previousId, int id, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mId(id)
{
    mTilePositions.append(tilePos);
    mPreviousIds.append(previousId);

    qCDebug(lcApplyTilePenCommand) << "constructed" << this;
}

void ApplyTilePenCommand::undo()
{
    qCDebug(lcApplyTilePenCommand) << "undoing" << this;
    for (int i = 0; i < mTilePositions.size(); ++i) {
        mCanvas->applyTilePenTool(mTilePositions.at(i), mPreviousIds.at(i));
    }
}

void ApplyTilePenCommand::redo()
{
    qCDebug(lcApplyTilePenCommand) << "redoing" << this;
    for (int i = 0; i < mTilePositions.size(); ++i) {
        mCanvas->applyTilePenTool(mTilePositions.at(i), mId);
    }
}

int ApplyTilePenCommand::id() const
{
    return ApplyTilePenCommandId;
}

bool ApplyTilePenCommand::mergeWith(const QUndoCommand *other)
{
    const ApplyTilePenCommand *otherCommand = dynamic_cast<const ApplyTilePenCommand*>(other);
    if (!otherCommand) {
        return false;
    }

    if (otherCommand->mId != mId) {
        return false;
    }

    // Duplicate position; we can just discard the other command.
    if (otherCommand->mTilePositions.size() == 1 && mTilePositions.contains(otherCommand->mTilePositions.first())) {
        return true;
    }

    // A unique tile that we haven't touched yet; add it.
    qCDebug(lcApplyTilePenCommand) << "\nmerging:\n    " << otherCommand << "\nwith:\n    " << this;
    mTilePositions.append(otherCommand->mTilePositions);
    mPreviousIds.append(otherCommand->mPreviousIds);
    return true;
}

QDebug operator<<(QDebug debug, const ApplyTilePenCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ApplyTilePenCommand(0x0)";

    debug.nospace() << "(ApplyTilePenCommand tilePositions=" << command->mTilePositions
        << ", previousIds=" << command->mPreviousIds
        << ", id=" << command->mId
        << ")";
    return debug;
}
