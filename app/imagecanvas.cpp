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

#include "imagecanvas.h"

#include <QClipboard>
#include <QCursor>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QQuickWindow>
#include <QtMath>

#include "applypixelerasercommand.h"
#include "applypixelfillcommand.h"
#include "applypixelpencommand.h"
#include "deleteimagecanvasselectioncommand.h"
#include "flipimagecanvasselectioncommand.h"
#include "floodfill.h"
#include "imageproject.h"
#include "moveimagecanvasselectioncommand.h"
#include "pasteimagecanvascommand.h"
#include "project.h"
#include "tileset.h"
#include "utils.h"

Q_LOGGING_CATEGORY(lcCanvas, "app.canvas")
Q_LOGGING_CATEGORY(lcCanvasLifecycle, "app.canvas.lifecycle")

ImageCanvas::ImageCanvas() :
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
    mCursorPixelColour(Qt::black),
    mContainsMouse(false),
    mMouseButtonPressed(Qt::NoButton),
    mScrollZoom(false),
    mTool(PenTool),
    mToolSize(1),
    mMaxToolSize(100),
    mPenForegroundColour(Qt::black),
    mPenBackgroundColour(Qt::white),
    mPotentiallySelecting(false),
    mHasSelection(false),
    mMovingSelection(false),
    mHasMovedSelection(false),
    mIsSelectionFromPaste(false),
    mAltPressed(false),
    mToolBeforeAltPressed(PenTool),
    mSpacePressed(false),
    mHasBlankCursor(false)
{
    setEnabled(false);
    setFlag(QQuickItem::ItemIsFocusScope);

    mFirstPane.setObjectName("firstPane");
    mSecondPane.setObjectName("secondPane");
    mSplitter.setPosition(mFirstPane.size());

    connect(&mFirstPane, SIGNAL(zoomLevelChanged()), this, SLOT(update()));
    connect(&mSecondPane, SIGNAL(zoomLevelChanged()), this, SLOT(update()));
    connect(&mSplitter, SIGNAL(positionChanged()), this, SLOT(onSplitterPositionChanged()));

    mCheckerPixmap = QPixmap(":/images/checker.png");
    if (mCheckerPixmap.isNull()) {
        qWarning() << "Failed to load checker pixmap";
    }

    installEventFilter(this);

    qCDebug(lcCanvasLifecycle) << "constructing" << this;
}

ImageCanvas::~ImageCanvas()
{
    qCDebug(lcCanvasLifecycle) << "destructing" << this;
}

Project *ImageCanvas::project() const
{
    return mProject;
}

void ImageCanvas::setProject(Project *project)
{
    if (project == mProject)
        return;

    if (mProject) {
        disconnectSignals();
    }

    mProject = project;
    mImageProject = qobject_cast<ImageProject*>(mProject);

    if (mProject) {
        connectSignals();

        setAcceptedMouseButtons(Qt::AllButtons);
        setAcceptHoverEvents(true);
        setCursor(Qt::BlankCursor);
        setEnabled(true);
        forceActiveFocus();
    } else {
        setAcceptedMouseButtons(Qt::NoButton);
        setAcceptHoverEvents(false);
        setCursor(Qt::ArrowCursor);
        setEnabled(false);
    }

    emit projectChanged();
}

bool ImageCanvas::gridVisible() const
{
    return mGridVisible;
}

void ImageCanvas::setGridVisible(bool gridVisible)
{
    mGridVisible = gridVisible;
    update();
    emit gridVisibleChanged();
}

QColor ImageCanvas::gridColour() const
{
    return mGridColour;
}

void ImageCanvas::setGridColour(const QColor &gridColour)
{
    if (gridColour == mGridColour)
        return;

    mGridColour = gridColour;
    update();
    emit gridColourChanged();
}

QColor ImageCanvas::splitColour() const
{
    return mSplitColour;
}

void ImageCanvas::setSplitColour(const QColor &splitColour)
{
    if (splitColour == mSplitColour)
        return;

    mSplitColour = splitColour;
    update();
    emit splitColourChanged();
}

QColor ImageCanvas::backgroundColour() const
{
    return mBackgroundColour;
}

void ImageCanvas::setBackgroundColour(const QColor &backgroundColour)
{
    if (backgroundColour == mBackgroundColour)
        return;

    mBackgroundColour = backgroundColour;
    update();
    emit backgroundColourChanged();
}

int ImageCanvas::cursorX() const
{
    return mCursorX;
}

void ImageCanvas::setCursorX(int cursorX)
{
    if (cursorX == mCursorX)
        return;

    mCursorX = cursorX;
    emit cursorXChanged();
}

int ImageCanvas::cursorY() const
{
    return mCursorY;
}

void ImageCanvas::setCursorY(int cursorY)
{
    if (cursorY == mCursorY)
        return;

    mCursorY = cursorY;
    emit cursorYChanged();
}

