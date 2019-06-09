/*
    Copyright 2018, Mitch Curtis

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

#ifndef SELECTIONCURSORGUIDE_H
#define SELECTIONCURSORGUIDE_H

#include <QQuickPaintedItem>

#include "slate-global.h"

class ImageCanvas;
class CanvasPane;

class SLATE_EXPORT SelectionCursorGuide : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit SelectionCursorGuide(ImageCanvas *canvas);
    ~SelectionCursorGuide() override;

    void paint(QPainter *painter) override;

private:
    void drawPane(QPainter *painter, const CanvasPane *pane, int paneIndex);

    ImageCanvas *mCanvas;
};

#endif
