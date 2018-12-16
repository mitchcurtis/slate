#include "bufferedundocommand.h"

BufferedUndoCommand::BufferedUndoCommand(const QUndoCommand *previousCommand, QUndoCommand *parent) :
    QUndoCommand(parent),
    mPreviousCommand(previousCommand)
{

}

void BufferedUndoCommand::undo()
{
    // Copy image from undo buffer
    QPainter painter(destImage());
    for (auto const &rect : mBufferRegion) {
        undoRect(painter, rect);
    }
}

void BufferedUndoCommand::redo()
{
/*    // Merging so initially skip drawing then draw after merge
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
                painter.drawImage(rect.topLeft() - mBufferBounds.topLeft(), *destImage(), rect);
            }
            painter.end();

//            // Draw stroke to image
//            painter.begin(destImage());
//            for (auto const &drawRect : instanceDrawRects) {
//                painter.save();
//                painter.setClipRect(drawRect.bounds);
//                painter.translate(drawRect.offset);
//                mCanvas->drawStroke(&painter, mStroke, mCompositionMode);
//                painter.restore();
//            }
//            painter.end();

            // Store changed image in redo buffer
            painter.begin(&mRedoBuffer);
            for (auto const &rect : bufferDrawRegion) {
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.drawImage(rect.topLeft() - mBufferBounds.topLeft(), *destImage(), rect);
            }
            painter.end();
        }

        needDraw = false;
    }
    // Otherwise copy image from redo buffer
    else {
        QPainter painter(destImage());
        for (auto const &rect : mBufferRegion) {
            redoRect(painter, rect);
        }
    }*/
}

bool BufferedUndoCommand::mergeWith(const QUndoCommand *const command)
{
    const BufferedUndoCommand *const newCommand = dynamic_cast<const BufferedUndoCommand *const>(command);
    if (!newCommand || !newCommand->canMerge(this)) return false;

    // Merge new stroke with old stroke
    mDrawBounds = newCommand->mDrawBounds;

    // Draw merged stroke
    mAllowMerge = false;
    needDraw = true;
    redo();

    return true;
}

void BufferedUndoCommand::undoRect(QPainter &painter, const QRect &rect) const
{
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(rect, mUndoBuffer, rect.translated(-mBufferBounds.topLeft()));
}

void BufferedUndoCommand::redoRect(QPainter &painter, const QRect &rect) const
{
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(rect, mRedoBuffer, rect.translated(-mBufferBounds.topLeft()));
}