int ImageCanvas::cursorSceneX() const
{
    return mCursorSceneX;
}

void ImageCanvas::setCursorSceneX(int x)
{
    if (x == mCursorSceneX)
        return;

    mCursorSceneX = x;
    emit cursorSceneXChanged();
}

int ImageCanvas::cursorSceneY() const
{
    return mCursorSceneY;
}

void ImageCanvas::setCursorSceneY(int y)
{
    if (y == mCursorSceneY)
        return;

    mCursorSceneY = y;
    emit cursorSceneYChanged();
}

ImageCanvas::Tool ImageCanvas::tool() const
{
    return mTool;
}

void ImageCanvas::setTool(const Tool &tool)
{
    if (tool == mTool)
        return;

    mTool = tool;
    // The selection tool doesn't follow the undo rules, so we have to clear
    // the selected area if a different tool is chosen.
    if (mTool != SelectionTool) {
        clearOrConfirmSelection();
    }
    toolChange();
    emit toolChanged();
}

int ImageCanvas::toolSize() const
{
    return mToolSize;
}

void ImageCanvas::setToolSize(int toolSize)
{
    const int clamped = qBound(1, toolSize, mMaxToolSize);
    if (clamped == mToolSize)
        return;

    mToolSize = clamped;
    emit toolSizeChanged();
}

int ImageCanvas::maxToolSize() const
{
    return mMaxToolSize;
}

QColor ImageCanvas::penForegroundColour() const
{
    return mPenForegroundColour;
}

void ImageCanvas::setPenForegroundColour(const QColor &penForegroundColour)
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

QColor ImageCanvas::penBackgroundColour() const
{
    return mPenBackgroundColour;
}

void ImageCanvas::setPenBackgroundColour(const QColor &penBackgroundColour)
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

bool ImageCanvas::hasSelection() const
{
    return mHasSelection;
}

QRect ImageCanvas::selectionArea() const
{
    return mSelectionArea;
}

void ImageCanvas::setSelectionArea(const QRect &selectionArea)
{
    QRect adjustedSelectionArea = selectionArea;
    if (!mMouseButtonPressed && selectionArea.size().isEmpty())
        adjustedSelectionArea = QRect();

    if (adjustedSelectionArea == mSelectionArea)
        return;

    mSelectionArea = adjustedSelectionArea;
    setHasSelection(!mSelectionArea.isEmpty());
    update();
    emit selectionAreaChanged();
}

QColor ImageCanvas::cursorPixelColour() const
{
    return mCursorPixelColour;
}

void ImageCanvas::setCursorPixelColour(const QColor &cursorPixelColour)
{
    if (cursorPixelColour == mCursorPixelColour)
        return;

    mCursorPixelColour = cursorPixelColour;
    emit cursorPixelColourChanged();
}

bool ImageCanvas::isWithinImage(const QPoint &scenePos) const
{
    return scenePos.x() >= 0 && scenePos.x() < mImageProject->image()->width()
            && scenePos.y() >= 0 && scenePos.y() < mImageProject->image()->height();
}

QPoint ImageCanvas::clampToImageBounds(const QPoint &scenePos, bool inclusive) const
{
    return QPoint(qBound(0, scenePos.x(), mImageProject->image()->width() - (inclusive ? 0 : 1)),
                  qBound(0, scenePos.y(), mImageProject->image()->height() - (inclusive ? 0 : 1)));
}

bool ImageCanvas::containsMouse() const
{
    return mContainsMouse;
}

void ImageCanvas::setContainsMouse(bool containsMouse)
{
    if (containsMouse == mContainsMouse)
        return;

    mContainsMouse = containsMouse;
    updateWindowCursorShape();
    emit containsMouseChanged();
}

bool ImageCanvas::isSplitScreen() const
{
    return mSplitScreen;
}

void ImageCanvas::setSplitScreen(bool splitScreen)
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

bool ImageCanvas::scrollZoom() const
{
    return mScrollZoom;
}

void ImageCanvas::setScrollZoom(bool scrollZoom)
{
    if (scrollZoom == mScrollZoom)
        return;

    mScrollZoom = scrollZoom;
    emit scrollZoomChanged();
}

CanvasPane *ImageCanvas::firstPane()
{
    return &mFirstPane;
}

CanvasPane *ImageCanvas::secondPane()
{
    return mSplitScreen ? &mSecondPane : nullptr;
}

CanvasPane *ImageCanvas::currentPane()
{
    return mCurrentPane;
}

Splitter *ImageCanvas::splitter()
{
    return &mSplitter;
}

QColor ImageCanvas::mapBackgroundColour() const
{
    return mBackgroundColour;
}

bool ImageCanvas::isAltPressed() const
{
    return mAltPressed;
}

