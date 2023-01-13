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

#include "pasteacrosslayerscommand.h"

#include <QLoggingCategory>

#include "imagelayer.h"
#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcPasteAcrossLayersCommand, "app.undo.pasteAcrossLayersCommand")

PasteAcrossLayersCommand::PasteAcrossLayersCommand(LayeredImageProject *project,
        const QVector<QImage> &previousImages, const QVector<QImage> &newImages, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mPreviousImages(previousImages),
    mNewImages(newImages)
{
    qCDebug(lcPasteAcrossLayersCommand) << "constructed" << this;
}

void PasteAcrossLayersCommand::undo()
{
    qCDebug(lcPasteAcrossLayersCommand) << "undoing" << this;
    mProject->doPasteAcrossLayers(mPreviousImages);
}

void PasteAcrossLayersCommand::redo()
{
    qCDebug(lcPasteAcrossLayersCommand) << "redoing" << this;
    mProject->doPasteAcrossLayers(mNewImages);
}

int PasteAcrossLayersCommand::id() const
{
    return -1;
}

bool PasteAcrossLayersCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const PasteAcrossLayersCommand *)
{
    debug.nospace() << "(PasteAcrossLayersCommand)";
    return debug.space();
}
