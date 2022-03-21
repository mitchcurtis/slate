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

#ifndef MERGELAYERSCOMMAND_H
#define MERGELAYERSCOMMAND_H

#include <memory>

#include <QDebug>
#include <QImage>

#include "slate-global.h"
#include "undocommand.h"

class ImageLayer;
class LayeredImageProject;

class SLATE_EXPORT MergeLayersCommand : public UndoCommand
{
public:
    MergeLayersCommand(LayeredImageProject *project,
        int sourceIndex, ImageLayer *sourceLayer,
        int targetIndex, ImageLayer *targetLayer, UndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

    bool modifiesContents() const override;

private:
    friend QDebug operator<<(QDebug debug, const MergeLayersCommand *command);

    LayeredImageProject *mProject;
    int mSourceIndex;
    ImageLayer *mSourceLayer;
    std::unique_ptr<ImageLayer> mSourceLayerGuard;
    int mTargetIndex;
    ImageLayer *mTargetLayer;
    QImage mPreviousTargetLayerImage;
};

#endif // MERGELAYERSCOMMAND_H
