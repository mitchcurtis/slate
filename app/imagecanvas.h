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
    Q_PROPERTY(bool hasBlankCursor READ hasBlankCursor NOTIFY hasBlankCursorChanged)
    Q_PROPERTY(bool modalPopupsOpen READ arePopupsOpen NOTIFY modalPopupsOpenChanged)
    Q_PROPERTY(bool altPressed READ isAltPressed NOTIFY altPressedChanged)

public:
    enum Tool {
        PenTool,
        EyeDropperTool,
        EraserTool,
        FillTool
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

    bool hasBlankCursor() const;

    bool arePopupsOpen() const;

    bool isAltPressed() const;

    QPoint scenePosToTilePixelPos(const QPoint &scenePos) const;

    void paint(QPainter *painter) override;

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
    void currentPaneChanged();
    void toolChanged();
    void toolSizeChanged();
    void penForegroundColourChanged();
    void penBackgroundColourChanged();
    void hasBlankCursorChanged();
    void modalPopupsOpenChanged();
    void altPressedChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void reset();
    void centreView();

protected slots:
    virtual void onLoadedChanged();
    void updateWindowCursorShape();
    void checkIfPopupsOpen();
    void onSplitterPositionChanged();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

    friend class ApplyPixelPenCommand;
    friend class ApplyPixelEraserCommand;
    friend class ApplyPixelFillCommand;

    struct PixelCandidateData
    {
        QVector<QPoint> scenePositions;
        QVector<QColor> previousColours;
    };
    virtual PixelCandidateData penEraserPixelCandidates(Tool tool) const;
    virtual PixelCandidateData fillPixelCandidates() const;

    virtual void applyCurrentTool();
    virtual void applyPixelPenTool(const QPoint &scenePos, const QColor &colour);

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
    void drawPane(QPainter *painter, const CanvasPane &pane, int paneIndex);
    int paneWidth(int index) const;
    void centrePanes(bool respectSceneCentred = true);
    bool mouseOverSplitterHandle(const QPoint &mousePos);

    void setAltPressed(bool altPressed);

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
    Qt::MouseButton mMouseButtonPressed;
    QPoint mPressPosition;
    QPoint mCurrentPaneOffsetBeforePress;
    Tool mTool;
    int mToolSize;
    int mMaxToolSize;
    QColor mPenForegroundColour;
    QColor mPenBackgroundColour;

    bool mAltPressed;
    Tool mToolBeforeAltPressed;
    bool mSpacePressed;
    bool mHasBlankCursor;

    bool mPopupsOpen;
};

#endif // IMAGECANVAS_H
