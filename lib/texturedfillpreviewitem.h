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

#ifndef TEXTUREDFILLPREVIEWITEM_H
#define TEXTUREDFILLPREVIEWITEM_H

#include <QImage>
#include <QQuickPaintedItem>

#include "slate-global.h"
#include "texturedfillparameters.h"

class ImageCanvas;

class SLATE_EXPORT TexturedFillPreviewItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(ImageCanvas *canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(TexturedFillParameters *parameters READ parameters CONSTANT FINAL)
    QML_ELEMENT
    Q_MOC_INCLUDE("imagecanvas.h")

public:
    TexturedFillPreviewItem();

    void paint(QPainter *painter) override;

    ImageCanvas *canvas() const;
    void setCanvas(ImageCanvas *canvas);

    TexturedFillParameters *parameters();

signals:
    void canvasChanged();
    void colourClicked(const QColor &colour);

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    ImageCanvas *mCanvas;
    TexturedFillParameters mParameters;
    QImage mPreviewImage;
};

#endif // TEXTUREDFILLPREVIEWITEM_H
