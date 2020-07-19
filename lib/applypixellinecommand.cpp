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

#include "applypixellinecommand.h"

#include <QLoggingCategory>
#include <QPainter>
#include <QImage>

#include "commands.h"

Q_LOGGING_CATEGORY(lcApplyPixelLineCommand, "app.undo.applyPixelLineCommand")

// The undo command for lines needs the project image before and after
// the line was drawn on it.
ApplyPixelLineCommand::ApplyPixelLineCommand(ImageCanvas *canvas, int layerIndex, QImage &currentProjectImage, const QPointF &point1, const QPointF &point2,
        const QPointF &newLastPixelPenReleaseScenePos, const QPointF &oldLastPixelPenReleaseScenePos,
        QPainter::CompositionMode mode, QUndoCommand *parent) :
    QUndoCommand(parent),
    mCanvas(canvas),
    mLayerIndex(layerIndex),
    mNewLastPixelPenReleaseScenePos(newLastPixelPenReleaseScenePos),
    mOldLastPixelPenReleaseScenePos(oldLastPixelPenReleaseScenePos)
{
    const QRect lineRect = mCanvas->normalisedLineRect(point1, point2);
    const QList<ImageCanvas::SubImage> subImages = canvas->subImagesInBounds(lineRect);
    for (auto const &subImage : subImages) {
        // subimage-space to scene-space offset
        const QPoint offset = subImage.bounds.topLeft() - subImage.offset;
        // line rect offset to scene space and clipped to subimage bounds
        const QRect subImageLineRect = subImage.bounds.intersected(lineRect.translated(offset));

        SubImageData subImageData;
        subImageData.subImage = subImage;
        subImageData.lineRect = subImageLineRect;
        subImageData.imageWithoutLine = currentProjectImage.copy(subImageLineRect);

        QPainter painter(&currentProjectImage);
        // Clip drawing to subimage
        painter.setClipRect(subImageLineRect);
        // Draw line with offset to subimage
        mCanvas->drawLine(&painter, point1 + offset, point2 + offset, mode);
        painter.end();

        subImageData.imageWithLine = currentProjectImage.copy(subImageLineRect);
        subImageDatas.append(subImageData);
    }

    qCDebug(lcApplyPixelLineCommand) << "constructed" << this;
}

ApplyPixelLineCommand::~ApplyPixelLineCommand()
{
    qCDebug(lcApplyPixelLineCommand) << "destructed" << this;
}

void ApplyPixelLineCommand::undo()
{
    qCDebug(lcApplyPixelLineCommand) << "undoing" << this;
    for (auto const &subImageData : qAsConst(subImageDatas)) {
        mCanvas->applyPixelLineTool(mLayerIndex, subImageData.imageWithoutLine, subImageData.lineRect, mOldLastPixelPenReleaseScenePos);
    }
}

void ApplyPixelLineCommand::redo()
{
    qCDebug(lcApplyPixelLineCommand) << "redoing" << this;
    for (auto const &subImageData : qAsConst(subImageDatas)) {
        mCanvas->applyPixelLineTool(mLayerIndex, subImageData.imageWithLine, subImageData.lineRect, mNewLastPixelPenReleaseScenePos);
    }
}

int ApplyPixelLineCommand::id() const
{
    return ApplyPixelLineCommandId;
}

bool ApplyPixelLineCommand::mergeWith(const QUndoCommand *)
{
    return false;
}

QDebug operator<<(QDebug debug, const ApplyPixelLineCommand *command)
{
    QDebugStateSaver saver(debug);
    if (!command)
        return debug << "ApplyPixelLineCommand(0x0)";

    debug.nospace() << "(ApplyPixelLineCommand"
        << " layerIndex=" << command->mLayerIndex
//        << ", lineRect" << command->mLineRect
        << ", newLastPixelPenReleaseScenePos=" << command->mNewLastPixelPenReleaseScenePos
        << ", oldLastPixelPenReleaseScenePos=" << command->mOldLastPixelPenReleaseScenePos
        << ")";
    return debug;
}
