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

#include "project.h"

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>

#include "changecanvassizecommand.h"

Q_LOGGING_CATEGORY(lcProject, "app.project")

Project::Project() :
    mFromNew(false),
    mTilesWide(0),
    mTilesHigh(0),
    mTileWidth(0),
    mTileHeight(0),
    mTileset(nullptr),
    mUsingTempTilesetImage(false),
    mComposingMacro(false),
    mHadUnsavedChangesBeforeMacroBegan(false)
{
    connect(&mUndoStack, SIGNAL(cleanChanged(bool)), this, SIGNAL(unsavedChangesChanged()));
}

QUrl Project::url() const
{
    return mUrl;
}

void Project::setUrl(const QUrl &url)
{
    if (url == mUrl)
        return;

    const bool wasLoaded = hasLoaded();

    mUrl = url;

    if (wasLoaded != hasLoaded()) {
        emit loadedChanged();
    }

    emit urlChanged();
}

bool Project::hasLoaded() const
{
    return mFromNew || mUrl.isValid();
}

bool Project::isNewProject() const
{
    return mFromNew;
}

void Project::setNewProject(bool newProject)
{
    if (newProject == mFromNew)
        return;

    const bool wasLoaded = hasLoaded();
    const bool couldSave = canSave();

    mFromNew = newProject;

    if (wasLoaded != hasLoaded()) {
        emit loadedChanged();
    }

    if (couldSave != canSave()) {
        emit canSaveChanged();
    }

    emit newProjectChanged();
}

bool Project::hasUnsavedChanges() const
{
    return !mUndoStack.isClean();
}

bool Project::canSave() const
{
    return !mComposingMacro && (isNewProject() || hasUnsavedChanges());
}

void Project::createTilesetTiles(int tilesetTilesWide, int tilesetTilesHigh)
{
    Q_ASSERT(mTileDatabase.isEmpty());
    for (int row = 0; row < tilesetTilesHigh; ++row) {
        for (int column = 0; column < tilesetTilesWide; ++column) {
            const int x = column * mTileWidth;
            const int y = row * mTileHeight;
            const int tileId = tileIdFromPosInTileset(x, y);
            mTileDatabase.insert(tileId, new Tile(tileId, mTileset, QRect(x, y, mTileWidth, mTileHeight), this));
        }
    }
    Q_ASSERT(!mTileDatabase.isEmpty());
}

QUrl Project::createTemporaryTilesetImage(int tileWidth, int tileHeight,
    int tilesetTilesWide, int tilesetTilesHigh, const QColor &colour)
{
    if (!mTempDir.isValid()) {
        error(QString::fromLatin1("Failed to create temporary tileset image directory: %1").arg(mTempDir.errorString()));
        return QUrl();
    }

    QImage tempImage(tileWidth * tilesetTilesWide, tileHeight * tilesetTilesHigh, QImage::Format_ARGB32_Premultiplied);
    tempImage.fill(colour);

    const QString dateString = QDateTime::currentDateTime().toString(QLatin1String("hh-mm-ss-zzz"));
    const QString fileName = QString::fromLatin1("%1/tileset-%2.png").arg(mTempDir.path()).arg(dateString);
    if (!tempImage.save(fileName)) {
        error(QString::fromLatin1("Failed to save temporary tileset image to %1").arg(fileName));
        return QUrl();
    }

    qCDebug(lcProject) << "Successfully created temporary tileset image:" << fileName;
    mUsingTempTilesetImage = true;
    return QUrl::fromLocalFile(fileName);
}

