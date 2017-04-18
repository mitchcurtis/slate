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

#include "applytileerasercommand.h"

#include <QLoggingCategory>

#include "tile.h"

Q_LOGGING_CATEGORY(lcApplyTileEraserCommand, "app.applyTileEraserCommand")

ApplyTileEraserCommand::ApplyTileEraserCommand(TileCanvas *canvas, const QPoint &tilePos,
    int previousId, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas)
{
    mTilePositions.append(tilePos);
    mPreviousIds.append(previousId);
}

void ApplyTileEraserCommand::undo()
{
    qCDebug(lcApplyTileEraserCommand) << "undoing tile eraser command:" << mTilePositions << mPreviousIds;
    for (int i = 0; i < mTilePositions.size(); ++i) {
        mCanvas->applyTilePenTool(mTilePositions.at(i), mPreviousIds.at(i));
    }
}

void ApplyTileEraserCommand::redo()
{
    qCDebug(lcApplyTileEraserCommand) << "redoing tile eraser command:" << mTilePositions;
    for (int i = 0; i < mTilePositions.size(); ++i) {
        mCanvas->applyTilePenTool(mTilePositions.at(i), Tile::invalidId());
    }
}

int ApplyTileEraserCommand::id() const
{
    return TileCanvas::TileMode;
}

bool ApplyTileEraserCommand::mergeWith(const QUndoCommand *other)
{
    const ApplyTileEraserCommand *otherCommand = dynamic_cast<const ApplyTileEraserCommand*>(other);
    if (!otherCommand) {
        return false;
    }

    // Duplicate position; we can just discard the other command.
    if (otherCommand->mTilePositions.size() == 1 && mTilePositions.contains(otherCommand->mTilePositions.first())) {
        return true;
    }

    // A unique tile that we haven't touched yet; add it.
    qCDebug(lcApplyTileEraserCommand) << "\nmerging:\n    " << *otherCommand << "\nwith:\n    " << *this;
    mTilePositions.append(otherCommand->mTilePositions);
    mPreviousIds.append(otherCommand->mPreviousIds);
    return true;
}

QDebug operator<<(QDebug debug, const ApplyTileEraserCommand &command)
{
    debug.nospace() << "(ApplyTileEraserCommand tilePositions=" << command.mTilePositions
        << ", previousIds=" << command.mPreviousIds
        << ")";
    return debug.space();
}
