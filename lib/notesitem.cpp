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

#include "notesitem.h"

#include "canvaspane.h"
#include "imagecanvas.h"
#include "note.h"
#include "panedrawinghelper.h"
#include "project.h"
#include "utils.h"

#include <QFont>
#include <QPainter>

NotesItem::NotesItem(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setObjectName("NotesItem");
    setRenderTarget(FramebufferObject);
}

NotesItem::~NotesItem()
{
}

ImageCanvas *NotesItem::canvas() const
{
    return mCanvas;
}

void NotesItem::setCanvas(ImageCanvas *newCanvas)
{
    if (mCanvas == newCanvas)
        return;

    if (mCanvas)
        mCanvas->disconnect(this);

    mCanvas = newCanvas;

    if (mCanvas) {
        connect(mCanvas, &ImageCanvas::notesChanged, this, [=]() { update(); });
    }

    emit canvasChanged();
}

CanvasPane *NotesItem::pane() const
{
    return mPane;
}

void NotesItem::setPane(CanvasPane *newPane)
{
    if (mPane == newPane)
        return;

    if (mPane)
        mPane->disconnect(this);

    mPane = newPane;

    if (mPane) {
        connect(mPane, &CanvasPane::zoomLevelChanged, this, [=]() { update(); });
        connect(mPane, &CanvasPane::integerOffsetChanged, this, [=]() { update(); });
        connect(mPane, &CanvasPane::sizeChanged, this, [=]() { update(); });
    }

    emit paneChanged();
}

int NotesItem::paneIndex() const
{
    return mPaneIndex;
}

void NotesItem::setPaneIndex(int paneIndex)
{
    if (mPaneIndex == paneIndex)
        return;

    mPaneIndex = paneIndex;
    emit paneIndexChanged();
}

void NotesItem::paint(QPainter *painter)
{
    if (!mCanvas->project()->hasLoaded())
        return;

    PaneDrawingHelper paneDrawingHelper(mCanvas, painter, mPane, mPaneIndex);

    QVector<Note> notes = mCanvas->project()->notes();
    for (int i = 0; i < notes.size(); ++i) {
        const Note note = notes.at(i);
        drawNote(&paneDrawingHelper, &note, i);
    }
}

void NotesItem::drawNote(PaneDrawingHelper *paneDrawingHelper, const Note *note, int noteIndex)
{
    QPainter *painter = paneDrawingHelper->painter();
    const CanvasPane *pane = paneDrawingHelper->pane();
    const int paneIndex = paneDrawingHelper->paneIndex();

    // If a note is currently being dragged, draw it in its dragged position.
    const bool draggingNote = mCanvas->pressedNoteIndex() == noteIndex;
    QPoint notePosition;
    if (draggingNote) {
        // The note is being dragged, so we have to render it at is preliminary position.
        const QPoint cursorPos(mCanvas->cursorSceneX(), mCanvas->cursorSceneY());
        notePosition = cursorPos - mCanvas->notePressOffset();
    } else {
        notePosition = note->position();
    }
    const QPoint zoomedNotePosition = notePosition * pane->integerZoomLevel() + QPoint(painter->pen().widthF() / 2.0, painter->pen().widthF() / 2.0);
    const QRect visibleSceneArea = mCanvas->paneVisibleSceneArea(paneIndex);

    // Don't bother drawing it if it's not visible within the scene.
    // We divide by the zoom level since visibleSceneArea was also created by doing that.
    if (visibleSceneArea.intersects(QRect(notePosition, note->size() / pane->integerZoomLevel()))) {
        painter->save();

        // We don't want to draw the text scaled.
        painter->setPen(QColor::fromRgb(0xffdead));
        painter->setBrush(QColor::fromRgb(0xffdead));
        painter->drawRect(QRect(zoomedNotePosition, note->size()));

        painter->setPen(QColor::fromRgb(0x000000));
        painter->drawText(QRect(zoomedNotePosition, note->size()), Qt::AlignCenter, note->text());

        painter->restore();
    }
}
