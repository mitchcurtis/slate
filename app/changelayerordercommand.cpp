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

#include "changelayerordercommand.h"

#include <QLoggingCategory>

#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcChangeLayerOrderCommand, "app.undo.changeLayerOrderCommand")

ChangeLayerOrderCommand::ChangeLayerOrderCommand(LayeredImageProject *project, int previousIndex, int newIndex,
    QUndoCommand *parent) :
    QUndoCommand(parent),
    mProject(project),
    mPreviousIndex(previousIndex),
    mNewIndex(newIndex)
{
    qCDebug(lcChangeLayerOrderCommand) << "constructed" << this;
}

void ChangeLayerOrderCommand::undo()
{
    qCDebug(lcChangeLayerOrderCommand) << "undoing" << this;
    mProject->moveLayer(mNewIndex, mPreviousIndex);
}

void ChangeLayerOrderCommand::redo()
{
    qCDebug(lcChangeLayerOrderCommand) << "redoing" << this;
    mProject->moveLayer(mPreviousIndex, mNewIndex);
}

int ChangeLayerOrderCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ChangeLayerOrderCommand *command)
{
    debug.nospace() << "(ChangeLayerOrderCommand newIndex=" << command->mNewIndex
        << "previousIndex=" << command->mPreviousIndex
        << ")";
    return debug.space();
}
