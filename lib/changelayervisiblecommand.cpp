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

#include "changelayervisiblecommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcChangeLayerVisibleCommand, "app.undo.changeLayerVisibleCommand")

ChangeLayerVisibleCommand::ChangeLayerVisibleCommand(LayeredImageProject *project, int layerIndex, bool previousVisible,
    bool newVisible, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mLayerIndex(layerIndex),
    mPreviousVisible(previousVisible),
    mNewVisible(newVisible)
{
    qCDebug(lcChangeLayerVisibleCommand) << "constructed" << this;
}

void ChangeLayerVisibleCommand::undo()
{
    qCDebug(lcChangeLayerVisibleCommand) << "undoing" << this;
    mProject->layerAt(mLayerIndex)->setVisible(mPreviousVisible);
}

void ChangeLayerVisibleCommand::redo()
{
    qCDebug(lcChangeLayerVisibleCommand) << "redoing" << this;
    mProject->layerAt(mLayerIndex)->setVisible(mNewVisible);
}

int ChangeLayerVisibleCommand::id() const
{
    return -1;
}

bool ChangeLayerVisibleCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const ChangeLayerVisibleCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ChangeLayerVisibleCommand(0x0)";

    debug.nospace() << "(ChangeLayerVisibleCommand layerIndex=" << command->mLayerIndex
        << " previousVisible=" << command->mPreviousVisible
        << " newVisible=" << command->mNewVisible
        << ")";
    return debug;
}
