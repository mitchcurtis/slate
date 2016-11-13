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

#include "tilecanvas.h"

#include <QCursor>
#include <QLoggingCategory>
#include <QPainter>
#include <QQuickWindow>
#include <QtMath>

#include "applypixelerasercommand.h"
#include "applypixelfillcommand.h"
#include "applypixelpencommand.h"
#include "applytileerasercommand.h"
#include "applytilefillcommand.h"
#include "applytilepencommand.h"
#include "floodfill.h"
#include "project.h"
#include "tileset.h"

Q_LOGGING_CATEGORY(lcCanvas, "app.canvas")

TileCanvas::TileCanvas() :
    mProject(nullptr),
    mBackgroundColour(Qt::gray),
    mGridVisible(false),
    mGridColour(Qt::black),
    mSplitColour(Qt::black),
    mSplitScreen(true),
    mSplitter(this),
    mCurrentPane(&mFirstPane),
    mCursorX(0),
    mCursorY(0),
    mCursorPaneX(0),
    mCursorPaneY(0),
    mCursorSceneX(0),
    mCursorSceneY(0),
    mCursorTilePixelX(0),
    mCursorTilePixelY(0),
    mCursorPixelColour(Qt::black),
    mContainsMouse(false),
    mMouseButtonPressed(Qt::NoButton),
    mMode(TileMode),
    mTool(PenTool),
    mToolSize(1),
    mMaxToolSize(100),
    mPenTile(nullptr),
    mTilePenPreview(false),
    mPenForegroundColour(Qt::black),
    mPenBackgroundColour(Qt::white),
    mAltPressed(false),
    mToolBeforeAltPressed(PenTool),
    mSpacePressed(false),
    mHasBlankCursor(false)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(QQuickItem::ItemIsFocusScope);
    setCursor(Qt::BlankCursor);

    mFirstPane.setObjectName("firstPane");
    mSecondPane.setObjectName("secondPane");
    mSplitter.setPosition(mFirstPane.size());

    connect(&mFirstPane, SIGNAL(zoomLevelChanged()), this, SLOT(update()));
    connect(&mSecondPane, SIGNAL(zoomLevelChanged()), this, SLOT(update()));
    connect(&mSplitter, SIGNAL(positionChanged()), this, SLOT(onSplitterPositionChanged()));
}

Project *TileCanvas::project() const
{
    return mProject;
}

