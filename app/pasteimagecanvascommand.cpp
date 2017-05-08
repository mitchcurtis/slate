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

#include "pasteimagecanvascommand.h"

#include "imagecanvas.h"
#include "imageproject.h"

PasteImageCanvasCommand::PasteImageCanvasCommand(ImageCanvas *canvas, const QImage &image,
    const QPoint &position, UndoCommand *parent) :
    UndoCommand(parent),
    mCanvas(canvas),
    mNewImage(image),
    mPreviousImage(canvas->mImageProject->image()->copy(QRect(position, image.size()))),
    mArea(QRect(position, image.size()))
{
}

void PasteImageCanvasCommand::undo()
{
    mCanvas->paintImageOntoPortionOfImage(mArea, mPreviousImage);
    mCanvas->clearSelection();
}

void PasteImageCanvasCommand::redo()
{
    mCanvas->paintImageOntoPortionOfImage(mArea, mNewImage);
}

int PasteImageCanvasCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const PasteImageCanvasCommand &command)
{
    debug.nospace() << "(PasteImageCanvasCommand area=" << command.mArea
        << ")";
    return debug.space();
}
