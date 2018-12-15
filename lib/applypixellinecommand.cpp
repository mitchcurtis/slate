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

#include "applypixellinecommand.h"

#include <QLoggingCategory>
#include <QPainter>
#include <QImage>

#include "commands.h"

Q_LOGGING_CATEGORY(lcApplyPixelLineCommand, "app.undo.applyPixelLineCommand")

ApplyPixelLineCommand::ApplyPixelLineCommand(ImageCanvas *const canvas, const int layerIndex, const QVector<QPointF> &stroke, const QPointF &oldLastPixelPenReleaseScenePos,
        const QPainter::CompositionMode compositionMode, const bool allowMerge, const QUndoCommand *const previousCommand, QUndoCommand *const parent) :
    QUndoCommand(parent),
    mCanvas(canvas), mLayerIndex(layerIndex),
    mOldLastPixelPenReleaseScenePos(oldLastPixelPenReleaseScenePos),
    mCompositionMode(compositionMode),
    mStroke(stroke), mDrawBounds(ImageCanvas::strokeBounds(mStroke, mCanvas->toolSize())),
    mBufferRegion(), mUndoBuffer(), mRedoBuffer(), mBufferBounds(),
    mAllowMerge(allowMerge), mPreviousCommand(previousCommand), needDraw(true)
{
    qCDebug(lcApplyPixelLineCommand) << "constructed" << this;
}

ApplyPixelLineCommand::~ApplyPixelLineCommand()
{
    qCDebug(lcApplyPixelLineCommand) << "destructed" << this;
}

void ApplyPixelLineCommand::undo()
{
    qCDebug(lcApplyPixelLineCommand) << "undoing" << this;

    // Copy image from undo buffer
    QPainter painter(mCanvas->imageForLayerAt(mLayerIndex));
    for (auto const &rect : mBufferRegion) {
        undoRect(painter, rect);
    }
    mCanvas->requestContentPaint();

    mCanvas->mLastPixelPenPressScenePositionF = mOldLastPixelPenReleaseScenePos;
}

