/*
    Copyright 2023, Mitch Curtis

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

#include "addlayercommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcAddLayerCommand, "app.undo.addLayerCommand")

AddLayerCommand::AddLayerCommand(LayeredImageProject *project, ImageLayer *layer, int index, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mIndex(index),
    mLayer(layer),
    mLayerGuard(layer)
{
    qCDebug(lcAddLayerCommand) << "constructed" << this;
}

void AddLayerCommand::undo()
{
    qCDebug(lcAddLayerCommand) << "undoing" << this;
    mProject->takeLayer(mIndex);

    // Prevent leaks.
    Q_ASSERT(!mLayerGuard.get());
    mLayerGuard.reset(mLayer);
}

void AddLayerCommand::redo()
{
    qCDebug(lcAddLayerCommand) << "redoing" << this;
    mProject->addLayer(mLayerGuard.release(), mIndex);
}

int AddLayerCommand::id() const
{
    return -1;
}

bool AddLayerCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const AddLayerCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "AddLayerCommand(0x0)";

    debug.nospace() << "(AddLayerCommand index=" << command->mIndex
        << " layer=" << command->mLayer << ")";
    return debug;
}
