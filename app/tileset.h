/*
    Copyright 2016, Mitch Curtis

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

#ifndef TILESET_H
#define TILESET_H

#include <QObject>
#include <QString>
#include <QImage>

class Tileset : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int tilesWide READ tilesWide CONSTANT)
    Q_PROPERTY(int tilesHigh READ tilesHigh CONSTANT)

public:
    Tileset(const QString &fileName, int tilesWide, int tilesHigh, QObject *parent);

    QString fileName() const;
    const QImage *image() const;
    void setPixelColor(int x, int y, const QColor &colour);
    void copy(const QPoint &sourceTopLeft, const QPoint &targetTopLeft);
    void rotateCounterClockwise(const QPoint &tileTopLeft);
    void rotateClockwise(const QPoint &tileTopLeft);
    int tilesWide() const;
    int tilesHigh() const;

public slots:

signals:
    void imageChanged();

private:
    int tileWidth() const;
    int tileHeight() const;
    bool validTopLeft(const QPoint &topLeft) const;

    void rotate(const QPoint &tileTopLeft, int angle);

    QString mFilename;
    QImage mImage;
    int mTilesWide;
    int mTilesHigh;
};

#endif // TILESET_H
