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

#include "addguidecommand.h"
#include "applicationsettings.h"
#include "applygreedypixelfillcommand.h"
#include "applypixelerasercommand.h"
#include "applypixelfillcommand.h"
#include "applypixellinecommand.h"
#include "applypixelpencommand.h"
#include "deleteguidecommand.h"
#include "deleteimagecanvasselectioncommand.h"
#include "fillalgorithms.h"
#include "flipimagecanvasselectioncommand.h"
#include "imageproject.h"
#include "moveimagecanvasselectioncommand.h"
#include "moveguidecommand.h"
#include "pasteimagecanvascommand.h"
#include "project.h"
#include "tileset.h"
#include "utils.h"

Q_LOGGING_CATEGORY(lcCanvas, "app.canvas")
Q_LOGGING_CATEGORY(lcCanvasCursorShape, "app.canvas.cursorshape")
Q_LOGGING_CATEGORY(lcCanvasLifecycle, "app.canvas.lifecycle")
Q_LOGGING_CATEGORY(lcCanvasSelection, "app.canvas.selection")

ImageCanvas::ImageCanvas() :
    mProject(nullptr),
    mBackgroundColour(Qt::gray),
    mGridVisible(false),
    mGridColour(Qt::black),
    mSplitColour(Qt::black),
    mSplitScreen(true),
    mSplitter(this),
    mCurrentPane(&mFirstPane),
    mFirstHorizontalRuler(new Ruler(Qt::Horizontal, this)),
    mFirstVerticalRuler(new Ruler(Qt::Vertical, this)),
    mSecondHorizontalRuler(new Ruler(Qt::Horizontal, this)),
    mSecondVerticalRuler(new Ruler(Qt::Vertical, this)),
    mPressedRuler(nullptr),
    mGuidesVisible(false),
    mGuidesLocked(false),
    mGuidePositionBeforePress(0),
    mPressedGuideIndex(-1),
    mCursorX(0),
    mCursorY(0),
    mCursorPaneX(0),
    mCursorPaneY(0),
    mCursorSceneX(0),
    mCursorSceneY(0),
    mCursorPixelColour(Qt::black),
    mContainsMouse(false),
    mMouseButtonPressed(Qt::NoButton),
    mLastMouseButtonPressed(Qt::NoButton),
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
    mShiftPressed(false),
    mToolBeforeAltPressed(PenTool),
    mSpacePressed(false),
    mHasBlankCursor(false)
{
    setEnabled(false);
    setFlag(QQuickItem::ItemIsFocusScope);

    mFirstPane.setObjectName("firstPane");
    mSecondPane.setObjectName("secondPane");
    mSplitter.setPosition(mFirstPane.size());

    mFirstHorizontalRuler->setObjectName("firstHorizontalRuler");
    mFirstVerticalRuler->setObjectName("firstVerticalRuler");
    mFirstVerticalRuler->setDrawCorner(true);

    mSecondHorizontalRuler->setObjectName("secondHorizontalRuler");
    mSecondVerticalRuler->setObjectName("secondVerticalRuler");
    mSecondVerticalRuler->setDrawCorner(true);

    connect(&mFirstPane, SIGNAL(zoomLevelChanged()), this, SLOT(onZoomLevelChanged()));
    connect(&mFirstPane, SIGNAL(offsetChanged()), this, SLOT(onPaneOffsetChanged()));
    connect(&mFirstPane, SIGNAL(sizeChanged()), this, SLOT(onPaneSizeChanged()));
    connect(&mSecondPane, SIGNAL(zoomLevelChanged()), this, SLOT(onZoomLevelChanged()));
    connect(&mSecondPane, SIGNAL(offsetChanged()), this, SLOT(onPaneOffsetChanged()));
    connect(&mSecondPane, SIGNAL(sizeChanged()), this, SLOT(onPaneSizeChanged()));
    connect(&mSplitter, SIGNAL(positionChanged()), this, SLOT(onSplitterPositionChanged()));

    mCheckerPixmap = QPixmap(":/images/checker.png");
    if (mCheckerPixmap.isNull()) {
        qWarning() << "Failed to load checker pixmap";
    }

    installEventFilter(this);

    qCDebug(lcCanvasLifecycle) << "constructing ImageCanvas" << this;
}

ImageCanvas::~ImageCanvas()
{
    qCDebug(lcCanvasLifecycle) << "destructing ImageCanvas" << this;
}

Project *ImageCanvas::project() const
{
    return mProject;
}

