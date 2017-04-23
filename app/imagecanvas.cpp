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

#include <QCursor>
#include <QImage>
#include <QPainter>
#include <QQuickWindow>
#include <QtMath>

#include "applypixelerasercommand.h"
#include "applypixelfillcommand.h"
#include "applypixelpencommand.h"
#include "floodfill.h"
#include "imageproject.h"
#include "project.h"
#include "tileset.h"

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
    mTool(PenTool),
    mToolSize(1),
    mMaxToolSize(100),
    mPenForegroundColour(Qt::black),
    mPenBackgroundColour(Qt::white),
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

QPoint ImageCanvas::clampToImageBounds(const QPoint &scenePos) const
{
    return QPoint(qBound(0, scenePos.x(), mImageProject->image()->width() - 1),
                  qBound(0, scenePos.y(), mImageProject->image()->height() - 1));
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

bool ImageCanvas::areModalPopupsOpen() const
{
    return mModalPopupsOpen;
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

    connect(window(), SIGNAL(activeFocusItemChanged()), this, SLOT(checkIfModalPopupsOpen()));

    centrePanes();
}

void ImageCanvas::disconnectSignals()
{
    mProject->disconnect(SIGNAL(loadedChanged()), this, SLOT(onLoadedChanged()));
    mProject->disconnect(SIGNAL(projectCreated()), this, SLOT(update()));
    mProject->disconnect(SIGNAL(projectClosed()), this, SLOT(reset()));
    mProject->disconnect(SIGNAL(sizeChanged()), this, SLOT(update()));

    window()->disconnect(SIGNAL(activeFocusItemChanged()), this, SLOT(checkIfModalPopupsOpen()));
}

void ImageCanvas::toolChange()
{
}

bool ImageCanvas::hasBlankCursor() const
{
    return mHasBlankCursor;
}

void ImageCanvas::checkIfModalPopupsOpen()
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
    painter->fillRect(firstPaneWidth, 0, width() - firstPaneWidth, height(), mBackgroundColour);
    drawPane(painter, mSecondPane, 1);

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

    const QImage image = *mImageProject->image();
    painter->drawImage(QRectF(QPointF(0, 0), pane.zoomedSize(image.size())), image, QRectF(0, 0, image.width(), image.height()));

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
    mCurrentPaneOffsetBeforePress = QPoint(0, 0);
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
    for (; scenePos.y() <= bottomRight.y(); ++scenePos.ry()) {
        for (scenePos.rx() = topLeft.x(); scenePos.x() <= bottomRight.x(); ++scenePos.rx()) {
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

    QVector<QPoint> scenePositions;
    imagePixelFloodFill(mImageProject->image(), scenePos, previousColour, penColour(), scenePositions);

    for (const QPoint &scenePos : scenePositions) {
        candidateData.scenePositions.append(scenePos);
    }

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
        // TODO
//        const QPoint tilePos = QPoint(mCursorSceneX, mCursorSceneY);
//        Tile *tile = mProject->tileAt(tilePos);
//        if (tile) {
//            if (mMode == PixelMode) {
//                setPenForegroundColour(tile->pixelColor(mCursorTilePixelX, mCursorTilePixelY));
//            }
//        }
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
    }
}

// This function actually operates on the image.
void ImageCanvas::applyPixelPenTool(const QPoint &scenePos, const QColor &colour)
{
    mImageProject->image()->setPixelColor(scenePos, colour);
    update();
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

void ImageCanvas::mouseReleaseEvent(QMouseEvent *event)
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

    const bool wasSplitterHovered = mSplitter.isHovered();
    mSplitter.setHovered(mouseOverSplitterHandle(event->pos()));
    if (mSplitter.isHovered() != wasSplitterHovered) {
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
