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

#include "modifyimagecanvasselectioncommand.h"

#include "imageproject.h"

Q_LOGGING_CATEGORY(lcModifyImageCanvasSelectionCommand, "app.undo.modifyImageCanvasSelectionCommand")

/*
    We use one command for all types of image selection modifications
    because we don't consider modifications to image selections as undoable:
    after a selection is made and modified (via e.g. moves, rotations, etc.)
    and then undone, it will go straight back to how the canvas was before those
    modifications, rather than stepping back one modification at a time.

    This also simplifies the code.
*/

ModifyImageCanvasSelectionCommand::ModifyImageCanvasSelectionCommand(ImageCanvas *canvas,
        int layerIndex, ImageCanvas::SelectionModification modification,
        const QRect &sourceArea, const QImage &sourceAreaImage,
        const QRect &targetArea, const QImage &targetAreaImageBeforeModification,
        const QImage &targetAreaImageAfterModification,
        bool fromPaste, const QImage &pasteContents, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mLayerIndex(layerIndex),
    mModification(modification),
    mSourceArea(sourceArea),
    mSouceAreaImage(sourceAreaImage),
    mTargetArea(targetArea),
    mTargetAreaImageBeforeModification(targetAreaImageBeforeModification),
    mTargetAreaImageAfterModification(targetAreaImageAfterModification),
    mFromPaste(fromPaste),
    mPasteContents(pasteContents),
    mUsed(false)
{
    qCDebug(lcModifyImageCanvasSelectionCommand) << "constructed" << this;
}

ModifyImageCanvasSelectionCommand::~ModifyImageCanvasSelectionCommand()
{
    qCDebug(lcModifyImageCanvasSelectionCommand) << "destructed" << this;
}

void ModifyImageCanvasSelectionCommand::undo()
{
    qCDebug(lcModifyImageCanvasSelectionCommand) << "replacing new/destination/undone area"
        << mTargetArea << "of canvas with" << mTargetAreaImageBeforeModification << "...";

    // Probably makes sense to do this first so that we follow the reverse order of the operations we did in redo().
    mCanvas->replacePortionOfImage(mLayerIndex, mTargetArea, mTargetAreaImageBeforeModification);

    if (mFromPaste) {
        qCDebug(lcModifyImageCanvasSelectionCommand) << "undoing" << this << "- ... and painting original/source/previous area"
            << mSourceArea << "of canvas with paste contents" << mPasteContents;

        mCanvas->paintImageOntoPortionOfImage(mLayerIndex, mSourceArea, mPasteContents);
    } else {
        qCDebug(lcModifyImageCanvasSelectionCommand) << "undoing" << this << "- ... and painting original/source/previous area"
            << mSourceArea << "of canvas with" << mSouceAreaImage;
        mCanvas->paintImageOntoPortionOfImage(mLayerIndex, mSourceArea, mSouceAreaImage);
    }

    // This matches what mspaint does; undoing a selection move causes the selection to be cleared.
    mCanvas->clearSelection();
}

void ModifyImageCanvasSelectionCommand::redo()
{
    qCDebug(lcModifyImageCanvasSelectionCommand) << "redoing" << this;
    if (!mFromPaste)
        mCanvas->erasePortionOfImage(mLayerIndex, mSourceArea);
    else if (mUsed) {
        // It is a paste and it has been redone already (moving contents that haven't been applied
        // to the canvas), so redoing it now means that we should apply the paste contents,
        // and not the previous area image portion.
        mCanvas->paintImageOntoPortionOfImage(mLayerIndex, mSourceArea, mSouceAreaImage);
    }

    mCanvas->paintImageOntoPortionOfImage(mLayerIndex, mTargetArea,
        mFromPaste ? mPasteContents : mTargetAreaImageAfterModification);

    mUsed = true;
}

int ModifyImageCanvasSelectionCommand::id() const
{
    return -1;
}

bool ModifyImageCanvasSelectionCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const ModifyImageCanvasSelectionCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ModifyImageCanvasSelectionCommand(0x0)";

    debug.nospace() << "(ModifyImageCanvasSelectionCommand"
        << " layerIndex=" << command->mLayerIndex
        << " modification=" << command->mModification
        << " newArea=" << command->mTargetArea
        << " previousSize=" << command->mSourceArea
        << " fromPaste=" << command->mFromPaste
        << ")";
    return debug;
}
