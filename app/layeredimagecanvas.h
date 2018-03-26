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

#ifndef LAYEREDIMAGECANVAS_H
#define LAYEREDIMAGECANVAS_H

#include <QtUndo/undostack.h>

#include "imagecanvas.h"

class LayeredImageProject;

class LayeredImageCanvas : public ImageCanvas
{
    Q_OBJECT

public:
    LayeredImageCanvas();
    ~LayeredImageCanvas();

//    void paint(QPainter *painter) override;

signals:

public slots:

protected slots:
    void onPostLayerAdded(int index);
    void onPreLayerRemoved(int index);
    void onPostLayerRemoved();
    void onPostLayerMoved();
    void onLayerVisibleChanged();
    void onLayerOpacityChanged();
    void onPreCurrentLayerChanged();
    void onPostCurrentLayerChanged();

protected:
    void connectSignals() override;
    void disconnectSignals() override;
    QImage *currentProjectImage() override;
    const QImage *currentProjectImage() const override;
    QImage *imageForLayerAt(int layerIndex) override;
    QImage contentImage() const override;

    void replaceImage(int layerIndex, const QImage &replacementImage) override;

    bool areToolsForbidden() const override;

private:
//    friend class ApplyTileFillCommand;

//    void drawPane(QPainter *painter, const CanvasPane &pane, int paneIndex) override;

    LayeredImageProject *mLayeredImageProject;
};

#endif // LAYEREDIMAGECANVAS_H
