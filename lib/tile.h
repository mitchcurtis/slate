/*
    Copyright 2023, Mitch Curtis

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

#ifndef TILE_H
#define TILE_H

#include <QDebug>
#include <QImage>
#include <QObject>
#include <QPointer>
#include <QRect>
#include <QString>

#include "slate-global.h"

class Tileset;

class SLATE_EXPORT Tile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QRect sourceRect READ sourceRect CONSTANT)

public:
    Tile();
    Tile(int id, const Tileset *tileset, const QRect &sourceRect, QObject *parent);

    bool isValid() const;
    int id() const;
    QRect sourceRect() const;
    QColor pixelColor(int x, int y) const;
    QColor pixelColor(const QPoint &position) const;
    const Tileset *tileset() const;
    QImage image() const;

    static int invalidId();

private:
    int mId;
    QRect mSourceRect;
    QPointer<const Tileset> mTileset;
};

QDebug operator<<(QDebug debug, const Tile *tile);

#endif // TILE_H
