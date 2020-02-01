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

#include "tilecanvas.h"

#include <QCursor>
#include <QLoggingCategory>
#include <QPainter>
#include <QQuickWindow>
#include <QtMath>

#include "applypixelerasercommand.h"
#include "applypixelfillcommand.h"
#include "applypixellinecommand.h"
#include "applypixelpencommand.h"
#include "applytilecanvaspixelfillcommand.h"
#include "applytileerasercommand.h"
#include "applytilefillcommand.h"
#include "applytilepencommand.h"
#include "fillalgorithms.h"
#include "tileset.h"
#include "tilesetproject.h"
#include "utils.h"

TileCanvas::TileCanvas() :
    mTilesetProject(nullptr),
    mCursorTilePixelX(0),
    mCursorTilePixelY(0),
    mMode(TileMode),
    mPenTile(nullptr),
    mTilePenPreview(false),
    mGridVisible(false)
{
    qCDebug(lcImageCanvasLifecycle) << "constructing TileCanvas" << this;
}

TileCanvas::~TileCanvas()
{
    qCDebug(lcImageCanvasLifecycle) << "destructing TileCanvas" << this;
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

bool TileCanvas::isGridVisible() const
{
    return mGridVisible;
}

void TileCanvas::setGridVisible(bool gridVisible)
{
    mGridVisible = gridVisible;
    requestContentPaint();
    emit gridVisibleChanged();
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

void TileCanvas::reset()
{
    ImageCanvas::reset();

    setCursorTilePixelX(0);
    setCursorTilePixelY(0);
    setPenTile(nullptr);
    setTilePenPreview(false);

    // Things that we don't want to set, as they
    // don't really need to be reset each time:
    // - mode
    // - tool
    // - toolSize

    requestContentPaint();
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
        disconnect(oldTileset, &Tileset::imageChanged, this, &TileCanvas::requestContentPaint);
    }

    if (newTileset) {
        connect(newTileset, &Tileset::imageChanged, this, &TileCanvas::requestContentPaint);
    }
}

void TileCanvas::restoreState()
{
    ImageCanvas::restoreState();

    setGridVisible(mProject->uiState()->value("gridVisible", true).toBool());
}

void TileCanvas::saveState()
{
    ImageCanvas::saveState();

    if (!mGridVisible)
        mProject->uiState()->setValue("gridVisible", false);
}

void TileCanvas::connectSignals()
{
    ImageCanvas::connectSignals();

    mTilesetProject = qobject_cast<TilesetProject*>(mProject);
    Q_ASSERT_X(mTilesetProject, Q_FUNC_INFO, "Non-tileset project set on TileCanvas");

    connect(mTilesetProject, &TilesetProject::tilesCleared, this, &TileCanvas::requestContentPaint);
    connect(mTilesetProject, &TilesetProject::tilesetChanged, this, &TileCanvas::onTilesetChanged);

    setPenTile(mTilesetProject->tilesetTileAt(0, 0));

    // If the project already has this, we won't get the signal, so force it.
    if (mTilesetProject->tileset())
        onTilesetChanged(nullptr, mTilesetProject->tileset());
}

void TileCanvas::disconnectSignals()
{
    ImageCanvas::disconnectSignals();

    disconnect(mTilesetProject, &TilesetProject::tilesCleared, this, &TileCanvas::requestContentPaint);
    disconnect(mTilesetProject, &TilesetProject::tilesetChanged, this, &TileCanvas::onTilesetChanged);

    setPenTile(nullptr);

    mTilesetProject = nullptr;
}

void TileCanvas::toolChange()
{
    updateTilePenPreview();
}

bool TileCanvas::supportsSelectionTool() const
{
    return false;
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

ImageCanvas::PixelCandidateData TileCanvas::greedyFillPixelCandidates() const
{
    // TODO
    return PixelCandidateData();
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
    switch (effectiveTool()) {
    case PenTool: {
        if (mMode == PixelMode) {
            mProject->beginMacro(QLatin1String("PixelLineTool"));
            // Draw the line on top of what has already been painted using a special composition mode.
            // This ensures that e.g. a translucent red overwrites whatever pixels it
            // lies on, rather than blending with them.
            mProject->addChange(new ApplyPixelLineCommand(this, -1, *mTilesetProject->tileset()->image(), linePoint1(), linePoint2(),
                mPressScenePosition, mLastPixelPenPressScenePositionF, QPainter::CompositionMode_Source));
            break;
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
            const PixelCandidateData candidateData = penEraserPixelCandidates(EraserTool);
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mTilesetProject->beginMacro(QLatin1String("PixelEraserTool"));
            mTilesetProject->addChange(new ApplyPixelEraserCommand(this, -1, candidateData.scenePositions, candidateData.previousColours));
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
            mTilesetProject->addChange(new ApplyTileCanvasPixelFillCommand(this, candidateData.scenePositions,
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

QRect TileCanvas::sceneRectToTileRect(const QRect &sceneRect) const
{
    return QRect(QPoint(Utils::divFloor(sceneRect.left(), mTilesetProject->tileWidth()), Utils::divFloor(sceneRect.top(), mTilesetProject->tileHeight())),
                 QPoint(Utils::divFloor(sceneRect.right(), mTilesetProject->tileWidth()), Utils::divFloor(sceneRect.bottom(), mTilesetProject->tileHeight())));
}

QList<ImageCanvas::SubImage> TileCanvas::subImagesInBounds(const QRect &bounds) const
{
    const QRect tileRect = sceneRectToTileRect(bounds);
    QList<ImageCanvas::SubImage> subImages;
    for (int y = tileRect.top(); y <= tileRect.bottom(); ++y) {
        for (int x = tileRect.left(); x <= tileRect.right(); ++x) {
            const Tile *const tile = mTilesetProject->tileAtTilePos({x, y});
            if (tile) {
                subImages.append({tile->sourceRect(), {x * mTilesetProject->tileWidth(), y * mTilesetProject->tileHeight()}});
            }
        }
    }
    return subImages;
}

// This function actually operates on the image.
void TileCanvas::applyPixelPenTool(int layerIndex, const QPoint &scenePos, const QColor &colour, bool markAsLastRelease)
{
    Q_ASSERT(layerIndex == -1);

    Tile *tile = mTilesetProject->tileAt(scenePos);
    Q_ASSERT_X(tile, Q_FUNC_INFO, qPrintable(QString::fromLatin1(
        "No tile at scene pos {%1, %2}").arg(scenePos.x()).arg(scenePos.y())));
    const QPoint pixelPos = scenePosToTilePixelPos(scenePos);
    const QPoint tilsetPixelPos = tile->sourceRect().topLeft() + pixelPos;
    mTilesetProject->tileset()->setPixelColor(tilsetPixelPos.x(), tilsetPixelPos.y(), colour);
    if (markAsLastRelease)
        mLastPixelPenPressScenePositionF = scenePos;
    requestContentPaint();
}

void TileCanvas::applyTilePenTool(const QPoint &tilePos, int id)
{
    mTilesetProject->setTileAtPixelPos(tilePos, id);
    requestContentPaint();
}

void TileCanvas::applyPixelLineTool(int, const QImage &lineImage, const QRect &lineRect, const QPointF &lastPixelPenReleaseScenePosition)
{
    mLastPixelPenPressScenePositionF = lastPixelPenReleaseScenePosition;
    QPainter painter(mTilesetProject->tileset()->image());
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(lineRect, lineImage);
    requestContentPaint();
    mTilesetProject->tileset()->notifyImageChanged();
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
    const int zoomLevel = mCurrentPane->integerZoomLevel();
    mCursorSceneFX = qreal(mCursorPaneX - mCurrentPane->integerOffset().x()) / mTilesetProject->tileWidth() / zoomLevel * mTilesetProject->tileWidth();
    mCursorSceneFY = qreal(mCursorPaneY - mCurrentPane->integerOffset().y()) / mTilesetProject->tileHeight() / zoomLevel * mTilesetProject->tileHeight();
    mCursorSceneX = mCursorSceneFX;
    mCursorSceneY = mCursorSceneFY;

    if (!isCursorWithinProjectBounds()) {
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
            requestContentPaint();
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

void TileCanvas::updateTilePenPreview()
{
    setTilePenPreview(mMode == TileMode && mTool == PenTool);
}

void TileCanvas::setTilePenPreview(bool tilePenPreview)
{
    if (tilePenPreview == mTilePenPreview)
        return;

    mTilePenPreview = tilePenPreview;
    requestContentPaint();
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
