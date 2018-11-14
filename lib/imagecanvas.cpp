/*
    Copyright 2018, Mitch Curtis

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
#include <QEasingCurve>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QQmlEngine>
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
#include "guidesitem.h"
#include "imageproject.h"
#include "modifyimagecanvasselectioncommand.h"
#include "moveguidecommand.h"
#include "panedrawinghelper.h"
#include "pasteimagecanvascommand.h"
#include "project.h"
#include "selectioncursorguide.h"
#include "tileset.h"
#include "utils.h"

Q_LOGGING_CATEGORY(lcImageCanvas, "app.canvas")
Q_LOGGING_CATEGORY(lcImageCanvasCursorShape, "app.canvas.cursorshape")
Q_LOGGING_CATEGORY(lcImageCanvasLifecycle, "app.canvas.lifecycle")
Q_LOGGING_CATEGORY(lcImageCanvasSelection, "app.canvas.selection")
Q_LOGGING_CATEGORY(lcImageCanvasSelectionCursorGuideVisibility, "app.canvas.selection.cursorguidevisibility")
Q_LOGGING_CATEGORY(lcImageCanvasSelectionPreviewImage, "app.canvas.selection.previewimage")

ImageCanvas::ImageCanvas() :
    mProject(nullptr),
    mImageProject(nullptr),
    mBackgroundColour(Qt::gray),
    mGridVisible(false),
    mGridColour(Qt::black),
    mSplitColour(Qt::black),
    mCheckerColour1(QColor::fromRgb(0x7e7e7e)),
    mCheckerColour2(Qt::white),
    mSplitScreen(false),
    mSplitter(this),
    mCurrentPane(&mFirstPane),
    mCurrentPaneIndex(0),
    mFirstHorizontalRuler(nullptr),
    mFirstVerticalRuler(nullptr),
    mSecondHorizontalRuler(nullptr),
    mSecondVerticalRuler(nullptr),
    mPressedRuler(nullptr),
    mGuidesVisible(false),
    mGuidesLocked(false),
    mGuidePositionBeforePress(0),
    mPressedGuideIndex(-1),
    mGuidesItem(nullptr),
    mCursorX(0),
    mCursorY(0),
    mCursorPaneX(0),
    mCursorPaneY(0),
    mCursorSceneX(0),
    mCursorSceneY(0),
    mCursorSceneFX(0),
    mCursorSceneFY(0),
    mCursorPixelColour(Qt::black),
    mContainsMouse(false),
    mMouseButtonPressed(Qt::NoButton),
    mLastMouseButtonPressed(Qt::NoButton),
    mScrollZoom(false),
    mGesturesEnabled(false),
    mTool(PenTool),
    mToolShape(SquareToolShape),
    mLastFillToolUsed(FillTool),
    mToolSize(1),
    mMaxToolSize(100),
    mPenForegroundColour(Qt::black),
    mPenBackgroundColour(Qt::white),
    mPotentiallySelecting(false),
    mHasSelection(false),
    mMovingSelection(false),
    mIsSelectionFromPaste(false),
    mConfirmingSelectionModification(false),
    mSelectionCursorGuide(nullptr),
    mLastSelectionModification(NoSelectionModification),
    mHasModifiedSelection(false),
    mAltPressed(false),
    mShiftPressed(false),
    mToolBeforeAltPressed(PenTool),
    mSpacePressed(false),
    mHasBlankCursor(false)
{
    setEnabled(false);
    setFlag(QQuickItem::ItemIsFocusScope);

    mFirstPane.setObjectName("firstPane");
    QQmlEngine::setObjectOwnership(&mFirstPane, QQmlEngine::CppOwnership);
    mSecondPane.setObjectName("secondPane");
    QQmlEngine::setObjectOwnership(&mSecondPane, QQmlEngine::CppOwnership);
    mSplitter.setPosition(mFirstPane.size());

    // We create child items in the body rather than the initialiser list
    // in order to ensure the correct drawing order.
    mGuidesItem = new GuidesItem(this);
    qreal itemZ = 3;
    mGuidesItem->setZ(itemZ++);

    mSelectionItem = new SelectionItem(this);
    mSelectionItem->setZ(itemZ++);

    mSelectionCursorGuide = new SelectionCursorGuide(this);
    mSelectionCursorGuide->setZ(itemZ++);

    mFirstHorizontalRuler = new Ruler(Qt::Horizontal, this);
    mFirstHorizontalRuler->setObjectName("firstHorizontalRuler");
    mFirstHorizontalRuler->setZ(itemZ++);

    mFirstVerticalRuler = new Ruler(Qt::Vertical, this);
    mFirstVerticalRuler->setObjectName("firstVerticalRuler");
    mFirstVerticalRuler->setDrawCorner(true);
    mFirstVerticalRuler->setZ(itemZ++);

    mSecondHorizontalRuler = new Ruler(Qt::Horizontal, this);
    mSecondHorizontalRuler->setObjectName("secondHorizontalRuler");
    mSecondHorizontalRuler->setZ(itemZ++);

    mSecondVerticalRuler = new Ruler(Qt::Vertical, this);
    mSecondVerticalRuler->setObjectName("secondVerticalRuler");
    mSecondVerticalRuler->setDrawCorner(true);
    mSecondVerticalRuler->setZ(itemZ++);

    // Give some defaults so that the range slider handles aren't stuck together.
    mTexturedFillParameters.hue()->setVarianceLowerBound(-0.2);
    mTexturedFillParameters.hue()->setVarianceUpperBound(0.2);
    mTexturedFillParameters.saturation()->setVarianceLowerBound(-0.2);
    mTexturedFillParameters.saturation()->setVarianceUpperBound(0.2);
    mTexturedFillParameters.lightness()->setEnabled(true);
    mTexturedFillParameters.lightness()->setVarianceLowerBound(-0.2);
    mTexturedFillParameters.lightness()->setVarianceUpperBound(0.2);

    connect(&mFirstPane, SIGNAL(zoomLevelChanged()), this, SLOT(onZoomLevelChanged()));
    connect(&mFirstPane, SIGNAL(integerOffsetChanged()), this, SLOT(onPaneintegerOffsetChanged()));
    connect(&mFirstPane, SIGNAL(sizeChanged()), this, SLOT(onPaneSizeChanged()));
    connect(&mSecondPane, SIGNAL(zoomLevelChanged()), this, SLOT(onZoomLevelChanged()));
    connect(&mSecondPane, SIGNAL(integerOffsetChanged()), this, SLOT(onPaneintegerOffsetChanged()));
    connect(&mSecondPane, SIGNAL(sizeChanged()), this, SLOT(onPaneSizeChanged()));
    connect(&mSplitter, SIGNAL(positionChanged()), this, SLOT(onSplitterPositionChanged()));

    recreateCheckerImage();

    qCDebug(lcImageCanvasLifecycle) << "constructing ImageCanvas" << this;
}

ImageCanvas::~ImageCanvas()
{
    qCDebug(lcImageCanvasLifecycle) << "destructing ImageCanvas" << this;
}

Project *ImageCanvas::project() const
{
    return mProject;
}

void ImageCanvas::setProject(Project *project)
{
    qCDebug(lcImageCanvas) << "setting project" << project << "on canvas" << this;

    if (project == mProject)
        return;

    if (mProject) {
        disconnectSignals();
    }

    mProject = project;
    mImageProject = qobject_cast<ImageProject*>(mProject);

    if (mProject) {
        connectSignals();

        // Read the canvas data that was stored in the project, if there is any.
        // New projects or projects that don't have their own Slate extension
        // won't have any JSON data.
        QJsonObject *cachedProjectJson = project->cachedProjectJson();

        if (cachedProjectJson->contains("lastFillToolUsed")) {
            mLastFillToolUsed = static_cast<Tool>(QMetaEnum::fromType<Tool>().keyToValue(
                qPrintable(cachedProjectJson->value("lastFillToolUsed").toString())));
        }

        bool readPanes = false;
        if (cachedProjectJson->contains("firstPane")) {
            mFirstPane.read(cachedProjectJson->value("firstPane").toObject());
            readPanes = true;
        }
        if (cachedProjectJson->contains("firstPane")) {
            mSecondPane.read(cachedProjectJson->value("secondPane").toObject());
            readPanes = true;
        }
        doSetSplitScreen(cachedProjectJson->value("splitScreen").toBool(false), DontResetPaneSizes);
        mSplitter.setEnabled(cachedProjectJson->value("splitterLocked").toBool(false));
        if (!readPanes) {
            // If there were no panes stored, then the project hasn't been saved yet,
            // so we can do what we want with the panes.
            setDefaultPaneSizes();
            centrePanes();
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
    requestContentPaint();
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
    requestContentPaint();
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

    mGuidesItem->setVisible(mGuidesVisible);
    if (mGuidesVisible)
        mGuidesItem->update();

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
    requestContentPaint();
    emit splitColourChanged();
}

QColor ImageCanvas::checkerColour1() const
{
    return mCheckerColour1;
}

void ImageCanvas::setCheckerColour1(const QColor &colour)
{
    if (colour == mCheckerColour1)
        return;

    mCheckerColour1 = colour;
    recreateCheckerImage();
    emit checkerColour1Changed();
}

QColor ImageCanvas::checkerColour2() const
{
    return mCheckerColour2;
}

void ImageCanvas::setCheckerColour2(const QColor &colour)
{
    if (colour == mCheckerColour2)
        return;

    mCheckerColour2 = colour;
    recreateCheckerImage();
    emit checkerColour2Changed();
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
    requestContentPaint();
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
        if (mIsSelectionFromPaste)
            confirmPasteSelection();
        else
            clearOrConfirmSelection();
    }

    if (mTool == FillTool || mTool == TexturedFillTool) {
        if (mTool != mLastFillToolUsed) {
            mLastFillToolUsed = mTool;
            emit lastFillToolUsedChanged();
        }
    }

    updateSelectionCursorGuideVisibility();

    toolChange();

    emit toolChanged();
}

ImageCanvas::ToolShape ImageCanvas::toolShape() const
{
    return mToolShape;
}

void ImageCanvas::setToolShape(const ImageCanvas::ToolShape &toolShape)
{
    if (toolShape == mToolShape)
        return;

    mToolShape = toolShape;

    emit toolShapeChanged();
}

ImageCanvas::Tool ImageCanvas::lastFillToolUsed() const
{
    return mLastFillToolUsed;
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

    const QColor colour = penForegroundColour.toRgb();
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

    const QColor colour = penBackgroundColour.toRgb();
    if (colour == mPenBackgroundColour)
        return;

    mPenBackgroundColour = colour;
    emit penBackgroundColourChanged();
}

TexturedFillParameters *ImageCanvas::texturedFillParameters()
{
    return &mTexturedFillParameters;
}

bool ImageCanvas::hasSelection() const
{
    return mHasSelection;
}

bool ImageCanvas::hasModifiedSelection() const
{
    return mHasModifiedSelection;
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
    mSelectionItem->update();
    emit selectionAreaChanged();
}

QColor ImageCanvas::cursorPixelColour() const
{
    return mCursorPixelColour;
}

QColor ImageCanvas::invertedCursorPixelColour() const
{
    return invertedColour(mCursorPixelColour);
}

QColor ImageCanvas::invertedColour(const QColor &colour)
{
    // https://stackoverflow.com/a/18142036/904422
    return (0xFFFFFF - colour.rgba()) | 0xFF000000;
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
    // Ensure that the selection cursor guide isn't still drawn when the mouse
    // is outside of us (e.g. over a panel).
    updateSelectionCursorGuideVisibility();

    emit containsMouseChanged();
}

QRect ImageCanvas::firstPaneVisibleSceneArea() const
{
    return mFirstPaneVisibleSceneArea;
}

QRect ImageCanvas::secondPaneVisibleSceneArea() const
{
    return mSecondPaneVisibleSceneArea;
}

QRect ImageCanvas::paneVisibleSceneArea(int paneIndex) const
{
    return paneIndex == 0 ? mFirstPaneVisibleSceneArea : mSecondPaneVisibleSceneArea;
}

bool ImageCanvas::isSplitScreen() const
{
    return mSplitScreen;
}

void ImageCanvas::setSplitScreen(bool splitScreen)
{
    doSetSplitScreen(splitScreen, ResetPaneSizes);
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

bool ImageCanvas::areGesturesEnabled() const
{
    return mGesturesEnabled;
}

void ImageCanvas::setGesturesEnabled(bool gesturesEnabled)
{
    if (gesturesEnabled == mGesturesEnabled)
        return;

    mGesturesEnabled = gesturesEnabled;
    emit gesturesEnabledChanged();
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

const CanvasPane *ImageCanvas::firstPane() const
{
    return &mFirstPane;
}

CanvasPane *ImageCanvas::secondPane()
{
    return &mSecondPane;
}

const CanvasPane *ImageCanvas::secondPane() const
{
    return &mSecondPane;
}

CanvasPane *ImageCanvas::currentPane()
{
    return mCurrentPane;
}

CanvasPane *ImageCanvas::paneAt(int index)
{
    if (index < 0 || index >= 2)
        return nullptr;

    return index == 0 ? &mFirstPane : &mSecondPane;
}

int ImageCanvas::paneWidth(int index) const
{
    return index == 0 ? mFirstPane.size() * width() : width() - mFirstPane.size() * width();
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

    requestContentPaint();
}

void ImageCanvas::connectSignals()
{
    qCDebug(lcImageCanvas) << "connecting signals for" << this << "as we have a new project" << mProject;

    connect(mProject, SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    connect(mProject, SIGNAL(projectCreated()), this, SLOT(requestContentPaint()));
    connect(mProject, SIGNAL(projectClosed()), this, SLOT(reset()));
    connect(mProject, SIGNAL(sizeChanged()), this, SLOT(requestContentPaint()));
    connect(mProject, SIGNAL(guidesChanged()), this, SLOT(onGuidesChanged()));
    connect(mProject, SIGNAL(readyForWritingToJson(QJsonObject*)),
        this, SLOT(onReadyForWritingToJson(QJsonObject*)));
    connect(mProject, SIGNAL(aboutToBeginMacro(QString)),
        this, SLOT(onAboutToBeginMacro(QString)));

    connect(window(), SIGNAL(activeFocusItemChanged()), this, SLOT(updateWindowCursorShape()));
}

void ImageCanvas::disconnectSignals()
{
    qCDebug(lcImageCanvas) << "disconnecting signals for" << this;

    mProject->disconnect(SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    mProject->disconnect(SIGNAL(projectCreated()), this, SLOT(requestContentPaint()));
    mProject->disconnect(SIGNAL(projectClosed()), this, SLOT(reset()));
    mProject->disconnect(SIGNAL(sizeChanged()), this, SLOT(requestContentPaint()));
    mProject->disconnect(SIGNAL(guidesChanged()), this, SLOT(onGuidesChanged()));
    mProject->disconnect(SIGNAL(readyForWritingToJson(QJsonObject*)),
        this, SLOT(onReadyForWritingToJson(QJsonObject*)));
    mProject->disconnect(SIGNAL(aboutToBeginMacro(QString)),
        this, SLOT(onAboutToBeginMacro(QString)));

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
    requestContentPaint();
}

void ImageCanvas::componentComplete()
{
    QQuickItem::componentComplete();

    updateVisibleSceneArea();

    // For some reason, we need to force this stuff to update when creating a
    // tileset project after a layered image project.
    updateRulerVisibility();
    resizeRulers();

    updateSelectionCursorGuideVisibility();
    mGuidesItem->setVisible(mGuidesVisible);

    resizeChildren();

    requestContentPaint();
}

void ImageCanvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    centrePanes();
    resizeRulers();
    resizeChildren();

    if (mProject)
        updateCursorPos(QPoint(mCursorX, mCursorY));
}

void ImageCanvas::resizeChildren()
{
    mSelectionCursorGuide->setWidth(width());
    mSelectionCursorGuide->setHeight(height());

    mGuidesItem->setWidth(qFloor(width()));
    mGuidesItem->setHeight(height());

    mSelectionItem->setWidth(width());
    mSelectionItem->setHeight(height());
}

QImage *ImageCanvas::currentProjectImage()
{
    return mImageProject->image();
}

const QImage *ImageCanvas::currentProjectImage() const
{
    return mImageProject->image();
}

QImage *ImageCanvas::imageForLayerAt(int layerIndex)
{
    Q_ASSERT(layerIndex == -1);
    return mImageProject->image();
}

int ImageCanvas::currentLayerIndex() const
{
    return -1;
}

QImage ImageCanvas::contentImage()
{
    mCachedContentImage = getContentImage();
    return mCachedContentImage;
}

QImage ImageCanvas::getContentImage()
{
    QImage image = !shouldDrawSelectionPreviewImage() ? *currentProjectImage() : mSelectionPreviewImage;
    // Draw the pixel-pen-line indicator over the content.
    if (isLineVisible()) {
        QPainter linePainter(&image);
        drawLine(&linePainter);
    }
    return image;
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

void ImageCanvas::centrePanes(bool respectSceneCentred)
{
    if (!mProject)
        return;

    if (!respectSceneCentred || (respectSceneCentred && mFirstPane.isSceneCentered())) {
        const QPoint newOffset(paneWidth(0) / 2 - (mProject->widthInPixels() * mFirstPane.integerZoomLevel()) / 2,
            height() / 2 - (mProject->heightInPixels() * mFirstPane.integerZoomLevel()) / 2);
        mFirstPane.setIntegerOffset(newOffset);
    }

    if (!respectSceneCentred || (respectSceneCentred && mSecondPane.isSceneCentered())) {
        const QPoint newOffset(paneWidth(1) / 2 - (mProject->widthInPixels() * mFirstPane.integerZoomLevel()) / 2,
            height() / 2 - (mProject->heightInPixels() * mFirstPane.integerZoomLevel()) / 2);
        mSecondPane.setIntegerOffset(newOffset);
    }

    requestContentPaint();
}

void ImageCanvas::doSetSplitScreen(bool splitScreen, ImageCanvas::ResetPaneSizePolicy resetPaneSizePolicy)
{
    if (splitScreen == mSplitScreen)
        return;

    mSplitScreen = splitScreen;

    if (mCurrentPane == &mSecondPane) {
        setCurrentPane(&mFirstPane);
    }

    if (resetPaneSizePolicy == ResetPaneSizes) {
        setDefaultPaneSizes();
        centrePanes();
    }

    updateVisibleSceneArea();
    updateRulerVisibility();
    resizeRulers();

    requestContentPaint();

    emit splitScreenChanged();
}

void ImageCanvas::setDefaultPaneSizes()
{
    mFirstPane.setSize(mSplitScreen ? 0.5 : 1.0);
    mSecondPane.setSize(mSplitScreen ? 0.5 : 0.0);
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
    mPressedRuler = !guidesLocked() ? rulerAtCursorPos() : nullptr;
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
    mGuidesItem->update();
}

// TODO: make projectJson a reference to make this neater
void ImageCanvas::onReadyForWritingToJson(QJsonObject *projectJson)
{
    (*projectJson)["lastFillToolUsed"] = QMetaEnum::fromType<Tool>().valueToKey(mLastFillToolUsed);

    QJsonObject firstPaneJson;
    mFirstPane.write(firstPaneJson);
    (*projectJson)["firstPane"] = firstPaneJson;

    QJsonObject secondPaneJson;
    mSecondPane.write(secondPaneJson);
    (*projectJson)["secondPane"] = secondPaneJson;

    if (mSplitScreen)
        (*projectJson)["splitScreen"] = true;
    if (mSplitter.isEnabled())
        (*projectJson)["splitterLocked"] = true;
}

void ImageCanvas::onAboutToBeginMacro(const QString &macroText)
{
    // See Project::beginMacro() for the justification for this function's existence.
    if (mConfirmingSelectionModification)
        return;

    if (macroText.contains(QLatin1String("Selection"))) {
        // The macro involves a selection, so we shouldn't clear it.
        return;
    }

    clearOrConfirmSelection();
}

void ImageCanvas::recreateCheckerImage()
{
    mCheckerImage = QImage(32, 32, QImage::Format_ARGB32_Premultiplied);

    QPainter painter(&mCheckerImage);
    int i = 0;
    for (int y = 0; y < mCheckerImage.height(); y += 8) {
        for (int x = 0; x < mCheckerImage.width(); x += 8, ++i) {
            const int row = y / 8;
            if (i % 2 == 0)
                painter.fillRect(QRect(x, y, 8, 8), row % 2 == 0 ? mCheckerColour2 : mCheckerColour1);
            else
                painter.fillRect(QRect(x, y, 8, 8), row % 2 == 0 ? mCheckerColour1 : mCheckerColour2);
        }
    }

    mCheckerPixmap = QPixmap::fromImage(mCheckerImage);

    requestContentPaint();
}

bool ImageCanvas::isPanning() const
{
    // Pressing the mouse while holding down space (or using middle mouse button) should pan.
    return mSpacePressed || mMouseButtonPressed == Qt::MiddleButton;
}

bool ImageCanvas::supportsSelectionTool() const
{
    return true;
}

void ImageCanvas::beginSelectionMove()
{
    qCDebug(lcImageCanvasSelection) << "beginning selection move... mIsSelectionFromPaste =" << mIsSelectionFromPaste;

    setMovingSelection(true);
    mSelectionAreaBeforeLastMove = mSelectionArea;

    if (mSelectionAreaBeforeFirstModification.isEmpty()) {
        // When the selection is moved for the first time in its life,
        // copy the contents within it so that we can moved them around as a preview.
        qCDebug(lcImageCanvasSelection) << "copying currentProjectImage()" << *currentProjectImage() << "into mSelectionContents";
        mSelectionAreaBeforeFirstModification = mSelectionArea;
        mSelectionContents = currentProjectImage()->copy(mSelectionAreaBeforeFirstModification);
        // Technically we don't need to call this until the selection has actually moved,
        // but updateCursorPos() calls pixelColor() on the result of contentImage(), which will be an invalid
        // image until we've updated the selection preview image (since shouldDrawSelectionPreviewImage() will
        // return true due to mMovingSelection being true).
        updateSelectionPreviewImage(SelectionMove);
    }
}

void ImageCanvas::updateOrMoveSelectionArea()
{
    if (!mMovingSelection) {
        updateSelectionArea();
    } else {
        moveSelectionArea();
    }
}

void ImageCanvas::updateSelectionArea()
{
    if (!mPotentiallySelecting) {
        // updateSelectionArea() can be called by updateOrMoveSelectionArea() as a result
        // of moving after panning (all without releasing the mouse). In that case,
        // we can't be selecting, as we were just panning, so we return early.
        // Previously we would assert that mPotentiallySelecting was true, but that's too strict.
        return;
    }

    QRect newSelectionArea(mPressScenePosition.x(), mPressScenePosition.y(),
        mCursorSceneX - mPressScenePosition.x(), mCursorSceneY - mPressScenePosition.y());

    newSelectionArea = clampSelectionArea(newSelectionArea.normalized());

    setSelectionArea(newSelectionArea);
}

void ImageCanvas::updateSelectionPreviewImage(SelectionModification reason)
{
    qCDebug(lcImageCanvasSelectionPreviewImage) << "updating selection preview image due to" << reason;

    if (!mIsSelectionFromPaste) {
        // Only if the selection wasn't pasted should we erase the area left behind.
        mSelectionPreviewImage = Utils::erasePortionOfImage(*currentProjectImage(), mSelectionAreaBeforeFirstModification);
        qCDebug(lcImageCanvasSelectionPreviewImage) << "... selection is not from paste; erasing area left behind"
            << "- new selection preview image:" << mSelectionPreviewImage;
    } else {
        mSelectionPreviewImage = *currentProjectImage();
        qCDebug(lcImageCanvasSelectionPreviewImage) << "... selection is from a paste; not touching existing canvas content"
            << "- new selection preview image:" << mSelectionPreviewImage;
    }

    // Then, move the dragged contents to their new location.
    // Doing this last ensures that the drag contents are painted over the transparency,
    // and not the other way around.
    qCDebug(lcImageCanvasSelectionPreviewImage) << "painting selection contents" << mSelectionContents
       << "within selection area" << mSelectionArea << "over top of current project image" << mSelectionPreviewImage;
    mSelectionPreviewImage = Utils::paintImageOntoPortionOfImage(mSelectionPreviewImage, mSelectionArea, mSelectionContents);
}

void ImageCanvas::moveSelectionArea()
{
//    qCDebug(lcImageCanvasSelection) << "moving selection area... mIsSelectionFromPaste =" << mIsSelectionFromPaste;

    QRect newSelectionArea = mSelectionAreaBeforeLastMove;
    const QPoint distanceMoved(mCursorSceneX - mPressScenePosition.x(), mCursorSceneY - mPressScenePosition.y());
    newSelectionArea.translate(distanceMoved);
    setSelectionArea(boundSelectionArea(newSelectionArea));

    // TODO: move this to be second-last once all tests are passing
    updateSelectionPreviewImage(SelectionMove);

    setLastSelectionModification(SelectionMove);

    requestContentPaint();
}

void ImageCanvas::moveSelectionAreaBy(const QPoint &pixelDistance)
{
    qCDebug(lcImageCanvasSelection) << "moving selection area by" << pixelDistance;

    // Moving a selection with the directional keys creates a single move command instantly.
    beginSelectionMove();

    const QRect newSelectionArea = mSelectionArea.translated(pixelDistance.x(), pixelDistance.y());
    setSelectionArea(boundSelectionArea(newSelectionArea));

    // see TODO in the function above
    updateSelectionPreviewImage(SelectionMove);

    setLastSelectionModification(SelectionMove);

    setMovingSelection(false);
    mLastValidSelectionArea = mSelectionArea;

    // setSelectionArea() should do this anyway, but just in case..
    requestContentPaint();
}

void ImageCanvas::confirmSelectionModification()
{
    Q_ASSERT(mLastSelectionModification != NoSelectionModification);
    qCDebug(lcImageCanvasSelection) << "confirming selection modification" << mLastSelectionModification;

    const QImage sourceAreaImage = currentProjectImage()->copy(mSelectionAreaBeforeFirstModification);
    const QImage targetAreaImageBeforeModification = currentProjectImage()->copy(mLastValidSelectionArea);
    const QImage targetAreaImageAfterModification = mSelectionContents;

    // Calling beginMacro() causes Project::aboutToBeginMacro() to be
    // emitted, and we're connected to it, so we have to avoid recursing.
    mConfirmingSelectionModification = true;

    mProject->beginMacro(QLatin1String("ModifySelection"));
    mProject->addChange(new ModifyImageCanvasSelectionCommand(
        this, currentLayerIndex(), mLastSelectionModification,
        mSelectionAreaBeforeFirstModification, sourceAreaImage,
        mLastValidSelectionArea, targetAreaImageBeforeModification, targetAreaImageAfterModification,
        mIsSelectionFromPaste, (mIsSelectionFromPaste ? mSelectionContents : QImage())));
    mProject->endMacro();

    mConfirmingSelectionModification = false;

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
    return Utils::ensureWithinArea(selectionArea, mProject->size());
}

void ImageCanvas::clearSelection()
{
    qCDebug(lcImageCanvasSelection) << "clearing selection";

    setSelectionArea(QRect());
    mPotentiallySelecting = false;
    setHasSelection(false);
    setMovingSelection(false);
    setSelectionFromPaste(false);
    mSelectionAreaBeforeFirstModification = QRect(0, 0, 0, 0);
    mSelectionAreaBeforeLastMove = QRect(0, 0, 0, 0);
    mLastValidSelectionArea = QRect(0, 0, 0, 0);
    mSelectionPreviewImage = QImage();
    mSelectionContents = QImage();
    setLastSelectionModification(NoSelectionModification);
    setHasModifiedSelection(false);
}

void ImageCanvas::clearOrConfirmSelection()
{
    if (mHasSelection) {
        if (mLastSelectionModification != NoSelectionModification && mLastSelectionModification != SelectionPaste)
            confirmSelectionModification();
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
    updateSelectionCursorGuideVisibility();
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
    return mMovingSelection || mIsSelectionFromPaste
        || mLastSelectionModification != NoSelectionModification;
}

bool ImageCanvas::shouldDrawSelectionCursorGuide() const
{
    return mTool == SelectionTool && !mHasSelection && mContainsMouse;
}

void ImageCanvas::updateSelectionCursorGuideVisibility()
{
    qCDebug(lcImageCanvasSelectionCursorGuideVisibility)
        << "mTool == SelectionTool:" << (mTool == SelectionTool)
        << "!mHasSelection:" << !mHasSelection
        << "mContainsMouse:" << mContainsMouse;

    const bool wasVisible = mSelectionCursorGuide->isVisible();

    mSelectionCursorGuide->setVisible(shouldDrawSelectionCursorGuide());

    // It seems to be necessary to request an update after making the guide visible.
    if (mSelectionCursorGuide->isVisible() && !wasVisible)
        mSelectionCursorGuide->update();
}

void ImageCanvas::confirmPasteSelection()
{
    // This is what the PasteImageCanvasCommand would usually do in redo().
    // Since we do it here and bypass the undo stack, we need to inform the auto swatch model that
    // a change in the image contents occurred, which is why we emit pasteSelectionConfirmed.
    paintImageOntoPortionOfImage(currentLayerIndex(), mSelectionAreaBeforeFirstModification, mSelectionContents);
    emit pasteSelectionConfirmed();
    clearSelection();
}

void ImageCanvas::setSelectionFromPaste(bool isSelectionFromPaste)
{
    mIsSelectionFromPaste = isSelectionFromPaste;
}

void ImageCanvas::panWithSelectionIfAtEdge(ImageCanvas::SelectionPanReason reason)
{
    // Only pan if there's a selection being moved or created.
    // Also, don't pan from mouse events, as the timer takes care of that
    // (prevents jumpiness when moving the mouse)
    if ((!mHasSelection && !mMovingSelection) || (mSelectionEdgePanTimer.isActive() && reason == SelectionPanMouseMovementReason))
        return;

    bool panned = false;
    static const int boostFactor = 6;
    // Scale the velocity by the zoom level to ensure that it's not too slow when zoomed in.
    static const int maxVelocity = boostFactor * mCurrentPane->zoomLevel();
    // The amount by which we should pan based on the distance by which the
    // mouse went over the edge. This is limited to max velocity, but no scaling
    // has been applied yet.
    QPoint baseOffsetChange;

    // Check the left edge.
    if (mCursorX < 0) {
        baseOffsetChange.rx() += qMin(qAbs(mCursorX), maxVelocity);
        panned = true;
    } else {
        // Check the right edge.
        const int distancePastRight = mCursorX - width();
        if (distancePastRight > 0) {
            baseOffsetChange.rx() += qMax(-distancePastRight, -maxVelocity);
            panned = true;
        }
    }

    // Check the top edge.
    if (mCursorY < 0) {
        baseOffsetChange.ry() += qMin(qAbs(mCursorY), maxVelocity);
        panned = true;
    } else {
        // Check the bottom edge.
        const int distancePastBottom = mCursorY - height();
        if (distancePastBottom > 0) {
            baseOffsetChange.ry() += qMax(-distancePastBottom, -maxVelocity);
            panned = true;
        }
    }

    if (panned) {
        // Scale the velocity based on a certain curve, rather than just doing it linearly.
        // With the right curve, this should make it easier to precisely pan with a slower velocity
        // most of the time, but also allow fast panning if the mouse is far enough past the edge.
        const int xOffsetChangeSign = baseOffsetChange.x() > 0 ? 1 : -1;
        const int yOffsetChangeSign = baseOffsetChange.y() > 0 ? 1 : -1;

        // Use qAbs() to ensure that the progress values we pass are between 0.0 and 1.0.
        const QEasingCurve curve(QEasingCurve::InCirc);
        qreal scaledXOffsetChange = curve.valueForProgress(qAbs(baseOffsetChange.x()) / qreal(maxVelocity));
        qreal scaledYOffsetChange = curve.valueForProgress(qAbs(baseOffsetChange.y()) / qreal(maxVelocity));

        // Althought InCirc works well, there is still a certain range of values that it produces
        // that will result in no panning, even though the mouse is past the edge.
        // Work around this by increasing the lower bound. We do it here instead of at the end
        // so that we don't have to mess around with getting the right sign (positive vs negative).
        if (!qFuzzyIsNull(scaledXOffsetChange))
            scaledXOffsetChange = qMin(scaledXOffsetChange + 0.15, 1.0);
        if (!qFuzzyIsNull(scaledYOffsetChange))
            scaledYOffsetChange = qMin(scaledYOffsetChange + 0.15, 1.0);

        const QPoint finalOffsetChange(
             (scaledXOffsetChange * xOffsetChangeSign) * maxVelocity,
             (scaledYOffsetChange * yOffsetChangeSign) * maxVelocity);
        mCurrentPane->setIntegerOffset(mCurrentPane->integerOffset() + finalOffsetChange);

        // Ensure that the panning still occurs when the mouse is at the edge but isn't moving.
        if (!mSelectionEdgePanTimer.isActive()) {
            static const int pansPerSecond = 25;
            static const int panInterval = 1000 / pansPerSecond;
            mSelectionEdgePanTimer.start(panInterval, this);
        }

        // The pane offset changing causes the cursor scene position to change, which
        // in turn affects the selection area.
        updateCursorPos(QPoint(mCursorX, mCursorY));
        updateOrMoveSelectionArea();

        requestContentPaint();
    } else {
        // If the mouse isn't over the edge, stop the timer.
        mSelectionEdgePanTimer.stop();
    }
}

void ImageCanvas::setLastSelectionModification(ImageCanvas::SelectionModification selectionModification)
{
    qCDebug(lcImageCanvasSelection) << "setting mLastSelectionModification to" << selectionModification;
    mLastSelectionModification = selectionModification;
    if (mLastSelectionModification == SelectionMove
            || mLastSelectionModification == SelectionFlip
            || mLastSelectionModification == SelectionRotate) {
        setHasModifiedSelection(true);
    } else if (mLastSelectionModification == NoSelectionModification) {
        setHasModifiedSelection(false);
    }
    // If it's paste, it should stay false.
    // TODO: verify that pasting multiple times in succession works as expected
}

void ImageCanvas::setHasModifiedSelection(bool hasModifiedSelection)
{
    if (hasModifiedSelection == mHasModifiedSelection)
        return;

    mHasModifiedSelection = hasModifiedSelection;
    emit hasModifiedSelectionChanged();
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
    mFirstPaneVisibleSceneArea = QRect();
    mSecondPaneVisibleSceneArea = QRect();

    setPenForegroundColour(Qt::black);
    setPenBackgroundColour(Qt::white);

    mTexturedFillParameters.reset();

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

    requestContentPaint();
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
        // TODO: need to prevent the selection from being cleared here
        mProject->beginMacro(QLatin1String("FlipSelection"));
        mProject->addChange(new FlipImageCanvasSelectionCommand(this, mSelectionArea, orientation));
        mProject->endMacro();
    } else {
        mSelectionContents = mSelectionContents.mirrored(orientation == Qt::Horizontal, orientation == Qt::Vertical);
        updateSelectionPreviewImage(SelectionFlip);
        requestContentPaint();
    }
}

void ImageCanvas::rotateSelection(int angle)
{
    qCDebug(lcImageCanvasSelection) << "rotating selection area" << mSelectionArea << angle << "by degrees";

    if (!mHasSelection)
        return;

    bool isFirstModification = false;
    if (mSelectionAreaBeforeFirstModification.isNull()) {
        mSelectionAreaBeforeFirstModification = mSelectionArea;
        isFirstModification = true;
    }

    QRect rotatedArea;
    // Only use the project's image the first time; for every consecutive rotation,
    // do the rotation on a fully transparent image and then paint it onto that.
    // This avoids contents that are under the selection as its rotated being picked up
    // and becoming part of the selection (rotateSelectionTransparentBackground() tests this).
    if (isFirstModification) {
        const QImage image = *imageForLayerAt(currentLayerIndex());
        mSelectionContents = Utils::rotateAreaWithinImage(image, mSelectionArea, angle, rotatedArea);
    } else {
        QImage image(mProject->size(), QImage::Format_ARGB32_Premultiplied);
        image.fill(Qt::transparent);
        QPainter painter(&image);
        painter.drawImage(mSelectionArea, mSelectionContents);
        mSelectionContents = Utils::rotateAreaWithinImage(image, mSelectionArea, angle, rotatedArea);
    }

    Q_ASSERT(mHasSelection);
    setSelectionArea(rotatedArea);
    mLastValidSelectionArea = mSelectionArea;

    setLastSelectionModification(SelectionRotate);

    updateSelectionPreviewImage(SelectionRotate);
    requestContentPaint();
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
    qCDebug(lcImageCanvasSelection) << "pasting selection from clipboard";

    QRect pastedArea = mSelectionArea;
    const bool fromExternalSource = !mHasSelection;

    clearOrConfirmSelection();

    QClipboard *clipboard = QGuiApplication::clipboard();
    QImage clipboardImage = clipboard->image();
    if (clipboardImage.isNull()) {
        qCDebug(lcImageCanvasSelection) << "Clipboard content is not an image; can't paste";
        return;
    }

    setTool(SelectionTool);

    const QSize adjustedSize(qMin(clipboardImage.width(), mProject->widthInPixels()),
        qMin(clipboardImage.height(), mProject->heightInPixels()));
    if (fromExternalSource) {
        // If the paste was from an external source, or there was no
        // selection prior to pasting, we just paste it at 0, 0.
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
    mProject->addChange(new PasteImageCanvasCommand(this, currentLayerIndex(), clipboardImage, pastedArea.topLeft()));
    mProject->endMacro();

    // Setting a selection area is only done when a paste is first created,
    // not when it's redone, so we do it here instead of in the command.
    setSelectionFromPaste(true);
    qCDebug(lcImageCanvasSelection) << "setting selection contents to clipboard image with area" << pastedArea;
    mSelectionContents = clipboardImage;

    setSelectionArea(pastedArea);

    // This part is also important, as it ensures that beginSelectionMove()
    // doesn't overwrite the paste contents.
    mSelectionAreaBeforeFirstModification = mSelectionArea;
    setLastSelectionModification(SelectionPaste);

    // moveSelectionArea() does this for us when we're moving, but for the initial
    // paste, we must do it ourselves.
    updateSelectionPreviewImage(SelectionPaste);

    requestContentPaint();
}

void ImageCanvas::deleteSelectionOrContents()
{
    mProject->beginMacro(QLatin1String("DeleteSelection"));
    const QRect deletionArea = mHasSelection ? mSelectionArea : mProject->bounds();
    mProject->addChange(new DeleteImageCanvasSelectionCommand(this, currentLayerIndex(), deletionArea));
    mProject->endMacro();
    clearSelection();
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

void ImageCanvas::cycleFillTools()
{
    setTool(mLastFillToolUsed == FillTool ? TexturedFillTool : FillTool);
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

QImage ImageCanvas::fillPixels() const
{
    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos))
        return QImage();

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    // Don't do anything if the colours are the same.
    if (previousColour == penColour())
        return QImage();

    return imagePixelFloodFill2(currentProjectImage(), scenePos, previousColour, penColour());
}

QImage ImageCanvas::greedyFillPixels() const
{
    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos))
        return QImage();

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    if (previousColour == penColour())
        return QImage();

    return imageGreedyPixelFill2(currentProjectImage(), scenePos, previousColour, penColour());
}

QImage ImageCanvas::texturedFillPixels() const
{
    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos))
        return QImage();

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    if (previousColour == penColour())
        return QImage();

    return texturedFill(currentProjectImage(), scenePos, previousColour, penColour(), mTexturedFillParameters);
}

QImage ImageCanvas::greedyTexturedFillPixels() const
{
    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos))
        return QImage();

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    if (previousColour == penColour())
        return QImage();

    return greedyTexturedFill(currentProjectImage(), scenePos, previousColour, penColour(), mTexturedFillParameters);
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
            mProject->addChange(new ApplyPixelPenCommand(this, mProject->currentLayerIndex(),
                candidateData.scenePositions, candidateData.previousColours, penColour()));
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
            mProject->addChange(new ApplyPixelLineCommand(this, mProject->currentLayerIndex(),
                imageWithLine, imageWithoutLine, lineRect, mPressScenePosition, mLastPixelPenPressScenePosition));
            mProject->endMacro();
        }
        break;
    }
    case EyeDropperTool: {
        const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
        if (isWithinImage(scenePos)) {
            setPenColour(currentProjectImage()->pixelColor(scenePos));
        }
        break;
    }
    case EraserTool: {
        const PixelCandidateData candidateData = penEraserPixelCandidates(mTool);
        if (candidateData.scenePositions.isEmpty()) {
            return;
        }

        mProject->beginMacro(QLatin1String("PixelEraserTool"));
        mProject->addChange(new ApplyPixelEraserCommand(this, mProject->currentLayerIndex(),
                candidateData.scenePositions, candidateData.previousColours));
        break;
    }
    case FillTool: {
        if (!mShiftPressed) {
            const QImage filledImage = fillPixels();
            if (filledImage.isNull())
                return;

            mProject->beginMacro(QLatin1String("PixelFillTool"));
            mProject->addChange(new ApplyPixelFillCommand(this, mProject->currentLayerIndex(),
                *currentProjectImage(), filledImage));
            // TODO: see if the tests pass with these added
            // mProject->endMacro();
        } else {
            const QImage filledImage = greedyFillPixels();
            if (filledImage.isNull())
                return;

            mProject->beginMacro(QLatin1String("GreedyPixelFillTool"));
            mProject->addChange(new ApplyGreedyPixelFillCommand(this, mProject->currentLayerIndex(),
                *currentProjectImage(), filledImage));
            // mProject->endMacro();
        }
        break;
    }
    case TexturedFillTool: {
        if (!mShiftPressed) {
            const QImage filledImage = texturedFillPixels();
            if (filledImage.isNull())
                return;

            mProject->beginMacro(QLatin1String("PixelTexturedFillTool"));
            mProject->addChange(new ApplyPixelFillCommand(this, mProject->currentLayerIndex(),
                *currentProjectImage(), filledImage));
            // mProject->endMacro();
        } else {
            const QImage filledImage = greedyFillPixels();
            if (filledImage.isNull())
                return;

            mProject->beginMacro(QLatin1String("GreedyPixelTexturedFillTool"));
            mProject->addChange(new ApplyGreedyPixelFillCommand(this, mProject->currentLayerIndex(),
                *currentProjectImage(), filledImage));
            // mProject->endMacro();
        }
        break;
    }
    default:
        break;
    }
}

// This function actually operates on the image.
void ImageCanvas::applyPixelPenTool(int layerIndex, const QPoint &scenePos, const QColor &colour, bool markAsLastRelease)
{
    imageForLayerAt(layerIndex)->setPixelColor(scenePos, colour);
    if (markAsLastRelease)
        mLastPixelPenPressScenePosition = scenePos;
    requestContentPaint();
}

void ImageCanvas::applyPixelLineTool(int layerIndex, const QImage &lineImage, const QRect &lineRect,
    const QPoint &lastPixelPenReleaseScenePosition)
{
    mLastPixelPenPressScenePosition = lastPixelPenReleaseScenePosition;
    QPainter painter(imageForLayerAt(layerIndex));
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(lineRect, lineImage);
    requestContentPaint();
}

void ImageCanvas::paintImageOntoPortionOfImage(int layerIndex, const QRect &portion, const QImage &replacementImage)
{
    QImage *image = imageForLayerAt(layerIndex);
    *image = Utils::paintImageOntoPortionOfImage(*image, portion, replacementImage);
    requestContentPaint();
}

void ImageCanvas::replacePortionOfImage(int layerIndex, const QRect &portion, const QImage &replacementImage)
{
    QImage *image = imageForLayerAt(layerIndex);
    *image = Utils::replacePortionOfImage(*image, portion, replacementImage);
    requestContentPaint();
}

void ImageCanvas::erasePortionOfImage(int layerIndex, const QRect &portion)
{
    QImage *image = imageForLayerAt(layerIndex);
    *image = Utils::erasePortionOfImage(*image, portion);
    requestContentPaint();
}

void ImageCanvas::replaceImage(int layerIndex, const QImage &replacementImage)
{
    // TODO: could ImageCanvas just be a LayeredImageCanvas with one layer?
    QImage *image = imageForLayerAt(layerIndex);
    *image = replacementImage;
    requestContentPaint();
}

void ImageCanvas::doFlipSelection(int layerIndex, const QRect &area, Qt::Orientation orientation)
{
    const QImage flippedImagePortion = currentProjectImage()->copy(area)
        .mirrored(orientation == Qt::Horizontal, orientation == Qt::Vertical);
    erasePortionOfImage(layerIndex, area);
    paintImageOntoPortionOfImage(layerIndex, area, flippedImagePortion);
}

QRect ImageCanvas::doRotateSelection(int layerIndex, const QRect &area, int angle)
{
    QImage *image = imageForLayerAt(layerIndex);
    QRect rotatedArea;
    *image = Utils::rotateAreaWithinImage(*image, area, angle, rotatedArea);
    // Only update the selection area when the commands are being created for the first time,
    // not when they're being undone and redone.
    if (mHasSelection)
        setSelectionArea(rotatedArea);
    requestContentPaint();
    return area.united(rotatedArea);
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
    // sqrt(2) is the ratio between the hypotenuse of a square and its side;
    // a simplification of Pythagoras’ theorem.
    // The bounds could be tighter by taking into account the specific rotation of the brush,
    // but the sqrt(2) ensures it is big enough for any rotation.
    const int margin = qCeil(M_SQRT2 * mToolSize / 2.0);
    return QRect(linePoint1().toPoint(), linePoint2().toPoint()).normalized()
            .marginsAdded({margin, margin, margin, margin});
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
    const bool inFirstPane = hoveredPane(eventPos) == &mFirstPane;
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
    mCursorSceneFX = qreal(mCursorPaneX - mCurrentPane->integerOffset().x()) / mCurrentPane->integerZoomLevel();
    mCursorSceneFY = qreal(mCursorPaneY - mCurrentPane->integerOffset().y()) / mCurrentPane->integerZoomLevel();

    const int oldCursorSceneX = mCursorSceneX;
    const int oldCursorSceneY = mCursorSceneY;
    setCursorSceneX(mCursorSceneFX);
    setCursorSceneY(mCursorSceneFY);

    if (mCursorSceneX < 0 || mCursorSceneX >= mProject->widthInPixels()
        || mCursorSceneY < 0 || mCursorSceneY >= mProject->heightInPixels()
        // The cached contents can be null for some reason during tests; probably because
        // the canvas gets events before it has rendered. No big deal, it wouldn't be visible yet anyway.
        || mCachedContentImage.isNull()) {
        setCursorPixelColour(QColor(Qt::black));
    } else {
        const QPoint cursorScenePos = QPoint(mCursorSceneX, mCursorSceneY);
        setCursorPixelColour(mCachedContentImage.pixelColor(cursorScenePos));
    }

    const bool cursorScenePosChanged = mCursorSceneX != oldCursorSceneX || mCursorSceneY != oldCursorSceneY;
    if (cursorScenePosChanged && mSelectionCursorGuide->isVisible())
        mSelectionCursorGuide->update();
}

void ImageCanvas::updateVisibleSceneArea()
{
    int integerZoomLevel = mFirstPane.integerZoomLevel();

    mFirstPaneVisibleSceneArea = QRect(
        -mFirstPane.integerOffset().x() / integerZoomLevel,
        -mFirstPane.integerOffset().y() / integerZoomLevel,
        paneWidth(0) / integerZoomLevel,
        height() / integerZoomLevel);

    if (mSplitScreen) {
        integerZoomLevel = mSecondPane.integerZoomLevel();

        mSecondPaneVisibleSceneArea = QRect(
            -mSecondPane.integerOffset().x() / integerZoomLevel,
            -mSecondPane.integerOffset().y() / integerZoomLevel,
            paneWidth(1) / integerZoomLevel,
            height() / integerZoomLevel);
    }
}

void ImageCanvas::onLoadedChanged()
{
    if (mProject->hasLoaded()) {
        centrePanes();
    }

    updateWindowCursorShape();
}

void ImageCanvas::requestContentPaint()
{
    // It's nice to be able to debug where a paint request comes from;
    // that's the only reason that these functions are slots and the signal isn't
    // just emitted immediately instead.
    emit contentPaintRequested(-1);
}

void ImageCanvas::requestPaneContentPaint(int paneIndex)
{
    emit contentPaintRequested(paneIndex);
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
        } else if (toolsForbidden && nothingOverUs) {
            cursorShape = Qt::ForbiddenCursor;
        } else {
            cursorShape = Qt::ArrowCursor;
        }
    }

    if (lcImageCanvasCursorShape().isDebugEnabled()) {
        qCDebug(lcImageCanvasCursorShape) << "Updating window cursor shape for" << objectName() << "..."
            << "\n... mProject->hasLoaded()" << mProject->hasLoaded()
            << "\n........ hasActiveFocus()" << hasActiveFocus()
            << "\n.......... mContainsMouse" << mContainsMouse
            << "\n............. isPanning()" << isPanning()
            << "\n... mSplitter.isHovered()" << mSplitter.isHovered()
            << "\n..... areToolsForbidden()" << toolsForbidden
            << "\n......... mHasBlankCursor" << mHasBlankCursor
            << "\n............ cursor shape" << Utils::enumToString(cursorShape);
    }

    if (window())
        window()->setCursor(QCursor(cursorShape));
}

void ImageCanvas::onZoomLevelChanged()
{
    updateVisibleSceneArea();

    mFirstHorizontalRuler->setZoomLevel(mFirstPane.integerZoomLevel());
    mFirstVerticalRuler->setZoomLevel(mFirstPane.integerZoomLevel());
    mSecondHorizontalRuler->setZoomLevel(mSecondPane.integerZoomLevel());
    mSecondVerticalRuler->setZoomLevel(mSecondPane.integerZoomLevel());

    requestContentPaint();

    if (mGuidesVisible)
        mGuidesItem->update();

    mSelectionItem->update();
}

void ImageCanvas::onPaneintegerOffsetChanged()
{
    updateVisibleSceneArea();

    mFirstHorizontalRuler->setFrom(mFirstPane.integerOffset().x());
    mFirstVerticalRuler->setFrom(mFirstPane.integerOffset().y());

    mSecondHorizontalRuler->setFrom(mSecondPane.integerOffset().x());
    mSecondVerticalRuler->setFrom(mSecondPane.integerOffset().y());

    if (mGuidesVisible)
        mGuidesItem->update();

    mSelectionItem->update();
}

void ImageCanvas::onPaneSizeChanged()
{
    updateVisibleSceneArea();

    resizeRulers();

    if (mGuidesVisible)
        mGuidesItem->update();

    mSelectionItem->update();
}

void ImageCanvas::error(const QString &message)
{
//    qWarning() << Q_FUNC_INFO << message;
    emit errorOccurred(message);
}

Qt::MouseButton ImageCanvas::pressedMouseButton() const
{
    // For some tools, like the line tool, the mouse button won't be pressed at times,
    // so we take the last mouse button that was pressed.
    return mMouseButtonPressed == Qt::NoButton ? mLastMouseButtonPressed : mMouseButtonPressed;
}

QColor ImageCanvas::penColour() const
{
    return pressedMouseButton() == Qt::LeftButton ? mPenForegroundColour : mPenBackgroundColour;
}

void ImageCanvas::setPenColour(const QColor &colour)
{
    if (pressedMouseButton() == Qt::LeftButton)
        setPenForegroundColour(colour);
    else
        setPenBackgroundColour(colour);
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
    if (pane == mCurrentPane)
        return;

    mCurrentPane = pane;
    mCurrentPaneIndex = (pane == &mSecondPane ? 1 : 0);
    emit currentPaneChanged();
}

CanvasPane *ImageCanvas::hoveredPane(const QPoint &pos)
{
    // Can only hover the first pane if there's only one screen.
    if (!mSplitScreen)
        return &mFirstPane;

    // If we're creating or moving a selection, don't let the current pane be changed.
    if (mHasSelection && mMouseButtonPressed == Qt::LeftButton)
        return mCurrentPane;

    const int firstPaneWidth = paneWidth(0);
    return pos.x() <= firstPaneWidth ? &mFirstPane : &mSecondPane;
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

bool ImageCanvas::event(QEvent *event)
{
    // This allows us to handle the two-finger pinch zoom gesture on macOS trackpads, for example.
    if (mGesturesEnabled && event->type() == QEvent::NativeGesture) {
        QNativeGestureEvent *gestureEvent = static_cast<QNativeGestureEvent*>(event);
        if (gestureEvent->gestureType() == Qt::ZoomNativeGesture
                || gestureEvent->gestureType() == Qt::BeginNativeGesture
                || gestureEvent->gestureType() == Qt::EndNativeGesture) {
            if (!qFuzzyIsNull(gestureEvent->value())) {
                mCurrentPane->setSceneCentered(false);

                // Zooming is a bit slow without this.
                static const qreal minZoomFactor = 3.0;
                static const qreal maxZoomFactor = 10.0;

                // Apply an easing curve to make zooming faster the more zoomed-in we are.
                const qreal percentageOfMaxZoomLevel = mCurrentPane->zoomLevel() / mCurrentPane->maxZoomLevel();
                const QEasingCurve zoomCurve(QEasingCurve::OutQuart);
                const qreal scaledZoomFactor = qMax(minZoomFactor, zoomCurve.valueForProgress(percentageOfMaxZoomLevel) * maxZoomFactor);

                const qreal zoomAmount = gestureEvent->value() * scaledZoomFactor;
                const qreal newZoom = mCurrentPane->zoomLevel() + zoomAmount;
                applyZoom(newZoom, gestureEvent->pos());
            }
            return true;
        }
    }

    return QQuickItem::event(event);
}

void ImageCanvas::applyZoom(qreal zoom, const QPoint &origin)
{
    const qreal oldZoomLevel = qreal(mCurrentPane->integerZoomLevel());

    mCurrentPane->setZoomLevel(zoom);

    // From: http://stackoverflow.com/a/38302057/904422
    const QPoint relativeEventPos = eventPosRelativeToCurrentPane(origin);
    // We still want to use integer zoom levels here; the real-based zoom level just allows
    // smaller changes in zoom level, rather than incrementing/decrementing by one every time
    // we get a wheel event.
    mCurrentPane->setIntegerOffset(relativeEventPos -
        qreal(mCurrentPane->integerZoomLevel()) / oldZoomLevel * (relativeEventPos - mCurrentPane->integerOffset()));
}

void ImageCanvas::wheelEvent(QWheelEvent *event)
{
    if (!mProject->hasLoaded() || !mScrollZoom) {
        event->ignore();
        return;
    }

    // We set this here rather than in applyZoom() because the user should
    // probably be able to use the menu items to zoom in without the
    // centered scene setting being disrupted.
    mCurrentPane->setSceneCentered(false);

    const QPoint pixelDelta = event->pixelDelta();
    const QPoint angleDelta = event->angleDelta();

    if (!mGesturesEnabled) {
        // Wheel events zoom.
        qreal newZoomLevel = 0;
        if (!pixelDelta.isNull()) {
            const qreal zoomAmount = pixelDelta.y() * 0.01;
            newZoomLevel = mCurrentPane->zoomLevel() + zoomAmount;
        } else if (!angleDelta.isNull()) {
            const qreal zoomAmount = 1.0;
            newZoomLevel = mCurrentPane->zoomLevel() + (angleDelta.y() > 0 ? zoomAmount : -zoomAmount);
        }

        if (!qFuzzyIsNull(newZoomLevel))
            applyZoom(newZoomLevel, event->pos());
    } else {
        // Wheel events pan.
        if (pixelDelta.isNull())
            return;

        const QPointF panDistance(pixelDelta.x() * 0.1, pixelDelta.y() * 0.1);
        mCurrentPane->setOffset(mCurrentPane->offset() + panDistance);
        requestPaneContentPaint(mCurrentPaneIndex);
    }
}

void ImageCanvas::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);

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
    mCurrentPaneOffsetBeforePress = mCurrentPane->integerOffset();
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
    QQuickItem::mouseMoveEvent(event);

    updateCursorPos(event->pos());

    if (!mProject->hasLoaded())
        return;

    setContainsMouse(true);

    if (mMouseButtonPressed) {
        if (mSplitter.isEnabled() && mSplitter.isPressed()) {
            mSplitter.setPosition(mCursorX / width());
        } else if (mPressedRuler) {
            requestContentPaint();
            // Ensure that the guide being created is drawn.
            mGuidesItem->update();
        } else if (mPressedGuideIndex != -1) {
            mGuidesItem->update();
        } else {
            if (!isPanning()) {
                if (mTool != SelectionTool) {
                    applyCurrentTool();
                } else {
                    panWithSelectionIfAtEdge(SelectionPanMouseMovementReason);

                    updateOrMoveSelectionArea();
                }
            } else {
                // Panning.
                mCurrentPane->setSceneCentered(false);
                mCurrentPane->setIntegerOffset(mCurrentPaneOffsetBeforePress + (event->pos() - mPressPosition));
                requestPaneContentPaint(mCurrentPaneIndex);
            }
        }
    }
}

void ImageCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);

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
                // Temporary note: this check used to be "if (!mHasMovedSelection) {"
                if (!mHasModifiedSelection) {
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
                        // There is no selection.
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
                    confirmSelectionModification();
                }
            }
        } else {
            // mspaint and photoshop both exhibit the same behaviour here:
            // moving a selection several times and then undoing once will undo all movement.
            // It's for this reason that we don't create a move command here.
            // They handle rotation differently though: mspaint will undo all rotation since
            // the selection was created, whereas photoshop will only undo one rotation at a time.
            moveSelectionArea();
            setMovingSelection(false);
            if (!mSelectionArea.size().isEmpty())
                mLastValidSelectionArea = mSelectionArea;
        }
        mPotentiallySelecting = false;

        mSelectionEdgePanTimer.stop();
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
    QQuickItem::hoverEnterEvent(event);

    updateCursorPos(event->pos());

    setContainsMouse(true);

    if (!mProject->hasLoaded())
        return;
}

void ImageCanvas::hoverMoveEvent(QHoverEvent *event)
{
    QQuickItem::hoverMoveEvent(event);

    updateCursorPos(event->pos());

    setContainsMouse(true);

    if (!mProject->hasLoaded())
        return;

    mSplitter.setHovered(mouseOverSplitterHandle(event->pos()));

    updateWindowCursorShape();

    if (mTool == PenTool && mShiftPressed)
        requestContentPaint();
}

void ImageCanvas::hoverLeaveEvent(QHoverEvent *event)
{
    QQuickItem::hoverLeaveEvent(event);

    setContainsMouse(false);

    if (!mProject->hasLoaded())
        return;
}

void ImageCanvas::keyPressEvent(QKeyEvent *event)
{
    QQuickItem::keyPressEvent(event);

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

    if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_5) {
        const Tool activatedTool = static_cast<ImageCanvas::Tool>(PenTool + event->key() - Qt::Key_1);
        if (activatedTool == FillTool && (mTool == FillTool || mTool == TexturedFillTool)) {
            // If the tool was already one of the fill tools, select the next fill tool.
            cycleFillTools();
        } else {
            if (activatedTool != SelectionTool || (activatedTool == SelectionTool && supportsSelectionTool())) {
                // Don't set the selection tool if the canvas doesn't support it.
                setTool(activatedTool);
            }
        }
    } else if (event->key() == Qt::Key_Space) {
        mSpacePressed = true;
        updateWindowCursorShape();
    } else if (event->key() == Qt::Key_Escape && mHasSelection) {
        if (mLastSelectionModification != NoSelectionModification && mLastSelectionModification != SelectionPaste) {
            // We've modified the selection since creating it, so, like mspaint, escape confirms it.
            confirmSelectionModification();
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
    QQuickItem::keyReleaseEvent(event);

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
    QQuickItem::focusInEvent(event);

    updateWindowCursorShape();
}

void ImageCanvas::focusOutEvent(QFocusEvent *event)
{
    QQuickItem::focusOutEvent(event);

    // The alt-to-eyedrop feature is meant to be temporary,
    // so it should restore the previous tool if focus is taken away.
    if (mAltPressed) {
        restoreToolBeforeAltPressed();
    }

    // When e.g. a dialog opens, we shouldn't keep drawing the line preview.
    setShiftPressed(false);

    updateWindowCursorShape();
}

void ImageCanvas::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != mSelectionEdgePanTimer.timerId()) {
        QQuickItem::timerEvent(event);
        return;
    }

    panWithSelectionIfAtEdge(SelectionPanTimerReason);
}

void ImageCanvas::undo()
{
    if (mHasSelection && !mIsSelectionFromPaste) {
        if (mLastSelectionModification != NoSelectionModification) {
            qCDebug(lcImageCanvasSelection) << "Undo activated while a selection that has previously been modified is active;"
                << "confirming selection to create undo command, and then instantly undoing it";

            // Create a move command so that the undo can be redone...
            confirmSelectionModification();
            // ... and then immediately undo it. This is weird, but it has
            // to be done this way, because we want to behave like mspsaint, where pressing Ctrl+Z
            // with a modified selection will undo *all* modifications done to the selection since it was created.
            // Since we have special undo behaviour, we can't use the undo framework for all of it, and so
            // we store the temporary state in mSelectionContents (which is displayed via mSelectionPreviewImage).
            // See the undo shortcut in Shortcuts.qml for more info.
            mProject->undoStack()->undo();
//            requestContentPaint();
        } else {
            // Nothing was ever modified, and this isn't a paste, so we can simply clear the selection.
            qCDebug(lcImageCanvasSelection) << "Overriding undo shortcut to cancel selection that hadn't been modified";
            clearSelection();
        }
    } else {
        mProject->undoStack()->undo();
    }
}