void ImageCanvas::setProject(Project *project)
{
    qCDebug(lcCanvas) << "setting project" << project << "on canvas" << this;

    if (project == mProject)
        return;

    if (mProject) {
        disconnectSignals();
    }

    mProject = project;
    mImageProject = qobject_cast<ImageProject*>(mProject);

    if (mProject) {
        connectSignals();

        // Read the pane data that was stored in the project, if there is any.
        // New projects or projects that don't have their own Slate extension
        // won't have any JSON data.
        QJsonObject *cachedProjectJson = project->cachedProjectJson();
        if (cachedProjectJson->contains("firstPane")) {
            mFirstPane.read(cachedProjectJson->value("firstPane").toObject());
        }
        if (cachedProjectJson->contains("firstPane")) {
            mSecondPane.read(cachedProjectJson->value("secondPane").toObject());
        }

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

bool ImageCanvas::rulersVisible() const
{
    return mFirstHorizontalRuler->isVisible();
}

void ImageCanvas::setRulersVisible(bool rulersVisible)
{
    if (rulersVisible == mFirstHorizontalRuler->isVisible())
        return;

    mFirstHorizontalRuler->setVisible(rulersVisible);
    mFirstVerticalRuler->setVisible(rulersVisible);
    mSecondHorizontalRuler->setVisible(rulersVisible && mSplitScreen);
    mSecondVerticalRuler->setVisible(rulersVisible && mSplitScreen);
    emit rulersVisibleChanged();
}

bool ImageCanvas::guidesVisible() const
{
    return mGuidesVisible;
}

void ImageCanvas::setGuidesVisible(bool guidesVisible)
{
    if (guidesVisible == mGuidesVisible)
        return;

    mGuidesVisible = guidesVisible;
    update();
    emit guidesVisibleChanged();
}

bool ImageCanvas::guidesLocked() const
{
    return mGuidesLocked;
}

void ImageCanvas::setGuidesLocked(bool guidesLocked)
{
    if (guidesLocked == mGuidesLocked)
        return;

    mGuidesLocked = guidesLocked;
    emit guidesLockedChanged();
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
    if (isLineVisible())
        emit lineLengthChanged();
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
    if (isLineVisible())
        emit lineLengthChanged();
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

QColor ImageCanvas::invertedCursorPixelColour() const
{
    // https://stackoverflow.com/a/18142036/904422
    return (0xFFFFFF - mCursorPixelColour.rgba()) | 0xFF000000;
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
    return scenePos.x() >= 0 && scenePos.x() < currentProjectImage()->width()
            && scenePos.y() >= 0 && scenePos.y() < currentProjectImage()->height();
}

QPoint ImageCanvas::clampToImageBounds(const QPoint &scenePos, bool inclusive) const
{
    return QPoint(qBound(0, scenePos.x(), currentProjectImage()->width() - (inclusive ? 0 : 1)),
                  qBound(0, scenePos.y(), currentProjectImage()->height() - (inclusive ? 0 : 1)));
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
    mSecondPane.setSize(splitScreen ? 0.5 : 0.0);

    updateRulerVisibility();
    resizeRulers();

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

Ruler *ImageCanvas::pressedRuler() const
{
    return mPressedRuler;
}

int ImageCanvas::pressedGuideIndex() const
{
    return mPressedGuideIndex;
}

CanvasPane *ImageCanvas::firstPane()
{
    return &mFirstPane;
}

CanvasPane *ImageCanvas::secondPane()
{
    return &mSecondPane;
}

CanvasPane *ImageCanvas::currentPane()
{
    return mCurrentPane;
}

QColor ImageCanvas::rulerForegroundColour() const
{
    return mFirstHorizontalRuler->foregroundColour();
}

void ImageCanvas::setRulerForegroundColour(const QColor &foregroundColour) const
{
    mFirstHorizontalRuler->setForegroundColour(foregroundColour);
    mFirstVerticalRuler->setForegroundColour(foregroundColour);
    mSecondHorizontalRuler->setForegroundColour(foregroundColour);
    mSecondVerticalRuler->setForegroundColour(foregroundColour);
}

QColor ImageCanvas::rulerBackgroundColour() const
{
    return mFirstHorizontalRuler->backgroundColour();
}

void ImageCanvas::setRulerBackgroundColour(const QColor &backgroundColour) const
{
    mFirstHorizontalRuler->setBackgroundColour(backgroundColour);
    mFirstVerticalRuler->setBackgroundColour(backgroundColour);
    mSecondHorizontalRuler->setBackgroundColour(backgroundColour);
    mSecondVerticalRuler->setBackgroundColour(backgroundColour);
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

bool ImageCanvas::isLineVisible() const
{
    // Don't show line info in the status bar if there hasn't been a mouse press yet.
    // This is the same as what penColour() does.
    const Qt::MouseButton lastButtonPressed = mMouseButtonPressed == Qt::NoButton ? mLastMouseButtonPressed : mMouseButtonPressed;
    return mShiftPressed && mTool == PenTool && lastButtonPressed != Qt::NoButton;
}

int ImageCanvas::lineLength() const
{
    if (!isLineVisible())
        return 0;

    const QPointF point1 = mLastPixelPenPressScenePosition;
    const QPointF point2 = QPointF(mCursorSceneX, mCursorSceneY);
    const QLineF line(point1, point2);
    return line.length();
}

qreal ImageCanvas::lineAngle() const
{
    if (!isLineVisible())
        return 0;

    const QPointF point1 = mLastPixelPenPressScenePosition;
    const QPointF point2 = QPointF(mCursorSceneX, mCursorSceneY);
    const QLineF line(point1, point2);
    return line.angle();
}

void ImageCanvas::setAltPressed(bool altPressed)
{
    if (altPressed == mAltPressed)
        return;

    mAltPressed = altPressed;
    emit altPressedChanged();
}

void ImageCanvas::setShiftPressed(bool shiftPressed)
{
    if (shiftPressed == mShiftPressed)
        return;

    const bool wasLineVisible = isLineVisible();

    mShiftPressed = shiftPressed;

    if (isLineVisible() != wasLineVisible)
        emit lineVisibleChanged();

    update();
}

void ImageCanvas::connectSignals()
{
    qCDebug(lcCanvas) << "connecting signals for" << this << "as we have a new project" << mProject;

    connect(mProject, SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    connect(mProject, SIGNAL(projectCreated()), this, SLOT(update()));
    connect(mProject, SIGNAL(projectClosed()), this, SLOT(reset()));
    connect(mProject, SIGNAL(sizeChanged()), this, SLOT(update()));
    connect(mProject, SIGNAL(guidesChanged()), this, SLOT(onGuidesChanged()));
    connect(mProject, SIGNAL(readyForWritingToJson(QJsonObject*)),
        this, SLOT(onReadyForWritingToJson(QJsonObject*)));

    connect(window(), SIGNAL(activeFocusItemChanged()), this, SLOT(updateWindowCursorShape()));

    centrePanes();
}

void ImageCanvas::disconnectSignals()
{
    qCDebug(lcCanvas) << "disconnecting signals for" << this;

    mProject->disconnect(SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    mProject->disconnect(SIGNAL(projectCreated()), this, SLOT(update()));
    mProject->disconnect(SIGNAL(projectClosed()), this, SLOT(reset()));
    mProject->disconnect(SIGNAL(sizeChanged()), this, SLOT(update()));
    mProject->disconnect(SIGNAL(guidesChanged()), this, SLOT(onGuidesChanged()));
    mProject->disconnect(SIGNAL(readyForWritingToJson(QJsonObject*)),
        this, SLOT(onReadyForWritingToJson(QJsonObject*)));

    if (window()) {
        window()->disconnect(SIGNAL(activeFocusItemChanged()), this, SLOT(updateWindowCursorShape()));
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

void ImageCanvas::componentComplete()
{
    QQuickPaintedItem::componentComplete();
    // For some reason, we need to force this stuff to update when creating a
    // tileset project after a layered image project.
    updateRulerVisibility();
    resizeRulers();
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

QImage *ImageCanvas::currentProjectImage()
{
    return mImageProject->image();
}

const QImage *ImageCanvas::currentProjectImage() const
{
    return mImageProject->image();
}

QImage ImageCanvas::contentImage() const
{
    QImage image = !shouldDrawSelectionPreviewImage() ? *currentProjectImage() : mSelectionPreviewImage;
    // Draw the pixel-pen-line indicator over the content.
    if (isLineVisible()) {
        QPainter linePainter(&image);
        drawLine(&linePainter);
    }
    return image;
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

    const QSize zoomedCanvasSize = pane.zoomedSize(currentProjectImage()->size());
    const int zoomedCanvasWidth = qMin(zoomedCanvasSize.width(), paneWidth);
    const int zoomedCanvasHeight = qMin(zoomedCanvasSize.height(), qFloor(height()));
    painter->fillRect(0, 0, zoomedCanvasWidth, zoomedCanvasHeight, mapBackgroundColour());

    // Draw the checkered pixmap that acts as an indicator for transparency.
    // We use the unbounded canvas size here, otherwise the drawn area is too small past a certain zoom level.
    painter->drawTiledPixmap(0, 0, zoomedCanvasSize.width(), zoomedCanvasSize.height(), mCheckerPixmap);

    const QImage image = contentImage();
    const QSize zoomedImageSize = pane.zoomedSize(image.size());
    painter->drawImage(QRectF(QPointF(0, 0), zoomedImageSize), image, QRectF(0, 0, image.width(), image.height()));

    // Draw the selection area.
    QPen pen;
    QVector<qreal> dashes;
    dashes << 4 << 4;
    pen.setDashPattern(dashes);
    const QRect zoomedSelectionArea(mSelectionArea.topLeft() * pane.integerZoomLevel(), pane.zoomedSize(mSelectionArea.size()));
    painter->setPen(pen);
    painter->drawRect(zoomedSelectionArea);

    if (mGuidesVisible) {
        // Draw the existing guides.
        QVector<Guide> guides = mProject->guides();
        for (int i = 0; i < guides.size(); ++i) {
            const Guide guide = guides.at(i);
            drawGuide(painter, pane, paneIndex, guide, i);
        }

        // Draw the guide that's being dragged from the ruler, if any.
        if (mPressedRuler) {
            const bool horizontal = mPressedRuler->orientation() == Qt::Horizontal;
            const Guide guide(horizontal ? mCursorSceneY : mCursorSceneX, mPressedRuler->orientation());
            drawGuide(painter, pane, paneIndex, guide, -1);
        }
    }

    painter->restore();
}

void ImageCanvas::drawLine(QPainter *painter) const
{
    painter->save();

    const QPointF point1 = linePoint1();
    const QPointF point2 = linePoint2();
    QPen pen;
    pen.setColor(penColour());
    pen.setWidth(mToolSize);
    painter->setPen(pen);

    QLineF line(point1, point2);
    // Draw the line on top of what has already been painted using a special composition mode.
    // This ensures that e.g. a translucent red overwrites whatever pixels it
    // lies on, rather than blending with them.
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->drawLine(line);

    painter->restore();
}

void ImageCanvas::drawGuide(QPainter *painter, const CanvasPane &pane, int paneIndex, const Guide &guide, int guideIndex)
{
    painter->save();
    painter->setPen(Qt::cyan);

    // If this is an existing guide that is currently being dragged, draw it in its dragged position.
    const bool draggingExistingGuide = mPressedGuideIndex != -1 && mPressedGuideIndex == guideIndex;
    const bool vertical = guide.orientation() == Qt::Vertical;
    const int guidePosition = draggingExistingGuide ? (vertical ? mCursorSceneX : mCursorSceneY) : guide.position();
    const qreal zoomedGuidePosition = guidePosition * pane.integerZoomLevel() + painter->pen().widthF() / 2.0;

    if (vertical) {
        // Don't need to account for the vertical offset anymore, as vertical guides go across the whole height of the pane.
        painter->translate(0, -pane.offset().y());
        painter->drawLine(QLineF(zoomedGuidePosition, 0, zoomedGuidePosition, height()));
    } else {
        // Don't need to account for the horizontal offset anymore, as horizontal guides go across the whole width of the pane.
        painter->translate(-pane.offset().x(), 0);
        painter->drawLine(QLineF(0, zoomedGuidePosition, paneWidth(paneIndex), zoomedGuidePosition));
    }
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
        const QPoint newOffset(paneWidth(0) / 2 - (mProject->widthInPixels() * mFirstPane.integerZoomLevel()) / 2,
            height() / 2 - (mProject->heightInPixels() * mFirstPane.integerZoomLevel()) / 2);
        mFirstPane.setOffset(newOffset);
    }

    if (!respectSceneCentred || (respectSceneCentred && mSecondPane.isSceneCentered())) {
        const QPoint newOffset(paneWidth(1) / 2 - (mProject->widthInPixels() * mFirstPane.integerZoomLevel()) / 2,
            height() / 2 - (mProject->heightInPixels() * mFirstPane.integerZoomLevel()) / 2);
        mSecondPane.setOffset(newOffset);
    }

    update();
}

bool ImageCanvas::mouseOverSplitterHandle(const QPoint &mousePos)
{
    const QRect splitterRegion(paneWidth(0) - mSplitter.width() / 2, 0, mSplitter.width(), height());
    return splitterRegion.contains(mousePos);
}

void ImageCanvas::updateRulerVisibility()
{
    mSecondHorizontalRuler->setVisible(mSplitScreen);
    mSecondVerticalRuler->setVisible(mSplitScreen);
}

void ImageCanvas::resizeRulers()
{
    const bool splitScreen = isSplitScreen();
    const int firstPaneWidth = paneWidth(0);

    mFirstHorizontalRuler->setSize(QSizeF(splitScreen ? firstPaneWidth : width(), 20));
    mFirstVerticalRuler->setSize(QSizeF(20, height()));

    if (splitScreen) {
        const int secondPaneWidth = paneWidth(1);
        mSecondHorizontalRuler->setX(firstPaneWidth);
        mSecondHorizontalRuler->setSize(QSizeF(secondPaneWidth, 20));
        mSecondVerticalRuler->setX(firstPaneWidth);
        mSecondVerticalRuler->setSize(QSizeF(20, height()));
    }
}

void ImageCanvas::updatePressedRuler()
{
    mPressedRuler = rulerAtCursorPos();
}

Ruler *ImageCanvas::rulerAtCursorPos()
{
    QPointF cursorPos = QPointF(mCursorX, mCursorY);
    Ruler *ruler = nullptr;

    if (mFirstHorizontalRuler->contains(mapToItem(mFirstHorizontalRuler, cursorPos))) {
        ruler = mFirstHorizontalRuler;
    } else if (mFirstVerticalRuler->contains(mapToItem(mFirstVerticalRuler, cursorPos))) {
        ruler = mFirstVerticalRuler;
    } else if (mSecondHorizontalRuler->isVisible()
        && mSecondHorizontalRuler->contains(mapToItem(mSecondHorizontalRuler, cursorPos))) {
        ruler = mSecondHorizontalRuler;
    } else if (mSecondVerticalRuler->isVisible()
        && mSecondVerticalRuler->contains(mapToItem(mSecondVerticalRuler, cursorPos))) {
        ruler = mSecondVerticalRuler;
    }

    return ruler;
}

void ImageCanvas::addNewGuide()
{
    mProject->beginMacro(QLatin1String("AddGuide"));
    mProject->addChange(new AddGuideCommand(mProject, Guide(
        mPressedRuler->orientation() == Qt::Horizontal ? mCursorSceneY : mCursorSceneX,
        mPressedRuler->orientation())));
    mProject->endMacro();

    // The update for these guide commands happens in onGuidesChanged.
}

void ImageCanvas::moveGuide()
{
    const Guide guide = mProject->guides().at(mPressedGuideIndex);

    mProject->beginMacro(QLatin1String("MoveGuide"));
    mProject->addChange(new MoveGuideCommand(mProject, guide,
        guide.orientation() == Qt::Horizontal ? mCursorSceneY : mCursorSceneX));
    mProject->endMacro();
}

void ImageCanvas::removeGuide()
{
    const Guide guide = mProject->guides().at(mPressedGuideIndex);

    mProject->beginMacro(QLatin1String("DeleteGuide"));
    mProject->addChange(new DeleteGuideCommand(mProject, guide));
    mProject->endMacro();
}

void ImageCanvas::updatePressedGuide()
{
    mPressedGuideIndex = guideIndexAtCursorPos();

    if (mPressedGuideIndex != -1)
        mGuidePositionBeforePress = mProject->guides().at(mPressedGuideIndex).position();
}

int ImageCanvas::guideIndexAtCursorPos()
{
    const QVector<Guide> guides = mProject->guides();
    for (int i = 0; i < guides.size(); ++i) {
        const Guide guide = guides.at(i);
        if (guide.orientation() == Qt::Horizontal) {
            if (mCursorSceneY == guide.position()) {
                return i;
            }
        } else {
            if (mCursorSceneX == guide.position()) {
                return i;
            }
        }
    }

    return -1;
}

void ImageCanvas::onGuidesChanged()
{
    update();
}

void ImageCanvas::onReadyForWritingToJson(QJsonObject *projectJson)
{
    QJsonObject firstPaneJson;
    mFirstPane.write(firstPaneJson);
    (*projectJson)["firstPane"] = firstPaneJson;

    QJsonObject secondPaneJson;
    mSecondPane.write(secondPaneJson);
    (*projectJson)["secondPane"] = secondPaneJson;
}

bool ImageCanvas::isPanning() const
{
    // Pressing the mouse while holding down space (or using middle mouse button) should pan.
    return mSpacePressed || mMouseButtonPressed == Qt::MiddleButton;
}

void ImageCanvas::beginSelectionMove()
{
    qCDebug(lcCanvasSelection) << "beginning selection move... mIsSelectionFromPaste =" << mIsSelectionFromPaste;

    setMovingSelection(true);
    mSelectionAreaBeforeLastMove = mSelectionArea;

    if (mSelectionAreaBeforeFirstMove.isEmpty()) {
        // When the selection is moved for the first time in its life,
        // copy the contents within it so that we can moved them around as a preview.
        qCDebug(lcCanvasSelection) << "copying currentProjectImage()" << *currentProjectImage() << "into mSelectionContents";
        mSelectionAreaBeforeFirstMove = mSelectionArea;
        mSelectionContents = currentProjectImage()->copy(mSelectionAreaBeforeFirstMove);
        // Technically we don't need to call this until the selection has actually moved,
        // but updateCursorPos() calls pixelColor() on the result of contentImage(), which will be an invalid
        // image until we've updated the selection preview image (since shouldDrawSelectionPreviewImage() will
        // return true due to mMovingSelection being true).
        updateSelectionPreviewImage();
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

void ImageCanvas::updateSelectionPreviewImage()
{
    qCDebug(lcCanvasSelection) << "updating selection preview image...";

    if (!mIsSelectionFromPaste) {
        // Only if the selection wasn't pasted should we erase the area left behind.
        mSelectionPreviewImage = Utils::erasePortionOfImage(*currentProjectImage(), mSelectionAreaBeforeFirstMove);
        qCDebug(lcCanvasSelection) << "... selection is not from paste; erasing area left behind"
            << "- new selection preview image:" << mSelectionPreviewImage;
    } else {
        mSelectionPreviewImage = *currentProjectImage();
        qCDebug(lcCanvasSelection) << "... selection is from a paste; not touching existing canvas content"
            << "- new selection preview image:" << mSelectionPreviewImage;
    }

    // Then, move the dragged contents to their new location.
    // Doing this last ensures that the drag contents are painted over the transparency,
    // and not the other way around.
    qCDebug(lcCanvasSelection) << "painting selection contents" << mSelectionContents
       << "within selection area" << mSelectionArea << "over top of current project image" << mSelectionPreviewImage;
    mSelectionPreviewImage = Utils::paintImageOntoPortionOfImage(mSelectionPreviewImage, mSelectionArea, mSelectionContents);
}

void ImageCanvas::moveSelectionArea()
{
//    qCDebug(lcCanvasSelection) << "moving selection area... mIsSelectionFromPaste =" << mIsSelectionFromPaste;

    QRect newSelectionArea = mSelectionAreaBeforeLastMove;
    const QPoint distanceMoved(mCursorSceneX - mPressScenePosition.x(), mCursorSceneY - mPressScenePosition.y());
    newSelectionArea.translate(distanceMoved);
    setSelectionArea(boundSelectionArea(newSelectionArea));

    updateSelectionPreviewImage();

    mHasMovedSelection = true;

    update();
}

void ImageCanvas::moveSelectionAreaBy(const QPoint &pixelDistance)
{
    qCDebug(lcCanvasSelection) << "moving selection area by" << pixelDistance;

    // Moving a selection with the directional keys creates a single move command instantly.
    beginSelectionMove();

    const QRect newSelectionArea = mSelectionArea.translated(pixelDistance.x(), pixelDistance.y());
    setSelectionArea(boundSelectionArea(newSelectionArea));

    updateSelectionPreviewImage();

    mHasMovedSelection = true;

    setMovingSelection(false);
    mLastValidSelectionArea = mSelectionArea;

    // setSelectionArea() should do this anyway, but just in case..
    update();
}

void ImageCanvas::confirmSelectionMove(ClearSelectionFlag clear)
{
    Q_ASSERT(mHasMovedSelection);

    const QImage previousImageAreaPortion = currentProjectImage()->copy(mSelectionAreaBeforeFirstMove);

    mProject->beginMacro(QLatin1String("MoveSelection"));
    mProject->addChange(new MoveImageCanvasSelectionCommand(
        this, mSelectionAreaBeforeFirstMove, previousImageAreaPortion, mLastValidSelectionArea,
            mIsSelectionFromPaste, mSelectionContents));
    mProject->endMacro();

    if (clear == ClearSelection)
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
    qCDebug(lcCanvasSelection) << "clearing selection";

    setSelectionArea(QRect());
    mPotentiallySelecting = false;
    setHasSelection(false);
    setMovingSelection(false);
    mHasMovedSelection = false;
    setSelectionFromPaste(false);
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

bool ImageCanvas::shouldDrawSelectionPreviewImage() const
{
    return mMovingSelection || mHasMovedSelection || mIsSelectionFromPaste;
}

void ImageCanvas::confirmPasteSelection()
{
    paintImageOntoPortionOfImage(mSelectionAreaBeforeFirstMove, mSelectionContents);
    clearSelection();
}

void ImageCanvas::setSelectionFromPaste(bool isSelectionFromPaste)
{
    mIsSelectionFromPaste = isSelectionFromPaste;
}

void ImageCanvas::reset()
{
    mFirstPane.reset();
    mSecondPane.reset();
    setCurrentPane(nullptr);
    mSplitter.setPosition(mFirstPane.size());
    mSplitter.setPressed(false);
    mSplitter.setHovered(false);

    mPressedRuler = nullptr;
    mGuidePositionBeforePress = 0;
    mPressedGuideIndex = -1;

    setCursorX(0);
    setCursorY(0);
    mCursorPaneX = 0;
    mCursorPaneY = 0;
    mCursorSceneX = 0;
    mCursorSceneY = 0;
    mContainsMouse = false;
    mMouseButtonPressed = Qt::NoButton;
    mLastMouseButtonPressed = Qt::NoButton;
    mPressPosition = QPoint(0, 0);
    mPressScenePosition = QPoint(0, 0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);

    setPenForegroundColour(Qt::black);
    setPenBackgroundColour(Qt::white);

    mLastPixelPenPressScenePosition = QPoint(0, 0);
    mLinePreviewImage = QImage();

    mCropArea = QRect();

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

    pane->setZoomLevel(pane->integerZoomLevel() + 1);
}

void ImageCanvas::zoomOut()
{
    CanvasPane *pane = currentPane();
    if (!pane)
        return;

    pane->setZoomLevel(pane->integerZoomLevel() - 1);
}

void ImageCanvas::flipSelection(Qt::Orientation orientation)
{
    if (!mHasSelection)
        return;

    // Just like mspaint, flipping a pasted selection has no effect on the undo stack -
    // undoing will simply remove the pasted selection and its contents.
    if (!mIsSelectionFromPaste) {
        mProject->beginMacro(QLatin1String("FlipSelection"));
        mProject->addChange(new FlipImageCanvasSelectionCommand(this, mSelectionArea, orientation));
        mProject->endMacro();
    } else {
        mSelectionContents = mSelectionContents.mirrored(orientation == Qt::Horizontal, orientation == Qt::Vertical);
        updateSelectionPreviewImage();
        update();
    }
}

void ImageCanvas::copySelection()
{
    if (!mHasSelection)
        return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setImage(currentProjectImage()->copy(mSelectionArea));
}

void ImageCanvas::paste()
{
    qCDebug(lcCanvasSelection) << "pasting selection from clipboard";

    QRect pastedArea = mSelectionArea;
    const bool fromExternalSource = !mHasSelection;

    clearOrConfirmSelection();

    QClipboard *clipboard = QGuiApplication::clipboard();
    QImage clipboardImage = clipboard->image();
    if (clipboardImage.isNull()) {
        qCDebug(lcCanvasSelection) << "Clipboard content is not an image; can't paste";
        return;
    }

    setTool(SelectionTool);

    const QSize adjustedSize(qMin(clipboardImage.width(), mProject->widthInPixels()),
        qMin(clipboardImage.height(), mProject->heightInPixels()));
    if (fromExternalSource) {
        // If the paste was from an external source, we just paste it at 0, 0.
        pastedArea = QRect(0, 0, adjustedSize.width(), adjustedSize.height());
        // TODO: #16 - pastes too far to the top left for some reason
//        if (mCurrentPane->offset().x() < 0)
//            pastedArea.moveLeft(qAbs(mCurrentPane->offset().x()) / mCurrentPane->zoomLevel());
//        if (mCurrentPane->offset().y() < 0)
//            pastedArea.moveTop(qAbs(mCurrentPane->offset().y()) / mCurrentPane->zoomLevel());
    }
    // Crop the clipboard image if it's larger than the canvas,
    if (adjustedSize != clipboardImage.size())
        clipboardImage = clipboardImage.copy(pastedArea);

    mProject->beginMacro(QLatin1String("PasteCommand"));
    mProject->addChange(new PasteImageCanvasCommand(this, clipboardImage, pastedArea.topLeft()));
    mProject->endMacro();

    // Setting a selection area is only done when a paste is first created,
    // not when it's redone, so we do it here instead of in the command.
    setSelectionFromPaste(true);
    qCDebug(lcCanvasSelection) << "setting selection contents to clipboard image with area" << pastedArea;
    mSelectionContents = clipboardImage;

    setSelectionArea(pastedArea);

    // This part is also important, as it ensures that beginSelectionMove()
    // doesn't overwrite the paste contents.
    mSelectionAreaBeforeFirstMove = mSelectionArea;

    // moveSelectionArea() does this for us when we're moving, but for the initial
    // paste, we must do it ourselves.
    updateSelectionPreviewImage();

    update();
}

void ImageCanvas::selectAll()
{
    if (mTool == SelectionTool) {
        clearOrConfirmSelection();
    } else {
        setTool(SelectionTool);
    }

    setSelectionArea(QRect(0, 0, mProject->widthInPixels(), mProject->heightInPixels()));
}

void ImageCanvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    centrePanes();
    resizeRulers();
    if (mProject)
        updateCursorPos(QPoint(mCursorX, mCursorY));
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
            const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
            // Let the pen tool draw over the same colour, as the line tool requires
            // a press point to start from, which we don't get if we make this a no-op.
            if (tool == PenTool || (tool == EraserTool && previousColour != QColor(Qt::transparent))) {
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

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    // Don't do anything if the colours are the same.
    if (previousColour == penColour()) {
        return candidateData;
    }

    QVector<QPoint> scenePositions = imagePixelFloodFill(currentProjectImage(), scenePos, previousColour, penColour());
    candidateData.scenePositions = scenePositions;

    candidateData.previousColours.append(previousColour);
    return candidateData;
}

ImageCanvas::PixelCandidateData ImageCanvas::greedyFillPixelCandidates() const
{
    PixelCandidateData candidateData;

    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos)) {
        return candidateData;
    }

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    // Don't do anything if the colours are the same.
    if (previousColour == penColour()) {
        return candidateData;
    }

    QVector<QPoint> scenePositions = imageGreedyPixelFill(currentProjectImage(), scenePos, previousColour, penColour());
    candidateData.scenePositions = scenePositions;

    candidateData.previousColours.append(previousColour);
    return candidateData;
}

void ImageCanvas::applyCurrentTool()
{
    if (areToolsForbidden())
        return;

    switch (mTool) {
    case PenTool: {
        if (!mShiftPressed) {
            const PixelCandidateData candidateData = penEraserPixelCandidates(mTool);
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mProject->beginMacro(QLatin1String("PixelPenTool"));
            mProject->addChange(new ApplyPixelPenCommand(this, candidateData.scenePositions, candidateData.previousColours, penColour()));
        } else {
            // The undo command for lines needs the project image before and after
            // the line was drawn on it.
            const QRect lineRect = normalisedLineRect();
            const QImage imageWithoutLine = currentProjectImage()->copy(lineRect);

            QImage imageWithLine = *currentProjectImage();
            QPainter painter(&imageWithLine);
            drawLine(&painter);
            painter.end();
            imageWithLine = imageWithLine.copy(lineRect);

            mProject->beginMacro(QLatin1String("PixelLineTool"));
            mProject->addChange(new ApplyPixelLineCommand(this, imageWithLine, imageWithoutLine, lineRect,
                mPressScenePosition, mLastPixelPenPressScenePosition));
            mProject->endMacro();
        }
        break;
    }
    case EyeDropperTool: {
        const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
        if (isWithinImage(scenePos)) {
            setPenForegroundColour(currentProjectImage()->pixelColor(scenePos));
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
        if (!mShiftPressed) {
            const PixelCandidateData candidateData = fillPixelCandidates();
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mProject->beginMacro(QLatin1String("PixelFillTool"));
            mProject->addChange(new ApplyPixelFillCommand(this, candidateData.scenePositions,
                candidateData.previousColours.first(), penColour()));
        } else {
            const PixelCandidateData candidateData = greedyFillPixelCandidates();
            if (candidateData.scenePositions.isEmpty()) {
                return;
            }

            mProject->beginMacro(QLatin1String("GreedyPixelFillTool"));
            mProject->addChange(new ApplyGreedyPixelFillCommand(this, candidateData.scenePositions,
                candidateData.previousColours.first(), penColour()));
        }
        break;
    }
    default:
        break;
    }
}

// This function actually operates on the image.
void ImageCanvas::applyPixelPenTool(const QPoint &scenePos, const QColor &colour, bool markAsLastRelease)
{
    currentProjectImage()->setPixelColor(scenePos, colour);
    if (markAsLastRelease)
        mLastPixelPenPressScenePosition = scenePos;
    update();
}

void ImageCanvas::applyPixelLineTool(const QImage &lineImage, const QRect &lineRect, const QPoint &lastPixelPenReleaseScenePosition)
{
    mLastPixelPenPressScenePosition = lastPixelPenReleaseScenePosition;
    QPainter painter(currentProjectImage());
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(lineRect, lineImage);
    update();
}

void ImageCanvas::paintImageOntoPortionOfImage(const QRect &portion, const QImage &replacementImage)
{
    *currentProjectImage() = Utils::paintImageOntoPortionOfImage(*currentProjectImage(), portion, replacementImage);
    update();
}

void ImageCanvas::replacePortionOfImage(const QRect &portion, const QImage &replacementImage)
{
    *currentProjectImage() = Utils::replacePortionOfImage(*currentProjectImage(), portion, replacementImage);
    update();
}

void ImageCanvas::erasePortionOfImage(const QRect &portion)
{
    *currentProjectImage() = Utils::erasePortionOfImage(*currentProjectImage(), portion);
    update();
}

void ImageCanvas::doFlipSelection(const QRect &area, Qt::Orientation orientation)
{
    const QImage flippedImagePortion = currentProjectImage()->copy(area)
        .mirrored(orientation == Qt::Horizontal, orientation == Qt::Vertical);
    erasePortionOfImage(area);
    paintImageOntoPortionOfImage(area, flippedImagePortion);
}

QPointF ImageCanvas::linePoint1() const
{
    return mLastPixelPenPressScenePosition;
}

QPointF ImageCanvas::linePoint2() const
{
    return QPointF(mCursorSceneX, mCursorSceneY);
}

QRect ImageCanvas::normalisedLineRect() const
{
    return QRect(linePoint1().toPoint(), linePoint2().toPoint()).normalized();
}

void ImageCanvas::updateCursorPos(const QPoint &eventPos)
{
    setCursorX(eventPos.x());
    setCursorY(eventPos.y());
    // Don't change current panes if panning, as the mouse position should
    // be allowed to go outside of the original pane.
    if (!isPanning()) {
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
    mCursorSceneFX = qreal(mCursorPaneX - mCurrentPane->offset().x()) / mCurrentPane->integerZoomLevel();
    mCursorSceneFY = qreal(mCursorPaneY - mCurrentPane->offset().y()) / mCurrentPane->integerZoomLevel();
    setCursorSceneX(mCursorSceneFX);
    setCursorSceneY(mCursorSceneFY);

    if (mCursorSceneX < 0 || mCursorSceneX >= mProject->widthInPixels()
        || mCursorSceneY < 0 || mCursorSceneY >= mProject->heightInPixels()) {
        setCursorPixelColour(QColor(Qt::black));
    } else {
        const QPoint cursorScenePos = QPoint(mCursorSceneX, mCursorSceneY);
        // TODO: this may slow down the application; consider caching it or using
        // ShaderSourceEffect to pick the colour instead
        setCursorPixelColour(contentImage().pixelColor(cursorScenePos));
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

    bool overRuler = false;
    if (rulersVisible()) {
        // TODO: use rulerAtCursorPos()?
        const QPointF cursorPos(mCursorX, mCursorY);
        overRuler = mFirstHorizontalRuler->contains(cursorPos) || mFirstVerticalRuler->contains(cursorPos);

        if (isSplitScreen() && !overRuler) {
            QPointF mappedCursorPos = mSecondHorizontalRuler->mapFromItem(this, cursorPos);
            overRuler = mSecondHorizontalRuler->contains(mappedCursorPos);

            if (!overRuler) {
                mappedCursorPos = mSecondVerticalRuler->mapFromItem(this, cursorPos);
                overRuler = mSecondVerticalRuler->contains(mappedCursorPos);
            }
        }
    }

    bool overGuide = false;
    if (guidesVisible() && !guidesLocked() && !overRuler) {
        overGuide = guideIndexAtCursorPos() != -1;
    }

    // Hide the window's cursor when we're in the spotlight; otherwise, use the non-custom arrow cursor.
    const bool nothingOverUs = mProject->hasLoaded() && hasActiveFocus() /*&& !mModalPopupsOpen*/ && mContainsMouse;
    const bool splitterHovered = mSplitter.isEnabled() && mSplitter.isHovered();
    const bool overSelection = cursorOverSelection();
    const bool toolsForbidden = areToolsForbidden();
    setHasBlankCursor(nothingOverUs && !isPanning() && !splitterHovered && !overSelection && !overRuler && !overGuide && !toolsForbidden);

    Qt::CursorShape cursorShape = Qt::BlankCursor;
    if (!mHasBlankCursor) {
        if (isPanning()) {
            // If panning while space is pressed, the left mouse button is used, otherwise it's the middle mouse button.
            cursorShape = (mMouseButtonPressed == Qt::LeftButton || mMouseButtonPressed == Qt::MiddleButton) ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
        } else if (overGuide) {
            cursorShape = mPressedGuideIndex != -1 ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
        } else if (overSelection) {
            cursorShape = Qt::SizeAllCursor;
        } else if (splitterHovered) {
            cursorShape = Qt::SplitHCursor;
        } else if (toolsForbidden) {
            cursorShape = Qt::ForbiddenCursor;
        } else {
            cursorShape = Qt::ArrowCursor;
        }
    }

    if (lcCanvasCursorShape().isDebugEnabled()) {
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
            break;
        case Qt::ForbiddenCursor:
            cursorName = "ForbiddenCursor";
            break;
        default:
            break;
        }

        qCDebug(lcCanvasCursorShape) << this
            << "Updating window cursor shape... mProject->hasLoaded():" << mProject->hasLoaded()
            << "hasActiveFocus()" << hasActiveFocus()
            << "mContainsMouse" << mContainsMouse
            << "isPanning()" << isPanning()
            << "mSplitter.isHovered()" << mSplitter.isHovered()
            << "areToolsForbidden()" << toolsForbidden
            << "cursor shape" << cursorName;
    }

    if (window())
        window()->setCursor(QCursor(cursorShape));
}

void ImageCanvas::onZoomLevelChanged()
{
    mFirstHorizontalRuler->setZoomLevel(mFirstPane.integerZoomLevel());
    mFirstVerticalRuler->setZoomLevel(mFirstPane.integerZoomLevel());
    mSecondHorizontalRuler->setZoomLevel(mSecondPane.integerZoomLevel());
    mSecondVerticalRuler->setZoomLevel(mSecondPane.integerZoomLevel());

    update();
}

void ImageCanvas::onPaneOffsetChanged()
{
    mFirstHorizontalRuler->setFrom(mFirstPane.offset().x());
    mFirstVerticalRuler->setFrom(mFirstPane.offset().y());

    mSecondHorizontalRuler->setFrom(mSecondPane.offset().x());
    mSecondVerticalRuler->setFrom(mSecondPane.offset().y());
}

void ImageCanvas::onPaneSizeChanged()
{
    resizeRulers();
}

void ImageCanvas::error(const QString &message)
{
//    qWarning() << Q_FUNC_INFO << message;
    emit errorOccurred(message);
}

QColor ImageCanvas::penColour() const
{
    // For some tools, like the line tool, the mouse button won't be pressed at times,
    // so we take the last mouse button that was pressed.
    const Qt::MouseButton button = mMouseButtonPressed == Qt::NoButton ? mLastMouseButtonPressed : mMouseButtonPressed;
    return button == Qt::LeftButton ? mPenForegroundColour : mPenBackgroundColour;
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

bool ImageCanvas::areToolsForbidden() const
{
    return false;
}

void ImageCanvas::wheelEvent(QWheelEvent *event)
{
    if (!mProject->hasLoaded() || !mScrollZoom) {
        event->ignore();
        return;
    }

    mCurrentPane->setSceneCentered(false);

    const int oldZoomLevel = mCurrentPane->integerZoomLevel();
    const qreal zoomAmount = 0.15;
    const qreal newZoomLevel = mCurrentPane->zoomLevel() + (event->angleDelta().y() > 0 ? zoomAmount : -zoomAmount);
    mCurrentPane->setZoomLevel(newZoomLevel);

    // From: http://stackoverflow.com/a/38302057/904422
    QPoint relativeEventPos = eventPosRelativeToCurrentPane(event->pos());
    // We still want to use integer zoom levels here; the real-based zoom level just allows
    // smaller changes in zoom level, rather than incrementing/decrementing by one every time
    // we get a wheel event.
    mCurrentPane->setOffset(relativeEventPos -
        float(mCurrentPane->integerZoomLevel()) / float(oldZoomLevel) * (relativeEventPos - mCurrentPane->offset()));
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
    mLastMouseButtonPressed = mMouseButtonPressed;
    mPressPosition = event->pos();
    mPressScenePosition = QPoint(mCursorSceneX, mCursorSceneY);
    mCurrentPaneOffsetBeforePress = mCurrentPane->offset();
    setContainsMouse(true);

    if (!isPanning()) {
        if (mSplitter.isEnabled() && mouseOverSplitterHandle(event->pos())) {
            mSplitter.setPressed(true);
            return;
        }

        if (rulersVisible() && guidesVisible()) {
            updatePressedRuler();
            if (mPressedRuler)
                return;

            if (!guidesLocked()) {
                updatePressedGuide();
                if (mPressedGuideIndex != -1) {
                    updateWindowCursorShape();
                    return;
                }
            }
        }

        if (mTool != SelectionTool) {
            applyCurrentTool();
            return;
        }

        if (!cursorOverSelection()) {
            mPotentiallySelecting = true;
            updateSelectionArea();
        } else {
            // The user has just clicked the selection. We don't actually
            // move anything on press events; we wait until mouseMoveEvent()
            // and then start moving the selection and its contents.
            beginSelectionMove();
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
        } else if (mPressedRuler || mPressedGuideIndex != -1) {
            update();
        } else {
            if (!isPanning()) {
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
    if (mTool == SelectionTool && !mPressedRuler) {
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
                        if (mIsSelectionFromPaste) {
                            // Pasting an image creates a selection, and clicking outside of that selection
                            // without moving it should apply the paste.
                            confirmPasteSelection();
                        } else {
                            // Since we hadn't done anything to the selection that we might have had
                            // before (that this event cycle is interrupting), releasing the mouse
                            // should cancel it.
                            clearSelection();
                        }
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

    Ruler *hoveredRuler = rulerAtCursorPos();
    if (mPressedRuler && !hoveredRuler) {
        // A ruler was pressed but isn't hovered; create a new guide.
        addNewGuide();
    } else if (mPressedGuideIndex != -1) {
        if (hoveredRuler) {
            if (hoveredRuler->orientation() == mProject->guides().at(mPressedGuideIndex).orientation()) {
                // A ruler wasn't pressed but a guide is, and now a ruler is hovered;
                // the user has dragged a guide onto a ruler with the correct orientation, so remove it.
                removeGuide();
            }
        } else {
            moveGuide();
        }

        mPressedGuideIndex = -1;
        updateWindowCursorShape();
    }

    mPressPosition = QPoint(0, 0);
    mPressScenePosition = QPoint(0, 0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
    updateWindowCursorShape();
    mSplitter.setPressed(false);
    mPressedRuler = nullptr;
    mGuidePositionBeforePress = 0;
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

    mSplitter.setHovered(mouseOverSplitterHandle(event->pos()));

    updateWindowCursorShape();

    if (mTool == PenTool && mShiftPressed)
        update();
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

    if (mHasSelection && !mMovingSelection && (event->key() >= Qt::Key_Left && event->key() <= Qt::Key_Down)) {
        switch (event->key()) {
        case Qt::Key_Left:
            moveSelectionAreaBy(QPoint(-1, 0));
            return;
        case Qt::Key_Right:
            moveSelectionAreaBy(QPoint(1, 0));
            return;
        case Qt::Key_Up:
            moveSelectionAreaBy(QPoint(0, -1));
            return;
        case Qt::Key_Down:
            moveSelectionAreaBy(QPoint(0, 1));
            return;
        }
    }

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
        } else if (mIsSelectionFromPaste) {
            // Pressing escape to clear a pasted selection should apply that selection, like mspaint.
            confirmPasteSelection();
        } else {
            clearSelection();
        }
    } else if (event->modifiers().testFlag(Qt::AltModifier)) {
        setAltPressed(true);
        mToolBeforeAltPressed = mTool;
        setTool(EyeDropperTool);
    } else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        setShiftPressed(true);
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
    } else if (event->key() == Qt::Key_Shift || event->modifiers().testFlag(Qt::ShiftModifier)) {
        setShiftPressed(false);
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

    // When e.g. a dialog opens, we shouldn't keep drawing the line preview.
    setShiftPressed(false);

    updateWindowCursorShape();
}

bool ImageCanvas::overrideShortcut(const QKeySequence &keySequence)
{
    if (keySequence == mProject->settings()->undoShortcut() && mHasSelection && !mIsSelectionFromPaste) {
        if (mHasMovedSelection) {
            qCDebug(lcCanvasSelection) << "Undo activated while a selection that has previously been moved is active;"
                << "confirming selection to create move command, and then instantly undoing it";
            // Create a move command so that the undo can be redone...
            confirmSelectionMove(ClearSelection);
            // ... and then immediately undo it. This is weird, but it has
            // to be done this way, because using the undo framework to take
            // care of everything is not an option, as using one macro for several
            // commands means that undo/redo is disable. See Shortcuts.qml for more info.
            mProject->undoStack()->undo();
        } else {
            // Nothing was ever moved, and this isn't a paste, so we can simply clear the selection.
            qCDebug(lcCanvasSelection) << "Overriding undo shortcut to cancel selection that hadn't been moved";
            clearSelection();
        }
        return true;
    }

    return false;
}
