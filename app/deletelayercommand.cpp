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

#include "deletelayercommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcDeleteLayerCommand, "app.undo.deleteLayerCommand")

DeleteLayerCommand::DeleteLayerCommand(LayeredImageProject *project, int index, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mIndex(index),
    mLayer(project->layerAt(index))
{
    qCDebug(lcDeleteLayerCommand) << "constructed" << this;
}

void DeleteLayerCommand::undo()
{
    qCDebug(lcDeleteLayerCommand) << "undoing" << this;
    mProject->addLayer(mLayer, mIndex);
}

void DeleteLayerCommand::redo()
{
    qCDebug(lcDeleteLayerCommand) << "redoing" << this;
    mProject->takeLayer(mIndex);
    // Prevent leaks.
    mLayer->setParent(this);
}

int DeleteLayerCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const DeleteLayerCommand *command)
{
    debug.nospace() << "(DeleteLayerCommand index=" << command->mIndex
        << " layer=" << command->mLayer << ")";
    return debug.space();
}