void Project::createNew(QUrl tilesetUrl, int tileWidth, int tileHeight,
    int tilesetTilesWide, int tilesetTilesHigh,
    int canvasTilesWide, int canvasTilesHigh, bool transparentBackground)
{
    if (hasLoaded()) {
        close();
    }

    if (tilesetUrl.isEmpty()) {
        // Not using an existing tileset image, so we must create one.
        tilesetUrl = createTemporaryTilesetImage(tileWidth, tileHeight, tilesetTilesWide, tilesetTilesHigh,
            transparentBackground ? Qt::transparent : Qt::white);
        if (!tilesetUrl.isValid()) {
            return;
        }
    }

    QImage tilesetImage(tilesetUrl.toLocalFile());
    if (tileWidth * tilesetTilesWide > tilesetImage.width()) {
        error(QString::fromLatin1("Tile width of %1 pixels combined with a tileset width "
            "of %2 tiles is too large for a tileset image whose width is %3 pixels")
                .arg(tileWidth).arg(tilesetTilesWide).arg(tilesetImage.width()));
        return;
    }

    if (tileHeight * tilesetTilesHigh > tilesetImage.height()) {
        error(QString::fromLatin1("Tile height of %1 pixels combined with a tileset height "
            "of %2 tiles is too large for a tileset image whose height is %3 pixels")
                .arg(tileHeight).arg(tilesetTilesHigh).arg(tilesetImage.height()));
        return;
    }

    qCDebug(lcProject) << "creating new project:"
        << "tileWidth =" << tileWidth
        << "tileHeight =" << tileHeight
        << "tilesetTilesWide =" << tilesetTilesWide
        << "tilesetTilesHigh =" << tilesetTilesHigh
        << "canvasTilesWide =" << canvasTilesWide
        << "canvasTilesHigh =" << canvasTilesHigh
        << "tilesetPath =" << tilesetUrl;

    Q_ASSERT(mUndoStack.count() == 0);
    Q_ASSERT(!mTileset);

    setTileWidth(tileWidth);
    setTileHeight(tileHeight);
    setTilesWide(canvasTilesWide);
    setTilesHigh(canvasTilesHigh);
    setTilesetUrl(tilesetUrl);
    setTileset(new Tileset(tilesetUrl.toLocalFile(), tilesetTilesWide, tilesetTilesHigh, this));

    createTilesetTiles(tilesetTilesWide, tilesetTilesHigh);

    mTiles.clear();
    mTiles.fill(-1, mTilesWide * mTilesHigh);

    setUrl(QUrl());
    setNewProject(true);
    emit projectCreated();

    qCDebug(lcProject) << "finished creating new project";
}

static QJsonValue strictValue(const QJsonObject &object, const QString &key)
{
    QString objectString;
    QDebug(&objectString) << object.toVariantMap();
    Q_ASSERT_X(object.contains(key), Q_FUNC_INFO, qPrintable(QString::fromLatin1(
        "json object %1 doesn't contain key %2").arg(objectString).arg(key)));
    return object[key];
}

void Project::load(const QUrl &url)
{
    qCDebug(lcProject) << "loading project:" << url;

    QFile jsonFile(url.toLocalFile());
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        error(QString::fromLatin1("Project files must have a .json extension (%1)").arg(url.toLocalFile()));
        return;
    }

    if (QFileInfo(jsonFile).suffix() != "json") {
        error(QString::fromLatin1("Failed to open project's JSON file at %1").arg(url.toLocalFile()));
        return;
    }

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonFile.readAll());
    QJsonObject rootJson = jsonDoc.object();
    QJsonObject projectObject = strictValue(rootJson, "project").toObject();

    setTilesWide(strictValue(projectObject, "tilesWide").toInt());
    setTilesHigh(strictValue(projectObject, "tilesHigh").toInt());
    setTileWidth(strictValue(projectObject, "tileWidth").toInt());
    setTileHeight(strictValue(projectObject, "tileHeight").toInt());
    setTilesetUrl(QUrl::fromLocalFile(strictValue(projectObject, "tilesetPath").toString()));
    mUsingTempTilesetImage = false;

    Q_ASSERT(!mTileset);
    const QString tilesetPath = mTilesetUrl.toLocalFile();
    QJsonObject tilesetObject = strictValue(projectObject, "tileset").toObject();
    const int tilesetTilesWide = strictValue(tilesetObject, "tilesWide").toInt();
    const int tilesetTilesHigh = strictValue(tilesetObject, "tilesHigh").toInt();
    QScopedPointer<Tileset> tempTileset(new Tileset(tilesetPath, tilesetTilesWide, tilesetTilesHigh, this));
    if (tempTileset->isValid()) {
        setTileset(tempTileset.take());
    } else {
        error(QString::fromLatin1("Failed to open project's tileset at %1").arg(tilesetPath));
        return;
    }

    mTileDatabase.clear();
    createTilesetTiles(tilesetTilesWide, tilesetTilesHigh);

    QJsonArray tileArray = projectObject["tiles"].toArray();
    mTiles.resize(tileArray.size());
    for (int i = 0; i < tileArray.size(); ++i) {
        int tileId = tileArray.at(i).toInt(-2);
        Q_ASSERT(tileId != -2);
        mTiles[i] = tileId;
        if (tileId > -1) {
            Q_ASSERT(mTileDatabase.contains(tileId));
        }
    }

    setUrl(url);
    emit projectLoaded();

    qCDebug(lcProject) << "loaded project";
}

