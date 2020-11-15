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

#ifndef CHANGELAYERVISIBLECOMMAND_H
#define CHANGELAYERVISIBLECOMMAND_H

#include <QDebug>

#include "slate-global.h"
#include "undocommand.h"

class LayeredImageProject;

class SLATE_EXPORT ChangeLayerVisibleCommand : public UndoCommand
{
public:
    ChangeLayerVisibleCommand(LayeredImageProject *project, int layerIndex, bool previousVisible,
        bool newVisible, UndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

    bool modifiesContents() const override;

private:
    friend QDebug operator<<(QDebug debug, const ChangeLayerVisibleCommand *command);

    LayeredImageProject *mProject;
    int mLayerIndex;
    bool mPreviousVisible;
    bool mNewVisible;
};

#endif // CHANGELAYERVISIBLECOMMAND_H