void ImageCanvas::setAltPressed(bool altPressed)
{
    if (altPressed == mAltPressed)
        return;

    mAltPressed = altPressed;
    emit altPressedChanged();
}

void ImageCanvas::connectSignals()
{
    connect(mProject, SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    connect(mProject, SIGNAL(projectCreated()), this, SLOT(update()));
    connect(mProject, SIGNAL(projectClosed()), this, SLOT(reset()));
    connect(mProject, SIGNAL(sizeChanged()), this, SLOT(update()));

    connect(window(), SIGNAL(activeFocusItemChanged()), this, SLOT(updateWindowCursorShape()));
    // More hacks. Doing this because activeFocusItemChanged() doesn't seem to get called
    // when the last new project popup has finished its exit transition.
    // TODO: still necessary?
//    QQuickItem *overlay = window()->property("overlay").value<QQuickItem*>();
//    connect(overlay, SIGNAL(childrenChanged()), this, SLOT(updateWindowCursorShape()));

    centrePanes();
}

void ImageCanvas::disconnectSignals()
{
    mProject->disconnect(SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    mProject->disconnect(SIGNAL(projectCreated()), this, SLOT(update()));
    mProject->disconnect(SIGNAL(projectClosed()), this, SLOT(reset()));
    mProject->disconnect(SIGNAL(sizeChanged()), this, SLOT(update()));

    if (window()) {
        window()->disconnect(SIGNAL(activeFocusItemChanged()), this, SLOT(checkIfPopupsOpen()));
        QQuickItem *overlay = window()->property("overlay").value<QQuickItem*>();
        disconnect(overlay, SIGNAL(childrenChanged()), this, SLOT(checkIfPopupsOpen()));
    }
}

void ImageCanvas::toolChange()
{
}

bool ImageCanvas::hasBlankCursor() const
{
    return mHasBlankCursor;
}

void ImageCanvas::onSplitterPositionChanged()
{
    mFirstPane.setSize(mSplitter.position());
    mSecondPane.setSize(1.0 - mSplitter.position());
    update();
}

void ImageCanvas::paint(QPainter *painter)
{
    if (!mProject || !mProject->hasLoaded()) {
        painter->fillRect(0, 0, width(), height(), mBackgroundColour);
        return;
    }

    // The order here is deliberate; see the clip region code in drawPane().
    const int firstPaneWidth = width() * mFirstPane.size();
    if (mSplitScreen) {
        painter->fillRect(firstPaneWidth, 0, width() - firstPaneWidth, height(), mBackgroundColour);
        drawPane(painter, mSecondPane, 1);
    }

    painter->fillRect(0, 0, firstPaneWidth, height(), mBackgroundColour);
    drawPane(painter, mFirstPane, 0);

    if (mSplitScreen) {
        painter->setPen(QPen(mSplitColour));
        painter->drawLine(firstPaneWidth, 0, firstPaneWidth, height());
    }
}

void ImageCanvas::drawPane(QPainter *painter, const CanvasPane &pane, int paneIndex)
{
    const int paneWidth = width() * pane.size();

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

    const QSize zoomedCanvasSize = pane.zoomedSize(mImageProject->image()->size());
    const int zoomedCanvasWidth = qMin(zoomedCanvasSize.width(), paneWidth);
    const int zoomedCanvasHeight = qMin(zoomedCanvasSize.height(), qFloor(height()));
    painter->fillRect(0, 0, zoomedCanvasWidth, zoomedCanvasHeight, mapBackgroundColour());

    // Draw the checkered pixmap that acts as an indicator for transparency.
    // We use the unbounded canvas size here, otherwise the drawn area is too small past a certain zoom level.
    painter->drawTiledPixmap(0, 0, zoomedCanvasSize.width(), zoomedCanvasSize.height(), mCheckerPixmap);

    const bool shouldDrawSelectionPreviewImage = mMovingSelection || mHasMovedSelection;
    const QImage image = !shouldDrawSelectionPreviewImage ? *mImageProject->image() : mSelectionPreviewImage;
    painter->drawImage(QRectF(QPointF(0, 0), pane.zoomedSize(image.size())), image, QRectF(0, 0, image.width(), image.height()));

    // Draw the selection area.
    QPen pen;
    QVector<qreal> dashes;
    dashes << 4 << 4;
    pen.setDashPattern(dashes);
    const QRect zoomedSelectionArea(mSelectionArea.topLeft() * pane.zoomLevel(), pane.zoomedSize(mSelectionArea.size()));
    painter->setPen(pen);
    painter->drawRect(zoomedSelectionArea);

    painter->restore();
}

int ImageCanvas::paneWidth(int index) const
{
    return index == 0 ? mFirstPane.size() * width() : width() - mFirstPane.size() * width();
}

void ImageCanvas::centrePanes(bool respectSceneCentred)
{
    if (!mProject)
        return;

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

bool ImageCanvas::mouseOverSplitterHandle(const QPoint &mousePos)
{
    const QRect splitterRegion(paneWidth(0) - mSplitter.width() / 2, 0, mSplitter.width(), height());
    return splitterRegion.contains(mousePos);
}

void ImageCanvas::beginSelectionMove()
{
    setMovingSelection(true);
    mSelectionAreaBeforeLastMove = mSelectionArea;

    if (mSelectionAreaBeforeFirstMove.isEmpty()) {
        // When the selection is moved for the first time in its life,
        // copy the contents within it so that we can moved them around as a preview.
        mSelectionAreaBeforeFirstMove = mSelectionArea;
        mSelectionContents = mImageProject->image()->copy(mSelectionAreaBeforeFirstMove);
    }
}

void ImageCanvas::updateSelectionArea()
{
    Q_ASSERT(mPotentiallySelecting);

    QRect newSelectionArea(mPressScenePosition.x(), mPressScenePosition.y(),
        mCursorSceneX - mPressScenePosition.x(), mCursorSceneY - mPressScenePosition.y());

    newSelectionArea = clampSelectionArea(newSelectionArea.normalized());

    setSelectionArea(newSelectionArea);
}

void ImageCanvas::moveSelectionArea()
{
    QRect newSelectionArea = mSelectionAreaBeforeLastMove;
    const QPoint distanceMoved(mCursorSceneX - mPressScenePosition.x(), mCursorSceneY - mPressScenePosition.y());
    newSelectionArea.translate(distanceMoved);
    setSelectionArea(boundSelectionArea(newSelectionArea));

    if (!mIsSelectionFromPaste) {
        // Only if the selection wasn't pased should we erase the area left behind.
        mSelectionPreviewImage = Utils::erasePortionOfImage(*mImageProject->image(), mSelectionAreaBeforeFirstMove);
    } else {
        mSelectionPreviewImage = *mImageProject->image();
    }

    // Then, move the dragged contents to their new location.
    // Doing this last ensures that the drag contents are painted over the transparency,
    // and not the other way around.
    mSelectionPreviewImage = Utils::paintImageOntoPortionOfImage(mSelectionPreviewImage, mSelectionArea, mSelectionContents);

    mHasMovedSelection = true;

    update();
}

void ImageCanvas::confirmSelectionMove(bool andClear)
{
    Q_ASSERT(mHasMovedSelection);

    mProject->beginMacro(QLatin1String("MoveSelection"));
    mProject->addChange(new MoveImageCanvasSelectionCommand(this, mSelectionAreaBeforeFirstMove, mLastValidSelectionArea));
    mProject->endMacro();

    if (andClear)
        clearSelection();
}

// Limits selectionArea to the canvas' bounds, shrinking it if necessary.
QRect ImageCanvas::clampSelectionArea(const QRect &selectionArea) const
{
    QRect newSelectionArea = selectionArea;

    if (selectionArea.x() < 0) {
        newSelectionArea.setLeft(0);
    }

    if (selectionArea.y() < 0) {
        newSelectionArea.setTop(0);
    }

    if (newSelectionArea.width() < 0 || newSelectionArea.height() < 0
            || newSelectionArea.x() >= mProject->widthInPixels()
            || newSelectionArea.y() >= mProject->heightInPixels()) {
        newSelectionArea.setSize(QSize(0, 0));
    }

    if (newSelectionArea.x() + newSelectionArea.width() > mProject->widthInPixels()) {
        newSelectionArea.setWidth(mProject->widthInPixels() - newSelectionArea.x());
    }

    if (newSelectionArea.y() + newSelectionArea.height() > mProject->heightInPixels()) {
        newSelectionArea.setHeight(mProject->heightInPixels() - newSelectionArea.y());
    }

    if (newSelectionArea.width() < 0 || newSelectionArea.height() < 0) {
        newSelectionArea.setSize(QSize(0, 0));
    }

    return newSelectionArea;
}

// Limits selectionArea to the canvas' bounds without shrinking it.
// This should be used when the selection area has already been created and is being dragged.
QRect ImageCanvas::boundSelectionArea(const QRect &selectionArea) const
{
    QRect newSelectionArea = selectionArea;

    if (selectionArea.x() + selectionArea.width() > mProject->widthInPixels())
        newSelectionArea.moveLeft(mProject->widthInPixels() - selectionArea.width());
    else if (selectionArea.x() < 0)
        newSelectionArea.moveLeft(0);

    if (selectionArea.y() + selectionArea.height() > mProject->heightInPixels())
        newSelectionArea.moveTop(mProject->heightInPixels() - selectionArea.height());
    else if (selectionArea.y() < 0)
        newSelectionArea.moveTop(0);

    return newSelectionArea;
}

void ImageCanvas::clearSelection()
{
    setSelectionArea(QRect());
    mPotentiallySelecting = false;
    setHasSelection(false);
    setMovingSelection(false);
    mHasMovedSelection = false;
    mIsSelectionFromPaste = false;
    mSelectionAreaBeforeFirstMove = QRect(0, 0, 0, 0);
    mSelectionAreaBeforeLastMove = QRect(0, 0, 0, 0);
    mLastValidSelectionArea = QRect(0, 0, 0, 0);
    mSelectionPreviewImage = QImage();
    mSelectionContents = QImage();
}

void ImageCanvas::clearOrConfirmSelection()
{
    if (mHasSelection) {
        if (mHasMovedSelection)
            confirmSelectionMove();
        else
            clearSelection();
    }
}

void ImageCanvas::setHasSelection(bool hasSelection)
{
    if (hasSelection == mHasSelection)
        return;

    mHasSelection = hasSelection;
    updateWindowCursorShape();
    emit hasSelectionChanged();
}

void ImageCanvas::setMovingSelection(bool movingSelection)
{
    mMovingSelection = movingSelection;
}

bool ImageCanvas::cursorOverSelection() const
{
    return mHasSelection ? mSelectionArea.contains(QPoint(mCursorSceneX, mCursorSceneY)) : false;
}

void ImageCanvas::reset()
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
    mContainsMouse = false;
    mMouseButtonPressed = Qt::NoButton;
    mPressPosition = QPoint(0, 0);
    mPressScenePosition = QPoint(0, 0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
    clearSelection();
    setAltPressed(false);
    mToolBeforeAltPressed = PenTool;
    mSpacePressed = false;
    mHasBlankCursor = false;

    // Things that we don't want to set, as they
    // don't really need to be reset each time:
    // - tool
    // - toolSize

    update();
}

void ImageCanvas::centreView()
{
    centrePanes(false);
}

void ImageCanvas::zoomIn()
{
    CanvasPane *pane = currentPane();
    if (!pane)
        return;

    pane->setZoomLevel(pane->zoomLevel() + 1);
}

void ImageCanvas::zoomOut()
{
    CanvasPane *pane = currentPane();
    if (!pane)
        return;

    pane->setZoomLevel(pane->zoomLevel() - 1);
}

void ImageCanvas::flipSelection(Qt::Orientation orientation)
{
    if (!mHasSelection)
        return;

    mProject->beginMacro(QLatin1String("FlipSelection"));
    mProject->addChange(new FlipImageCanvasSelectionCommand(this, mSelectionArea, orientation));
    mProject->endMacro();
}

void ImageCanvas::copySelection()
{
    if (!mHasSelection)
        return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setImage(mImageProject->image()->copy(mSelectionArea));
}

void ImageCanvas::paste()
{
    clearOrConfirmSelection();

    QClipboard *clipboard = QGuiApplication::clipboard();
    QImage clipboardImage = clipboard->image();
    if (clipboardImage.isNull())
        return;

    const QSize adjustedSize(qMin(clipboardImage.width(), mProject->widthInPixels()),
        qMin(clipboardImage.height(), mProject->heightInPixels()));
    const QRect pastedArea(QPoint(0, 0), adjustedSize);
    if (adjustedSize != clipboardImage.size())
        clipboardImage = clipboardImage.copy(pastedArea);

    mProject->beginMacro(QLatin1String("PasteCommand"));
    mProject->addChange(new PasteImageCanvasCommand(this, clipboardImage, pastedArea.topLeft()));
    mProject->endMacro();

    // Setting a selection area is only done when a paste is first created,
    // not when it's redone, so we do it here instead of in the command.
    mIsSelectionFromPaste = true;
    mSelectionContents = clipboardImage;
    setSelectionArea(pastedArea);

    update();
}

void ImageCanvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    centrePanes();
}

ImageCanvas::PixelCandidateData ImageCanvas::penEraserPixelCandidates(Tool tool) const
{
    PixelCandidateData candidateData;

    QPoint topLeft(qRound(mCursorSceneFX - mToolSize / 2.0), qRound(mCursorSceneFY - mToolSize / 2.0));
    topLeft = clampToImageBounds(topLeft);
    QPoint bottomRight(qRound(mCursorSceneFX + mToolSize / 2.0), qRound(mCursorSceneFY + mToolSize / 2.0));
    bottomRight = clampToImageBounds(bottomRight);
    QPoint scenePos(topLeft);
    for (; scenePos.y() < bottomRight.y(); ++scenePos.ry()) {
        for (scenePos.rx() = topLeft.x(); scenePos.x() < bottomRight.x(); ++scenePos.rx()) {
            const QColor previousColour = mImageProject->image()->pixelColor(scenePos);
            // Don't do anything if the colours are the same; this prevents issues
            // with undos not undoing everything across tiles.
            const bool hasEffect = tool == PenTool ? penColour() != previousColour : previousColour != QColor(Qt::transparent);
            if (hasEffect) {
                candidateData.scenePositions.append(scenePos);
                candidateData.previousColours.append(previousColour);
            }
        }
    }

    return candidateData;
}

ImageCanvas::PixelCandidateData ImageCanvas::fillPixelCandidates() const
{
    PixelCandidateData candidateData;

    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos)) {
        return candidateData;
    }

    const QColor previousColour = mImageProject->image()->pixelColor(scenePos);
    // Don't do anything if the colours are the same.
    if (previousColour == penColour()) {
        return candidateData;
    }

    QVector<QPoint> scenePositions = imagePixelFloodFill(mImageProject->image(), scenePos, previousColour, penColour());
    candidateData.scenePositions = scenePositions;

    candidateData.previousColours.append(previousColour);
    return candidateData;
}

