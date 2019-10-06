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

#ifndef PANEDRAWINGHELPER_H
#define PANEDRAWINGHELPER_H

#include <QPainter>

class CanvasPane;
class ImageCanvas;

/*!
    Does saving/restoring, translating, clipping, etc. for drawable items.
*/
class PaneDrawingHelper
{
public:
    explicit PaneDrawingHelper(const ImageCanvas *canvas, QPainter *painter, const CanvasPane *pane, int paneIndex);
    ~PaneDrawingHelper();

    const ImageCanvas *canvas() const;
    QPainter *painter();
    const CanvasPane *pane() const;
    int paneIndex() const;

private:
    const ImageCanvas *mCanvas;
    QPainter *mPainter;
    const CanvasPane *mPane;
    int mPaneIndex;
};

#endif
