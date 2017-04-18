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

#include "applytilefillcommand.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcApplyTileFillCommand, "app.applyTileFillCommand")

ApplyTileFillCommand::ApplyTileFillCommand(TileCanvas *canvas, const QVector<QPoint> &tilePositions,
    int previousTile, int tile, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas),
    mPreviousTile(previousTile),
    mTile(tile)
{
    mTilePositions = tilePositions;
    mPreviousTile = previousTile;
}

void ApplyTileFillCommand::undo()
{
    qCDebug(lcApplyTileFillCommand) << "undoing tile fill command:" << mTilePositions << mPreviousTile;
    for (int i = 0; i < mTilePositions.size(); ++i) {
        mCanvas->applyTilePenTool(mTilePositions.at(i), mPreviousTile);
    }
}

void ApplyTileFillCommand::redo()
{
    qCDebug(lcApplyTileFillCommand) << "redoing tile fill command:" << mTilePositions << mTile;
    for (int i = 0; i < mTilePositions.size(); ++i) {
        mCanvas->applyTilePenTool(mTilePositions.at(i), mTile);
    }
}

int ApplyTileFillCommand::id() const
{
    return TileCanvas::TileMode;
}

bool ApplyTileFillCommand::mergeWith(const QUndoCommand *)
{
    return false;
}

QDebug operator<<(QDebug debug, const ApplyTileFillCommand &command)
{
    debug.nospace() << "(ApplyTileFillCommand scenePositions=" << command.mTilePositions
        << ", previousTile=" << command.mPreviousTile
        << ", tile=" << command.mTile
        << ")";
    return debug.space();
}