void ImageCanvas::applyCurrentTool()
{
    switch (mTool) {
    case PenTool: {
        const PixelCandidateData candidateData = penEraserPixelCandidates(mTool);
        if (candidateData.scenePositions.isEmpty()) {
            return;
        }

        mProject->beginMacro(QLatin1String("PixelPenTool"));
        mProject->addChange(new ApplyPixelPenCommand(this, candidateData.scenePositions, candidateData.previousColours, penColour()));
        break;
    }
    case EyeDropperTool: {
        const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
        if (isWithinImage(scenePos)) {
            setPenForegroundColour(mImageProject->image()->pixelColor(scenePos));
        }
        break;
    }
    case EraserTool: {
        const PixelCandidateData candidateData = penEraserPixelCandidates(mTool);
        if (candidateData.scenePositions.isEmpty()) {
            return;
        }

        mProject->beginMacro(QLatin1String("PixelEraserTool"));
        mProject->addChange(new ApplyPixelEraserCommand(this, candidateData.scenePositions, candidateData.previousColours));
        break;
    }
    case FillTool: {
        const PixelCandidateData candidateData = fillPixelCandidates();
        if (candidateData.scenePositions.isEmpty()) {
            return;
        }

        mProject->beginMacro(QLatin1String("PixelFillTool"));
        mProject->addChange(new ApplyPixelFillCommand(this, candidateData.scenePositions,
            candidateData.previousColours.first(), penColour()));
        break;
    }
    default:
        break;
    }
}

