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

#ifndef APPLYPIXELLINECOMMAND_H
#define APPLYPIXELLINECOMMAND_H

#include <QDebug>
#include <QPointF>

#include "imagecanvas.h"
#include "slate-global.h"
#include "undocommand.h"

class SLATE_EXPORT ApplyPixelLineCommand : public UndoCommand
{
public:
    ApplyPixelLineCommand(ImageCanvas *canvas, int layerIndex, QImage &currentProjectImage, const QPointF &point1, const QPointF &point2,
        const QPointF &newLastPixelPenReleaseScenePos, const QPointF &oldLastPixelPenReleaseScenePos,
        QPainter::CompositionMode mode, UndoCommand *parent = nullptr);
    ~ApplyPixelLineCommand() override;

    void undo() override;
    void redo() override;

    int id() const override;
    bool mergeWith(const QUndoCommand *other) override;

    bool modifiesContents() const override;

private:
    friend QDebug operator<<(QDebug debug, const ApplyPixelLineCommand *command);

    ImageCanvas *mCanvas;
    int mLayerIndex;
    QPointF mNewLastPixelPenReleaseScenePos;
    QPointF mOldLastPixelPenReleaseScenePos;

    struct SubImageData {
        ImageCanvas::SubImage subImage;
        QRect lineRect;
        QImage imageWithoutLine;
        QImage imageWithLine;
    };
    QList<SubImageData> subImageDatas;
};


#endif // APPLYPIXELLINECOMMAND_H
