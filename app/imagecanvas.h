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

#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QObject>
#include <QLoggingCategory>
#include <QPixmap>
#include <QQuickPaintedItem>
#include <QStack>
#include <QWheelEvent>

#include <QtUndo/undostack.h>

#include "splitter.h"
#include "canvaspane.h"

Q_DECLARE_LOGGING_CATEGORY(lcCanvas)
Q_DECLARE_LOGGING_CATEGORY(lcCanvasLifecycle)

class ImageProject;
class Project;
class Tile;
class Tileset;

class ImageCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Project *project READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(QColor backgroundColour READ backgroundColour WRITE setBackgroundColour NOTIFY backgroundColourChanged)
    Q_PROPERTY(bool gridVisible READ gridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(QColor gridColour READ gridColour WRITE setGridColour NOTIFY gridColourChanged)
    Q_PROPERTY(QColor splitColour READ splitColour WRITE setSplitColour NOTIFY splitColourChanged)
    Q_PROPERTY(bool splitScreen READ isSplitScreen WRITE setSplitScreen NOTIFY splitScreenChanged)
    Q_PROPERTY(bool scrollZoom READ scrollZoom WRITE setScrollZoom NOTIFY scrollZoomChanged)
    Q_PROPERTY(Splitter *splitter READ splitter CONSTANT)
    Q_PROPERTY(CanvasPane *firstPane READ firstPane CONSTANT)
    Q_PROPERTY(CanvasPane *secondPane READ secondPane CONSTANT)
    Q_PROPERTY(CanvasPane *currentPane READ currentPane NOTIFY currentPaneChanged)
    Q_PROPERTY(int cursorX READ cursorX NOTIFY cursorXChanged)
    Q_PROPERTY(int cursorY READ cursorY NOTIFY cursorYChanged)
    Q_PROPERTY(int cursorSceneX READ cursorSceneX NOTIFY cursorSceneXChanged)
    Q_PROPERTY(int cursorSceneY READ cursorSceneY NOTIFY cursorSceneYChanged)
    Q_PROPERTY(QColor cursorPixelColour READ cursorPixelColour NOTIFY cursorPixelColourChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
    Q_PROPERTY(Tool tool READ tool WRITE setTool NOTIFY toolChanged)
    Q_PROPERTY(int toolSize READ toolSize WRITE setToolSize NOTIFY toolSizeChanged)
    Q_PROPERTY(int maxToolSize READ maxToolSize CONSTANT)
    Q_PROPERTY(QColor penForegroundColour READ penForegroundColour WRITE setPenForegroundColour NOTIFY penForegroundColourChanged)
    Q_PROPERTY(QColor penBackgroundColour READ penBackgroundColour WRITE setPenBackgroundColour NOTIFY penBackgroundColourChanged)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
    Q_PROPERTY(QRect selectionArea READ selectionArea NOTIFY selectionAreaChanged)
    Q_PROPERTY(bool hasBlankCursor READ hasBlankCursor NOTIFY hasBlankCursorChanged)
    Q_PROPERTY(bool altPressed READ isAltPressed NOTIFY altPressedChanged)

public:
    enum Tool {
        PenTool,
        EyeDropperTool,
        EraserTool,
        FillTool,
        SelectionTool
    };

    Q_ENUM(Tool)

    ImageCanvas();
    ~ImageCanvas();

    Project *project() const;
    void setProject(Project *project);

    int cursorX() const;
    void setCursorX(int cursorX);

    int cursorY() const;
    void setCursorY(int cursorY);

    int cursorSceneX() const;
    void setCursorSceneX(int x);

    int cursorSceneY() const;
    void setCursorSceneY(int y);

    QColor cursorPixelColour() const;

    bool containsMouse() const;
    void setContainsMouse(bool containsMouse);

    bool gridVisible() const;
    void setGridVisible(bool gridVisible);

    QColor gridColour() const;
    void setGridColour(const QColor &gridColour);

    QColor splitColour() const;
    void setSplitColour(const QColor &splitColour);

    QColor backgroundColour() const;
    void setBackgroundColour(const QColor &backgroundColour);

    bool isSplitScreen() const;
    void setSplitScreen(bool splitScreen);

    bool scrollZoom() const;
    void setScrollZoom(bool scrollZoom);

    Splitter *splitter();

    CanvasPane *firstPane();
    CanvasPane *secondPane();
    CanvasPane *currentPane();

    QColor mapBackgroundColour() const;

    Tool tool() const;
    void setTool(const Tool &tool);

    int toolSize() const;
    void setToolSize(int toolSize);
    int maxToolSize() const;

    QColor penForegroundColour() const;
    void setPenForegroundColour(const QColor &penForegroundColour);

    QColor penBackgroundColour() const;
    void setPenBackgroundColour(const QColor &penBackgroundColour);

    bool hasSelection() const;

    QRect selectionArea() const;
    void setSelectionArea(const QRect &selectionArea);

    bool hasBlankCursor() const;

    bool isAltPressed() const;

    QPoint scenePosToTilePixelPos(const QPoint &scenePos) const;

    void paint(QPainter *painter) override;

    Q_INVOKABLE bool overrideShortcut(const QKeySequence &keySequence);

    // The image that is currently being drawn on. For regular image canvases, this is
    // the project's image. For layered image canvases, this is the image belonging to
    // the current layer.
    virtual QImage *currentProjectImage();
    virtual const QImage *currentProjectImage() const;

signals:
    void projectChanged();
    void zoomLevelChanged();
    void cursorXChanged();
    void cursorYChanged();
    void cursorSceneXChanged();
    void cursorSceneYChanged();
    void cursorPixelColourChanged();
    void containsMouseChanged();
    void backgroundColourChanged();
    void gridVisibleChanged();
    void gridColourChanged();
    void splitColourChanged();
    void splitScreenChanged();
    void scrollZoomChanged();
    void currentPaneChanged();
    void toolChanged();
    void toolSizeChanged();
    void penForegroundColourChanged();
    void penBackgroundColourChanged();
    void hasBlankCursorChanged();
    void hasSelectionChanged();
    void selectionAreaChanged();
    void altPressedChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void centreView();
    void zoomIn();
    void zoomOut();
    void flipSelection(Qt::Orientation orientation);
    void copySelection();
    void paste();

protected slots:
    virtual void reset();
    virtual void onLoadedChanged();
    void updateWindowCursorShape();
    void onSplitterPositionChanged();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    friend class ApplyPixelEraserCommand;
    friend class ApplyPixelFillCommand;
    friend class ApplyPixelLineCommand;
    friend class ApplyPixelPenCommand;
    friend class MoveImageCanvasSelectionCommand;
    friend class DeleteImageCanvasSelectionCommand;
    friend class FlipImageCanvasSelectionCommand;
    friend class PasteImageCanvasCommand;

    struct PixelCandidateData
    {
        QVector<QPoint> scenePositions;
        QVector<QColor> previousColours;
    };
    virtual PixelCandidateData penEraserPixelCandidates(Tool tool) const;
    virtual PixelCandidateData fillPixelCandidates() const;

    virtual void applyCurrentTool();
    virtual void applyPixelPenTool(const QPoint &scenePos, const QColor &colour, bool markAsLastRelease = false);
    virtual void applyPixelLineTool(const QImage &lineImage, const QPoint &lastPixelPenReleaseScenePosition);
    void paintImageOntoPortionOfImage(const QRect &portion, const QImage &replacementImage);
    void erasePortionOfImage(const QRect &portion);
    void doFlipSelection(const QRect &area, Qt::Orientation orientation);

    virtual void updateCursorPos(const QPoint &eventPos);
    void error(const QString &message);
    QColor penColour() const;
    void setHasBlankCursor(bool hasBlankCursor);
    void restoreToolBeforeAltPressed();
    void setCursorPixelColour(const QColor &cursorPixelColour);
    bool isWithinImage(const QPoint &scenePos) const;
    QPoint clampToImageBounds(const QPoint &scenePos, bool inclusive = true) const;

    void setCurrentPane(CanvasPane *pane);
    CanvasPane *hoveredPane(const QPoint &pos);
    QPoint eventPosRelativeToCurrentPane(const QPoint &pos);
    // Essentially currentProjectImage() for regular image canvas, but may return a
    // preview image if there is a selection active. For layered image canvases, this
    // should return all layers flattened into one image, or the same flattened image
    // as part of a selection preview image.
    virtual QImage contentImage() const;
    void drawPane(QPainter *painter, const CanvasPane &pane, int paneIndex);
    int paneWidth(int index) const;
    void centrePanes(bool respectSceneCentred = true);
    bool mouseOverSplitterHandle(const QPoint &mousePos);

    bool isPanning() const;

    enum ClearSelectionFlag {
        DontClearSelection,
        ClearSelection
    };

    void beginSelectionMove();
    void updateSelectionArea();
    void updateSelectionPreviewImage();
    void moveSelectionArea();
    void moveSelectionAreaBy(const QPoint &pixelDistance);
    void confirmSelectionMove(ClearSelectionFlag clearSelection = ClearSelection);
    QRect clampSelectionArea(const QRect &selectionArea) const;
    QRect boundSelectionArea(const QRect &selectionArea) const;
    void clearSelection();
    void clearOrConfirmSelection();
    void setHasSelection(bool hasSelection);
    void setMovingSelection(bool movingSelection);
    bool cursorOverSelection() const;
    bool shouldDrawSelectionPreviewImage() const;

    void setAltPressed(bool altPressed);

    void setShiftPressed(bool shiftPressed);

    virtual void connectSignals();
    virtual void disconnectSignals();
    virtual void toolChange();

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

    Project *mProject;

private:
    ImageProject *mImageProject;

protected:
    QColor mBackgroundColour;
    bool mGridVisible;
    QColor mGridColour;
    QColor mSplitColour;
    QPixmap mCheckerPixmap;

    bool mSplitScreen;
    Splitter mSplitter;
    CanvasPane mFirstPane;
    CanvasPane mSecondPane;
    CanvasPane *mCurrentPane;

    // The position of the cursor in view coordinates.
    int mCursorX;
    int mCursorY;
    // The position of the cursor in view coordinates relative to a pane.
    int mCursorPaneX;
    int mCursorPaneY;
    // The position of the cursor in scene coordinates.
    int mCursorSceneX;
    int mCursorSceneY;
    qreal mCursorSceneFX;
    qreal mCursorSceneFY;
    QColor mCursorPixelColour;
    bool mContainsMouse;
    // The mouse button that is currently pressed.
    Qt::MouseButton mMouseButtonPressed;
    // The mouse button that was last pressed (could still be currently pressed).
    Qt::MouseButton mLastMouseButtonPressed;
    // The position at which the mouse is currently pressed.
    QPoint mPressPosition;
    QPoint mPressScenePosition;
    // The scene position at which the mouse was pressed before the most-recent press.
    QPoint mCurrentPaneOffsetBeforePress;
    bool mScrollZoom;

    Tool mTool;
    int mToolSize;
    int mMaxToolSize;
    QColor mPenForegroundColour;
    QColor mPenBackgroundColour;

    // The scene position at which the mouse was last pressed.
    // This is used by the pixel line tool to draw the line preview.
    // It is set by the pixel tool as the last pixel in the command,
    // and by the pixel line tool command.
    QPoint mLastPixelPenPressScenePosition;
    QImage mLinePreviewImage;

    bool mPotentiallySelecting;
    bool mHasSelection;
    bool mMovingSelection;
    bool mHasMovedSelection;
    bool mIsSelectionFromPaste;
    // The current selection area. This is set as soon as we receive a mouse press event
    // outside of any existing selection, which means that it starts off with an "empty" size.
    QRect mSelectionArea;
    QRect mSelectionAreaBeforeFirstMove;
    // The selection area before the most recent move.
    QRect mSelectionAreaBeforeLastMove;
    // The last selection area with a non-empty size. This is set after a mouse release event.
    QRect mLastValidSelectionArea;
    QImage mSelectionContents;
    // The entire image as it would look if the selection (that is currently being dragged)
    // was dropped where it is now.
    QImage mSelectionPreviewImage;

    bool mAltPressed;
    bool mShiftPressed;
    Tool mToolBeforeAltPressed;
    bool mSpacePressed;
    bool mHasBlankCursor;
};

#endif // IMAGECANVAS_H
