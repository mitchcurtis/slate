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

#include "tilesetswatchimage.h"

#include <QPainter>

#include "tileset.h"

TilesetSwatchImage::TilesetSwatchImage() :
    mTileset(nullptr)
{
}

Tileset *TilesetSwatchImage::tileset() const
{
    return mTileset;
}

void TilesetSwatchImage::setTileset(Tileset *tileset)
{
    if (tileset == mTileset)
        return;

    if (mTileset) {
        disconnect(mTileset, SIGNAL(imageChanged()), this, SIGNAL(update()));
        setSourceRect(QRect());
    }

    mTileset = tileset;

    if (tileset) {
        connect(mTileset, SIGNAL(imageChanged()), this, SLOT(update()));

        const QSize imageSize = tileset->image()->size();
        setImplicitSize(imageSize.width(), imageSize.height());

        setSourceRect(QRect(0, 0, imageSize.width(), imageSize.height()));
    }

    update();

    emit tilesetChanged();
}

QRect TilesetSwatchImage::sourceRect() const
{
    return mSourceRect;
}

void TilesetSwatchImage::setSourceRect(const QRect &sourceRect)
{
    if (sourceRect == mSourceRect)
        return;

    mSourceRect = sourceRect;
    emit sourceRectChanged();
}

void TilesetSwatchImage::paint(QPainter *painter)
{
    if (mTileset) {
        painter->drawImage(0, 0, *mTileset->image(),
            mSourceRect.x(), mSourceRect.y(), mSourceRect.width(), mSourceRect.height());
    }
}