void Project::close()
{
    qCDebug(lcProject) << "closing project...";

    setNewProject(false);
    setUrl(QUrl());
    mUsingTempTilesetImage = false;
    clearTiles();
    mTileDatabase.clear();
    setTileset(nullptr);
    mUndoStack.clear();
    emit projectClosed();

    qCDebug(lcProject) << "... closed project";
}

void Project::save()
{
    if (mFromNew) {
        Q_ASSERT_X(mUrl.isEmpty(), Q_FUNC_INFO, "New projects must have a valid URL to save to");
    }

    saveAs(mUrl);
}

void Project::saveAs(const QUrl &url)
{
    if (!hasLoaded())
        return;

    if (url.isEmpty())
        return;

    const QString filePath = url.toLocalFile();
    const QFileInfo projectSaveFileInfo(filePath);
    if (mTempDir.isValid()) {
        if (projectSaveFileInfo.dir().path() == mTempDir.path()) {
            error(QLatin1String("Cannot save project in internal temporary directory"));
            return;
        }
    }

    if (mTileset->image()->isNull()) {
        error(QString::fromLatin1("Failed to save project: tileset image is null"));
        return;
    }

    QFile jsonFile;
    if (QFile::exists(filePath)) {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to open project's JSON file at %1").arg(filePath));
            return;
        }
    } else {
        jsonFile.setFileName(filePath);
        if (!jsonFile.open(QIODevice::WriteOnly)) {
            error(QString::fromLatin1("Failed to create project's JSON file at %1").arg(filePath));
            return;
        }
    }

    const QFileInfo tileFileInfo(mTileset->fileName());
    if (!tileFileInfo.exists()) {
        error(QString::fromLatin1("Failed to save project: tileset path %1 doesn't exist").arg(mTileset->fileName()));
        return;
    }

    if (mUsingTempTilesetImage) {
        // Save the image in the same directory as the project, using the same base name as the project url.
        const QString path = projectSaveFileInfo.path() + "/" + projectSaveFileInfo.completeBaseName() + ".png";
        qCDebug(lcProject) << "saving temporary tileset image to" << path;
        if (!mTileset->image()->save(path)) {
            error(QString::fromLatin1("Failed to save project: failed to save tileset image %1").arg(path));
            return;
        }

        mTileset->setFileName(path);
        mTilesetUrl = QUrl::fromLocalFile(path);
        mUsingTempTilesetImage = false;
    } else {
        if (!mTileset->image()->save(mTileset->fileName())) {
            error(QString::fromLatin1("Failed to save project: failed to save tileset image %1").arg(mTileset->fileName()));
            return;
        }
    }

    QJsonObject rootJson;

    QJsonObject projectObject;
    projectObject["tilesWide"] = mTilesWide;
    projectObject["tilesHigh"] = mTilesHigh;
    projectObject["tileWidth"] = mTileWidth;
    projectObject["tileHeight"] = mTileHeight;
    projectObject["tilesetPath"] = mTilesetUrl.toLocalFile();

    QJsonObject tilesetObject;
    tilesetObject["tilesWide"] = mTileset->tilesWide();
    tilesetObject["tilesHigh"] = mTileset->tilesHigh();
    projectObject.insert("tileset", tilesetObject);

    QJsonArray tileArray;
    foreach (int tile, mTiles) {
        tileArray.append(QJsonValue(tile));
    }
    projectObject.insert("tiles", tileArray);

    rootJson.insert("project", projectObject);

    QJsonDocument jsonDoc(rootJson);
    const qint64 bytesWritten = jsonFile.write(jsonDoc.toJson());
    if (bytesWritten == -1) {
        error(QString::fromLatin1("Failed to save project: couldn't write to JSON project file: %1")
            .arg(jsonFile.errorString()));
        return;
    }

    if (bytesWritten == 0) {
        error(QString::fromLatin1("Failed to save project: wrote zero bytes to JSON project file"));
        return;
    }

    if (mFromNew) {
        // The project was successfully saved, so it can now save
        // to the same URL by default from now on.
        setNewProject(false);
    }
    setUrl(url);
    mUndoStack.setClean();
    mHadUnsavedChangesBeforeMacroBegan = false;
}

void Project::revert()
{
    qCDebug(lcProject) << "reverting changes...";

    if (!hasLoaded())
        return;

    clearChanges();

    qCDebug(lcProject) << "... reverted changes";
}