void TileCanvas::setProject(Project *project)
{
    if (project == mProject)
        return;

    if (mProject) {
        mProject->disconnect(SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
        mProject->disconnect(SIGNAL(tilesCleared()), this, SLOT(update()));
        mProject->disconnect(SIGNAL(projectCreated()), this, SLOT(update()));
        mProject->disconnect(SIGNAL(projectClosed()), this, SLOT(reset()));
        mProject->disconnect(SIGNAL(sizeChanged()), this, SLOT(update()));
        mProject->disconnect(SIGNAL(tilesetChanged(Tileset *, Tileset *)), this, SLOT(onTilesetChanged(Tileset *, Tileset *)));

        window()->disconnect(SIGNAL(activeFocusItemChanged()), this, SLOT(checkIfModalPopupsOpen()));
    }

    mProject = project;

    if (mProject) {
        connect(mProject, SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
        connect(mProject, SIGNAL(tilesCleared()), this, SLOT(update()));
        connect(mProject, SIGNAL(projectCreated()), this, SLOT(update()));
        connect(mProject, SIGNAL(projectClosed()), this, SLOT(reset()));
        connect(mProject, SIGNAL(sizeChanged()), this, SLOT(update()));
        connect(mProject, SIGNAL(tilesetChanged(Tileset *, Tileset *)), this, SLOT(onTilesetChanged(Tileset *, Tileset *)));

        connect(window(), SIGNAL(activeFocusItemChanged()), this, SLOT(checkIfModalPopupsOpen()));

        // If the project already has this, we won't get the signal, so force it.
        if (mProject->tileset())
            onTilesetChanged(nullptr, mProject->tileset());
    }

    emit projectChanged();
}

bool TileCanvas::gridVisible() const
{
    return mGridVisible;
}

void TileCanvas::setGridVisible(bool gridVisible)
{
    mGridVisible = gridVisible;
    update();
    emit gridVisibleChanged();
}

QColor TileCanvas::gridColour() const
{
    return mGridColour;
}

void TileCanvas::setGridColour(const QColor &gridColour)
{
    if (gridColour == mGridColour)
        return;

    mGridColour = gridColour;
    update();
    emit gridColourChanged();
}

QColor TileCanvas::splitColour() const
{
    return mSplitColour;
}

void TileCanvas::setSplitColour(const QColor &splitColour)
{
    if (splitColour == mSplitColour)
        return;

    mSplitColour = splitColour;
    update();
    emit splitColourChanged();
}

QColor TileCanvas::backgroundColour() const
{
    return mBackgroundColour;
}

void TileCanvas::setBackgroundColour(const QColor &backgroundColour)
{
    if (backgroundColour == mBackgroundColour)
        return;

    mBackgroundColour = backgroundColour;
    update();
    emit backgroundColourChanged();
}

int TileCanvas::cursorX() const
{
    return mCursorX;
}

void TileCanvas::setCursorX(int cursorX)
{
    if (cursorX == mCursorX)
        return;

    mCursorX = cursorX;
    emit cursorXChanged();
}

int TileCanvas::cursorY() const
{
    return mCursorY;
}

void TileCanvas::setCursorY(int cursorY)
{
    if (cursorY == mCursorY)
        return;

    mCursorY = cursorY;
    emit cursorYChanged();
}

TileCanvas::Mode TileCanvas::mode() const
{
    return mMode;
}

void TileCanvas::setMode(const Mode &mode)
{
    if (mode == mMode)
        return;

    mMode = mode;
    updateTilePenPreview();
    emit modeChanged();
}

TileCanvas::Tool TileCanvas::tool() const
{
    return mTool;
}

void TileCanvas::setTool(const Tool &tool)
{
    if (tool == mTool)
        return;

    mTool = tool;
    updateTilePenPreview();
    emit toolChanged();
}

int TileCanvas::toolSize() const
{
    return mToolSize;
}

void TileCanvas::setToolSize(int toolSize)
{
    const int clamped = qBound(1, toolSize, mMaxToolSize);
    if (clamped == mToolSize)
        return;

    mToolSize = clamped;
    emit toolSizeChanged();
}

int TileCanvas::maxToolSize() const
{
    return mMaxToolSize;
}

Tile *TileCanvas::penTile() const
{
    return mPenTile;
}

void TileCanvas::setPenTile(Tile *penTile)
{
    if (penTile == mPenTile)
        return;

    mPenTile = penTile;
    emit penTileChanged();
}

QColor TileCanvas::penForegroundColour() const
{
    return mPenForegroundColour;
}

void TileCanvas::setPenForegroundColour(const QColor &penForegroundColour)
{
    if (!penForegroundColour.isValid()) {
        qWarning() << "Invalid penForegroundColour";
        return;
    }

    QColor colour = penForegroundColour.toRgb();
    if (colour == mPenForegroundColour)
        return;

    mPenForegroundColour = colour;
    emit penForegroundColourChanged();
}

QColor TileCanvas::penBackgroundColour() const
{
    return mPenBackgroundColour;
}

void TileCanvas::setPenBackgroundColour(const QColor &penBackgroundColour)
{
    if (!penBackgroundColour.isValid()) {
        qWarning() << "Invalid penBackgroundColour";
        return;
    }

    QColor colour = penBackgroundColour.toRgb();
    if (colour == mPenBackgroundColour)
        return;

    mPenBackgroundColour = colour;
    emit penBackgroundColourChanged();
}

int TileCanvas::cursorTilePixelX() const
{
    return mCursorTilePixelX;
}

void TileCanvas::setCursorTilePixelX(int cursorTilePixelX)
{
    if (cursorTilePixelX == mCursorTilePixelX)
        return;

    mCursorTilePixelX = cursorTilePixelX;
    emit cursorTilePixelXChanged();
}

int TileCanvas::cursorTilePixelY() const
{
    return mCursorTilePixelY;
}

void TileCanvas::setCursorTilePixelY(int cursorTilePixelY)
{
    if (cursorTilePixelY == mCursorTilePixelY)
        return;

    mCursorTilePixelY = cursorTilePixelY;
    emit cursorTilePixelYChanged();
}

QColor TileCanvas::cursorPixelColour() const
{
    return mCursorPixelColour;
}

void TileCanvas::setCursorPixelColour(const QColor &cursorPixelColour)
{
    if (cursorPixelColour == mCursorPixelColour)
        return;

    mCursorPixelColour = cursorPixelColour;
    emit cursorPixelColourChanged();
}

bool TileCanvas::containsMouse() const
{
    return mContainsMouse;
}

void TileCanvas::setContainsMouse(bool containsMouse)
{
    if (containsMouse == mContainsMouse)
        return;

    mContainsMouse = containsMouse;
    updateWindowCursorShape();
    emit containsMouseChanged();
}

bool TileCanvas::isSplitScreen() const
{
    return mSplitScreen;
}

void TileCanvas::setSplitScreen(bool splitScreen)
{
    if (splitScreen == mSplitScreen)
        return;

    mSplitScreen = splitScreen;

    if (mCurrentPane == &mSecondPane) {
        setCurrentPane(&mFirstPane);
    }

    mFirstPane.setSize(splitScreen ? 0.5 : 1.0);

    update();

    emit splitScreenChanged();
}

TileCanvasPane *TileCanvas::firstPane()
{
    return &mFirstPane;
}

TileCanvasPane *TileCanvas::secondPane()
{
    return mSplitScreen ? &mSecondPane : nullptr;
}

TileCanvasPane *TileCanvas::currentPane()
{
    return mCurrentPane;
}

Splitter *TileCanvas::splitter()
{
    return &mSplitter;
}

QColor TileCanvas::mapBackgroundColour() const
{
    return mBackgroundColour;
}

bool TileCanvas::areModalPopupsOpen() const
{
    return mModalPopupsOpen;
}

bool TileCanvas::isAltPressed() const
{
    return mAltPressed;
}

void TileCanvas::setAltPressed(bool altPressed)
{
    if (altPressed == mAltPressed)
        return;

    mAltPressed = altPressed;
    emit altPressedChanged();
}

bool TileCanvas::hasBlankCursor() const
{
    return mHasBlankCursor;
}

void TileCanvas::checkIfModalPopupsOpen()
{
    if (!window())
        return;

    const bool wasOpen = mModalPopupsOpen;

    QQuickItem *overlay = window()->property("overlay").value<QQuickItem*>();
    Q_ASSERT(overlay);

    mModalPopupsOpen = false;
    foreach (QQuickItem *child, overlay->childItems()) {
        if (QString::fromLatin1(child->metaObject()->className()).contains("QQuickPopup")) {
            mModalPopupsOpen = true;
        }
    }

    if (mModalPopupsOpen != wasOpen) {
        updateWindowCursorShape();
        emit modalPopupsOpenChanged();
    }
}

void TileCanvas::onSplitterPositionChanged()
{
    mFirstPane.setSize(mSplitter.position());
    mSecondPane.setSize(1.0 - mSplitter.position());
    update();
}

void TileCanvas::paint(QPainter *painter)
{
    if (!mProject->hasLoaded()) {
        painter->fillRect(0, 0, width(), height(), mBackgroundColour);
        return;
    }

    // The order here is deliberate; see the clip region code in drawPane().
    const int firstPaneWidth = width() * mFirstPane.size();
    painter->fillRect(firstPaneWidth, 0, width() - firstPaneWidth, height(), mBackgroundColour);
    drawPane(painter, mSecondPane, 1);

    painter->fillRect(0, 0, firstPaneWidth, height(), mBackgroundColour);
    drawPane(painter, mFirstPane, 0);

    painter->setPen(QPen(mSplitColour));
    painter->drawLine(firstPaneWidth, 0, firstPaneWidth, height());
}

void TileCanvas::drawPane(QPainter *painter, const TileCanvasPane &pane, int paneIndex)
{
    const int paneWidth = width() * pane.size();
    const QSize zoomedTileSize = pane.zoomedSize(mProject->tileSize());
    // Try to only draw as much as could possibly fit within the pane.
//    const int horizontalStartTile = pane.offset().x() < 0 ? qFloor(qAbs(pane.offset().x()) / zoomedTileSize.width()) : 0;
//    const int verticalStartTile = pane.offset().y() < 0 ? qFloor(qAbs(pane.offset().y()) / zoomedTileSize.height()) : 0;
    const int tilesAcross = mProject->tilesWide();//qMin(mProject->tilesWide(), qCeil(paneWidth / zoomedTileSize.width()) + 1);
    const int tilesDown = mProject->tilesHigh();//qMin(mProject->tilesHigh(), qCeil(height() / zoomedTileSize.width()) + 1);

    painter->save();

    QPoint translateDistance;
    if (paneIndex == 1) {
        translateDistance.rx() = width() * mFirstPane.size();
    }
    translateDistance += pane.offset();
    painter->translate(translateDistance);

    // Can't set two clip regions on the same painting, so we only clip the left-hand pane
    // and simply paint it over the top of any out-of-bounds drawing the right-hand one has done.
    if (paneIndex == 0)
        painter->setClipRegion(QRegion(-translateDistance.x(), -translateDistance.y(), paneWidth, height()));

    const int zoomedMapWidth = qMin(tilesAcross * zoomedTileSize.width(), paneWidth);
    const int zoomedMapHeight = qMin(tilesDown * zoomedTileSize.height(), qFloor(height()));
    painter->fillRect(0, 0, zoomedMapWidth, zoomedMapHeight, mapBackgroundColour());

    for (int y = 0; y < tilesDown; ++y) {
        for (int x = 0; x < tilesAcross; ++x) {
            const QPoint topLeftInScene(x * mProject->tileWidth(), y * mProject->tileHeight());
            const QRect rect(x * zoomedTileSize.width(), y * zoomedTileSize.height(),
                zoomedTileSize.width(), zoomedTileSize.height());

            // If the tile pen is in use and it's over this tile, draw it, otherwise draw the current tile
            // at that location as usual.
            bool previewTile = false;
            if (mTilePenPreview) {
                QRect tileSceneRect(topLeftInScene.x(), topLeftInScene.y(), mProject->tileWidth(), mProject->tileHeight());
                previewTile = tileSceneRect.contains(mCursorSceneX, mCursorSceneY);
            }

            if (previewTile) {
                painter->drawImage(rect, *mPenTile->tileset()->image(), mPenTile->sourceRect());
            } else {
                const Tile *tile = mProject->tileAt(topLeftInScene);
                if (tile) {
                    painter->drawImage(rect, *tile->tileset()->image(), tile->sourceRect());
                }
            }

            if (mGridVisible) {
                QPen pen(mGridColour);
                painter->setPen(pen);

                painter->drawLine(rect.x(), rect.y(), rect.x(), rect.y() + rect.height() - 1);

                if (x == tilesAcross - 1) {
                    // If this is the right-most edge tile, draw a line on the outside of it.
                    painter->drawLine(rect.x() + zoomedTileSize.width(), rect.y(),
                        rect.x() + zoomedTileSize.width(), rect.y() + rect.height() - 1);
                }

                painter->drawLine(rect.x() + 1, rect.y(), rect.x() + rect.width() - 1, rect.y());

                if (y == tilesDown - 1) {
                    // If this is the bottom-most edge tile, draw a line on the outside of it.
                    painter->drawLine(rect.x(), rect.y() + zoomedTileSize.height(),
                        rect.x() + rect.width(), rect.y() + zoomedTileSize.width());
                }
            }
        }
    }

    painter->restore();
}

int TileCanvas::paneWidth(int index) const
{
    return index == 0 ? mFirstPane.size() * width() : width() - mFirstPane.size() * width();
}

void TileCanvas::centrePanes(bool respectSceneCentred)
{
    if (!respectSceneCentred || (respectSceneCentred && mFirstPane.isSceneCentered())) {
        const QPoint newOffset(paneWidth(0) / 2 - (mProject->widthInPixels() * mFirstPane.zoomLevel()) / 2,
            height() / 2 - (mProject->heightInPixels() * mFirstPane.zoomLevel()) / 2);
        mFirstPane.setOffset(newOffset);
    }

    if (!respectSceneCentred || (respectSceneCentred && mSecondPane.isSceneCentered())) {
        const QPoint newOffset(paneWidth(1) / 2 - (mProject->widthInPixels() * mFirstPane.zoomLevel()) / 2,
            height() / 2 - (mProject->heightInPixels() * mFirstPane.zoomLevel()) / 2);
        mSecondPane.setOffset(newOffset);
    }

    update();
}

bool TileCanvas::mouseOverSplitterHandle(const QPoint &mousePos)
{
    const QRect splitterRegion(paneWidth(0) - mSplitter.width() / 2, 0, mSplitter.width(), height());
    return splitterRegion.contains(mousePos);
}

void TileCanvas::reset()
{
    mFirstPane.reset();
    mSecondPane.reset();
    setCurrentPane(nullptr);
    mSplitter.setPosition(mFirstPane.size());
    mSplitter.setPressed(false);
    mSplitter.setHovered(false);
    setCursorX(0);
    setCursorY(0);
    mCursorPaneX = 0;
    mCursorPaneY = 0;
    mCursorSceneX = 0;
    mCursorSceneY = 0;
    setCursorTilePixelX(0);
    setCursorTilePixelY(0);
    mContainsMouse = false;
    mMouseButtonPressed = Qt::NoButton;
    mPressPosition = QPoint(0, 0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
    setAltPressed(false);
    mToolBeforeAltPressed = PenTool;
    mSpacePressed = false;
    mHasBlankCursor = false;

    // Things that we don't want to set, as they
    // don't really need to be reset each time:
    // - mode
    // - tool
    // - toolSize

    update();
}

void TileCanvas::centreView()
{
    centrePanes(false);
}

void TileCanvas::swatchLeft()
{
    QPoint tilePos = mProject->tileIdToTilePos(mPenTile->id());
    tilePos.setX(qMax(0, tilePos.x() - 1));
    Tile *newTile = mProject->tilesetTileAtTilePos(tilePos);
    setPenTile(newTile);
}

void TileCanvas::swatchRight()
{
    QPoint tilePos = mProject->tileIdToTilePos(mPenTile->id());
    tilePos.setX(qMin(tilePos.x() + 1, mProject->tileset()->tilesWide() - 1));
    Tile *newTile = mProject->tilesetTileAtTilePos(tilePos);
    setPenTile(newTile);
}

void TileCanvas::swatchUp()
{
    QPoint tilePos = mProject->tileIdToTilePos(mPenTile->id());
    tilePos.setY(qMax(0, tilePos.y() - 1));
    Tile *newTile = mProject->tilesetTileAtTilePos(tilePos);
    setPenTile(newTile);
}

void TileCanvas::swatchDown()
{
    QPoint tilePos = mProject->tileIdToTilePos(mPenTile->id());
    tilePos.setY(qMin(tilePos.y() + 1, mProject->tileset()->tilesHigh() - 1));
    Tile *newTile = mProject->tilesetTileAtTilePos(tilePos);
    setPenTile(newTile);
}

void TileCanvas::onTilesetChanged(Tileset *oldTileset, Tileset *newTileset)
{
    if (oldTileset) {
        oldTileset->disconnect(SIGNAL(imageChanged()), this, SLOT(update()));
    }

    if (newTileset) {
        connect(newTileset, SIGNAL(imageChanged()), this, SLOT(update()));
    }
}

void TileCanvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    centrePanes();
}

TileCanvas::PixelCandidateData TileCanvas::penEraserPixelCandidates(Tool tool) const
{
    PixelCandidateData candidateData;

    const QPoint topLeft(qRound(mCursorSceneFX - mToolSize / 2.0), qRound(mCursorSceneFY - mToolSize / 2.0));
    const QPoint bottomRight(qRound(mCursorSceneFX + mToolSize / 2.0), qRound(mCursorSceneFY + mToolSize / 2.0));
    QPoint scenePos(topLeft);
    for (; scenePos.y() < bottomRight.y(); ++scenePos.ry()) {
        for (scenePos.rx() = topLeft.x(); scenePos.x() < bottomRight.x(); ++scenePos.rx()) {
            const Tile *tile = mProject->tileAt(scenePos);
            if (tile) {
                const QPoint tilePixelPos = scenePosToTilePixelPos(scenePos);
                const QColor previousColour = tile->pixelColor(tilePixelPos);
                // Don't do anything if the colours are the same; this prevents issues
                // with undos not undoing everything across tiles.
                const bool hasEffect = tool == PenTool ? penColour() != previousColour : previousColour != QColor(Qt::transparent);
                if (hasEffect) {
                    candidateData.scenePositions.append(scenePos);
                    candidateData.previousColours.append(previousColour);
                }
            }
        }
    }

    return candidateData;
}

TileCanvas::PixelCandidateData TileCanvas::fillPixelCandidates() const
{
    PixelCandidateData candidateData;

    const QPoint tilePos = QPoint(mCursorSceneX, mCursorSceneY);
    Tile *tile = mProject->tileAt(tilePos);
    if (!tile) {
        return candidateData;
    }

    const QPoint tilePixelPos = scenePosToTilePixelPos(tilePos);
    const QPoint tileTopLeftScenePos = tilePos - tilePixelPos;
    const QColor previousColour = tile->pixelColor(tilePixelPos);
    // Don't do anything if the colours are the same.
    if (previousColour == penColour()) {
        return candidateData;
    }

    QVector<QPoint> tilePixelPositions;
    pixelFloodFill(tile, tilePixelPos, previousColour, penColour(), tilePixelPositions);

    for (const QPoint &pixelPos : tilePixelPositions) {
        candidateData.scenePositions.append(tileTopLeftScenePos + pixelPos);
    }

    candidateData.previousColours.append(previousColour);
    return candidateData;
}

TileCanvas::TileCandidateData TileCanvas::fillTileCandidates() const
{
    TileCandidateData candidateData;

    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    const Tile *tile = mProject->tileAt(scenePos);
    const int previousTileId = tile ? tile->id() : Tile::invalidId();
    const int newTileId = mPenTile ? mPenTile->id() : -1;
    // Don't do anything if the tiles are the same.
    if (newTileId == previousTileId) {
        return candidateData;
    }

    const int xTile = scenePos.x() / mProject->tileWidth();
    const int yTile = scenePos.y() / mProject->tileHeight();
    tileFloodFill(mProject, tile, QPoint(xTile, yTile), previousTileId, newTileId, candidateData.tilePositions);

    candidateData.previousTile = previousTileId;
    candidateData.newTileId = newTileId;
    return candidateData;
}

void TileCanvas::applyCurrentTool()
{
    switch (mTool) {
    case PenTool: {
        if (mMode == PixelMode) {
            const PixelCandidateData candidateData = penEraserPixelCandidates(mTool);
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mProject->beginMacro(QLatin1String("PixelPenTool"));
            mProject->addChange(new ApplyPixelPenCommand(this, candidateData.scenePositions, candidateData.previousColours, penColour()));
        } else {
            const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
            const Tile *tile = mProject->tileAt(scenePos);
            const int previousTileId = tile ? tile->id() : Tile::invalidId();
            const int newTileId = mPenTile ? mPenTile->id() : -1;
            // Don't do anything if the tiles are the same.
            if (newTileId == previousTileId) {
                return;
            }

            mProject->beginMacro(QLatin1String("TilePenTool"));
            const int xTile = scenePos.x() / mProject->tileWidth();
            const int yTile = scenePos.y() / mProject->tileHeight();
            mProject->addChange(new ApplyTilePenCommand(this, QPoint(xTile, yTile), previousTileId, newTileId));
        }
        break;
    }
    case EyeDropperTool: {
        const QPoint tilePos = QPoint(mCursorSceneX, mCursorSceneY);
        Tile *tile = mProject->tileAt(tilePos);
        if (tile) {
            if (mMode == PixelMode) {
                setPenForegroundColour(tile->pixelColor(mCursorTilePixelX, mCursorTilePixelY));
            } else {
                setPenTile(tile);
            }
        }
        break;
    }
    case EraserTool: {
        if (mMode == PixelMode) {
            const PixelCandidateData candidateData = penEraserPixelCandidates(mTool);
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mProject->beginMacro(QLatin1String("PixelEraserTool"));
            mProject->addChange(new ApplyPixelEraserCommand(this, candidateData.scenePositions, candidateData.previousColours));
        } else {
            const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
            const Tile *tile = mProject->tileAt(scenePos);
            const int previousTileId = tile ? tile->id() : Tile::invalidId();
            if (previousTileId == Tile::invalidId()) {
                return;
            }

            const int xTile = scenePos.x() / mProject->tileWidth();
            const int yTile = scenePos.y() / mProject->tileHeight();
            mProject->beginMacro(QLatin1String("PixelEraserTool"));
            mProject->addChange(new ApplyTileEraserCommand(this, QPoint(xTile, yTile), previousTileId));
        }
        break;
    }
    case FillTool: {
        if (mMode == PixelMode) {
            const PixelCandidateData candidateData = fillPixelCandidates();
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mProject->beginMacro(QLatin1String("PixelFillTool"));
            mProject->addChange(new ApplyPixelFillCommand(this, candidateData.scenePositions,
                candidateData.previousColours.first(), penColour()));
        } else {
            const TileCandidateData candidateData = fillTileCandidates();
            if (candidateData.tilePositions.isEmpty()) {
                return;
            }

            mProject->beginMacro(QLatin1String("TileFillTool"));
            mProject->addChange(new ApplyTileFillCommand(this, candidateData.tilePositions,
                candidateData.previousTile, candidateData.newTileId));
        }
        break;
    }
    }
}

QPoint TileCanvas::scenePosToTilePixelPos(const QPoint &scenePos) const
{
    return QPoint(scenePos.x() % mProject->tileWidth(),
        scenePos.y() % mProject->tileHeight());
}

// This function actually operates on the image.
void TileCanvas::applyPixelPenTool(const QPoint &scenePos, const QColor &colour)
{
    Tile *tile = mProject->tileAt(scenePos);
    Q_ASSERT_X(tile, Q_FUNC_INFO, qPrintable(QString::fromLatin1(
        "No tile at scene pos {%1, %2}").arg(scenePos.x()).arg(scenePos.y())));
    const QPoint pixelPos = scenePosToTilePixelPos(scenePos);
    const QPoint tilsetPixelPos = tile->sourceRect().topLeft() + pixelPos;
    mProject->tileset()->setPixelColor(tilsetPixelPos.x(), tilsetPixelPos.y(), colour);
    update();
}

void TileCanvas::applyTilePenTool(const QPoint &tilePos, int id)
{
    mProject->setTileAtPixelPos(tilePos, id);
    update();
}

void TileCanvas::updateCursorPos(const QPoint &eventPos)
{
    setCursorX(eventPos.x());
    setCursorY(eventPos.y());
    // Don't change current panes if panning, as the mouse position should
    // be allowed to go outside of the original pane.
    if (!mSpacePressed) {
        setCurrentPane(hoveredPane(eventPos));
    }

    const int firstPaneWidth = paneWidth(0);
    const bool inFirstPane = eventPos.x() <= firstPaneWidth;
    mCursorPaneX = eventPos.x() - (!inFirstPane ? firstPaneWidth : 0);
    mCursorPaneY = eventPos.y();

    // TODO: when would this happen?
    const QSize zoomedTileSize = mCurrentPane->zoomedSize(mProject->tileSize());
    if (zoomedTileSize.isEmpty()) {
        mCursorSceneFX = -1;
        mCursorSceneFY = -1;
        mCursorSceneX = -1;
        mCursorSceneY = -1;
        setCursorTilePixelX(-1);
        setCursorTilePixelY(-1);
        // We could do this once at the beginning of the function, but we
        // try to avoid unnecessary property changes.
        setCursorPixelColour(QColor(Qt::black));
        return;
    }

    // We need the position as floating point numbers so that pen sizes > 1 work properly.
    mCursorSceneFX = qreal(mCursorPaneX - mCurrentPane->offset().x()) / mProject->tileWidth() / mCurrentPane->zoomLevel() * mProject->tileWidth();
    mCursorSceneFY = qreal(mCursorPaneY - mCurrentPane->offset().y()) / mProject->tileHeight() / mCurrentPane->zoomLevel() * mProject->tileHeight();
    mCursorSceneX = mCursorSceneFX;
    mCursorSceneY = mCursorSceneFY;

    if (mCursorSceneX < 0 || mCursorSceneX >= mProject->widthInPixels()
        || mCursorSceneY < 0 || mCursorSceneY >= mProject->heightInPixels()) {
        setCursorTilePixelX(-1);
        setCursorTilePixelY(-1);

        setTilePenPreview(false);

        setCursorPixelColour(QColor(Qt::black));
    } else {
        const QPoint cursorPixelPos = scenePosToTilePixelPos(QPoint(mCursorSceneX, mCursorSceneY));
        setCursorTilePixelX(cursorPixelPos.x());
        setCursorTilePixelY(cursorPixelPos.y());

        updateTilePenPreview();

        const QPoint cursorScenePos = QPoint(mCursorSceneX, mCursorSceneY);
        const Tile *tile = mProject->tileAt(cursorScenePos);
        if (!tile) {
            setCursorPixelColour(QColor(Qt::black));
        } else {
            const QPoint tilePixelPos = scenePosToTilePixelPos(cursorScenePos);
            setCursorPixelColour(tile->pixelColor(tilePixelPos));
        }

        if (mTilePenPreview) {
            update();
        }
    }
}

void TileCanvas::onLoadedChanged()
{
    if (mProject->hasLoaded()) {
        centrePanes();

        setPenTile(mProject->tilesetTileAt(0, 0));
    } else {
        setPenTile(nullptr);
    }

    updateWindowCursorShape();
}

void TileCanvas::updateWindowCursorShape()
{
    // Hide the window's cursor when we're in the spotlight; otherwise, use the non-custom arrow cursor.
    const bool nothingOverUs = mProject->hasLoaded() && hasActiveFocus() /*&& !mModalPopupsOpen*/ && mContainsMouse;
    const bool splitterHovered = mSplitter.isEnabled() && mSplitter.isHovered();
    setHasBlankCursor(nothingOverUs && !mSpacePressed && !splitterHovered);

    Qt::CursorShape cursorShape = Qt::BlankCursor;
    if (!mHasBlankCursor) {
        if (mSpacePressed) {
            cursorShape = mMouseButtonPressed == Qt::LeftButton ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
        } else {
            cursorShape = mSplitter.isEnabled() && mSplitter.isHovered() ? Qt::SplitHCursor : Qt::ArrowCursor;
        }
        setCursor(Qt::ArrowCursor);
    } else {
        setCursor(Qt::BlankCursor);
    }

    if (lcCanvas().isDebugEnabled()) {
        QString cursorName;

        switch (cursorShape) {
        case Qt::ClosedHandCursor:
            cursorName = "ClosedHandCursor";
            break;
        case Qt::OpenHandCursor:
            cursorName = "OpenHandCursor";
            break;
        case Qt::BlankCursor:
            cursorName = "BlankCursor";
            break;
        case Qt::ArrowCursor:
            cursorName = "ArrowCursor";
            break;
        case Qt::SplitHCursor:
            cursorName = "SplitHCursor";
            break;
        default:
            break;
        }

        qDebug() << "Updating window cursor shape... mProject->hasLoaded():" << mProject->hasLoaded()
                 << "hasActiveFocus()" << hasActiveFocus()
                 << "mContainsMouse" << mContainsMouse
                 << "mSpacePressed" << mSpacePressed
                 << "mSplitter.isHovered()" << mSplitter.isHovered()
                 << "cursor shape" << cursorName;
    }
    window()->setCursor(QCursor(cursorShape));
}

void TileCanvas::error(const QString &message)
{
//    qWarning() << Q_FUNC_INFO << message;
    emit errorOccurred(message);
}

QColor TileCanvas::penColour() const
{
    return mMouseButtonPressed == Qt::LeftButton ? mPenForegroundColour : mPenBackgroundColour;
}

void TileCanvas::setHasBlankCursor(bool hasCustomCursor)
{
    if (hasCustomCursor == mHasBlankCursor)
        return;

    mHasBlankCursor = hasCustomCursor;
    emit hasBlankCursorChanged();
}

void TileCanvas::updateTilePenPreview()
{
    setTilePenPreview(!areModalPopupsOpen() && mMode == TileMode && mTool == PenTool);
}

void TileCanvas::setTilePenPreview(bool tilePenPreview)
{
    if (tilePenPreview == mTilePenPreview)
        return;

    mTilePenPreview = tilePenPreview;
    update();
}

void TileCanvas::setCurrentPane(TileCanvasPane *pane)
{
    if (pane == mCurrentPane) {
        return;
    }

    mCurrentPane = pane;
    emit currentPaneChanged();
}

TileCanvasPane *TileCanvas::hoveredPane(const QPoint &pos)
{
    const int firstPaneWidth = paneWidth(0);
    return pos.x() <= firstPaneWidth ? &mFirstPane : &mSecondPane;
    return &mSecondPane;
}

QPoint TileCanvas::eventPosRelativeToCurrentPane(const QPoint &pos)
{
    if (!mCurrentPane || mCurrentPane == &mFirstPane) {
        return pos;
    }

    return pos - QPoint(paneWidth(1), 0);
}

void TileCanvas::restoreToolBeforeAltPressed()
{
    setAltPressed(false);
    setTool(mToolBeforeAltPressed);
}

void TileCanvas::wheelEvent(QWheelEvent *event)
{
    if (!mProject->hasLoaded()) {
        event->ignore();
        return;
    }

    mCurrentPane->setSceneCentered(false);

    const int oldZoomLevel = mCurrentPane->zoomLevel();
    const int newZoomLevel = oldZoomLevel + (event->angleDelta().y() > 0 ? 1 : -1);
    mCurrentPane->setZoomLevel(newZoomLevel);

    // From: http://stackoverflow.com/a/38302057/904422
    QPoint relativeEventPos = eventPosRelativeToCurrentPane(event->pos());
    mCurrentPane->setOffset(relativeEventPos -
        float(mCurrentPane->zoomLevel()) / float(oldZoomLevel) * (relativeEventPos - mCurrentPane->offset()));
}

void TileCanvas::mousePressEvent(QMouseEvent *event)
{
    QQuickPaintedItem::mousePressEvent(event);

    // Is it possible to get a press without a hover enter? If so, we need this line.
    updateCursorPos(event->pos());

    if (!mProject->hasLoaded()) {
        return;
    }

    event->accept();

    mMouseButtonPressed = event->button();
    mPressPosition = event->pos();
    mCurrentPaneOffsetBeforePress = mCurrentPane->offset();
    setContainsMouse(true);
    // Pressing the mouse while holding down space should pan.
    if (!mSpacePressed) {
        if (mSplitter.isEnabled() && mouseOverSplitterHandle(event->pos())) {
            mSplitter.setPressed(true);
        } else {
            applyCurrentTool();
        }
    } else {
        updateWindowCursorShape();
    }
}

void TileCanvas::mouseMoveEvent(QMouseEvent *event)
{
    QQuickPaintedItem::mouseMoveEvent(event);

    updateCursorPos(event->pos());

    if (!mProject->hasLoaded())
        return;

    setContainsMouse(true);

    if (mMouseButtonPressed) {
        if (mSplitter.isEnabled() && mSplitter.isPressed()) {
            mSplitter.setPosition(mCursorX / width());
        } else {
            if (!mSpacePressed) {
                applyCurrentTool();
            } else {
                // Panning.
                mCurrentPane->setSceneCentered(false);
                mCurrentPane->setOffset(mCurrentPaneOffsetBeforePress + (event->pos() - mPressPosition));
                update();
            }
        }
    }
}

void TileCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickPaintedItem::mouseReleaseEvent(event);

    updateCursorPos(event->pos());

    if (!mProject->hasLoaded())
        return;

    if (mProject->isComposingMacro()) {
        mProject->endMacro();
    }

    mMouseButtonPressed = Qt::NoButton;
    mPressPosition = QPoint(0, 0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
    updateWindowCursorShape();
    mSplitter.setPressed(false);
}

void TileCanvas::hoverEnterEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverEnterEvent(event);

    updateCursorPos(event->pos());

    setContainsMouse(true);

    if (!mProject->hasLoaded())
        return;
}

void TileCanvas::hoverMoveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverMoveEvent(event);

    updateCursorPos(event->pos());

    setContainsMouse(true);

    if (!mProject->hasLoaded())
        return;

    const bool wasSplitterHovered = mSplitter.isHovered();
    mSplitter.setHovered(mouseOverSplitterHandle(event->pos()));
    if (mSplitter.isHovered() != wasSplitterHovered) {
        updateWindowCursorShape();
    }
}

