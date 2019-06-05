/*
    Copyright 2017, Mitch Curtis

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

#ifndef TILSEETPROJECT_H
#define TILSEETPROJECT_H

#include <QHash>
#include <QObject>
#include <QPoint>
#include <QTemporaryDir>
#include <QUrl>
#include <QVector>

#include "project.h"
#include "slate-global.h"
#include "tile.h"
#include "tileset.h"

class SLATE_EXPORT TilesetProject : public Project
{
    Q_OBJECT
    Q_PROPERTY(int tilesWide READ tilesWide NOTIFY tilesWideChanged)
    Q_PROPERTY(int tilesHigh READ tilesHigh NOTIFY tilesHighChanged)
    Q_PROPERTY(int tileWidth READ tileWidth NOTIFY tileWidthChanged)
    Q_PROPERTY(int tileHeight READ tileHeight NOTIFY tileHeightChanged)
    Q_PROPERTY(QUrl tilesetUrl READ tilesetUrl NOTIFY tilesetUrlChanged)
    Q_PROPERTY(Tileset *tileset READ tileset NOTIFY tilesetChanged)

public:
    TilesetProject();
    ~TilesetProject() override;

    Type type() const override;
    int tilesWide() const;
    void setTilesWide(int tilesWide);
    int tilesHigh() const;
    void setTilesHigh(int tilesHigh);
    int tileWidth() const;
    int tileHeight() const;

    QSize tileSize() const;
    QSize size() const override;
    void setSize(const QSize &newSize) override;
    int widthInPixels() const override;
    int heightInPixels() const override;
    QRect bounds() const override;

    QImage exportedImage() const override;

    QUrl tilesetUrl() const;
    Tileset *tileset() const;

    Tile *tileAt(const QPoint &scenePos);
    const Tile *tileAt(const QPoint &scenePos) const;
    // TODO: tileChanged signal that canvas connnects to repaint
    void setTileAtPixelPos(const QPoint &tilePos, int id);
    QVector<int> tiles() const;

    bool isTilePosWithinBounds(const QPoint &tilePos) const;
    const Tile *tileAtTilePos(const QPoint &tilePos) const;
    int tileIdAtTilePos(const QPoint &tilePos) const;

    Q_INVOKABLE Tile *tilesetTileAt(int xInPixels, int yInPixels);
    Q_INVOKABLE void duplicateTile(Tile *sourceTile, int xInPixels, int yInPixels);
    Q_INVOKABLE void rotateTileCounterClockwise(Tile *tile);
    Q_INVOKABLE void rotateTileClockwise(Tile *tile);

    QPoint tileIdToTilePos(int tileId) const;
    Tile *tilesetTileAtTilePos(const QPoint &tilePos) const;
    Tile *tilesetTileAtId(int id);

    // Sets all tiles to -1.
    void clearTiles();

signals:
    void tilesWideChanged();
    void tilesHighChanged();
    void tileWidthChanged();
    void tileHeightChanged();
    void tilesetUrlChanged();
    void tilesetChanged(Tileset *oldTileset, Tileset *newTileset);
    void tilesCleared();

public slots:
    void createNew(QUrl tilesetUrl, int tileWidth, int tileHeight,
        int tilesetTilesWide, int tilesetTilesHigh,
        int canvasTilesWide, int canvasTilesHigh,
        bool transparentBackground);

protected:
    void doLoad(const QUrl &url) override;
    void doClose() override;
    void doSaveAs(const QUrl &url) override;

private:
    friend class ChangeTileCanvasSizeCommand;

    int tileIdFromPosInTileset(int x, int y) const;
    int tileIdFromTilePosInTileset(int column, int row) const;

    bool warnIfTilePosInvalid(const QPoint &tilePos) const;

    void createTilesetTiles(int tilesetTilesWide, int tilesetTilesHigh);
    void setTileWidth(int tileWidth);
    void setTileHeight(int tileHeight);
    void setTilesetUrl(const QUrl &tilesetUrl);
    void setTileset(Tileset *tileset);
    void changeSize(const QSize &size, const QVector<int> &tiles = QVector<int>());

    int mTilesWide;
    int mTilesHigh;
    int mTileWidth;
    int mTileHeight;
    QUrl mTilesetUrl;
    QVector<int> mTiles;
    QHash<int, Tile*> mTileDatabase;
    Tileset* mTileset;
};

#endif // TILSEETPROJECT_H