void Project::error(const QString &message)
{
    emit errorOccurred(message);
}

int Project::tileIdFromPosInTileset(int x, int y) const
{
    const int column = x / mTileWidth;
    const int row = y / mTileHeight;
    return tileIdFromTilePosInTileset(column, row);
}

int Project::tileIdFromTilePosInTileset(int column, int row) const
{
    // IDs are one-based.
    return (row * tileset()->tilesWide() + column) + 1;
}

void Project::setComposingMacro(bool composingMacro)
{
    if (composingMacro == mComposingMacro)
        return;

    const bool couldSave = canSave();
    mComposingMacro = composingMacro;
    if (couldSave != canSave()) {
        emit canSaveChanged();
    }
}

void Project::changeSize(const QSize &newSize, const QVector<int> &tiles)
{
    Q_ASSERT(newSize != size());

    // Size < 1 doesn't make sense.
    Q_ASSERT(newSize.width() >= 1 && newSize.height() >= 1);

    // This function can be called as a result of the size changing or a size
    // change being undone. When the size shrinks, we never need a list of tiles.
    // When the size increases, we only need a list of tiles if the size was changed
    // as a result of an undo operation, as we need to restore those tiles.
    Q_ASSERT(tiles.isEmpty() || newSize.width() * newSize.height() == tiles.size());

    if (tiles.isEmpty()) {
        // Expand the 1d array into a 2d one, then simply resize the 2d one
        // and flatten back again afterwards.
        // Probably the slowest way of doing it, but it doesn't try to be tricky,
        // which makes it easier to understand and maintain.
        // Also, resizing shouldn't happen that often, so it doesn't matter.
        QVector<QVector<int> > tileGrid;
        tileGrid.resize(mTilesHigh);
        for (int row = 0; row < mTilesHigh; ++row) {
            tileGrid[row].resize(mTilesWide);

            for (int column = 0; column < mTilesWide; ++column) {
                const int index = row * mTilesWide + column;
                tileGrid[row][column] = mTiles.at(index);
            }
        }

        // First, shrink or expand the 2d grid's height to match the new height.
        if (newSize.height() < mTilesHigh) {
            while (tileGrid.size() > newSize.height())
                tileGrid.removeLast();
        } else {
            while (tileGrid.size() < newSize.height()) {
                QVector<int> row;
                row.fill(Tile::invalidId(), newSize.width());
                tileGrid.append(row);
            }
        }

        // Do the same for the width.
        if (newSize.width() < mTilesWide) {
            for (int row = 0; row < newSize.height(); ++row) {
                while (tileGrid[row].size() > newSize.width()) {
                    tileGrid[row].removeLast();
                }
            }
        } else {
            for (int row = 0; row < newSize.height(); ++row) {
                while (tileGrid[row].size() < newSize.width()) {
                    tileGrid[row].append(Tile::invalidId());
                }
            }
        }

        mTiles.resize(newSize.width() * newSize.height());

        for (int row = 0; row < newSize.height(); ++row) {
            for (int column = 0; column < newSize.width(); ++column) {
                const int index = row * newSize.width() + column;
                mTiles[index] = tileGrid.at(row).at(column);
            }
        }
    } else {
        mTiles = tiles;
    }

    setTilesWide(newSize.width());
    setTilesHigh(newSize.height());
}

int Project::tilesWide() const
{
    return mTilesWide;
}

void Project::setTilesWide(int tilesWide)
{
    if (tilesWide == mTilesWide)
        return;

    mTilesWide = tilesWide;
    emit tilesWideChanged();
    emit sizeChanged();
}

int Project::tilesHigh() const
{
    return mTilesHigh;
}

void Project::setTilesHigh(int tilesHigh)
{
    if (tilesHigh == mTilesHigh)
        return;

    mTilesHigh = tilesHigh;
    emit tilesHighChanged();
    emit sizeChanged();
}

int Project::tileWidth() const
{
    return mTileWidth;
}

void Project::setTileWidth(int width)
{
    if (width == mTileWidth)
        return;

    mTileWidth = width;
    emit tileWidthChanged();
}

void Project::setSize(const QSize &size)
{
    beginMacro(QLatin1String("ChangeCanvasSize"));
    addChange(new ChangeCanvasSizeCommand(this, QSize(mTilesWide, mTilesHigh), size));
    endMacro();
}

