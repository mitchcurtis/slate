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

#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QBasicTimer>
#include <QObject>
#include <QLoggingCategory>
#include <QPixmap>
#include <QQuickItem>
#include <QStack>
#include <QTimerEvent>
#include <QUndoStack>
#include <QWheelEvent>

#include "canvaspane.h"
#include "ruler.h"
#include "selectionitem.h"
#include "splitter.h"
#include "texturedfillparameters.h"

Q_DECLARE_LOGGING_CATEGORY(lcImageCanvas)
Q_DECLARE_LOGGING_CATEGORY(lcImageCanvasLifecycle)

class Guide;
class GuidesItem;
class ImageProject;
class Project;
class SelectionCursorGuide;
class Tile;
class Tileset;

class ImageCanvas : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Project *project READ project WRITE setProject NOTIFY projectChanged)
    Q_PROPERTY(QColor backgroundColour READ backgroundColour WRITE setBackgroundColour NOTIFY backgroundColourChanged)
    Q_PROPERTY(bool gridVisible READ gridVisible WRITE setGridVisible NOTIFY gridVisibleChanged)
    Q_PROPERTY(QColor gridColour READ gridColour WRITE setGridColour NOTIFY gridColourChanged)
    Q_PROPERTY(bool rulersVisible READ rulersVisible WRITE setRulersVisible NOTIFY rulersVisibleChanged)
    Q_PROPERTY(bool guidesVisible READ guidesVisible WRITE setGuidesVisible NOTIFY guidesVisibleChanged)
    Q_PROPERTY(bool guidesLocked READ guidesLocked WRITE setGuidesLocked NOTIFY guidesLockedChanged)
    Q_PROPERTY(QColor splitColour READ splitColour WRITE setSplitColour NOTIFY splitColourChanged)
    Q_PROPERTY(QColor checkerColour1 READ checkerColour1 WRITE setCheckerColour1 NOTIFY checkerColour1Changed)
    Q_PROPERTY(QColor checkerColour2 READ checkerColour2 WRITE setCheckerColour2 NOTIFY checkerColour2Changed)
    Q_PROPERTY(bool splitScreen READ isSplitScreen WRITE setSplitScreen NOTIFY splitScreenChanged)
    Q_PROPERTY(bool scrollZoom READ scrollZoom WRITE setScrollZoom NOTIFY scrollZoomChanged)
    Q_PROPERTY(bool gesturesEnabled READ areGesturesEnabled WRITE setGesturesEnabled NOTIFY gesturesEnabledChanged)
    Q_PROPERTY(Splitter *splitter READ splitter CONSTANT)
    Q_PROPERTY(CanvasPane *firstPane READ firstPane CONSTANT)
    Q_PROPERTY(CanvasPane *secondPane READ secondPane CONSTANT)
    Q_PROPERTY(CanvasPane *currentPane READ currentPane NOTIFY currentPaneChanged)
    Q_PROPERTY(QColor rulerForegroundColour READ rulerForegroundColour WRITE setRulerForegroundColour)
    Q_PROPERTY(QColor rulerBackgroundColour READ rulerBackgroundColour WRITE setRulerBackgroundColour)
    Q_PROPERTY(int cursorX READ cursorX NOTIFY cursorXChanged)
    Q_PROPERTY(int cursorY READ cursorY NOTIFY cursorYChanged)
    Q_PROPERTY(int cursorSceneX READ cursorSceneX NOTIFY cursorSceneXChanged)
    Q_PROPERTY(int cursorSceneY READ cursorSceneY NOTIFY cursorSceneYChanged)
    Q_PROPERTY(QColor cursorPixelColour READ cursorPixelColour NOTIFY cursorPixelColourChanged)
    Q_PROPERTY(QColor invertedCursorPixelColour READ invertedCursorPixelColour NOTIFY cursorPixelColourChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
    Q_PROPERTY(Tool tool READ tool WRITE setTool NOTIFY toolChanged)
    Q_PROPERTY(Tool lastFillToolUsed READ lastFillToolUsed NOTIFY lastFillToolUsedChanged)
    Q_PROPERTY(int toolSize READ toolSize WRITE setToolSize NOTIFY toolSizeChanged)
    Q_PROPERTY(int maxToolSize READ maxToolSize CONSTANT)
    Q_PROPERTY(QColor penForegroundColour READ penForegroundColour WRITE setPenForegroundColour NOTIFY penForegroundColourChanged)
    Q_PROPERTY(QColor penBackgroundColour READ penBackgroundColour WRITE setPenBackgroundColour NOTIFY penBackgroundColourChanged)
    Q_PROPERTY(TexturedFillParameters *texturedFillParameters READ texturedFillParameters CONSTANT FINAL)
    Q_PROPERTY(bool hasSelection READ hasSelection NOTIFY hasSelectionChanged)
    Q_PROPERTY(QRect selectionArea READ selectionArea NOTIFY selectionAreaChanged)
    Q_PROPERTY(bool hasBlankCursor READ hasBlankCursor NOTIFY hasBlankCursorChanged)
    Q_PROPERTY(bool altPressed READ isAltPressed NOTIFY altPressedChanged)
    Q_PROPERTY(bool lineVisible READ isLineVisible NOTIFY lineVisibleChanged)
    Q_PROPERTY(int lineLength READ lineLength NOTIFY lineLengthChanged)
    Q_PROPERTY(qreal lineAngle READ lineAngle NOTIFY lineLengthChanged)

public:
    // The order of these is important, as the number keys can activate the tools.
    enum Tool {
        PenTool,
        EyeDropperTool,
        EraserTool,
        FillTool,
        SelectionTool,
        TexturedFillTool,
        CropTool
    };

    Q_ENUM(Tool)

    ImageCanvas();
    ~ImageCanvas() override;

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
    QColor invertedCursorPixelColour() const;
    static QColor invertedColour(const QColor &colour);

    bool containsMouse() const;
    void setContainsMouse(bool containsMouse);

    QRect firstPaneVisibleSceneArea() const;
    QRect secondPaneVisibleSceneArea() const;
    QRect paneVisibleSceneArea(int paneIndex) const;

    bool gridVisible() const;
    void setGridVisible(bool gridVisible);

    QColor gridColour() const;
    void setGridColour(const QColor &gridColour);

    bool rulersVisible() const;
    void setRulersVisible(bool rulersVisible);

    bool guidesVisible() const;
    void setGuidesVisible(bool guidesVisible);

    bool guidesLocked() const;
    void setGuidesLocked(bool guidesLocked);

    QColor splitColour() const;
    void setSplitColour(const QColor &splitColour);

    QColor checkerColour1() const;
    void setCheckerColour1(const QColor &colour);

    QColor checkerColour2() const;
    void setCheckerColour2(const QColor &colour);

    QColor backgroundColour() const;
    void setBackgroundColour(const QColor &backgroundColour);

    bool isSplitScreen() const;
    void setSplitScreen(bool splitScreen);

    bool scrollZoom() const;
    void setScrollZoom(bool scrollZoom);

    bool areGesturesEnabled() const;
    void setGesturesEnabled(bool gesturesEnabled);

    Ruler *pressedRuler() const;
    int pressedGuideIndex() const;

    Splitter *splitter();

    CanvasPane *firstPane();
    const CanvasPane *firstPane() const;
    CanvasPane *secondPane();
    const CanvasPane *secondPane() const;
    CanvasPane *currentPane();
    Q_INVOKABLE CanvasPane *paneAt(int index);
    int paneWidth(int index) const;

    QColor rulerForegroundColour() const;
    void setRulerForegroundColour(const QColor &foregroundColour) const;

    QColor rulerBackgroundColour() const;
    void setRulerBackgroundColour(const QColor &backgroundColour) const;

    QColor mapBackgroundColour() const;

    Tool tool() const;
    void setTool(const Tool &tool);

    Tool lastFillToolUsed() const;

    int toolSize() const;
    void setToolSize(int toolSize);
    int maxToolSize() const;

    QColor penForegroundColour() const;
    void setPenForegroundColour(const QColor &penForegroundColour);

    QColor penBackgroundColour() const;
    void setPenBackgroundColour(const QColor &penBackgroundColour);

    TexturedFillParameters *texturedFillParameters();

    bool hasSelection() const;

    QRect selectionArea() const;
    void setSelectionArea(const QRect &selectionArea);

    bool hasBlankCursor() const;

    bool isAltPressed() const;

    bool isLineVisible() const;
    int lineLength() const;
    qreal lineAngle() const;

    Q_INVOKABLE bool overrideShortcut(const QKeySequence &keySequence);

    // The image that is currently being drawn on. For regular image canvases, this is
    // the project's image. For layered image canvases, this is the image belonging to
    // the current layer.
    virtual QImage *currentProjectImage();
    virtual const QImage *currentProjectImage() const;

    virtual QImage *imageForLayerAt(int layerIndex);

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
    void checkerColour1Changed();
    void checkerColour2Changed();
    void rulersVisibleChanged();
    void guidesVisibleChanged();
    void guidesLockedChanged();
    void splitColourChanged();
    void splitScreenChanged();
    void scrollZoomChanged();
    void gesturesEnabledChanged();
    void currentPaneChanged();
//    void rulerForegroundColourChanged();
//    void rulerBackgroundColourChanged();
    void toolChanged();
    void lastFillToolUsedChanged();
    void toolSizeChanged();
    void penForegroundColourChanged();
    void penBackgroundColourChanged();
    void hasBlankCursorChanged();
    void hasSelectionChanged();
    void selectionAreaChanged();
    void altPressedChanged();
    void lineVisibleChanged();
    void lineLengthChanged();

    // Used to signal CanvasPaneItem classes that they should redraw,
    // instead of them having to connect to lots of specific signals.
    // paneIndex is the index of the pane that should be redrawn,
    // or -1 for all panes.
    void contentPaintRequested(int paneIndex);

    void errorOccurred(const QString &errorMessage);

public slots:
    void centreView();
    void zoomIn();
    void zoomOut();
    void flipSelection(Qt::Orientation orientation);
    void copySelection();
    void paste();
    void deleteSelectionOrContents();
    void selectAll();

    void cycleFillTools();

protected slots:
    virtual void reset();
    virtual void onLoadedChanged();

    // Requests both panes to be repainted. Most operations
    // (like the user drawing pixels) require both panes to be redrawn,
    // but stuff like panning does not, and hence it should use
    // requestPaneContentPaint() and pass a specific index.
    void requestContentPaint();
    void requestPaneContentPaint(int paneIndex);
    void updateWindowCursorShape();
    void onZoomLevelChanged();
    void onPaneintegerOffsetChanged();
    void onPaneSizeChanged();
    void onSplitterPositionChanged();
    void onGuidesChanged();
    void onReadyForWritingToJson(QJsonObject* projectJson);
    void onAboutToBeginMacro();
    void recreateCheckerImage();

protected:
    void componentComplete() override;
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    void resizeChildren();

    friend class ApplyGreedyPixelFillCommand;
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
    QImage fillPixels() const;
    QImage greedyFillPixels() const;
    QImage texturedFillPixels() const;
    QImage greedyTexturedFillPixels() const;

    virtual void applyCurrentTool();
    virtual void applyPixelPenTool(int layerIndex, const QPoint &scenePos, const QColor &colour, bool markAsLastRelease = false);
    virtual void applyPixelLineTool(int layerIndex, const QImage &lineImage, const QRect &lineRect, const QPoint &lastPixelPenReleaseScenePosition);
    void paintImageOntoPortionOfImage(const QRect &portion, const QImage &replacementImage);
    void replacePortionOfImage(const QRect &portion, const QImage &replacementImage);
    void erasePortionOfImage(const QRect &portion);
    virtual void replaceImage(int layerIndex, const QImage &replacementImage);
    void doFlipSelection(const QRect &area, Qt::Orientation orientation);

    QPointF linePoint1() const;
    QPointF linePoint2() const;
    QRect normalisedLineRect() const;

    virtual void updateCursorPos(const QPoint &eventPos);
    void updateVisibleSceneArea();
    void error(const QString &message);
    QColor penColour() const;
    void setHasBlankCursor(bool hasBlankCursor);
    void restoreToolBeforeAltPressed();
    virtual bool areToolsForbidden() const;
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
    //
    // This function calls getContentImage() and caches the result so that we have
    // cheap lookup of pixel data, which is useful for e.g. mCursorPixelColour.
    QImage contentImage();
    virtual QImage getContentImage();
    void drawLine(QPainter *painter) const;
    void centrePanes(bool respectSceneCentred = true);
    enum ResetPaneSizePolicy {
        DontResetPaneSizes,
        ResetPaneSizes
    };
    void doSetSplitScreen(bool splitScreen, ResetPaneSizePolicy resetPaneSizePolicy);
    void setDefaultPaneSizes();
    bool mouseOverSplitterHandle(const QPoint &mousePos);

    void updateRulerVisibility();
    void resizeRulers();
    void updatePressedRuler();
    Ruler *rulerAtCursorPos();

    void addNewGuide();
    void moveGuide();
    void removeGuide();
    void updatePressedGuide();
    int guideIndexAtCursorPos();

    bool isPanning() const;

    enum ClearSelectionFlag {
        DontClearSelection,
        ClearSelection
    };

    enum SelectionPanReason {
        SelectionPanMouseMovementReason,
        SelectionPanTimerReason
    };

    virtual bool supportsSelectionTool() const;
    void beginSelectionMove();
    void updateOrMoveSelectionArea();
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
    bool shouldDrawSelectionCursorGuide() const;
    void updateSelectionCursorGuideVisibility();
    void confirmPasteSelection();
    void setSelectionFromPaste(bool isSelectionFromPaste);
    void panWithSelectionIfAtEdge(SelectionPanReason reason);

    void setAltPressed(bool altPressed);

    void setShiftPressed(bool shiftPressed);

    void applyZoom(qreal zoom, const QPoint &origin);

    virtual void connectSignals();
    virtual void disconnectSignals();
    virtual void toolChange();

    bool event(QEvent *event) override;
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
    void timerEvent(QTimerEvent *event) override;

    Project *mProject;

private:
    ImageProject *mImageProject;

protected:
    friend class CanvasPaneItem;

    // The background colour of the entire pane.
    QColor mBackgroundColour;
    bool mGridVisible;
    QColor mGridColour;
    QColor mSplitColour;
    QPixmap mCheckerPixmap;
    QImage mCheckerImage;
    QColor mCheckerColour1;
    QColor mCheckerColour2;

    bool mSplitScreen;
    Splitter mSplitter;
    CanvasPane mFirstPane;
    CanvasPane mSecondPane;
    CanvasPane *mCurrentPane;
    int mCurrentPaneIndex;
    Ruler *mFirstHorizontalRuler;
    Ruler *mFirstVerticalRuler;
    Ruler *mSecondHorizontalRuler;
    Ruler *mSecondVerticalRuler;
    Ruler *mPressedRuler;
    bool mGuidesVisible;
    bool mGuidesLocked;
    int mGuidePositionBeforePress;
    int mPressedGuideIndex;
    GuidesItem *mGuidesItem;
    SelectionItem *mSelectionItem;

    QImage mCachedContentImage;

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
    QRect mFirstPaneVisibleSceneArea;
    QRect mSecondPaneVisibleSceneArea;
    bool mScrollZoom;
    bool mGesturesEnabled;

    Tool mTool;
    Tool mLastFillToolUsed;
    int mToolSize;
    int mMaxToolSize;
    QColor mPenForegroundColour;
    QColor mPenBackgroundColour;

    TexturedFillParameters mTexturedFillParameters;

    // The scene position at which the mouse was last pressed.
    // This is used by the pixel line tool to draw the line preview.
    // It is set by the pixel tool as the last pixel in the command,
    // and by the pixel line tool command.
    QPoint mLastPixelPenPressScenePosition;
    // An image as large as the rectangle that contains the line that is being previewed.
    QImage mLinePreviewImage;

    bool mPotentiallySelecting;
    bool mHasSelection;
    bool mMovingSelection;
    bool mHasMovedSelection;
    bool mIsSelectionFromPaste;
    bool mConfirmingSelectionMove;
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
    QBasicTimer mSelectionEdgePanTimer;
    SelectionCursorGuide *mSelectionCursorGuide;

    QRect mCropArea;

    bool mAltPressed;
    bool mShiftPressed;
    Tool mToolBeforeAltPressed;
    bool mSpacePressed;
    bool mHasBlankCursor;
};

#endif // IMAGECANVAS_H
