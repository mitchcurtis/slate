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

#include "addguidescommand.h"
#include "addnotecommand.h"
#include "applicationsettings.h"
#include "applygreedypixelfillcommand.h"
#include "applypixelerasercommand.h"
#include "applypixelfillcommand.h"
#include "applypixellinecommand.h"
#include "applypixelpencommand.h"
#include "changenotecommand.h"
#include "deleteguidescommand.h"
#include "deletenotecommand.h"
#include "deleteimagecanvasselectioncommand.h"
#include "fillalgorithms.h"
#include "flipimagecanvasselectioncommand.h"
#include "imageproject.h"
#include "imageutils.h"
#include "modifyimagecanvasselectioncommand.h"
#include "moveguidecommand.h"
#include "note.h"
#include "panedrawinghelper.h"
#include "pasteimagecanvascommand.h"
#include "project.h"
#include "projectutils.h"
#include "qtutils.h"
#include "tileset.h"

Q_LOGGING_CATEGORY(lcImageCanvas, "app.canvas")
Q_LOGGING_CATEGORY(lcImageCanvasCursorPos, "app.canvas.cursorpos")
Q_LOGGING_CATEGORY(lcImageCanvasCursorShape, "app.canvas.cursorshape")
Q_LOGGING_CATEGORY(lcImageCanvasEvents, "app.canvas.events")
Q_LOGGING_CATEGORY(lcImageCanvasHoverEvents, "app.canvas.events.hover")
Q_LOGGING_CATEGORY(lcImageCanvasFocusEvents, "app.canvas.events.focus")
Q_LOGGING_CATEGORY(lcImageCanvasLifecycle, "app.canvas.lifecycle")
Q_LOGGING_CATEGORY(lcImageCanvasGuides, "app.canvas.guides")
Q_LOGGING_CATEGORY(lcImageCanvasNotes, "app.canvas.notes")
Q_LOGGING_CATEGORY(lcImageCanvasSelection, "app.canvas.selection")
Q_LOGGING_CATEGORY(lcImageCanvasSelectionPreviewImage, "app.canvas.selection.previewimage")
Q_LOGGING_CATEGORY(lcImageCanvasUiState, "app.canvas.uistate")
Q_LOGGING_CATEGORY(lcImageCanvasUndo, "app.canvas.undo")

/*
    ImageCanvas doesn't do any rendering itself; CanvasPaneItem does that.
    ImageCanvas just handles events in order to draw onto an image, which is
    then rendered by CanvasPaneItem. It also draws things like the selection
    marquee and selection contents onto the image.

    See CanvasPaneRepeater.qml for an illustration of stacking order.

    CanvasPaneItem
*/

ImageCanvas::ImageCanvas() :
    mProject(nullptr),
    mImageProject(nullptr),
    mBackgroundColour(Qt::gray),
    mGridColour(Qt::black),
    mSplitColour(Qt::black),
    mCheckerColour1(QColor::fromRgb(0x7e7e7e)),
    mCheckerColour2(Qt::white),
    mSplitScreen(false),
    mSplitter(this),
    mCurrentPane(&mFirstPane),
    mCurrentPaneIndex(0),
    mPressedRuler(nullptr),
    mGuidesVisible(true),
    mGuidesLocked(false),
    mNotesVisible(true),
    mAnimationMarkersVisible(true),
    mHighlightedAnimationFrameIndex(-1),
    mGuidePositionBeforePress(0),
    mPressedGuideIndex(-1),
    mPressedNoteIndex(-1),
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
    mRightClickBehaviour(PenToolRightClickAppliesEraser),
    mPotentiallySelecting(false),
    mHasSelection(false),
    mMovingSelection(false),
    mIsSelectionFromPaste(false),
    mConfirmingSelectionModification(false),
    mLastSelectionModificationBeforeImageAdjustment(NoSelectionModification),
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

    // Give some defaults so that the range slider handles aren't stuck together.
    mTexturedFillParameters.hue()->setVarianceLowerBound(-0.2);
    mTexturedFillParameters.hue()->setVarianceUpperBound(0.2);
    mTexturedFillParameters.saturation()->setVarianceLowerBound(-0.2);
    mTexturedFillParameters.saturation()->setVarianceUpperBound(0.2);
    mTexturedFillParameters.lightness()->setEnabled(true);
    mTexturedFillParameters.lightness()->setVarianceLowerBound(-0.2);
    mTexturedFillParameters.lightness()->setVarianceUpperBound(0.2);

    connect(&mFirstPane, SIGNAL(zoomLevelChanged()), this, SLOT(onZoomLevelChanged()));
    connect(&mFirstPane, SIGNAL(integerOffsetChanged()), this, SLOT(onPaneIntegerOffsetChanged()));
    connect(&mFirstPane, SIGNAL(sizeChanged()), this, SLOT(onPaneSizeChanged()));
    connect(&mSecondPane, SIGNAL(zoomLevelChanged()), this, SLOT(onZoomLevelChanged()));
    connect(&mSecondPane, SIGNAL(integerOffsetChanged()), this, SLOT(onPaneIntegerOffsetChanged()));
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

void ImageCanvas::restoreState()
{
    SerialisableState *uiState = mProject->uiState();
    qCDebug(lcImageCanvasUiState) << "restoring UI state:\n" << uiState->map();

    // Read the canvas data that was stored in the project, if there is any.
    // New projects or projects that don't have their own Slate extension
    // won't have any data.
    // We also don't write anything if properties are equal to their default values.

    bool readPanes = false;

    if (uiState->contains("lastFillToolUsed")) {
        mLastFillToolUsed = static_cast<Tool>(QMetaEnum::fromType<Tool>().keyToValue(
            qPrintable(uiState->value("lastFillToolUsed", QString()).toString())));
    }
    if (uiState->contains("firstPane")) {
        mFirstPane.read(QJsonObject::fromVariantMap(uiState->value("firstPane").toMap()));
        readPanes = true;
    }
    if (uiState->contains("secondPane")) {
        mSecondPane.read(QJsonObject::fromVariantMap(uiState->value("secondPane").toMap()));
        readPanes = true;
    }
    // TODO: make default* constants for these which we can use here and in the constructor's initialiser list.
    setRulersVisible(uiState->value("rulersVisible", true).toBool());
    setGuidesVisible(uiState->value("guidesVisible", true).toBool());
    setGuidesLocked(uiState->value("guidesLocked", false).toBool());
    setNotesVisible(uiState->value("notesVisible", true).toBool());
    setAnimationMarkersVisible(uiState->value("animationMarkersVisible", true).toBool());
    doSetSplitScreen(uiState->value("splitScreen", false).toBool(), DontResetPaneSizes);
    mSplitter.setEnabled(uiState->value("splitterLocked", false).toBool());

    if (!readPanes) {
        // If there were no panes stored, then the project hasn't been saved yet,
        // so we can do what we want with the panes.
        setDefaultPaneSizes();
        centrePanes();
    }

    qCDebug(lcImageCanvasUiState) << "... restored UI state.";
}

