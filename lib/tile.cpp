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

#include "tile.h"

#include "tileset.h"

#include <QQmlEngine>

Tile::Tile() :
    mId(-1),
    mTileset(nullptr)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Tile::Tile(int id, const Tileset *tileset, const QRect &sourceRect, QObject *parent) :
    QObject(parent),
    mId(id),
    mSourceRect(sourceRect),
    mTileset(tileset)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

bool Tile::isValid() const
{
    return mId != -1;
}

int Tile::id() const
{
    return mId;
}

QRect Tile::sourceRect() const
{
    return mSourceRect;
}

QColor Tile::pixelColor(int x, int y) const
{
    if (!isValid() || x < 0 || x >= mSourceRect.width()
        || y < 0 || y >= mSourceRect.height())
        return QColor();

    return mTileset->image()->pixelColor(mSourceRect.x() + x, mSourceRect.y() + y);
}

QColor Tile::pixelColor(const QPoint &position) const
{
    return pixelColor(position.x(), position.y());
}

const Tileset *Tile::tileset() const
{
    // We use QPointer as an extra sanity check, hence the extra null check here.
    return isValid() && mTileset ? mTileset.data() : nullptr;
}

QImage Tile::image() const
{
    return isValid() ? mTileset->image()->copy(mSourceRect) : QImage();
}

int Tile::invalidId()
{
    return -1;
}

QDebug operator<<(QDebug debug, const Tile *tile)
{
    debug.nospace() << "(Tile id=" << tile->id()
        << ", sourceRect=" << tile->sourceRect()
        << "tileset=" << tile->tileset()->fileName() << ")";
    return debug.space();
}
