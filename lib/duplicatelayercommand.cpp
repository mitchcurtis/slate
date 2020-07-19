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

#include "duplicatelayercommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcDuplicateLayerCommand, "app.undo.duplicateLayerCommand")

DuplicateLayerCommand::DuplicateLayerCommand(LayeredImageProject *project,
        int layerIndex, ImageLayer *layer, QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mLayerIndex(layerIndex),
    mLayer(layer),
    mLayerGuard(layer),
    mPreviousCurrentLayerIndex(project->currentLayerIndex())
{
    qCDebug(lcDuplicateLayerCommand) << "constructed" << this;
}

void DuplicateLayerCommand::undo()
{
    qCDebug(lcDuplicateLayerCommand) << "undoing" << this;
    // The layer loses its QObject parent, so manage it to prevent leaks.
    mLayerGuard.reset(mProject->takeLayer(mLayerIndex));
    mProject->setCurrentLayerIndex(mPreviousCurrentLayerIndex);
}

void DuplicateLayerCommand::redo()
{
    qCDebug(lcDuplicateLayerCommand) << "redoing" << this;
    mProject->addLayer(mLayerGuard.take(), mLayerIndex);
    mProject->setCurrentLayerIndex(mLayerIndex);
}

int DuplicateLayerCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const DuplicateLayerCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "DuplicateLayerCommand(0x0)";

    debug.nospace() << "(DuplicateLayerCommand sourceIndex=" << command->mLayerIndex
        << " layer=" << command->mLayer
        << ")";
    return debug;
}