void TileCanvas::hoverLeaveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverLeaveEvent(event);

    setContainsMouse(false);

    if (!mProject->hasLoaded())
        return;

    // Don't reset the cursor position here, because it looks jarring.
    setCursorTilePixelX(-1);
    setCursorTilePixelY(-1);
}

void TileCanvas::keyPressEvent(QKeyEvent *event)
{
    QQuickPaintedItem::keyPressEvent(event);

    if (!mProject->hasLoaded())
        return;

    if (event->isAutoRepeat())
        return;

    if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_3) {
        setTool(static_cast<TileCanvas::Tool>(PenTool + event->key() - Qt::Key_1));
    } else if (event->key() == Qt::Key_Space) {
        mSpacePressed = true;
        updateWindowCursorShape();
    } else if (event->modifiers().testFlag(Qt::AltModifier)) {
        setAltPressed(true);
        mToolBeforeAltPressed = mTool;
        setTool(EyeDropperTool);
    }
}

void TileCanvas::keyReleaseEvent(QKeyEvent *event)
{
    QQuickPaintedItem::keyReleaseEvent(event);

    if (!mProject->hasLoaded())
        return;

    if (event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Space) {
        mSpacePressed = false;
        updateWindowCursorShape();
    } else if (mAltPressed && (event->key() == Qt::Key_Alt || event->modifiers().testFlag(Qt::AltModifier))) {
        restoreToolBeforeAltPressed();
    }
}

void TileCanvas::focusInEvent(QFocusEvent *event)
{
    QQuickPaintedItem::focusInEvent(event);

    updateWindowCursorShape();

    updateTilePenPreview();
}

void TileCanvas::focusOutEvent(QFocusEvent *event)
{
    QQuickPaintedItem::focusOutEvent(event);

    // The alt-to-eyedrop feature is meant to be temporary,
    // so it should restore the previous tool if focus is taken away.
    if (mAltPressed) {
        restoreToolBeforeAltPressed();
    }

    updateWindowCursorShape();

    setTilePenPreview(false);
}
