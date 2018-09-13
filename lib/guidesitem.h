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

#ifndef GUIDESITEM_H
#define GUIDESITEM_H

#include <QQuickPaintedItem>

#include "slate-global.h"

class CanvasPane;
class Guide;
class ImageCanvas;
class PaneDrawingHelper;

class SLATE_EXPORT GuidesItem : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit GuidesItem(ImageCanvas *canvas);
    ~GuidesItem();

    void paint(QPainter *painter) override;

private:
    void drawPane(QPainter *painter, const CanvasPane *pane, int paneIndex);
    void drawGuide(PaneDrawingHelper *paneDrawingHelper, const Guide *guide, int guideIndex);

    ImageCanvas *mCanvas;
};

#endif
