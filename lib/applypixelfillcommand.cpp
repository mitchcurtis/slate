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

#include "applypixelfillcommand.h"

#include <QLoggingCategory>

#include "imagecanvas.h"

Q_LOGGING_CATEGORY(lcApplyPixelFillCommand, "app.undo.applyPixelFillCommand")

ApplyPixelFillCommand::ApplyPixelFillCommand(ImageCanvas *canvas, int layerIndex,
    const QImage &previousImage, const QImage &newImage, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas),
    mLayerIndex(layerIndex),
    mPreviousImage(previousImage),
    mNewImage(newImage)
{
    qCDebug(lcApplyPixelFillCommand) << "constructed" << this;
}

void ApplyPixelFillCommand::undo()
{
    qCDebug(lcApplyPixelFillCommand) << "undoing" << this;
    mCanvas->replaceImage(mLayerIndex, mPreviousImage);
}

void ApplyPixelFillCommand::redo()
{
    qCDebug(lcApplyPixelFillCommand) << "redoing" << this;
    mCanvas->replaceImage(mLayerIndex, mNewImage);
}

int ApplyPixelFillCommand::id() const
{
    return -1;
}

QDebug operator<<(QDebug debug, const ApplyPixelFillCommand *command)
{
    debug.nospace() << "(ApplyPixelFillCommand"
        << " layerIndex=" << command->mLayerIndex
        << ")";
    return debug.space();
}
