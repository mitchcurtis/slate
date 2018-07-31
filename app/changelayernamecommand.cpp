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

#include "changelayernamecommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcChangeLayerNameCommand, "app.undo.changeLayerNameCommand")

ChangeLayerNameCommand::ChangeLayerNameCommand(LayeredImageProject *project, int layerIndex, const QString &previousName,
    const QString &newName, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mLayerIndex(layerIndex),
    mPreviousName(previousName),
    mNewName(newName)
{
    qCDebug(lcChangeLayerNameCommand) << "constructed" << this;
}

void ChangeLayerNameCommand::undo()
{
    qCDebug(lcChangeLayerNameCommand) << "undoing" << this;
    mProject->layerAt(mLayerIndex)->setName(mPreviousName);
}

void ChangeLayerNameCommand::redo()
{
    qCDebug(lcChangeLayerNameCommand) << "redoing" << this;
    mProject->layerAt(mLayerIndex)->setName(mNewName);
}

int ChangeLayerNameCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeLayerNameCommand *command)
{
    debug.nospace() << "(ChangeLayerNameCommand layerIndex=" << command->mLayerIndex
        << " previousName=" << command->mPreviousName
        << " newName=" << command->mNewName
        << ")";
    return debug.space();
}