// This function actually operates on the image.
void ImageCanvas::applyPixelPenTool(const QPoint &scenePos, const QColor &colour)
{
    mImageProject->image()->setPixelColor(scenePos, colour);
    update();
}

void ImageCanvas::paintImageOntoPortionOfImage(const QRect &portion, const QImage &replacementImage)
{
    *mImageProject->image() = Utils::paintImageOntoPortionOfImage(*mImageProject->image(), portion, replacementImage);
    update();
}

void ImageCanvas::erasePortionOfImage(const QRect &portion)
{
    *mImageProject->image() = Utils::erasePortionOfImage(*mImageProject->image(), portion);
    update();
}

void ImageCanvas::doFlipSelection(const QRect &area, Qt::Orientation orientation)
{
    const QImage flippedImagePortion = mImageProject->image()->copy(area)
        .mirrored(orientation == Qt::Horizontal, orientation == Qt::Vertical);
    erasePortionOfImage(area);
    paintImageOntoPortionOfImage(area, flippedImagePortion);
}

void ImageCanvas::updateCursorPos(const QPoint &eventPos)
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

    if (!mProject->hasLoaded()) {
        mCursorSceneFX = -1;
        mCursorSceneFY = -1;
        setCursorSceneX(-1);
        setCursorSceneY(-1);
        // We could do this once at the beginning of the function, but we
        // try to avoid unnecessary property changes.
        setCursorPixelColour(QColor(Qt::black));
        return;
    }

    // We need the position as floating point numbers so that pen sizes > 1 work properly.
    mCursorSceneFX = qreal(mCursorPaneX - mCurrentPane->offset().x()) / mCurrentPane->zoomLevel();
    mCursorSceneFY = qreal(mCursorPaneY - mCurrentPane->offset().y()) / mCurrentPane->zoomLevel();
    setCursorSceneX(mCursorSceneFX);
    setCursorSceneY(mCursorSceneFY);

    if (mCursorSceneX < 0 || mCursorSceneX >= mProject->widthInPixels()
        || mCursorSceneY < 0 || mCursorSceneY >= mProject->heightInPixels()) {
        setCursorPixelColour(QColor(Qt::black));
    } else {
        const QPoint cursorScenePos = QPoint(mCursorSceneX, mCursorSceneY);
        setCursorPixelColour(mImageProject->image()->pixelColor(cursorScenePos));
    }
}

