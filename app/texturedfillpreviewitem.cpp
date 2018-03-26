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

#include "texturedfillpreviewitem.h"

#include <QPainter>

#include "fillalgorithms.h"
#include "imagecanvas.h"
#include "project.h"
#include "texturedfillparameters.h"

TexturedFillPreviewItem::TexturedFillPreviewItem() :
    mCanvas(nullptr)
{
}

void TexturedFillPreviewItem::paint(QPainter *painter)
{
    if (!mCanvas)
        return;

    // Ensure that the target colour is not the same as the pen colour
    // by inverting it.
    QColor targetColour;
    if (mCanvas->penForegroundColour() == QColor(Qt::black)) {
        // Can't invert black, apparently.
        targetColour = Qt::white;
    } else {
        targetColour = ImageCanvas::invertedColour(mCanvas->penForegroundColour());
    }

    QImage image(width(), height(), QImage::Format_ARGB32_Premultiplied);
    image.fill(targetColour);
    image = texturedFill(&image, QPoint(0, 0), targetColour, mCanvas->penForegroundColour(), mParameters);

    painter->drawImage(0, 0, image);
}

ImageCanvas *TexturedFillPreviewItem::canvas() const
{
    return mCanvas;
}

void TexturedFillPreviewItem::setCanvas(ImageCanvas *canvas)
{
    if (canvas == mCanvas)
        return;

    if (mCanvas)
        mCanvas->disconnect(this);

    mCanvas = canvas;

    if (mCanvas) {
        connect(mParameters.hue(), &TexturedFillParameter::enabledChanged, [=]{ update(); });
        connect(mParameters.hue(), &TexturedFillParameter::varianceLowerBoundChanged, [=]{ update(); });
        connect(mParameters.hue(), &TexturedFillParameter::varianceUpperBoundChanged, [=]{ update(); });

        connect(mParameters.saturation(), &TexturedFillParameter::enabledChanged, [=]{ update(); });
        connect(mParameters.saturation(), &TexturedFillParameter::varianceLowerBoundChanged, [=]{ update(); });
        connect(mParameters.saturation(), &TexturedFillParameter::varianceUpperBoundChanged, [=]{ update(); });

        connect(mParameters.lightness(), &TexturedFillParameter::enabledChanged, [=]{ update(); });
        connect(mParameters.lightness(), &TexturedFillParameter::varianceLowerBoundChanged, [=]{ update(); });
        connect(mParameters.lightness(), &TexturedFillParameter::varianceUpperBoundChanged, [=]{ update(); });
    }

    update();

    emit canvasChanged();
}

TexturedFillParameters *TexturedFillPreviewItem::parameters()
{
    return &mParameters;
}
