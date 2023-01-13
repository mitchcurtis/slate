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

#include "changelayeropacitycommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcChangeLayerOpacityCommand, "app.undo.changeLayerOpacityCommand")

ChangeLayerOpacityCommand::ChangeLayerOpacityCommand(LayeredImageProject *project, int layerIndex, qreal previousOpacity,
    qreal newOpacity, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mLayerIndex(layerIndex),
    mPreviousOpacity(previousOpacity),
    mNewOpacity(newOpacity)
{
    qCDebug(lcChangeLayerOpacityCommand) << "constructed" << this;
}

void ChangeLayerOpacityCommand::undo()
{
    qCDebug(lcChangeLayerOpacityCommand) << "undoing" << this;
    mProject->layerAt(mLayerIndex)->setOpacity(mPreviousOpacity);
}

void ChangeLayerOpacityCommand::redo()
{
    qCDebug(lcChangeLayerOpacityCommand) << "redoing" << this;
    mProject->layerAt(mLayerIndex)->setOpacity(mNewOpacity);
}

int ChangeLayerOpacityCommand::id() const
{
    return -1;
}

bool ChangeLayerOpacityCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const ChangeLayerOpacityCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ChangeLayerOpacityCommand(0x0)";

    debug.nospace() << "(ChangeLayerOpacityCommand layerIndex=" << command->mLayerIndex
        << " previousOpacity=" << command->mPreviousOpacity
        << " newOpacity=" << command->mNewOpacity
        << ")";
    return debug;
}