int Project::tileHeight() const
{
    return mTileHeight;
}

QSize Project::size() const
{
    return QSize(mTilesWide, mTilesHigh);
}

void Project::setTileHeight(int height)
{
    if (height == mTileHeight)
        return;

    mTileHeight = height;
    emit tileHeightChanged();
}

QSize Project::tileSize() const
{
    return QSize(mTileWidth, mTileHeight);
}

int Project::widthInPixels() const
{
    return mTilesWide * mTileWidth;
}

int Project::heightInPixels() const
{
    return mTilesHigh * mTileHeight;
}

QUrl Project::tilesetUrl() const
{
    return mTilesetUrl;
}

void Project::setTilesetUrl(const QUrl &tilesetUrl)
{
    if (tilesetUrl == mTilesetUrl)
        return;

    mTilesetUrl = tilesetUrl;
    emit tilesetUrlChanged();
}

Tileset *Project::tileset() const
{
    return mTileset;
}

void Project::setTileset(Tileset *tileset)
{
    if (tileset == mTileset)
        return;

    Tileset *old = mTileset;
    mTileset = tileset;
    emit tilesetChanged(old, mTileset);
}

Tile *Project::tileAt(const QPoint &scenePos)
{
    return const_cast<Tile*>(static_cast<const Project*>(this)->tileAt(scenePos));
}

const Tile *Project::tileAt(const QPoint &scenePos) const
{
    if (scenePos.x() < 0 || scenePos.x() >= widthInPixels()
        || scenePos.y() < 0 || scenePos.y() >= heightInPixels()) {
        return nullptr;
    }

    const int xTile = scenePos.x() / mTileWidth;
    const int yTile = scenePos.y() / mTileHeight;
    const int tileIndex = yTile * mTilesWide + xTile;
    if (tileIndex >= mTiles.size())
        return nullptr;

    const int tileId = mTiles[tileIndex];
    return tileId != -1 ? mTileDatabase.value(tileId) : nullptr;
}

bool Project::isTilePosWithinBounds(const QPoint &tilePos) const
{
    return tilePos.x() >= 0 && tilePos.x() < mTilesWide
        && tilePos.y() >= 0 && tilePos.y() < mTilesHigh;
}

bool Project::warnIfTilePosInvalid(const QPoint &tilePos) const
{
    if (!isTilePosWithinBounds(tilePos)) {
        qCDebug(lcProject) << "tilePos" << tilePos << "is out of bounds";
        return true;
    }

    return false;
}

void Project::setTileAtPixelPos(const QPoint &tilePos, int id)
{
    if (warnIfTilePosInvalid(tilePos)) {
        return;
    }

    const int tileIndex = tilePos.y() * mTilesWide + tilePos.x();
    Q_ASSERT(tileIndex < mTiles.size());
    mTiles[tileIndex] = id;
    qCDebug(lcProject) << "set tile at tile pos" << tilePos << "and index" << tileIndex << "to id" << id;
}

QVector<int> Project::tiles() const
{
    return mTiles;
}

const Tile *Project::tileAtTilePos(const QPoint &tilePos) const
{
    if (warnIfTilePosInvalid(tilePos)) {
        return nullptr;
    }

    const int tileIndex = tilePos.y() * mTilesWide + tilePos.x();
    Q_ASSERT(tileIndex < mTiles.size());
    const int tileId = mTiles[tileIndex];
    return tileId != -1 ? mTileDatabase.value(tileId) : nullptr;
}

int Project::tileIdAtTilePos(const QPoint &tilePos) const
{
    const Tile *tile = tileAtTilePos(tilePos);
    return tile ? tile->id() : Tile::invalidId();
}

Tile *Project::tilesetTileAt(int xInPixels, int yInPixels)
{
    Tileset *set = tileset();
    if (!set) {
        return nullptr;
    }

    if (xInPixels < 0 || xInPixels >= set->image()->width()
        || yInPixels < 0 || yInPixels >= set->image()->height()) {
        // qWarning() << Q_FUNC_INFO << xInPixels << "," << yInPixels << "is out of tileset swatch bounds";
        return nullptr;
    }

    return mTileDatabase.value(tileIdFromPosInTileset(xInPixels, yInPixels));
}

