/*
    Copyright 2016, Mitch Curtis

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

#ifndef APPLYPIXELERASERCOMMAND_H
#define APPLYPIXELERASERCOMMAND_H

#include <QColor>
#include <QDebug>
#include <QPoint>
#include <QVector>
#include <QUndoCommand>

#include "imagecanvas.h"
#include "slate-global.h"

class SLATE_EXPORT ApplyPixelEraserCommand : public QUndoCommand
{
public:
    ApplyPixelEraserCommand(ImageCanvas *canvas, int layerIndex, const QVector<QPoint> &scenePositions, const QVector<QColor> &previousColours,
        QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;
    bool mergeWith(const QUndoCommand *other) override;

private:
    friend QDebug operator<<(QDebug debug, const ApplyPixelEraserCommand *command);

    ImageCanvas *mCanvas;
    int mLayerIndex;
    QVector<QPoint> mScenePositions;
    QVector<QColor> mPreviousColours;
};


#endif // APPLYPIXELERASERCOMMAND_H
