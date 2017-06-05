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
#include "tileset.h"
#include "tilesetproject.h"

TileCanvas::TileCanvas() :
    mTilesetProject(nullptr),
    mCursorTilePixelX(0),
    mCursorTilePixelY(0),
    mMode(TileMode),
    mPenTile(nullptr),
    mTilePenPreview(false)
{
    qCDebug(lcCanvasLifecycle) << "constructing" << this;
}

TileCanvas::~TileCanvas()
{
    qCDebug(lcCanvasLifecycle) << "destructing" << this;
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

void TileCanvas::paint(QPainter *painter)
{
    if (!mTilesetProject || !mTilesetProject->hasLoaded()) {
        painter->fillRect(0, 0, width(), height(), mBackgroundColour);
        return;
    }

    // The order here is deliberate; see the clip region code in drawPane().
    const int firstPaneWidth = width() * mFirstPane.size();
    painter->fillRect(firstPaneWidth, 0, width() - firstPaneWidth, height(), mBackgroundColour);
    drawPane(painter, mSecondPane, 1);

    painter->fillRect(0, 0, firstPaneWidth, height(), mBackgroundColour);
    drawPane(painter, mFirstPane, 0);

    if (mSplitScreen) {
        painter->setPen(QPen(mSplitColour));
        painter->drawLine(firstPaneWidth, 0, firstPaneWidth, height());
    }
}

void TileCanvas::drawPane(QPainter *painter, const CanvasPane &pane, int paneIndex)
{
    const int paneWidth = width() * pane.size();
    const QSize zoomedTileSize = pane.zoomedSize(mTilesetProject->tileSize());
    // Try to only draw as much as could possibly fit within the pane.
//    const int horizontalStartTile = pane.offset().x() < 0 ? qFloor(qAbs(pane.offset().x()) / zoomedTileSize.width()) : 0;
//    const int verticalStartTile = pane.offset().y() < 0 ? qFloor(qAbs(pane.offset().y()) / zoomedTileSize.height()) : 0;
    const int tilesAcross = mTilesetProject->tilesWide();//qMin(mProject->tilesWide(), qCeil(paneWidth / zoomedTileSize.width()) + 1);
    const int tilesDown = mTilesetProject->tilesHigh();//qMin(mProject->tilesHigh(), qCeil(height() / zoomedTileSize.width()) + 1);

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
            const QPoint topLeftInScene(x * mTilesetProject->tileWidth(), y * mTilesetProject->tileHeight());
            const QRect rect(x * zoomedTileSize.width(), y * zoomedTileSize.height(),
                zoomedTileSize.width(), zoomedTileSize.height());

            // If the tile pen is in use and it's over this tile, draw it, otherwise draw the current tile
            // at that location as usual.
            bool previewTile = false;
            if (mTilePenPreview) {
                QRect tileSceneRect(topLeftInScene.x(), topLeftInScene.y(), mTilesetProject->tileWidth(), mTilesetProject->tileHeight());
                previewTile = tileSceneRect.contains(mCursorSceneX, mCursorSceneY);
            }

            if (previewTile) {
                painter->drawImage(rect, *mPenTile->tileset()->image(), mPenTile->sourceRect());
            } else {
                const Tile *tile = mTilesetProject->tileAt(topLeftInScene);
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

void TileCanvas::swatchLeft()
{
    QPoint tilePos = mTilesetProject->tileIdToTilePos(mPenTile->id());
    tilePos.setX(qMax(0, tilePos.x() - 1));
    Tile *newTile = mTilesetProject->tilesetTileAtTilePos(tilePos);
    setPenTile(newTile);
}

void TileCanvas::swatchRight()
{
    QPoint tilePos = mTilesetProject->tileIdToTilePos(mPenTile->id());
    tilePos.setX(qMin(tilePos.x() + 1, mTilesetProject->tileset()->tilesWide() - 1));
    Tile *newTile = mTilesetProject->tilesetTileAtTilePos(tilePos);
    setPenTile(newTile);
}

void TileCanvas::swatchUp()
{
    QPoint tilePos = mTilesetProject->tileIdToTilePos(mPenTile->id());
    tilePos.setY(qMax(0, tilePos.y() - 1));
    Tile *newTile = mTilesetProject->tilesetTileAtTilePos(tilePos);
    setPenTile(newTile);
}

void TileCanvas::swatchDown()
{
    QPoint tilePos = mTilesetProject->tileIdToTilePos(mPenTile->id());
    tilePos.setY(qMin(tilePos.y() + 1, mTilesetProject->tileset()->tilesHigh() - 1));
    Tile *newTile = mTilesetProject->tilesetTileAtTilePos(tilePos);
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

void TileCanvas::connectSignals()
{
    ImageCanvas::connectSignals();

    mTilesetProject = qobject_cast<TilesetProject*>(mProject);
    Q_ASSERT_X(mTilesetProject, Q_FUNC_INFO, "Non-tileset project set on TileCanvas");

    connect(mTilesetProject, SIGNAL(tilesCleared()), this, SLOT(update()));
    connect(mTilesetProject, SIGNAL(tilesetChanged(Tileset *, Tileset *)), this, SLOT(onTilesetChanged(Tileset *, Tileset *)));

    setPenTile(mTilesetProject->tilesetTileAt(0, 0));

    // If the project already has this, we won't get the signal, so force it.
    if (mTilesetProject->tileset())
        onTilesetChanged(nullptr, mTilesetProject->tileset());
}

void TileCanvas::disconnectSignals()
{
    ImageCanvas::disconnectSignals();

    mTilesetProject->disconnect(SIGNAL(tilesCleared()), this, SLOT(update()));
    mTilesetProject->disconnect(SIGNAL(tilesetChanged(Tileset *, Tileset *)), this, SLOT(onTilesetChanged(Tileset *, Tileset *)));

    setPenTile(nullptr);

    mTilesetProject = nullptr;
}

void TileCanvas::toolChange()
{
    updateTilePenPreview();
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
            const Tile *tile = mTilesetProject->tileAt(scenePos);
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
    Tile *tile = mTilesetProject->tileAt(tilePos);
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
    tilesetPixelFloodFill(tile, tilePixelPos, previousColour, penColour(), tilePixelPositions);

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
    const Tile *tile = mTilesetProject->tileAt(scenePos);
    const int previousTileId = tile ? tile->id() : Tile::invalidId();
    const int newTileId = mPenTile ? mPenTile->id() : -1;
    // Don't do anything if the tiles are the same.
    if (newTileId == previousTileId) {
        return candidateData;
    }

    const int xTile = scenePos.x() / mTilesetProject->tileWidth();
    const int yTile = scenePos.y() / mTilesetProject->tileHeight();
    tilesetTileFloodFill(mTilesetProject, tile, QPoint(xTile, yTile), previousTileId, newTileId, candidateData.tilePositions);

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

            mTilesetProject->beginMacro(QLatin1String("PixelPenTool"));
            mTilesetProject->addChange(new ApplyPixelPenCommand(this, candidateData.scenePositions, candidateData.previousColours, penColour()));
        } else {
            const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
            const Tile *tile = mTilesetProject->tileAt(scenePos);
            const int previousTileId = tile ? tile->id() : Tile::invalidId();
            const int newTileId = mPenTile ? mPenTile->id() : -1;
            // Don't do anything if the tiles are the same.
            if (newTileId == previousTileId) {
                return;
            }

            mTilesetProject->beginMacro(QLatin1String("TilePenTool"));
            const int xTile = scenePos.x() / mTilesetProject->tileWidth();
            const int yTile = scenePos.y() / mTilesetProject->tileHeight();
            mTilesetProject->addChange(new ApplyTilePenCommand(this, QPoint(xTile, yTile), previousTileId, newTileId));
        }
        break;
    }
    case EyeDropperTool: {
        const QPoint tilePos = QPoint(mCursorSceneX, mCursorSceneY);
        Tile *tile = mTilesetProject->tileAt(tilePos);
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

            mTilesetProject->beginMacro(QLatin1String("PixelEraserTool"));
            mTilesetProject->addChange(new ApplyPixelEraserCommand(this, candidateData.scenePositions, candidateData.previousColours));
        } else {
            const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
            const Tile *tile = mTilesetProject->tileAt(scenePos);
            const int previousTileId = tile ? tile->id() : Tile::invalidId();
            if (previousTileId == Tile::invalidId()) {
                return;
            }

            const int xTile = scenePos.x() / mTilesetProject->tileWidth();
            const int yTile = scenePos.y() / mTilesetProject->tileHeight();
            mTilesetProject->beginMacro(QLatin1String("PixelEraserTool"));
            mTilesetProject->addChange(new ApplyTileEraserCommand(this, QPoint(xTile, yTile), previousTileId));
        }
        break;
    }
    case FillTool: {
        if (mMode == PixelMode) {
            const PixelCandidateData candidateData = fillPixelCandidates();
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mTilesetProject->beginMacro(QLatin1String("PixelFillTool"));
            mTilesetProject->addChange(new ApplyPixelFillCommand(this, candidateData.scenePositions,
                candidateData.previousColours.first(), penColour()));
        } else {
            const TileCandidateData candidateData = fillTileCandidates();
            if (candidateData.tilePositions.isEmpty()) {
                return;
            }

            mTilesetProject->beginMacro(QLatin1String("TileFillTool"));
            mTilesetProject->addChange(new ApplyTileFillCommand(this, candidateData.tilePositions,
                candidateData.previousTile, candidateData.newTileId));
        }
        break;
    }
    default:
        break;
    }
}

