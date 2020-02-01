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

#ifndef MODIFYIMAGECANVASSELECTIONCOMMAND_H
#define MODIFYIMAGECANVASSELECTIONCOMMAND_H

#include <QDebug>
#include <QImage>
#include <QRect>
#include <QUndoCommand>

#include "imagecanvas.h"
#include "slate-global.h"

class SLATE_EXPORT ModifyImageCanvasSelectionCommand : public QUndoCommand
{
public:
    ModifyImageCanvasSelectionCommand(ImageCanvas *canvas, int layerIndex,
        ImageCanvas::SelectionModification modification,
        const QRect &sourceArea, const QImage &sourceAreaImage,
        const QRect &targetArea, const QImage &targetAreaImageBeforeModification,
        const QImage &targetAreaImageAfterModification,
        bool fromPaste, const QImage &pasteContents, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

private:
    friend QDebug operator<<(QDebug debug, const ModifyImageCanvasSelectionCommand *command);

    ImageCanvas *mCanvas;
    int mLayerIndex;
    // The most recent modification, just for debug purposes right now.
    ImageCanvas::SelectionModification mModification;
    // The area that the selection started off at.
    QRect mSourceArea;
    // The portion of the image under the selection before the selection was moved.
    QImage mSouceAreaImage;
    // The area that the selection finished at, due to moving, rotating, etc.
    QRect mTargetArea;
    // The portion of the image under the destination area, after the selection was moved.
    QImage mTargetAreaImageBeforeModification;
    QImage mTargetAreaImageAfterModification;
    bool mFromPaste;
    QImage mPasteContents;
    bool mUsed;
};

#endif // MODIFYIMAGECANVASSELECTIONCOMMAND_H
