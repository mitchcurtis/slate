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

#ifndef APPLYGREEDYPIXELFILLCOMMAND_H
#define APPLYGREEDYPIXELFILLCOMMAND_H

#include <QDebug>
#include <QImage>
#include <QUndoCommand>

#include "slate-global.h"

class ImageCanvas;

class SLATE_EXPORT ApplyGreedyPixelFillCommand : public QUndoCommand
{
public:
    ApplyGreedyPixelFillCommand(ImageCanvas *canvas, int layerIndex,const QImage &previousImage,
        const QImage &newImage, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

private:
    friend QDebug operator<<(QDebug debug, const ApplyGreedyPixelFillCommand *command);

    ImageCanvas *mCanvas;
    int mLayerIndex;
    QImage mPreviousImage;
    QImage mNewImage;
};

#endif // APPLYGREEDYPIXELFILLCOMMAND_H