QPoint TileCanvas::scenePosToTilePixelPos(const QPoint &scenePos) const
{
    return QPoint(scenePos.x() % mTilesetProject->tileWidth(),
        scenePos.y() % mTilesetProject->tileHeight());
}

// This function actually operates on the image.
void TileCanvas::applyPixelPenTool(const QPoint &scenePos, const QColor &colour, bool markAsLastRelease)
{
    Tile *tile = mTilesetProject->tileAt(scenePos);
    Q_ASSERT_X(tile, Q_FUNC_INFO, qPrintable(QString::fromLatin1(
        "No tile at scene pos {%1, %2}").arg(scenePos.x()).arg(scenePos.y())));
    const QPoint pixelPos = scenePosToTilePixelPos(scenePos);
    const QPoint tilsetPixelPos = tile->sourceRect().topLeft() + pixelPos;
    mTilesetProject->tileset()->setPixelColor(tilsetPixelPos.x(), tilsetPixelPos.y(), colour);
    if (markAsLastRelease)
        mLastPixelPenPressScenePosition = scenePos;
    update();
}

void TileCanvas::applyTilePenTool(const QPoint &tilePos, int id)
{
    mTilesetProject->setTileAtPixelPos(tilePos, id);
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
    const QSize zoomedTileSize = mCurrentPane->zoomedSize(mTilesetProject->tileSize());
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
    mCursorSceneFX = qreal(mCursorPaneX - mCurrentPane->offset().x()) / mTilesetProject->tileWidth() / mCurrentPane->zoomLevel() * mTilesetProject->tileWidth();
    mCursorSceneFY = qreal(mCursorPaneY - mCurrentPane->offset().y()) / mTilesetProject->tileHeight() / mCurrentPane->zoomLevel() * mTilesetProject->tileHeight();
    mCursorSceneX = mCursorSceneFX;
    mCursorSceneY = mCursorSceneFY;

    if (mCursorSceneX < 0 || mCursorSceneX >= mTilesetProject->widthInPixels()
        || mCursorSceneY < 0 || mCursorSceneY >= mTilesetProject->heightInPixels()) {
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
        const Tile *tile = mTilesetProject->tileAt(cursorScenePos);
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
    if (mTilesetProject->hasLoaded()) {
        centrePanes();

        setPenTile(mTilesetProject->tilesetTileAt(0, 0));
    } else {
        setPenTile(nullptr);
    }

    updateWindowCursorShape();
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
    setTilePenPreview(mMode == TileMode && mTool == PenTool);
}

void TileCanvas::setTilePenPreview(bool tilePenPreview)
{
    if (tilePenPreview == mTilePenPreview)
        return;

    mTilePenPreview = tilePenPreview;
    update();
}

void TileCanvas::hoverLeaveEvent(QHoverEvent *event)
{
    ImageCanvas::hoverLeaveEvent(event);

    if (!mTilesetProject->hasLoaded())
        return;

    // Don't reset the cursor position here, because it looks jarring.
    setCursorTilePixelX(-1);
    setCursorTilePixelY(-1);
}

void TileCanvas::focusInEvent(QFocusEvent *event)
{
    ImageCanvas::focusInEvent(event);
    updateTilePenPreview();
}

void TileCanvas::focusOutEvent(QFocusEvent *event)
{
    ImageCanvas::focusOutEvent(event);
    setTilePenPreview(false);
}
