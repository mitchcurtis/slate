#ifndef BUFFEREDUNDOCOMMAND_H
#define BUFFEREDUNDOCOMMAND_H

#include <QUndoCommand>

#include "slate-global.h"

#include <QImage>
#include <QPainter>

class SLATE_EXPORT BufferedUndoCommand : public QUndoCommand
{
public:
    BufferedUndoCommand(const QUndoCommand *previousCommand = nullptr, QUndoCommand *parent = nullptr);
    virtual ~BufferedUndoCommand() override = 0;

    void undo() override;
    void redo() override;

    bool mergeWith(const QUndoCommand *const command) override;

protected:
    virtual void draw() = 0;
    virtual bool canMerge(const QUndoCommand *const command) const = 0;
    virtual QImage *destImage() = 0;

    void undoRect(QPainter &painter, const QRect &rect) const;
    void redoRect(QPainter &painter, const QRect &rect) const;

    QRect mDrawBounds;
    QRegion mBufferRegion;
    QImage mUndoBuffer;
    QImage mRedoBuffer;
    QRect mBufferBounds;

    bool mAllowMerge;
    const QUndoCommand *const mPreviousCommand;

    bool needDraw;
};

#endif // BUFFEREDUNDOCOMMAND_H
