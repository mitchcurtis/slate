/*
    Copyright 2019, Mitch Curtis

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

#ifndef CHANGELAYEREDIMAGESIZECOMMAND_H
#define CHANGELAYEREDIMAGESIZECOMMAND_H

#include <QDebug>
#include <QImage>
#include <QUndoCommand>
#include <QVector>

#include "slate-global.h"

class LayeredImageProject;

class SLATE_EXPORT ChangeLayeredImageSizeCommand : public QUndoCommand
{
public:
    ChangeLayeredImageSizeCommand(LayeredImageProject *project, const QVector<QImage> &previousImages,
        const QVector<QImage> &newImages, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

    int id() const override;

private:
    friend QDebug operator<<(QDebug debug, const ChangeLayeredImageSizeCommand *command);

    LayeredImageProject *mProject;
    QVector<QImage> mPreviousImages;
    QVector<QImage> mNewImages;
};

#endif // CHANGELAYEREDIMAGESIZECOMMAND_H
