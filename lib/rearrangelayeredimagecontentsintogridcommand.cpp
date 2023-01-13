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

#include "rearrangelayeredimagecontentsintogridcommand.h"

#include <QLoggingCategory>

#include "layeredimageproject.h"

Q_LOGGING_CATEGORY(lcRearrangeLayeredImageContentsIntoGridCommand, "app.undo.rearrangeLayeredImageContentsIntoGridCommand")

RearrangeLayeredImageContentsIntoGridCommand::RearrangeLayeredImageContentsIntoGridCommand(LayeredImageProject *project,
    const QVector<QImage> &previousImages, const QVector<QImage> &newImages, UndoCommand *parent) :
    UndoCommand(parent),
    mProject(project),
    mPreviousImages(previousImages),
    mNewImages(newImages)
{
    qCDebug(lcRearrangeLayeredImageContentsIntoGridCommand) << "constructed" << this;
}

void RearrangeLayeredImageContentsIntoGridCommand::undo()
{
    qCDebug(lcRearrangeLayeredImageContentsIntoGridCommand) << "undoing" << this;
    mProject->doRearrangeContentsIntoGrid(mPreviousImages);
}

void RearrangeLayeredImageContentsIntoGridCommand::redo()
{
    qCDebug(lcRearrangeLayeredImageContentsIntoGridCommand) << "redoing" << this;
    mProject->doRearrangeContentsIntoGrid(mNewImages);
}

int RearrangeLayeredImageContentsIntoGridCommand::id() const
{
    return -1;
}

bool RearrangeLayeredImageContentsIntoGridCommand::modifiesContents() const
{
    return true;
}

QDebug operator<<(QDebug debug, const RearrangeLayeredImageContentsIntoGridCommand *)
{
    debug.nospace() << "(RearrangeLayeredImageContentsIntoGridCommand)";
    return debug.space();
}
