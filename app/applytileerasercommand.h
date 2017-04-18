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

#ifndef APPLYTILEERASERCOMMAND_H
#define APPLYTILEERASERCOMMAND_H

#include <QColor>
#include <QDebug>
#include <QPoint>
#include <QVector>
#include <QUndoCommand>

#include "tilecanvas.h"

class ApplyTileEraserCommand : public QUndoCommand
{
public:
    ApplyTileEraserCommand(TileCanvas *canvas, const QPoint &tilePos, int previousId,
        QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;
    bool mergeWith(const QUndoCommand *other) override;

private:
    friend QDebug operator<<(QDebug debug, const ApplyTileEraserCommand &command);

    TileCanvas *mCanvas;
    QVector<QPoint> mTilePositions;
    QVector<int> mPreviousIds;
};


#endif // APPLYTILEERASERCOMMAND_H
