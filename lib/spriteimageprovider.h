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

#ifndef SPRITEIMAGEPROVIDER_H
#define SPRITEIMAGEPROVIDER_H

#include <QHash>
#include <QImage>
#include <QString>
#include <QQuickImageProvider>

#include "slate-global.h"

class SLATE_EXPORT SpriteImageProvider : public QQuickImageProvider
{
public:
    SpriteImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    int parseFrameIndex(const QString &frameIndexStr) const;
    int parseFrameWidth(const QString &frameWidthStr) const;
    int parseFrameHeight(const QString &frameHeightStr) const;

    QHash<QString, QImage> mImages;
};

#endif // SPRITEIMAGEPROVIDER_H