void ImageCanvas::onLoadedChanged()
{
    if (mProject->hasLoaded()) {
        centrePanes();
    }

    updateWindowCursorShape();
}

void ImageCanvas::updateWindowCursorShape()
{
    if (!mProject)
        return;

    // Hide the window's cursor when we're in the spotlight; otherwise, use the non-custom arrow cursor.
    const bool nothingOverUs = mProject->hasLoaded() && hasActiveFocus() /*&& !mModalPopupsOpen*/ && mContainsMouse;
    const bool splitterHovered = mSplitter.isEnabled() && mSplitter.isHovered();
    const bool overSelection = cursorOverSelection();
    setHasBlankCursor(nothingOverUs && !mSpacePressed && !splitterHovered && !overSelection);

    Qt::CursorShape cursorShape = Qt::BlankCursor;
    if (!mHasBlankCursor) {
        if (mSpacePressed) {
            cursorShape = mMouseButtonPressed == Qt::LeftButton ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
        } else if (overSelection) {
            cursorShape = Qt::SizeAllCursor;
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
        case Qt::SizeAllCursor:
            cursorName = "SizeAllCursor";
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

    if (window())
        window()->setCursor(QCursor(cursorShape));
}

void ImageCanvas::error(const QString &message)
{
//    qWarning() << Q_FUNC_INFO << message;
    emit errorOccurred(message);
}

QColor ImageCanvas::penColour() const
{
    return mMouseButtonPressed == Qt::LeftButton ? mPenForegroundColour : mPenBackgroundColour;
}

void ImageCanvas::setHasBlankCursor(bool hasCustomCursor)
{
    if (hasCustomCursor == mHasBlankCursor)
        return;

    mHasBlankCursor = hasCustomCursor;
    emit hasBlankCursorChanged();
}

void ImageCanvas::setCurrentPane(CanvasPane *pane)
{
    if (pane == mCurrentPane) {
        return;
    }

    mCurrentPane = pane;
    emit currentPaneChanged();
}

CanvasPane *ImageCanvas::hoveredPane(const QPoint &pos)
{
    const int firstPaneWidth = paneWidth(0);
    return pos.x() <= firstPaneWidth ? &mFirstPane : &mSecondPane;
    return &mSecondPane;
}

QPoint ImageCanvas::eventPosRelativeToCurrentPane(const QPoint &pos)
{
    if (!mCurrentPane || mCurrentPane == &mFirstPane) {
        return pos;
    }

    return pos - QPoint(paneWidth(1), 0);
}

void ImageCanvas::restoreToolBeforeAltPressed()
{
    setAltPressed(false);
    setTool(mToolBeforeAltPressed);
}

void ImageCanvas::wheelEvent(QWheelEvent *event)
{
    if (!mProject->hasLoaded() || !mScrollZoom) {
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

void ImageCanvas::mousePressEvent(QMouseEvent *event)
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
    mPressScenePosition = QPoint(mCursorSceneX, mCursorSceneY);
    mCurrentPaneOffsetBeforePress = mCurrentPane->offset();
    setContainsMouse(true);
    // Pressing the mouse while holding down space should pan.
    if (!mSpacePressed) {
        if (mSplitter.isEnabled() && mouseOverSplitterHandle(event->pos())) {
            mSplitter.setPressed(true);
        } else if (mTool != SelectionTool) {
            applyCurrentTool();
        } else {
            if (!cursorOverSelection()) {
                mPotentiallySelecting = true;
                updateSelectionArea();
            } else {
                // The user has just clicked the selection. We don't actually
                // move anything on press events; we wait until mouseMoveEvent()
                // and then start moving the selection and its contents.
                beginSelectionMove();
            }
        }
    } else {
        updateWindowCursorShape();
    }
}

void ImageCanvas::mouseMoveEvent(QMouseEvent *event)
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
                if (mTool != SelectionTool) {
                    applyCurrentTool();
                } else {
                    if (!mMovingSelection) {
                        updateSelectionArea();
                    } else {
                        moveSelectionArea();
                    }
                }
            } else {
                // Panning.
                mCurrentPane->setSceneCentered(false);
                mCurrentPane->setOffset(mCurrentPaneOffsetBeforePress + (event->pos() - mPressPosition));
                update();
            }
        }
    }
}

void ImageCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickPaintedItem::mouseReleaseEvent(event);

    updateCursorPos(event->pos());

    if (!mProject->hasLoaded())
        return;

    mMouseButtonPressed = Qt::NoButton;

    // Make sure we do this after the mouse button has been cleared
    // (as setSelectionArea() relies on this to accurately set mHasSelection),
    // but before the press position has been cleared (as
    // updateSelectionArea() needs that information).
    if (mTool == SelectionTool) {
        if (!mMovingSelection) {
            if (mPotentiallySelecting) {
                // The mouse press that caused mPotentiallySelecting to be set to
                // true has now been accompanied by a release. If there was mouse movement
                // in between these two events, then we now have a selection.
                if (!mHasMovedSelection) {
                    // We haven't moved the selection, meaning that there have been no
                    // changes to the canvas during this "event cycle".
                    if (mHasSelection) {
                        // Finish off the selection by including the last release event.
                        updateSelectionArea();
                        if (!mSelectionArea.size().isEmpty()) {
                            // As this selection is valid, store it. If this branch (or the one above it)
                            // doesn't get run, then the user was simply clicking outside of the selection
                            // in order to clear it.
                            mLastValidSelectionArea = mSelectionArea;
                        }
                    } else {
                        // Since we hadn't done anything to the selection that we might have had
                        // before (that this event cycle is interrupting), releasing the mouse
                        // should cancel it.
                        clearSelection();
                    }
                } else {
                    // We have moved the selection since creating it, but we're not
                    // currently moving it, which means that the user just clicked outside of it,
                    // which means we clear it.
                    confirmSelectionMove();
                }
            }
        } else {
            moveSelectionArea();
            setMovingSelection(false);
            if (!mSelectionArea.size().isEmpty())
                mLastValidSelectionArea = mSelectionArea;
        }
        mPotentiallySelecting = false;
    }

    if (mProject->isComposingMacro()) {
        mProject->endMacro();
    }

    mPressPosition = QPoint(0, 0);
    mPressScenePosition = QPoint(0, 0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
    updateWindowCursorShape();
    mSplitter.setPressed(false);
}