void ApplyPixelLineCommand::redo()
{
    qCDebug(lcApplyPixelLineCommand) << "redoing" << this;

    // Merging so initially skip drawing then draw after merge
    if (mAllowMerge && canMerge(mPreviousCommand)) {
        // Do nothing
        return;
    }

    // First "redo" so draw and store undo/redo buffers
    if (needDraw) {
        // Find intersections and offsets of draw area and subimages
        QVector<QRect> subImageRects;
        for (auto const &instance : mCanvas->subImageInstancesInBounds(mDrawBounds)) {
            const ImageCanvas::SubImage subImage = mCanvas->getSubImage(instance.index);
            const QPoint instanceDrawOffset = subImage.bounds.topLeft() - instance.position;
            const QRect imageSpaceDrawBounds = mDrawBounds.translated(instanceDrawOffset).intersected(subImage.bounds);
            subImageRects.append(imageSpaceDrawBounds);
        }

        // Find draw offsets for each instance in stroke bounds
        struct DrawRect {
            QRect bounds;
            QPoint offset;
        };
        QRegion bufferDrawRegion;
        QVector<DrawRect> instanceDrawRects;
        const QRect fullStrokeBounds = ImageCanvas::strokeBounds(mStroke, mCanvas->toolSize());
        for (auto const &subImageDrawRect : subImageRects) {
            for (auto const &instance : mCanvas->subImageInstancesInBounds(fullStrokeBounds)) {
                const ImageCanvas::SubImage subImage = mCanvas->getSubImage(instance.index);
                const QPoint instanceDrawOffset = subImage.bounds.topLeft() - instance.position;
                const QRect intersection = fullStrokeBounds.translated(instanceDrawOffset).intersected(subImageDrawRect);
                if (!intersection.isEmpty()) {
                    const DrawRect drawRect{intersection, instanceDrawOffset};
                    instanceDrawRects.append(drawRect);
                    bufferDrawRegion = bufferDrawRegion.united(intersection);
                }
            }
        }

        if (!instanceDrawRects.isEmpty()) {
            QPainter painter;

            const QRegion bufferRegionIntersection = bufferDrawRegion.intersected(mBufferRegion);

            // Restore area to be drawn from undo buffer
            painter.begin(mCanvas->imageForLayerAt(mLayerIndex));
            for (auto const &rect : bufferRegionIntersection) {
                undoRect(painter, rect);
            }
            painter.end();
            mBufferRegion = mBufferRegion.subtracted(bufferRegionIntersection);

            // Add draw region to buffer region
            const QRect oldBufferBounds = mBufferBounds;
            mBufferRegion = mBufferRegion.united(bufferDrawRegion);
            mBufferBounds = mBufferRegion.boundingRect();
            // Resize undo/redo buffers if bounds changed
            if (mBufferBounds != oldBufferBounds) {
                if (mUndoBuffer.isNull()) {
                    mUndoBuffer = QImage(mBufferBounds.size(), mCanvas->imageForLayerAt(mLayerIndex)->format());
                    mRedoBuffer = QImage(mBufferBounds.size(), mCanvas->imageForLayerAt(mLayerIndex)->format());
                }
                else {
                    const QRect copyRect = QRect(mBufferBounds.topLeft() - oldBufferBounds.topLeft(), mBufferBounds.size());
                    mUndoBuffer = mUndoBuffer.copy(copyRect);
                    mRedoBuffer = mRedoBuffer.copy(copyRect);
                }
            }

            // Store original image in undo buffer
            painter.begin(&mUndoBuffer);
            for (auto const &rect : bufferDrawRegion) {
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.drawImage(rect.topLeft() - mBufferBounds.topLeft(), *mCanvas->imageForLayerAt(mLayerIndex), rect);
            }
            painter.end();

            // Draw stroke to image
            painter.begin(mCanvas->imageForLayerAt(mLayerIndex));
            for (auto const &drawRect : instanceDrawRects) {
                painter.save();
                painter.setClipRect(drawRect.bounds);
                painter.translate(drawRect.offset);
                mCanvas->drawStroke(&painter, mStroke, mCompositionMode);
                painter.restore();
            }
            painter.end();

            // Store changed image in redo buffer
            painter.begin(&mRedoBuffer);
            for (auto const &rect : bufferDrawRegion) {
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.drawImage(rect.topLeft() - mBufferBounds.topLeft(), *mCanvas->imageForLayerAt(mLayerIndex), rect);
            }
            painter.end();
        }

        needDraw = false;
    }
    // Otherwise copy image from redo buffer
    else {
        QPainter painter(mCanvas->imageForLayerAt(mLayerIndex));
        for (auto const &rect : mBufferRegion) {
            redoRect(painter, rect);
        }
    }

    mCanvas->requestContentPaint();
    mCanvas->mLastPixelPenPressScenePositionF = mStroke.last();
}

int ApplyPixelLineCommand::id() const
{
    return ApplyPixelLineCommandId;
}

bool ApplyPixelLineCommand::mergeWith(const QUndoCommand *const command)
{
    const ApplyPixelLineCommand *const newCommand = dynamic_cast<const ApplyPixelLineCommand *const>(command);
    if (!newCommand || !newCommand->canMerge(this)) return false;

    // Merge new stroke with old stroke
    mDrawBounds = newCommand->mDrawBounds;
    mStroke.append(newCommand->mStroke.mid(1));

    // Draw merged stroke
    mAllowMerge = false;
    needDraw = true;
    redo();

    return true;
}

bool ApplyPixelLineCommand::canMerge(const QUndoCommand *const command) const
{
    if (!command) return false;
    if (command->id() != id()) return false;
    const ApplyPixelLineCommand *previousCommand = static_cast<const ApplyPixelLineCommand*>(command);
    if (previousCommand->mStroke.last() != mStroke.first() || previousCommand->mCanvas != mCanvas) return false;

    return true;
}

void ApplyPixelLineCommand::undoRect(QPainter &painter, const QRect &rect) const
{
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(rect, mUndoBuffer, rect.translated(-mBufferBounds.topLeft()));
//    painter.fillRect(rect, QColor(0, 0, 255));
}

void ApplyPixelLineCommand::redoRect(QPainter &painter, const QRect &rect) const
{
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(rect, mRedoBuffer, rect.translated(-mBufferBounds.topLeft()));
}

QDebug operator<<(QDebug debug, const ApplyPixelLineCommand *command)
{
    debug.nospace() << "(ApplyPixelLineCommand"
        << " layerIndex=" << command->mLayerIndex
        << ", stroke" << command->mStroke
        << ", oldLastPixelPenReleaseScenePos=" << command->mOldLastPixelPenReleaseScenePos
        << ")";
    return debug.space();
}
