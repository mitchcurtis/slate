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

#ifndef MOVEIMAGECANVASSELECTIONCOMMAND_H
#define MOVEIMAGECANVASSELECTIONCOMMAND_H

#include <QDebug>
#include <QImage>
#include <QRect>
#include <QUndoCommand>

class ImageCanvas;

class MoveImageCanvasSelectionCommand : public QUndoCommand
{
public:
    MoveImageCanvasSelectionCommand(ImageCanvas *canvas, int layerIndex, const QRect &previousArea,
        const QImage &previousAreaImagePortion, const QRect &newArea,
        bool fromPaste, const QImage &pasteContents, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

private:
    friend QDebug operator<<(QDebug debug, const MoveImageCanvasSelectionCommand &command);

    ImageCanvas *mCanvas;
    int mLayerIndex;
    QRect mPreviousArea;
    // The portion of the image under the selection before the selection was moved.
    QImage mPreviousAreaImagePortion;
    QRect mNewArea;
    // The portion of the image under the destination area, before the selection was moved.
    QImage mNewAreaImagePortion;
    bool mFromPaste;
    QImage mPasteContents;
    bool mUsed;
};

#endif // MOVEIMAGECANVASSELECTIONCOMMAND_H
