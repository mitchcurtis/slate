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

#ifndef TILESETSWATCHIMAGE_H
#define TILESETSWATCHIMAGE_H

#include <QQuickPaintedItem>

#include "slate-global.h"

class Tileset;

class SLATE_EXPORT TilesetSwatchImage : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Tileset *tileset READ tileset WRITE setTileset NOTIFY tilesetChanged)
    Q_PROPERTY(QRect sourceRect READ sourceRect WRITE setSourceRect NOTIFY sourceRectChanged)
    QML_ELEMENT

public:
    TilesetSwatchImage();

    Tileset *tileset() const;
    void setTileset(Tileset *tileset);

    QRect sourceRect() const;
    void setSourceRect(const QRect &sourceRect);

    void paint(QPainter *painter) override;

signals:
    void tilesetChanged();
    void sourceRectChanged();

private:
    Tileset *mTileset;
    QRect mSourceRect;
};

#endif // TILESETSWATCHIMAGE_H