Tile *Project::tilesetTileAtTilePos(const QPoint &tilePos) const
{
    Tileset *set = tileset();
    if (!set) {
        return nullptr;
    }

    if (tilePos.x() < 0 || tilePos.x() >= set->tilesWide()
        || tilePos.y() < 0 || tilePos.y() >= set->tilesHigh()) {
        // qWarning() << Q_FUNC_INFO << tileX << "," << tileY << "is out of tileset swatch bounds";
        return nullptr;
    }

    return mTileDatabase.value(tileIdFromTilePosInTileset(tilePos.x(), tilePos.y()));
}

Tile *Project::tilesetTileAtId(int id)
{
    Tileset *set = tileset();
    if (!set) {
        return nullptr;
    }

    QHash<int, Tile*>::iterator it = mTileDatabase.find(id);
    if (it == mTileDatabase.end())
        return nullptr;

    return it.value();
}

void Project::duplicateTile(Tile *sourceTile, int xInPixels, int yInPixels)
{
    if (!sourceTile) {
        return;
    }

    Tileset *set = tileset();
    if (!set) {
        return;
    }

    // Make sure that the input positions are multiples of the tile size.
    QPoint sourceTopLeft = sourceTile->sourceRect().topLeft();
    sourceTopLeft.rx() -= sourceTopLeft.x() % mTileWidth;
    sourceTopLeft.ry() -= sourceTopLeft.y() % mTileHeight;

    QPoint targetTopLeft(xInPixels, yInPixels);
    targetTopLeft.rx() -= targetTopLeft.x() % mTileWidth;
    targetTopLeft.ry() -= targetTopLeft.y() % mTileHeight;

    set->copy(sourceTopLeft, targetTopLeft);
}

void Project::rotateTileCounterClockwise(Tile *tile)
{
    Tileset *set = tileset();
    if (!set)
        return;

    QPoint topLeft = tile->sourceRect().topLeft();
    topLeft.rx() -= topLeft.x() % mTileWidth;
    topLeft.ry() -= topLeft.y() % mTileHeight;
    set->rotateCounterClockwise(topLeft);
}

void Project::rotateTileClockwise(Tile *tile)
{
    Tileset *set = tileset();
    if (!set)
        return;

    QPoint topLeft = tile->sourceRect().topLeft();
    topLeft.rx() -= topLeft.x() % mTileWidth;
    topLeft.ry() -= topLeft.y() % mTileHeight;
    set->rotateClockwise(topLeft);
}

QPoint Project::tileIdToTilePos(int tileId) const
{
    return QPoint((tileId - 1) % mTileset->tilesWide(), (tileId - 1) / mTileset->tilesWide());
}

void Project::clearTiles()
{
    if (mTiles.isEmpty())
        return;

    mTiles.fill(mTiles.size(), -1);
    emit tilesCleared();
}

QUndoStack *Project::undoStack()
{
    return &mUndoStack;
}

bool Project::isComposingMacro() const
{
    return mComposingMacro;
}

void Project::beginMacro(const QString &text)
{
    if (isComposingMacro()) {
        qCDebug(lcProject).nospace() << "tried to begin macro" << text
            << ", but we're busy composing another right now";
        return;
    }

    qCDebug(lcProject) << "beginning macro" << text;

    mHadUnsavedChangesBeforeMacroBegan = hasUnsavedChanges();
    mUndoStack.beginMacro(text);
    setComposingMacro(true);
}

void Project::endMacro()
{
    Q_ASSERT(mComposingMacro);

    qCDebug(lcProject) << "ending macro";

    mUndoStack.endMacro();
    // This handles the emission of the canSaveChanged signal.
    setComposingMacro(false);

    // It's not enough to rely on the cleanChanged signal to cause
    // our unchangedChangesSignal to be called, because cleanChanged
    // apparently does not get emitted when a macro ends. So, we do it ourselves here.
    if (hasUnsavedChanges() != mHadUnsavedChangesBeforeMacroBegan) {
        emit unsavedChangesChanged();
    }
}

QDebug operator<<(QDebug debug, const QUndoCommand &command)
{
    debug << &command;
    return debug;
}

void Project::addChange(QUndoCommand *undoCommand)
{
    qCDebug(lcProject) << "adding change" << *undoCommand;
    mUndoStack.push(undoCommand);
}

void Project::clearChanges()
{
    const bool hadUnsavedChanges = hasUnsavedChanges();

    mUndoStack.setIndex(mUndoStack.cleanIndex());
    mHadUnsavedChangesBeforeMacroBegan = false;

    if (hasUnsavedChanges() != hadUnsavedChanges) {
        emit unsavedChangesChanged();
    }
}