void ImageCanvas::hoverEnterEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverEnterEvent(event);

    updateCursorPos(event->pos());

    setContainsMouse(true);

    if (!mProject->hasLoaded())
        return;
}

void ImageCanvas::hoverMoveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverMoveEvent(event);

    updateCursorPos(event->pos());

    setContainsMouse(true);

    if (!mProject->hasLoaded())
        return;

    bool updateCursorShape = false;
    const bool wasSplitterHovered = mSplitter.isHovered();
    mSplitter.setHovered(mouseOverSplitterHandle(event->pos()));
    if (mSplitter.isHovered() != wasSplitterHovered) {
        updateCursorShape = true;
    }

    if (mHasSelection) {
        updateCursorShape |= true;
    }

    if (updateCursorShape) {
        updateWindowCursorShape();
    }
}

void ImageCanvas::hoverLeaveEvent(QHoverEvent *event)
{
    QQuickPaintedItem::hoverLeaveEvent(event);

    setContainsMouse(false);

    if (!mProject->hasLoaded())
        return;
}

void ImageCanvas::keyPressEvent(QKeyEvent *event)
{
    QQuickPaintedItem::keyPressEvent(event);

    if (!mProject->hasLoaded())
        return;

    if (event->isAutoRepeat())
        return;

    if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_3) {
        setTool(static_cast<ImageCanvas::Tool>(PenTool + event->key() - Qt::Key_1));
    } else if (event->key() == Qt::Key_Space) {
        mSpacePressed = true;
        updateWindowCursorShape();
    } else if (event->key() == Qt::Key_Delete && mHasSelection) {
        mProject->beginMacro(QLatin1String("DeleteSelection"));
        mProject->addChange(new DeleteImageCanvasSelectionCommand(this, mSelectionArea));
        mProject->endMacro();
        clearSelection();
    } else if (event->key() == Qt::Key_Escape && mHasSelection) {
        if (mHasMovedSelection) {
            // We've moved the selection since creating it, so, like mspaint, escape confirms it.
            confirmSelectionMove();
        } else {
            clearSelection();
        }

    } else if (event->modifiers().testFlag(Qt::AltModifier)) {
        setAltPressed(true);
        mToolBeforeAltPressed = mTool;
        setTool(EyeDropperTool);
    }
}

void ImageCanvas::keyReleaseEvent(QKeyEvent *event)
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

void ImageCanvas::focusInEvent(QFocusEvent *event)
{
    QQuickPaintedItem::focusInEvent(event);

    updateWindowCursorShape();
}

void ImageCanvas::focusOutEvent(QFocusEvent *event)
{
    QQuickPaintedItem::focusOutEvent(event);

    // The alt-to-eyedrop feature is meant to be temporary,
    // so it should restore the previous tool if focus is taken away.
    if (mAltPressed) {
        restoreToolBeforeAltPressed();
    }

    updateWindowCursorShape();
}

// A selection should be cleared when Ctrl + Z is pressed, as this is
// what mspaint does. However, it doesn't make sense for a selection
// to have its own undo command, so we intercept the undo shortcut
// to handle this special case ourselves.
bool ImageCanvas::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::ShortcutOverride) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->k)
        QShortcutEvent *shortcutEvent = static_cast<QShortcutEvent*>(event);
        return true;
    }

    return QObject::eventFilter(object, event);
}
