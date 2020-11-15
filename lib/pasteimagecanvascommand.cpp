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

#include "pasteimagecanvascommand.h"

#include "imagecanvas.h"
#include "imageproject.h"

Q_LOGGING_CATEGORY(lcPasteImageCanvasCommand, "app.undo.pasteImageCanvasCommand")

PasteImageCanvasCommand::PasteImageCanvasCommand(ImageCanvas *canvas, int layerIndex, const QImage &image,
    const QPoint &position, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mLayerIndex(layerIndex),
    mNewImage(image),
    mPreviousImage(canvas->imageForLayerAt(layerIndex)->copy(QRect(position, image.size()))),
    mArea(QRect(position, image.size())),
    mUsed(false)
{
    qCDebug(lcPasteImageCanvasCommand) << "constructed" << this;
}

void PasteImageCanvasCommand::undo()
{
    qCDebug(lcPasteImageCanvasCommand) << "undoing" << this;
    mCanvas->replacePortionOfImage(mLayerIndex, mArea, mPreviousImage);
    mCanvas->clearSelection();
}

void PasteImageCanvasCommand::redo()
{
    if (mUsed) {
        qCDebug(lcPasteImageCanvasCommand) << "redoing" << this;
        // ImageCanvas handles everything for us for the initial paste,
        // as we need a selection area on that occasion. However,
        // for every other redo and undo, we can do the following.
        mCanvas->paintImageOntoPortionOfImage(mLayerIndex, mArea, mNewImage);
        mCanvas->clearSelection();
    } else {
        // Although this special-casing might seem odd, the whole thing allows
        // pasting a selection and dragging it around without picking up the contents
        // of the area underneath it, as would occur if we didn't do this.
        qCDebug(lcPasteImageCanvasCommand).nospace() << "skipping redo of " << this
            << ", as ImageCanvas does it for us the first time";
    }
    mUsed = true;
}

int PasteImageCanvasCommand::id() const
{
    return -1;
}

bool PasteImageCanvasCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const PasteImageCanvasCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "PasteImageCanvasCommand(0x0)";

    debug.nospace() << "(PasteImageCanvasCommand area=" << command->mArea
        << ")";
    return debug.space();
}
