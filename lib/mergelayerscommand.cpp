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

#include "mergelayerscommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcMergeLayersCommand, "app.undo.mergeLayersCommand")

MergeLayersCommand::MergeLayersCommand(LayeredImageProject *project,
        int sourceIndex, ImageLayer *sourceLayer, int targetIndex, ImageLayer *targetLayer,
    QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mSourceIndex(sourceIndex),
    mSourceLayer(sourceLayer),
    mTargetIndex(targetIndex),
    mTargetLayer(targetLayer),
    mPreviousTargetLayerImage(*mTargetLayer->image())
{
    qCDebug(lcMergeLayersCommand) << "constructed" << this;
}

void MergeLayersCommand::undo()
{
    qCDebug(lcMergeLayersCommand) << "undoing" << this;
    // Restore the source layer..
    mProject->addLayer(mSourceLayerGuard.take(), mSourceIndex);
    // .. and then restore the target layer.
    mProject->setLayerImage(mTargetIndex, mPreviousTargetLayerImage);
}

void MergeLayersCommand::redo()
{
    qCDebug(lcMergeLayersCommand) << "redoing" << this;
    mProject->mergeLayers(mSourceIndex, mTargetIndex);
    // The source layer loses its QObject parent, so manage it to prevent leaks.
    mSourceLayerGuard.reset(mSourceLayer);
}

int MergeLayersCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const MergeLayersCommand *command)
{
    debug.nospace() << "(MergeLayersCommand sourceIndex=" << command->mSourceIndex
        << " targetIndex=" << command->mTargetIndex
        << ")";
    return debug.space();
}
