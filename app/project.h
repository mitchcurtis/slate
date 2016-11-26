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

#ifndef PROJECT_H
#define PROJECT_H

#include <QHash>
#include <QObject>
#include <QPoint>
#include <QTemporaryDir>
#include <QUrl>
#include <QVector>

#include <QtUndo/undostack.h>

#include "tile.h"
#include "tileset.h"

class Project : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loaded READ hasLoaded NOTIFY loadedChanged)
    Q_PROPERTY(bool newProject READ isNewProject WRITE setNewProject NOTIFY newProjectChanged)
    Q_PROPERTY(bool unsavedChanges READ hasUnsavedChanges NOTIFY unsavedChangesChanged)
    Q_PROPERTY(bool canSave READ canSave NOTIFY canSaveChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(int tilesWide READ tilesWide NOTIFY tilesWideChanged)
    Q_PROPERTY(int tilesHigh READ tilesHigh NOTIFY tilesHighChanged)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(int tileWidth READ tileWidth NOTIFY tileWidthChanged)
    Q_PROPERTY(int tileHeight READ tileHeight NOTIFY tileHeightChanged)
    Q_PROPERTY(QUrl tilesetUrl READ tilesetUrl NOTIFY tilesetUrlChanged)
    Q_PROPERTY(Tileset *tileset READ tileset NOTIFY tilesetChanged)
    Q_PROPERTY(UndoStack *undoStack READ undoStack CONSTANT)

public:
    Project();

    bool hasLoaded() const;
    bool isNewProject() const;
    void setNewProject(bool newProject);
    bool hasUnsavedChanges() const;
    bool canSave() const;

    QUrl url() const;
    void setUrl(const QUrl &url);

    int tilesWide() const;
    void setTilesWide(int tilesWide);
    int tilesHigh() const;
    void setTilesHigh(int tilesHigh);
    int tileWidth() const;
    int tileHeight() const;
    QSize size() const;
    void setSize(const QSize &size);

    QSize tileSize() const;

    int widthInPixels() const;
    int heightInPixels() const;

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

    UndoStack *undoStack();

    bool isComposingMacro() const;
    void beginMacro(const QString &text);
    void endMacro();
    void addChange(UndoCommand *undoCommand);
    void clearChanges();

signals:
    void projectCreated();
    void projectLoaded();
    void projectClosed();
    void loadedChanged();
    void newProjectChanged();
    void unsavedChangesChanged();
    void canSaveChanged();
    void urlChanged();
    void tilesWideChanged();
    void tilesHighChanged();
    void sizeChanged();
    void tileWidthChanged();
    void tileHeightChanged();
    void tilesetUrlChanged();
    void tilesetChanged(Tileset *oldTileset, Tileset *newTileset);
    void tilesCleared();
    void errorOccurred(const QString &errorMessage);

public slots:
    void createNew(QUrl tilesetUrl, int tileWidth, int tileHeight,
        int tilesetTilesWide, int tilesetTilesHigh,
        int canvasTilesWide, int canvasTilesHigh);
    void load(const QUrl &url);
    void close();
    void save();
    void saveAs(const QUrl &url);
    void revert();

private:
    friend class ChangeCanvasSizeCommand;

    void error(const QString &message);
    bool projectKeysMissing(const QJsonObject &object, const QStringList &keys);

    int tileIdFromPosInTileset(int x, int y) const;
    int tileIdFromTilePosInTileset(int column, int row) const;

    bool warnIfTilePosInvalid(const QPoint &tilePos) const;

    void createTilesetTiles(int tilesetTilesWide, int tilesetTilesHigh);
    QUrl createTemporaryTilesetImage(int tileWidth, int tileHeight, int tilesetTilesWide, int tilesetTilesHigh, const QColor &colour);
    void setTileWidth(int tileWidth);
    void setTileHeight(int tileHeight);
    void setTilesetUrl(const QUrl &tilesetUrl);
    void setTileset(Tileset *tileset);
    void setComposingMacro(bool composingMacro);
    void changeSize(const QSize &size, const QVector<int> &tiles = QVector<int>());

    bool mFromNew;
    QUrl mUrl;
    int mTilesWide;
    int mTilesHigh;
    int mTileWidth;
    int mTileHeight;
    QUrl mTilesetUrl;
    QVector<int> mTiles;
    QHash<int, Tile*> mTileDatabase;
    Tileset* mTileset;
    QTemporaryDir mTempDir;
    bool mUsingTempTilesetImage;

    UndoStack mUndoStack;
    bool mComposingMacro;
    bool mHadUnsavedChangesBeforeMacroBegan;
};

#endif // PROJECT_H