void ImageCanvas::saveState()
{
    qCDebug(lcImageCanvasUiState) << "saving UI state...";

    // The line between what is UI state and what is project state in an image
    // editor is a bit blurry. For example, guides are written by the project,
    // but could also be considered UI state. In general, we consider UI state
    // anything that is declared in QML and hence we do not and should not have
    // access to in C++. However, in the interest of having less code (we would
    // need to have access to the project's cached JSON data, which we're trying
    // to move away from (see restoreState())), we also consider ImageCanvas
    // state as UI state.

    mProject->uiState()->setValue("lastFillToolUsed",
        QMetaEnum::fromType<Tool>().valueToKey(mLastFillToolUsed));

    QJsonObject firstPaneJson;
    mFirstPane.write(firstPaneJson);
    mProject->uiState()->setValue("firstPane", firstPaneJson.toVariantMap());

    QJsonObject secondPaneJson;
    mSecondPane.write(secondPaneJson);
    mProject->uiState()->setValue("secondPane", secondPaneJson.toVariantMap());

    mProject->uiState()->setValue("rulersVisible", areRulersVisible());
    mProject->uiState()->setValue("guidesVisible", mGuidesVisible);
    mProject->uiState()->setValue("guidesLocked", mGuidesLocked);
    mProject->uiState()->setValue("notesVisible", mNotesVisible);
    mProject->uiState()->setValue("animationMarkersVisible", mAnimationMarkersVisible);
    mProject->uiState()->setValue("splitScreen", mSplitScreen);
    mProject->uiState()->setValue("splitterLocked", mSplitter.isEnabled());

    qCDebug(lcImageCanvasUiState) << "... saved UI state:\n" << mProject->uiState()->map();
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

        restoreState();

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

bool ImageCanvas::areRulersVisible() const
{
    return mRulersVisible;
}

void ImageCanvas::setRulersVisible(bool rulersVisible)
{
    if (rulersVisible == mRulersVisible)
        return;

    mRulersVisible = rulersVisible;
    emit rulersVisibleChanged();
}

bool ImageCanvas::areGuidesVisible() const
{
    return mGuidesVisible;
}

void ImageCanvas::setGuidesVisible(bool guidesVisible)
{
    qCDebug(lcImageCanvasGuides) << "setting guidesVisible to" << guidesVisible;
    if (guidesVisible == mGuidesVisible)
        return;

    mGuidesVisible = guidesVisible;
    emit guidesVisibleChanged();
}

bool ImageCanvas::areGuidesLocked() const
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

bool ImageCanvas::areNotesVisible() const
{
    return mNotesVisible;
}

void ImageCanvas::setNotesVisible(bool notesVisible)
{
    qCDebug(lcImageCanvasNotes) << "setting notesVisible to" << notesVisible;
    if (notesVisible == mNotesVisible)
        return;

    mNotesVisible = notesVisible;
    emit notesVisibleChanged();
}

bool ImageCanvas::areAnimationMarkersVisible() const
{
    return mAnimationMarkersVisible;
}

void ImageCanvas::setAnimationMarkersVisible(bool animationMarkersVisible)
{
    if (animationMarkersVisible == mAnimationMarkersVisible)
        return;

    mAnimationMarkersVisible = animationMarkersVisible;
    emit animationMarkersVisibleChanged();
}

int ImageCanvas::highlightedAnimationFrameIndex() const
{
    return mHighlightedAnimationFrameIndex;
}

void ImageCanvas::setHighlightedAnimationFrameIndex(int newIndex)
{
    if (mHighlightedAnimationFrameIndex == newIndex)
        return;

    mHighlightedAnimationFrameIndex = newIndex;
    emit highlightedAnimationFrameIndexChanged();
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
        emit lineChanged();
    emit cursorSceneXChanged();
    emit cursorScenePosChanged();
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
        emit lineChanged();
    emit cursorSceneYChanged();
    emit cursorScenePosChanged();
}

void ImageCanvas::setCursorScenePos(const QPoint &pos)
{
    const int oldCursorSceneX = mCursorSceneX;
    const int oldCursorSceneY = mCursorSceneY;

    setCursorSceneX(pos.x());
    setCursorSceneY(pos.y());

    if (oldCursorSceneX != mCursorSceneX || oldCursorSceneY != mCursorSceneY)
        emit cursorScenePosChanged();
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

    if (mTool == FillTool || mTool == TexturedFillTool)
        setLastFillToolUsed(mTool);

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

void ImageCanvas::setLastFillToolUsed(Tool lastFillToolUsed)
{
    Q_ASSERT(lastFillToolUsed == FillTool || lastFillToolUsed == TexturedFillTool);
    qCDebug(lcImageCanvas) << "setting lastFillToolUsed to" << lastFillToolUsed;
    if (mLastFillToolUsed == lastFillToolUsed)
        return;

    mLastFillToolUsed = lastFillToolUsed;
    emit lastFillToolUsedChanged();
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
    qCDebug(lcImageCanvasSelection) << "setSelectionArea called with" << selectionArea;

    QRect adjustedSelectionArea = selectionArea;
    if (!mMouseButtonPressed && selectionArea.size().isEmpty())
        adjustedSelectionArea = QRect();

    if (adjustedSelectionArea == mSelectionArea)
        return;

    mSelectionArea = adjustedSelectionArea;
    setHasSelection(!mSelectionArea.isEmpty());
    emit selectionAreaChanged();
}

bool ImageCanvas::isSelectionPanning() const
{
    return hasSelection() && (mCursorX < 0 || mCursorX > width() || mCursorY < 0 || mCursorY > height());
}

bool ImageCanvas::isAdjustingImage() const
{
    return !mSelectionContentsBeforeImageAdjustment.isNull();
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

    emit containsMouseChanged();
}

Qt::MouseButton ImageCanvas::mouseButtonPressed() const
{
    return mMouseButtonPressed;
}

void ImageCanvas::setMouseButtonPressed(Qt::MouseButton mouseButton)
{
    if (mouseButton == mMouseButtonPressed)
        return;

    mMouseButtonPressed = mouseButton;
    emit mouseButtonPressedChanged();
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

bool ImageCanvas::isScrollZoom() const
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

ImageCanvas::PenToolRightClickBehaviour ImageCanvas::penToolRightClickBehaviour() const
{
    return mRightClickBehaviour;
}

void ImageCanvas::setPenToolRightClickBehaviour(ImageCanvas::PenToolRightClickBehaviour rightClickBehaviour)
{
    if (rightClickBehaviour == mRightClickBehaviour)
        return;

    mRightClickBehaviour = rightClickBehaviour;
    emit penToolRightClickBehaviourChanged();
}

Ruler *ImageCanvas::pressedRuler() const
{
    return mPressedRuler;
}

int ImageCanvas::pressedGuideIndex() const
{
    return mPressedGuideIndex;
}

int ImageCanvas::pressedNoteIndex() const
{
    return mPressedNoteIndex;
}

QPoint ImageCanvas::notePressOffset() const
{
    return mNotePressOffset;
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

QPoint ImageCanvas::centredPaneOffset(int paneIndex) const
{
    const CanvasPane *pane = const_cast<ImageCanvas*>(this)->paneAt(paneIndex);
    const int paneXCentre = paneWidth(paneIndex) / 2;
    const int imageXCentre = (mProject->widthInPixels() * pane->integerZoomLevel()) / 2;
    const int paneYCentre = height() / 2;
    const int imageYCentre = (mProject->heightInPixels() * pane->integerZoomLevel()) / 2;
    return QPoint(paneXCentre - imageXCentre, paneYCentre - imageYCentre);
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
    // This check determines if a line should be rendered,
    // and also if the line info should be shown in the status bar.
    const Qt::MouseButton lastButtonPressed = mMouseButtonPressed == Qt::NoButton
        ? mLastMouseButtonPressed : mMouseButtonPressed;
    return mShiftPressed && mTool == PenTool && lastButtonPressed != Qt::NoButton;
}

int ImageCanvas::lineLength() const
{
    if (!isLineVisible())
        return 0;

    const QPointF point1 = mLastPixelPenPressScenePositionF.toPoint();
    const QPointF point2 = QPointF(mCursorSceneX, mCursorSceneY);
    const QLineF line(point1, point2);
    return line.length();
}

qreal ImageCanvas::lineAngle() const
{
    if (!isLineVisible())
        return 0;

    const QPointF point1 = mLastPixelPenPressScenePositionF.toPoint();
    const QPointF point2 = QPointF(mCursorSceneX, mCursorSceneY);
    const QLineF line(point1, point2);
    return line.angle();
}

QList<ImageCanvas::SubImage> ImageCanvas::subImagesInBounds(const QRect &bounds) const
{
    QList<SubImage> subImages;
    if (bounds.intersects(mProject->bounds())) {
        subImages.append(SubImage{mProject->bounds(), {}});
    }
    return subImages;
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

    if (mShiftPressed) {
        // Force the length and angle to be re-calculated.
        emit lineChanged();
    }

    requestContentPaint();
}

void ImageCanvas::connectSignals()
{
    qCDebug(lcImageCanvas) << "connecting signals for" << this << "as we have a new project" << mProject;

    connect(mProject, SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    connect(mProject, SIGNAL(projectCreated()), this, SLOT(requestContentPaint()));
    connect(mProject, SIGNAL(projectClosed()), this, SLOT(reset()));
    connect(mProject, SIGNAL(sizeChanged()), this, SLOT(requestContentPaint()));
    connect(mProject, SIGNAL(notesChanged()), this, SLOT(onNotesChanged()));
    // As a convenience for GuidesItem. Our guidesChanged signal is only emitted for changes in pane visibility,
    // offset, etc., but doesn't account for guides being added or removed. So to ensure that GuidesItem only
    // has to care about one object (us) and connect to one signal, forward the project's guidesChanged signal to ours.
    connect(mProject, SIGNAL(guidesChanged()), this, SIGNAL(guidesChanged()));
    connect(mProject, SIGNAL(preProjectSaved()), this, SLOT(saveState()));
    connect(mProject, SIGNAL(aboutToBeginMacro(QString)),
        this, SLOT(onAboutToBeginMacro(QString)));
    connect(mProject, SIGNAL(contentsModified()), this, SLOT(requestContentPaint()));

    connect(window(), SIGNAL(activeFocusItemChanged()), this, SLOT(updateWindowCursorShape()));
}

void ImageCanvas::disconnectSignals()
{
    qCDebug(lcImageCanvas) << "disconnecting signals for" << this;

    mProject->disconnect(SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    mProject->disconnect(SIGNAL(projectCreated()), this, SLOT(requestContentPaint()));
    mProject->disconnect(SIGNAL(projectClosed()), this, SLOT(reset()));
    mProject->disconnect(SIGNAL(sizeChanged()), this, SLOT(requestContentPaint()));
    mProject->disconnect(SIGNAL(notesChanged()), this, SLOT(onNotesChanged()));
    mProject->disconnect(SIGNAL(guidesChanged()), this, SIGNAL(guidesChanged()));
    mProject->disconnect(SIGNAL(preProjectSaved()), this, SLOT(saveState()));
    mProject->disconnect(SIGNAL(aboutToBeginMacro(QString)),
        this, SLOT(onAboutToBeginMacro(QString)));
    mProject->disconnect(SIGNAL(contentsModified()), this, SLOT(requestContentPaint()));

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

    findRulers();

    requestContentPaint();
}

void ImageCanvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);

    centrePanes();
    updateVisibleSceneArea();

    if (mProject)
        updateCursorPos(QPoint(mCursorX, mCursorY));
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
        // Draw the line on top of what has already been painted using a special composition mode.
        // This ensures that e.g. a translucent red overwrites whatever pixels it
        // lies on, rather than blending with them.
        drawLine(&linePainter, linePoint1(), linePoint2(), QPainter::CompositionMode_Source);
    }
    return image;
}

void ImageCanvas::drawLine(QPainter *painter, QPointF point1, QPointF point2, const QPainter::CompositionMode mode) const
{
    painter->save();

    QPen pen;
    pen.setColor(penColour());
    pen.setWidth(mToolSize);
    if (mToolShape == ToolShape::SquareToolShape) {
        pen.setCapStyle(Qt::PenCapStyle::SquareCap);
        pen.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    }
    else {
        pen.setCapStyle(Qt::PenCapStyle::RoundCap);
        pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
    }
    painter->setPen(pen);

    // Offset odd sized pens to pixel centre to centre pen
    const QPointF penOffset = (mToolSize % 2 == 1) ? QPointF(0.5, 0.5) : QPointF(0.0, 0.0);

    // Snap points to points to pixel grid
    if (mToolSize > 1) {
        point1 = (point1 + penOffset).toPoint() - penOffset;
        point2 = (point2 + penOffset).toPoint() - penOffset;
    }
    else {
        // Handle inconsitant width 1 pen behaviour, off pixel centres so results in non-ideal asymetrical lines but
        // would require either redrawing previous segment as part of stroke or custom line function to prevent spurs
        point1 = QPointF(qFloor(point1.x()), qFloor(point1.y()));
        point2 = QPointF(qFloor(point2.x()), qFloor(point2.y()));
    }

    const QLineF line(point1, point2);

    painter->setCompositionMode(mode);
    // Zero-length line doesn't draw with round pen so handle case with drawPoint
    if (line.p1() == line.p2()) {
        painter->drawPoint(line.p1());
    }
    else {
        painter->drawLine(line);
    }

    painter->restore();
}

void ImageCanvas::centrePanes(bool respectSceneCentred)
{
    if (!mProject)
        return;

    if (!respectSceneCentred || (respectSceneCentred && mFirstPane.isSceneCentered()))
        mFirstPane.setIntegerOffset(centredPaneOffset(0));

    if (!respectSceneCentred || (respectSceneCentred && mSecondPane.isSceneCentered()))
        mSecondPane.setIntegerOffset(centredPaneOffset(1));

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

void ImageCanvas::findRulers()
{
    if (!mRulers.isEmpty()) {
        qWarning() << "findRulers can only be called once";
        return;
    }

    const QList<Ruler*> rulers = QtUtils::findChildItems<Ruler*>(this);
    for (auto ruler : rulers)
        mRulers.append(QPointer<Ruler>(ruler));

    if (mRulers.isEmpty())
        qWarning() << "Failed to find rulers!";
}

void ImageCanvas::updatePressedRuler()
{
    mPressedRuler = !areGuidesLocked() ? rulerAtCursorPos() : nullptr;
}

Ruler *ImageCanvas::rulerAtCursorPos()
{
    if (!mRulersVisible)
        return nullptr;

    QPointF cursorPos = QPointF(mCursorX, mCursorY);
    Ruler *rulerAtCursor = nullptr;

    for (const auto ruler : qAsConst(mRulers)) {
        if (ruler->contains(mapToItem(ruler, cursorPos))) {
            rulerAtCursor = ruler;
            break;
        }
    }

    return rulerAtCursor;
}

void ImageCanvas::addNewGuide()
{
    QVector<Guide> guidesToAdd = {
        Guide(mPressedRuler->orientation() == Qt::Horizontal ? mCursorSceneY : mCursorSceneX, mPressedRuler->orientation())
    };
    const QVector<Guide> uniqueGuides = ProjectUtils::uniqueGuides(mProject, guidesToAdd);
    if (uniqueGuides.isEmpty()) {
        // They were all duplicates.
        return;
    }

    mProject->beginMacro(QLatin1String("AddGuide"));
    mProject->addChange(new AddGuidesCommand(mProject, uniqueGuides));
    mProject->endMacro();

    // The update for these guide commands happens via Project's guidesChanged signal.
}

void ImageCanvas::addNewGuides(int horizontalSpacing, int verticalSpacing)
{
    QVector<Guide> guides;
    ProjectUtils::addGuidesForSpacing(mProject, guides, horizontalSpacing, verticalSpacing);
    if (guides.isEmpty()) {
        // They were all duplicates.
        return;
    }

    mProject->beginMacro(QLatin1String("AddGuides"));
    mProject->addChange(new AddGuidesCommand(mProject, guides));
    mProject->endMacro();
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
    mProject->addChange(new DeleteGuidesCommand(mProject, { guide }));
    mProject->endMacro();
}

void ImageCanvas::removeAllGuides()
{
    mProject->beginMacro(QLatin1String("DeleteGuide"));
    mProject->addChange(new DeleteGuidesCommand(mProject, mProject->guides()));
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

void ImageCanvas::updatePressedNote()
{
    mPressedNoteIndex = noteIndexAtCursorPos();

    if (mPressedNoteIndex != -1) {
        mNotePositionBeforePress = mProject->notes().at(mPressedNoteIndex).position();
        mNotePressOffset = QPoint(mCursorSceneX, mCursorSceneY) - mNotePositionBeforePress;
    }
}

void ImageCanvas::updateNotesVisible()
{
    setNotesVisible(mProject && mProject->hasNotes());
}

int ImageCanvas::noteIndexAtCursorPos() const
{
    const QVector<Note> notes = mProject->notes();
    for (int i = 0; i < notes.size(); ++i) {
        const Note note = notes.at(i);
        // The note's size doesn't scale with the pane's zoom level; a note stays the same
        // size on screen as you zoom in, instead of getting bigger.
        const QRect noteGeometry(note.position(), note.size() / mCurrentPane->integerZoomLevel());
        if (noteGeometry.contains(mCursorSceneX, mCursorSceneY))
            return i;
    }

    return -1;
}

void ImageCanvas::onNotesChanged()
{
    qCDebug(lcImageCanvasNotes) << "onNotesChanged";
    updateNotesVisible();
    emit notesChanged();
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
    mCheckerImage = ImageUtils::filledImage(32, 32);

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

    QRectF newSelectionAreaF(mPressScenePositionF.x(), mPressScenePositionF.y(),
        mCursorSceneFX - mPressScenePositionF.x(), mCursorSceneFY - mPressScenePositionF.y());
    qCDebug(lcImageCanvasSelection) << "raw selection area:" << newSelectionAreaF;

    newSelectionAreaF = newSelectionAreaF.normalized();
    qCDebug(lcImageCanvasSelection) << "normalized selection area:" << newSelectionAreaF;

    QRect newSelectionArea = newSelectionAreaF.toRect();
    qCDebug(lcImageCanvasSelection) << "integer selection area:" << newSelectionArea;

    newSelectionArea = clampSelectionArea(newSelectionArea);
    qCDebug(lcImageCanvasSelection) << "final (clamped) selection area:" << newSelectionArea;

    setSelectionArea(newSelectionArea);
}

void ImageCanvas::updateSelectionPreviewImage(SelectionModification reason)
{
    qCDebug(lcImageCanvasSelectionPreviewImage) << "updating selection preview image due to" << reason;

    if (!mIsSelectionFromPaste) {
        // Only if the selection wasn't pasted should we erase the area left behind.
        mSelectionPreviewImage = ImageUtils::erasePortionOfImage(*currentProjectImage(), mSelectionAreaBeforeFirstModification);
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
    mSelectionPreviewImage = ImageUtils::paintImageOntoPortionOfImage(mSelectionPreviewImage, mSelectionArea, mSelectionContents);
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
    return ImageUtils::ensureWithinArea(selectionArea, mProject->size());
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
    if (mCursorX <= 0) {
        baseOffsetChange.rx() += qMin(qAbs(mCursorX), maxVelocity);
        panned = true;
    } else {
        // Check the right edge.
        const int distancePastRight = mCursorX - width() - 1;
        if (distancePastRight > 0) {
            baseOffsetChange.rx() += qMax(-distancePastRight, -maxVelocity);
            panned = true;
        }
    }

    // Check the top edge.
    if (mCursorY <= 0) {
        baseOffsetChange.ry() += qMin(qAbs(mCursorY), maxVelocity);
        panned = true;
    } else {
        // Check the bottom edge.
        const int distancePastBottom = mCursorY - height() - 1;
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
            || mLastSelectionModification == SelectionRotate
            || mLastSelectionModification == SelectionHsl) {
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
    mNotePositionBeforePress = QPoint();
    mNotePressOffset = QPoint();
    mPressedNoteIndex = -1;

    setCursorX(0);
    setCursorY(0);
    mCursorPaneX = 0;
    mCursorPaneY = 0;
    mCursorSceneX = 0;
    mCursorSceneY = 0;
    mContainsMouse = false;
    setMouseButtonPressed(Qt::NoButton);
    mLastMouseButtonPressed = Qt::NoButton;
    mPressPosition = QPoint(0, 0);
    mPressScenePosition = QPoint(0, 0);
    mPressScenePositionF = QPointF(0.0, 0.0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
    mFirstPaneVisibleSceneArea = QRect();
    mSecondPaneVisibleSceneArea = QRect();

    setPenForegroundColour(Qt::black);
    setPenBackgroundColour(Qt::white);

    mTexturedFillParameters.reset();

    mLastPixelPenPressScenePositionF = QPoint(0, 0);

    clearSelection();
    mLastCopiedSelectionArea = QRect();
    mLastCopiedSelectionContents = QImage();

    setAltPressed(false);
    mToolBeforeAltPressed = PenTool;
    mSpacePressed = false;
    mHasBlankCursor = false;

    // Things that we don't want to set, as they
    // don't really need to be reset each time:
    // - tool
    // - toolSize
    // TODO: ^ why?
    mToolsForbiddenReason.clear();

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

void ImageCanvas::copySelection()
{
    if (!mHasSelection)
        return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    const QImage copiedImage = currentProjectImage()->copy(mSelectionArea);
    clipboard->setImage(copiedImage);
    mLastCopiedSelectionArea = mSelectionArea;
    mLastCopiedSelectionContents = copiedImage;
}

void ImageCanvas::paste()
{
    qCDebug(lcImageCanvasSelection) << "pasting selection from clipboard";

    QClipboard *clipboard = QGuiApplication::clipboard();
    QImage clipboardImage = clipboard->image();
    if (clipboardImage.isNull()) {
        qCDebug(lcImageCanvasSelection) << "Clipboard content is not an image; can't paste";
        return;
    }

    // If the user copied a section of the canvas and then the selection was lost (e.g. due to changing layers)
    // then we want to paste that image into the same area.
    // This is just an educated guess - it is possible that the user copied an identical image
    // from outside the application and pasted it in, but... it's not a big deal.
    const bool wasLastCopiedImageFromCanvas = clipboardImage == mLastCopiedSelectionContents;
    // If the user didn't copy from the canvas, just use an empty rect so that it falls back to using
    // the image's size at the top left of the canvas.
    QRect pastedArea = wasLastCopiedImageFromCanvas ? mLastCopiedSelectionArea : QRect();
    const bool fromExternalSource = !mHasSelection && !wasLastCopiedImageFromCanvas;

    clearOrConfirmSelection();

    setTool(SelectionTool);

    const QSize adjustedSize(qMin(clipboardImage.width(), mProject->widthInPixels()),
        qMin(clipboardImage.height(), mProject->heightInPixels()));
    if (fromExternalSource) {
        // If the paste was copied from an external source, we just paste it at 0, 0.
        pastedArea = QRect(0, 0, adjustedSize.width(), adjustedSize.height());
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
        mSelectionContents = ImageUtils::rotateAreaWithinImage(image, mSelectionArea, angle, rotatedArea);
    } else {
        QImage image = ImageUtils::filledImage(mProject->size());
        QPainter painter(&image);
        painter.drawImage(mSelectionArea, mSelectionContents);
        mSelectionContents = ImageUtils::rotateAreaWithinImage(image, mSelectionArea, angle, rotatedArea);
    }

    Q_ASSERT(mHasSelection);
    setSelectionArea(rotatedArea);
    mLastValidSelectionArea = mSelectionArea;

    setLastSelectionModification(SelectionRotate);

    updateSelectionPreviewImage(SelectionRotate);
    requestContentPaint();
}

// How we do HSL modifications:
//
// We apply the HSL modification directly on the selection contents,
// just as we do for e.g. rotation. This is easier than having a separate,
// intermediate preview pane.
//
// Why these begin/end functions exist:
//
// If we only had modifySelectionHsl(), each cumulative HSL modification
// would be applied to the result of the last. The visible result of this is
// that it's not possible to move the saturation slider back and forth
// without losing detail in the image, for example.
// By keeping track of when the HSL modifications begin and end, we can
// ensure that all modifications are done on the original image.
//
// Also, no other modifications to the canvas are possible while
// the Hue/Saturation dialog is open, so we don't have to worry about conflict there.
void ImageCanvas::beginModifyingSelectionHsl()
{
    if (!mHasSelection) {
        qWarning() << "Can't modify HSL without a selection";
        return;
    }

    if (!mSelectionContentsBeforeImageAdjustment.isNull()) {
        qWarning() << "Already modifying selection's HSL";
        return;
    }

    qCDebug(lcImageCanvasSelection) << "beginning modification of selection's HSL";

    if (mSelectionAreaBeforeFirstModification.isNull()) {
        mSelectionAreaBeforeFirstModification = mSelectionArea;
        mSelectionContents = currentProjectImage()->copy(mSelectionAreaBeforeFirstModification);
    }

    // Store the original selection contents before we start modifying it
    // so that each modification is done on a copy of the contents instead of the original contents.
    mSelectionContentsBeforeImageAdjustment = mSelectionContents;
    mLastSelectionModificationBeforeImageAdjustment = mLastSelectionModification;
    emit adjustingImageChanged();
}

void ImageCanvas::modifySelectionHsl(qreal hue, qreal saturation, qreal lightness, qreal alpha,
    AlphaAdjustmentFlags alphaAdjustmentFlags)
{
    if (!isAdjustingImage()) {
        qWarning() << "Not adjusting an image; can't modify selection's HSL";
        return;
    }

    qCDebug(lcImageCanvasSelection).nospace() << "modifying HSL of selection"
        << mSelectionArea << " with h=" << hue << " s=" << saturation << " l=" << lightness << " a=" << alpha
        << "alpha flags=" << alphaAdjustmentFlags;

    // Copy the original so we don't just modify the result of the last adjustment (if any).
    mSelectionContents = mSelectionContentsBeforeImageAdjustment;

    ImageUtils::modifyHsl(mSelectionContents, hue, saturation, lightness, alpha, alphaAdjustmentFlags);

    // Set this so that the check in shouldDrawSelectionPreviewImage() evaluates to true.
    setLastSelectionModification(SelectionHsl);

    updateSelectionPreviewImage(SelectionHsl);
    requestContentPaint();
}

void ImageCanvas::endModifyingSelectionHsl(AdjustmentAction adjustmentAction)
{
    qCDebug(lcImageCanvasSelection) << "ended modification of selection's HSL";

    if (adjustmentAction == RollbackAdjustment) {
        mSelectionContents = mSelectionContentsBeforeImageAdjustment;
        setLastSelectionModification(mLastSelectionModificationBeforeImageAdjustment);
        updateSelectionPreviewImage(SelectionHsl);
        requestContentPaint();
    } else {
        // Commit the adjustments. We don't need to request a repaint
        // since nothing has changed since the last one.
        setLastSelectionModification(SelectionHsl);
    }

    mSelectionContentsBeforeImageAdjustment = QImage();
    emit adjustingImageChanged();
}

void ImageCanvas::addNote(const QPoint &newPosition, const QString &newText)
{
    const Note newNote(newPosition, newText);
    if (mProject->notes().contains(newNote)) {
        error(tr("Note at position %1, %2 with text \"%3\" already exists")
            .arg(QString::number(newPosition.x())).arg(QString::number(newPosition.y())).arg(newText));
        return;
    }

    mProject->beginMacro(QLatin1String("AddNote"));
    mProject->addChange(new AddNoteCommand(mProject, newNote));
    mProject->endMacro();

    // The update for these note commands happens in onNotesChanged.
}

void ImageCanvas::modifyNote(int noteIndex, const QPoint &newPosition, const QString &newText)
{
    if (!mProject->isValidNoteIndex(noteIndex)) {
        // Should not be possible for the user to trigger this, so it's not a user-facing error.
        qWarning() << "Cannot modify note at index" << noteIndex << "as it's not a valid index";
        return;
    }

    Note oldNote = mProject->notes().at(noteIndex);
    if (newPosition == oldNote.position() && newText == oldNote.text()) {
        // Nothing changed.
        return;
    }

    const Note newNote(newPosition, newText);
    mProject->beginMacro(QLatin1String("ChangeNote"));
    mProject->addChange(new ChangeNoteCommand(mProject, noteIndex, oldNote, newNote));
    mProject->endMacro();
}

void ImageCanvas::removeNote(int noteIndex)
{
    if (!mProject->isValidNoteIndex(noteIndex)) {
        // Should not be possible for the user to trigger this, so it's not a user-facing error.
        qWarning() << "Cannot remove note at index" << noteIndex << "as it's not a valid index";
        return;
    }

    const Note note = mProject->notes().at(noteIndex);
    mProject->beginMacro(QLatin1String("DeleteNote"));
    mProject->addChange(new DeleteNoteCommand(mProject, note));
    mProject->endMacro();
}

void ImageCanvas::addSelectedColoursToTexturedFillSwatch()
{
    if (!mHasSelection)
        return;

    // Since this operation is blocking, we have to make it quite limited in the size of selections it allows.
    static const int maxSelectionSize = 256;
    if (mSelectionArea.width() * mSelectionArea.height() > maxSelectionSize * maxSelectionSize) {
        error(tr("Too many pixels selected: %1x%2 exceeds limit of %3/%4.")
            .arg(mSelectionArea.width()).arg(mSelectionArea.height()).arg(maxSelectionSize).arg(maxSelectionSize));
        return;
    }

    QVector<QColor> uniqueColours;
    QVector<qreal> probabilities;
    static const int maxUniqueColours = 100;
    // If the user only selected an area without doing modifications like moving or rotating,
    // then mSelectionContents will be invalid. I can't remember if that's by design or not,
    // so just get the contents manually here.
    const QImage selectionContents = currentProjectImage()->copy(mSelectionArea);
    const ImageUtils::FindUniqueColoursResult result = ImageUtils::findUniqueColoursAndProbabilities(
        selectionContents, maxUniqueColours, uniqueColours, probabilities);
    if (result == ImageUtils::MaximumUniqueColoursExceeded) {
        emit errorOccurred(tr("Too many unique colours selected: the maximum is %1 colours.")
            .arg(maxUniqueColours));
        return;
    }

    qCDebug(lcImageCanvas) << "adding" << uniqueColours << "unique colours from selection to textured fill swatch";
    mTexturedFillParameters.swatch()->addColoursWithProbabilities(uniqueColours, probabilities);
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
    return imagePixelFloodFill(currentProjectImage(), scenePos, previousColour, penColour());
}

QImage ImageCanvas::greedyFillPixels() const
{
    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos))
        return QImage();

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    return imageGreedyPixelFill(currentProjectImage(), scenePos, previousColour, penColour());
}

QImage ImageCanvas::texturedFillPixels() const
{
    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos))
        return QImage();

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    return texturedFill(currentProjectImage(), scenePos, previousColour, penColour(), mTexturedFillParameters);
}

QImage ImageCanvas::greedyTexturedFillPixels() const
{
    const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
    if (!isWithinImage(scenePos))
        return QImage();

    const QColor previousColour = currentProjectImage()->pixelColor(scenePos);
    return greedyTexturedFill(currentProjectImage(), scenePos, previousColour, penColour(), mTexturedFillParameters);
}

ImageCanvas::Tool ImageCanvas::effectiveTool() const
{
    if (mMouseButtonPressed != Qt::RightButton)
        return mTool;

    return mTool == PenTool ? penRightClickTool() : mTool;
}

ImageCanvas::Tool ImageCanvas::penRightClickTool() const
{
    if (mRightClickBehaviour == PenToolRightClickAppliesEraser)
        return EraserTool;

    if (mRightClickBehaviour == PenToolRightClickAppliesEyeDropper)
        return EyeDropperTool;

    return PenTool;
}

void ImageCanvas::applyCurrentTool()
{
    updateToolsForbidden();
    if (areToolsForbidden())
        return;

    switch (effectiveTool()) {
    case PenTool: {
        mProject->beginMacro(QLatin1String("PixelLineTool"));
        // Draw the line on top of what has already been painted using a special composition mode.
        // This ensures that e.g. a translucent red overwrites whatever pixels it
        // lies on, rather than blending with them.
        mProject->addChange(new ApplyPixelLineCommand(this, mProject->currentLayerIndex(), *currentProjectImage(), linePoint1(), linePoint2(),
            mPressScenePositionF, mLastPixelPenPressScenePositionF, QPainter::CompositionMode_Source));
        break;
    }
    case EyeDropperTool: {
        const QPoint scenePos = QPoint(mCursorSceneX, mCursorSceneY);
        if (isWithinImage(scenePos)) {
            setPenColourThroughEyedropper(currentProjectImage()->pixelColor(scenePos));
        }
        break;
    }
    case EraserTool: {
        mProject->beginMacro(QLatin1String("PixelEraserTool"));
        // Draw the line on top of what has already been painted using a special composition mode to erase pixels.
        mProject->addChange(new ApplyPixelLineCommand(this, mProject->currentLayerIndex(), *currentProjectImage(), linePoint1(), linePoint2(),
            mPressScenePositionF, mLastPixelPenPressScenePositionF, QPainter::CompositionMode_Clear));
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
             mProject->endMacro();
        } else {
            const QImage filledImage = greedyFillPixels();
            if (filledImage.isNull())
                return;

            mProject->beginMacro(QLatin1String("GreedyPixelFillTool"));
            mProject->addChange(new ApplyGreedyPixelFillCommand(this, mProject->currentLayerIndex(),
                *currentProjectImage(), filledImage));
             mProject->endMacro();
        }
        break;
    }
    case TexturedFillTool: {
        if (mTexturedFillParameters.type() == TexturedFillParameters::SwatchFillType
                && !mTexturedFillParameters.swatch()->hasNonZeroProbabilitySum()) {
            error(tr("Cannot use textured swatch fill without swatch colours or at least one non-zero swatch colour probability"));
            return;
        }

        if (!mShiftPressed) {
            const QImage filledImage = texturedFillPixels();
            if (filledImage.isNull())
                return;

            mProject->beginMacro(QLatin1String("PixelTexturedFillTool"));
            mProject->addChange(new ApplyPixelFillCommand(this, mProject->currentLayerIndex(),
                *currentProjectImage(), filledImage));
             mProject->endMacro();
        } else {
            const QImage filledImage = greedyTexturedFillPixels();
            if (filledImage.isNull())
                return;

            mProject->beginMacro(QLatin1String("GreedyPixelTexturedFillTool"));
            mProject->addChange(new ApplyGreedyPixelFillCommand(this, mProject->currentLayerIndex(),
                *currentProjectImage(), filledImage));
             mProject->endMacro();
        }
        break;
    }
    case NoteTool: {
        if (mPressedNoteIndex == -1) {
            if (isCursorWithinProjectBounds()) {
                emit noteCreationRequested();
            } else {
                error(tr("Notes must be within the image boundaries."));
                return;
            }
        }
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
        mLastPixelPenPressScenePositionF = scenePos;
    requestContentPaint();
}

void ImageCanvas::applyPixelLineTool(int layerIndex, const QImage &lineImage, const QRect &lineRect,
    const QPointF &lastPixelPenReleaseScenePosition)
{
    mLastPixelPenPressScenePositionF = lastPixelPenReleaseScenePosition;
    QPainter painter(imageForLayerAt(layerIndex));
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(lineRect, lineImage);
    requestContentPaint();
}

void ImageCanvas::paintImageOntoPortionOfImage(int layerIndex, const QRect &portion, const QImage &replacementImage)
{
    QImage *image = imageForLayerAt(layerIndex);
    *image = ImageUtils::paintImageOntoPortionOfImage(*image, portion, replacementImage);
    requestContentPaint();
}

void ImageCanvas::replacePortionOfImage(int layerIndex, const QRect &portion, const QImage &replacementImage)
{
    QImage *image = imageForLayerAt(layerIndex);
    *image = ImageUtils::replacePortionOfImage(*image, portion, replacementImage);
    requestContentPaint();
}

void ImageCanvas::erasePortionOfImage(int layerIndex, const QRect &portion)
{
    QImage *image = imageForLayerAt(layerIndex);
    *image = ImageUtils::erasePortionOfImage(*image, portion);
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
    *image = ImageUtils::rotateAreaWithinImage(*image, area, angle, rotatedArea);
    // Only update the selection area when the commands are being created for the first time,
    // not when they're being undone and redone.
    if (mHasSelection)
        setSelectionArea(rotatedArea);
    requestContentPaint();
    return area.united(rotatedArea);
}

QPointF ImageCanvas::linePoint1() const
{
    return mLastPixelPenPressScenePositionF;
}

QPointF ImageCanvas::linePoint2() const
{
    return QPointF(mCursorSceneFX, mCursorSceneFY);
}

QRect ImageCanvas::normalisedLineRect(const QPointF &point1, const QPointF &point2) const
{
    // sqrt(2) is the ratio between the hypotenuse of a square and its side;
    // a simplification of Pythagoras’ theorem.
    // The bounds could be tighter by taking into account the specific rotation of the brush,
    // but the sqrt(2) ensures it is big enough for any rotation.
    const int margin = qCeil(M_SQRT2 * mToolSize / 2.0) + 1;
    return QRect(point1.toPoint(), point2.toPoint()).normalized()
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
        setCursorScenePos(QPoint(-1, -1));
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
    setCursorScenePos(QPoint(mCursorSceneFX, mCursorSceneFY));

    if (!isCursorWithinProjectBounds()
        // The cached contents can be null for some reason during tests; probably because
        // the canvas gets events before it has rendered. No big deal, it wouldn't be visible yet anyway.
        || mCachedContentImage.isNull()) {
        setCursorPixelColour(QColor(Qt::black));
    } else {
        const QPoint cursorScenePos = QPoint(mCursorSceneX, mCursorSceneY);
        setCursorPixelColour(mCachedContentImage.pixelColor(cursorScenePos));
    }

    qCDebug(lcImageCanvasCursorPos) << "mCursorX" << mCursorX << "mCursorY" << mCursorY
        << "mCursorSceneX" << mCursorSceneX << "mCursorSceneY" << mCursorSceneY;
}

bool ImageCanvas::isCursorWithinProjectBounds() const
{
    const QRect projectBounds(0, 0, mProject->widthInPixels(), mProject->heightInPixels());
    return projectBounds.contains(QPoint(mCursorSceneX, mCursorSceneY));
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
    qCDebug(lcImageCanvas) << "onLoadedChanged - mProject->hasLoaded():" << mProject->hasLoaded();

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
    // Note that this function can be called for drawing _and_ e.g. panning, zooming, etc.
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

    const bool overRuler = rulerAtCursorPos() != nullptr;

    // Do this check first since notes should go above guides.
    bool overNote = false;
    if (mTool == NoteTool && areNotesVisible() && !overRuler)
        overNote = noteIndexAtCursorPos() != -1;

    bool overGuide = false;
    if (areGuidesVisible() && !areGuidesLocked() && !overRuler && !overNote)
        overGuide = guideIndexAtCursorPos() != -1;

    updateToolsForbidden();

    // Hide the window's cursor when we're in the spotlight; otherwise, use the non-custom arrow cursor.
    const bool nothingOverUs = mProject->hasLoaded() && hasActiveFocus() /*&& !mModalPopupsOpen*/ && mContainsMouse;
    const bool splitterHovered = mSplitter.isEnabled() && mSplitter.isHovered();
    const bool overSelection = cursorOverSelection();
    const bool toolsForbidden = areToolsForbidden();
    setHasBlankCursor(nothingOverUs && !isPanning() && !splitterHovered && (!overSelection || (overSelection && mPotentiallySelecting))
        && !overNote && !overRuler && !overGuide && !toolsForbidden);

    Qt::CursorShape cursorShape = Qt::BlankCursor;
    if (!mHasBlankCursor) {
        if (isPanning()) {
            // If panning while space is pressed, the left mouse button is used, otherwise it's the middle mouse button.
            cursorShape = (mMouseButtonPressed == Qt::LeftButton || mMouseButtonPressed == Qt::MiddleButton) ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
        } else if (overNote || overGuide) {
            cursorShape = mPressedGuideIndex != -1 || mPressedNoteIndex != -1 ? Qt::ClosedHandCursor : Qt::OpenHandCursor;
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
        qCDebug(lcImageCanvasCursorShape).nospace()
            << "Updating window cursor shape for " << objectName() << " ..."
            << "\n... mProject->hasLoaded() " << mProject->hasLoaded()
            << "\n........ hasActiveFocus() " << hasActiveFocus()
            << "\n.......... mContainsMouse " << mContainsMouse
            << "\n....... mPressedNoteIndex " << mPressedNoteIndex
            << "\n...... mPressedGuideIndex " << mPressedGuideIndex
            << "\n......... mHasBlankCursor " << mHasBlankCursor
            << " (nothingOverUs=" << nothingOverUs
            << " !isPanning()=" << !isPanning()
            << " !splitterHovered=" << !splitterHovered
            << " !overSelection=" << !overSelection
            << " !overNote=" << !overNote
            << " !overRuler=" << !overRuler
            << " !overGuide=" << !overGuide
            << " !toolsForbidden=" << !toolsForbidden << ")"
            << "\n............ cursor shape " << QtUtils::enumToString(cursorShape);
    }

    if (window()) {
        window()->setCursor(QCursor(cursorShape));
    }
}

void ImageCanvas::onZoomLevelChanged()
{
    updateVisibleSceneArea();

    requestContentPaint();

    if (mGuidesVisible)
        emit guidesChanged();
}

void ImageCanvas::onPaneIntegerOffsetChanged()
{
    updateVisibleSceneArea();

    if (mGuidesVisible)
        emit guidesChanged();
}

void ImageCanvas::onPaneSizeChanged()
{
    updateVisibleSceneArea();

    if (mGuidesVisible)
        emit guidesChanged();
}

void ImageCanvas::error(const QString &message)
{
//    qWarning() << Q_FUNC_INFO << message;
    emit errorOccurred(message);
}

Qt::MouseButton ImageCanvas::effectivePressedMouseButton() const
{
    // For some tools, like the line tool, the mouse button won't be pressed at times,
    // so we take the last mouse button that was pressed.
    return mMouseButtonPressed == Qt::NoButton ? mLastMouseButtonPressed : mMouseButtonPressed;
}

QColor ImageCanvas::penColour() const
{
    // When using the right button we want to use the background colour.
    // For every other mouse button, use the foreground. This affects e.g. the line preview too.
    return effectivePressedMouseButton() == Qt::RightButton ? mPenBackgroundColour : mPenForegroundColour;
}

void ImageCanvas::setPenColourThroughEyedropper(const QColor &colour)
{
    const Qt::MouseButton pressedButton = effectivePressedMouseButton();
    const bool setForeground = pressedButton == Qt::LeftButton ||
        (pressedButton == Qt::RightButton && mRightClickBehaviour == PenToolRightClickAppliesEyeDropper);
    if (setForeground)
        setPenForegroundColour(colour);
    else
        setPenBackgroundColour(colour);
}

void ImageCanvas::setHasBlankCursor(bool hasBlankCursor)
{
    if (hasBlankCursor == mHasBlankCursor)
        return;

    mHasBlankCursor = hasBlankCursor;
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
    return !mToolsForbiddenReason.isEmpty();
}

QString ImageCanvas::toolsForbiddenReason() const
{
    return mToolsForbiddenReason;
}

void ImageCanvas::updateToolsForbidden()
{
    // See: https://github.com/mitchcurtis/slate/issues/2.
    // QPainter doesn't support drawing into an QImage whose format is QImage::Format_Indexed8.
    static const QString index8Reason =
        tr("Image cannot be edited because its format is indexed 8-bit, which does not support modification.");
    const bool is8Bit = mImageProject && mImageProject->image()->format() == QImage::Format_Indexed8;
    setToolsForbiddenReason(is8Bit ? index8Reason : QString());
}

void ImageCanvas::setToolsForbiddenReason(const QString &reason)
{
    if (reason == mToolsForbiddenReason)
        return;

    mToolsForbiddenReason = reason;
    emit toolsForbiddenChanged();
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

void ImageCanvas::applyZoom(qreal newZoomLevel, const QPoint &origin)
{
    const qreal oldZoomLevel = qreal(mCurrentPane->integerZoomLevel());

    mCurrentPane->setZoomLevel(newZoomLevel);

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
    qCDebug(lcImageCanvasEvents) << "wheelEvent:" << event;

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
        qCDebug(lcImageCanvasEvents) << "mousePressEvent -" << event
            << "mCursorSceneFX:" << mCursorSceneFX << "mCursorSceneFY:" << mCursorSceneFY;
        return;
    }

    event->accept();

    setMouseButtonPressed(event->button());
    mLastMouseButtonPressed = mMouseButtonPressed;
    mPressPosition = event->pos();
    mPressScenePosition = QPoint(mCursorSceneX, mCursorSceneY);
    mPressScenePositionF = QPointF(mCursorSceneFX, mCursorSceneFY);
    mCurrentPaneOffsetBeforePress = mCurrentPane->integerOffset();
    setContainsMouse(true);

    qCDebug(lcImageCanvasEvents) << "mousePressEvent -" << event
        << "mPressPosition:" << mPressPosition
        << "mPressScenePosition:" << mPressScenePosition
        << "mCursorSceneFX:" << mCursorSceneFX << "mCursorSceneFY:" << mCursorSceneFY;

    if (!isPanning()) {
        if (mSplitter.isEnabled() && mouseOverSplitterHandle(event->pos())) {
            mSplitter.setPressed(true);
            return;
        }

        if (areRulersVisible() && areGuidesVisible()) {
            updatePressedRuler();
            if (mPressedRuler)
                return;

            if (!areGuidesLocked()) {
                updatePressedGuide();
                if (mPressedGuideIndex != -1) {
                    // A guide was just pressed.
                    updateWindowCursorShape();
                    return;
                }
            }
        }

        if (mNotesVisible && mTool == NoteTool) {
            updatePressedNote();
            if (mPressedNoteIndex != -1) {
                qCDebug(lcImageCanvasEvents) << "the note at index" << mPressedNoteIndex
                    << "was pressed - mNotePressOffset:" << mNotePressOffset;
                updateWindowCursorShape();
                return;
            }
        }

        if (!mShiftPressed && (mTool == PenTool || mTool == EraserTool)) {
            mLastPixelPenPressScenePositionF = mPressScenePositionF;
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
    qCDebug(lcImageCanvasEvents) << "mouseMoveEvent -" << event;
    QQuickItem::mouseMoveEvent(event);

    const QPointF oldCursorScenePosition = QPointF(mCursorSceneFX, mCursorSceneFY);
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
            emit guidesChanged();
        } else if (mPressedGuideIndex != -1) {
            emit guidesChanged();
        } else if (mPressedNoteIndex != -1) {
            emit notesChanged();
        } else {
            if (!isPanning()) {
                if (mTool != SelectionTool) {
                    mPressScenePosition = QPoint(mCursorSceneX, mCursorSceneY);
                    mPressScenePositionF = QPointF(mCursorSceneFX, mCursorSceneFY);
                    if (!mShiftPressed) {
                        mLastPixelPenPressScenePositionF = oldCursorScenePosition;
                    }
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
    qCDebug(lcImageCanvasEvents) << "mouseReleaseEvent -" << event
         << "mCursorX:" << mCursorX << "mCursorY:" << mCursorY
         << "mCursorSceneFX:" << mCursorSceneFX << "mCursorSceneFY:" << mCursorSceneFY;
    QQuickItem::mouseReleaseEvent(event);

    updateCursorPos(event->pos());

    if (!mProject->hasLoaded())
        return;

    setMouseButtonPressed(Qt::NoButton);

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
            // The selection is being moved.

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
        // A guide was pressed.
        if (hoveredRuler) {
            if (hoveredRuler->orientation() == mProject->guides().at(mPressedGuideIndex).orientation()) {
                // A ruler wasn't pressed but a guide is, and now a ruler is hovered;
                // the user has dragged a guide onto a ruler with the correct orientation, so remove it.
                removeGuide();
            }
        } else {
            // No ruler was hovered, which means we were moving a guide.
            moveGuide();
        }

        mPressedGuideIndex = -1;
        updateWindowCursorShape();
    } else if (mPressedNoteIndex != -1) {
        if (event->button() == Qt::LeftButton) {
            // A note was dragged (regardless of whether it actually changed position).
            qCDebug(lcImageCanvasNotes) << "the note at index" << mPressedNoteIndex << "was dragged -"
                << "mCursorSceneFX:" << mCursorSceneFX << "mCursorSceneFY:" << mCursorSceneFY
                << "mNotePositionBeforePress:" << mNotePositionBeforePress
                << "mPressPosition:" << mPressPosition << "mPressScenePosition:" << mPressScenePosition
                << "mNotePressOffset:" << mNotePressOffset;

            const Note oldNote = mProject->notes().at(mPressedNoteIndex);
            const QString oldText = oldNote.text();
            const QPoint newPosition = QPoint(mCursorSceneX, mCursorSceneY) - mNotePressOffset;
            modifyNote(mPressedNoteIndex, newPosition, oldText);
        } else if (event->button() == Qt::RightButton) {
            emit noteContextMenuRequested(mPressedNoteIndex);
        }

        mPressedNoteIndex = -1;
        updateWindowCursorShape();
    }

    mPressPosition = QPoint(0, 0);
    mPressScenePosition = QPoint(0, 0);
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
    updateWindowCursorShape();
    mSplitter.setPressed(false);
    mPressedRuler = nullptr;
    mGuidePositionBeforePress = 0;
    mNotePositionBeforePress = QPoint(0, 0);
    mNotePressOffset = QPoint(0, 0);
}

void ImageCanvas::hoverEnterEvent(QHoverEvent *event)
{
    qCDebug(lcImageCanvasHoverEvents) << "hoverEnterEvent:" << event;
    QQuickItem::hoverEnterEvent(event);

    updateCursorPos(event->pos());

    setContainsMouse(true);

    if (!mProject->hasLoaded())
        return;
}

void ImageCanvas::hoverMoveEvent(QHoverEvent *event)
{
    qCDebug(lcImageCanvasHoverEvents) << "hoverMoveEvent:" << event->posF();
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
    qCDebug(lcImageCanvasHoverEvents) << "hoverLeaveEvent:" << event;
    QQuickItem::hoverLeaveEvent(event);

    setContainsMouse(false);

    if (!mProject->hasLoaded())
        return;
}

void ImageCanvas::keyPressEvent(QKeyEvent *event)
{
    qCDebug(lcImageCanvasEvents) << "keyPressEvent:" << event;
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

    if (event->key() >= Qt::Key_1 && event->key() <= Qt::Key_6) {
        const int zeroBaseNumberPressed = event->key() - Qt::Key_1;
        Tool activatedTool = static_cast<ImageCanvas::Tool>(zeroBaseNumberPressed);
        if (activatedTool == TexturedFillTool) {
            // This is a bit hacky, but the TexturedFillTool can't be activated directly
            // with the number keys (it has to be cycled with the key used for the fill tool),
            // so we have to skip it here to ensure tools after it are activated correctly.
            activatedTool = static_cast<ImageCanvas::Tool>(zeroBaseNumberPressed + 1);
        }

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
    qCDebug(lcImageCanvasEvents) << "keyReleaseEvent:" << event;
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
    qCDebug(lcImageCanvasFocusEvents) << "focusInEvent:" << event;
    QQuickItem::focusInEvent(event);

    // When alt-tabbing away from and back to our window, we'd previously
    // just call updateWindowCursorShape(). However, that alone isn't enough,
    // as hoverLeaveEvent() calls setContainsMouse(false), which gets called
    // when when the window loses focus.
    // So, we check the position of the mouse manually and set it ourselves
    // when the window regains focus.
    setContainsMouse(contains(mapFromGlobal(QCursor::pos())));
}

void ImageCanvas::focusOutEvent(QFocusEvent *event)
{
    qCDebug(lcImageCanvasFocusEvents) << "focusOutEvent:" << event;
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
    qCDebug(lcImageCanvasUndo) << "about to undo";

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
        /*
            This is hacky, but I can't find another way to do it.
            The problem is tested by undoAfterMovedPaste() - for reasons I can't remember,
            we create a ModifyImageCanvasSelectionCommand after a PasteImageCanvasCommand
            when e.g. moving a pasted image. When undoing, we accordingly need to undo
            both commands.

            A better way of doing this would be to make the ModifyImageCanvasSelectionCommand
            a child of the PasteImageCanvasCommand, but since it's possible to have a paste
            without e.g. a move afterwards, we can't know whether or not we should parent it
            at the time of its creation. It's also not possible to modify commands that are in
            the stack (see QUndoStack::command() function docs).

            I would prefer to get the last two commands and dynamic_cast them, but since we're
            using macros (again, I can't remember why, but stuff breaks without it), the commands
            that are returned are plain QUndoCommand objects, so we lose access to the
            actual commands.
        */
        auto undoStack = mProject->undoStack();
        const int count = undoStack->count();
        if (count >= 2) {
            const QString lastText = undoStack->text(count - 1);
            const QString secondLastText = undoStack->text(count - 2);
            if (lastText == "ModifySelection" && secondLastText == "PasteCommand")
                undoStack->undo();
        }

        undoStack->undo();
    }
}
