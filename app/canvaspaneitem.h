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

#ifndef CANVASPANEITEM_H
#define CANVASPANEITEM_H

#include <QQuickPaintedItem>

class CanvasPane;
class Guide;
class ImageCanvas;
class PaneDrawingHelper;

class CanvasPaneItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(ImageCanvas *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(CanvasPane *pane READ pane WRITE setPane NOTIFY paneChanged)
    Q_PROPERTY(int paneIndex READ paneIndex WRITE setPaneIndex NOTIFY paneIndexChanged)

public:
    explicit CanvasPaneItem(QQuickItem *parent = nullptr);
    ~CanvasPaneItem() override;

    void paint(QPainter *painter) override;

    ImageCanvas *canvas() const;
    void setCanvas(ImageCanvas *canvas);

    CanvasPane *pane() const;
    void setPane(CanvasPane *pane);

    int paneIndex() const;
    void setPaneIndex(int paneIndex);

signals:
    void canvasChanged();
    void paneChanged();
    void paneIndexChanged();

protected:
    void itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData &value) override;

    void connectToCanvas();
    void disconnectFromCanvas();

    ImageCanvas *mCanvas = nullptr;
    CanvasPane *mPane = nullptr;
    int mPaneIndex = -1;
};

#endif
