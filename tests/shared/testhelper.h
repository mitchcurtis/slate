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

#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <QQuickItemGrabResult>
#include <QQuickWindow>
#include <QMetaObject>
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>

#include "application.h"
#include "imagecanvas.h"
#include "imageproject.h"
#include "layeredimagecanvas.h"
#include "layeredimageproject.h"
#include "project.h"
#include "testutils.h"
#include "tilecanvas.h"
#include "tilesetproject.h"
#include "tilesetproject.h"

class ProjectManager;

#define VERIFY_NO_CREATION_ERRORS_OCCURRED() \
QVERIFY2_THROW(projectCreationFailedSpy->isEmpty(), \
    qPrintable(QString::fromLatin1("Unexpected error occurred: ") + \
    (!projectCreationFailedSpy->isEmpty() ? projectCreationFailedSpy->first().first().toString() : "")));

#define VERIFY_NO_CREATION_ERRORS_OCCURRED() \
QVERIFY2_THROW(projectCreationFailedSpy->isEmpty(), \
    qPrintable(QString::fromLatin1("Unexpected error occurred: ") + \
    (!projectCreationFailedSpy->isEmpty() ? projectCreationFailedSpy->first().first().toString() : "")));

class QQuickItem;
class QQuickWindow;

class ImageGrabber
{
public:
    bool requestImage(QQuickItem *item) {
        result = item->grabToImage();
        return result;
    }

    bool isReady() const {
        return result && !result->image().isNull();
    }

    QImage takeImage() {
        if (!isReady()) {
            return QImage();
        }

        QImage image = result->image();
        result.reset();
        return image;
    }

    QSharedPointer<QQuickItemGrabResult> result;
};

class TestHelper : public QObject
{
    Q_OBJECT

public:
    TestHelper(int &argc, char **argv);
    ~TestHelper();

private Q_SLOTS:
    void initTestCase();
    void cleanup();

protected:
    void resetCreationErrorSpy();

    enum TestMouseEventType
    {
        MousePress,
        MouseRelease,
        MouseClick,
        MouseDoubleClick
    };
    void mouseEventOnCentre(QQuickItem *item, TestMouseEventType eventType, Qt::MouseButton button = Qt::LeftButton);
    void mouseEvent(QQuickItem *item, const QPointF &localPos, TestMouseEventType eventType,
        Qt::MouseButton button = Qt::LeftButton, Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(), int delay = -1);
    void wheelEvent(QQuickItem *item, const QPoint &localPos, const int degrees);
    void keyClicks(const QString &text);
    void clearAndEnterText(QQuickItem *textField, const QString &text);

    QObject *findPopupFromTypeName(const QString &typeName) const;
    QQuickItem *findDialogButtonFromText(const QObject *dialog, const QString &text);
    QQuickItem *findDialogButtonFromObjectName(const QObject *dialog, const QString &objectName);
    QQuickItem *findListViewChild(QQuickItem *listView, const QString &childObjectName) const;
    QQuickItem *findListViewChild(const QString &listViewObjectName, const QString &childObjectName) const;
    static QQuickItem *findChildWithText(QQuickItem *item, const QString &text);
    QQuickItem *findViewDelegateAtIndex(QQuickItem *view, int index);
    QQuickItem* findSplitViewHandle(const QString &splitViewObjectName, int handleIndex) const;

    // Returns the position of the cursor in a tile's local coordinates.
    QPoint mapToTile(const QPoint &cursorPos) const;
    QPoint tileSceneCentre(int xPosInTiles, int yPosInTiles) const;
    QPoint tileCanvasCentre(int xPosInTiles, int yPosInTiles) const;
    QPointF canvasCentre() const;
    QPoint canvasSceneCentre() const;
    QPoint firstPaneSceneCentre() const;
    void setCursorPosInPixels(const QPoint &posInPixels);
    void setCursorPosInTiles(int xPosInTiles, int yPosInTiles);
    void setCursorPosInScenePixels(int xPosInScenePixels, int yPosInScenePixels, bool assertWithinWindow = true);
    void setCursorPosInScenePixels(const QPoint &posInScenePixels);
    QPoint tilesetTileCentre(int xPosInTiles, int yPosInTiles) const;
    QPoint tilesetTileSceneCentre(int xPosInTiles, int yPosInTiles) const;
    int digits(int number);
    int digitAt(int number, int index);

    bool isUsingAnimation() const;
    AnimationPlayback *animationPlayback();

    enum InputType {
        MouseInputType,
        KeyboardInputType
    };

    enum CloseDialogFlag {
        CloseDialog,
        DoNotCloseDialog
    };

    // Platform-dependent actions
    void triggerShortcut(const QString &objectName, const QString &sequenceAsString);
    void triggerNewProject();
    void triggerCloseProject();
    void triggerSaveProject();
    void triggerSaveProjectAs();
    void triggerOpenProject();
    void triggerRevert();

    void triggerPaste();
    void triggerFlipHorizontally();
    void triggerFlipVertically();

    void triggerSelectAll();

    void triggerCentre();
    void triggerGridVisible();
    void triggerRulersVisible();
    void triggerGuidesVisible();
    void triggerSplitScreen();
    void triggerSplitterLocked();
    void setSplitterLocked(bool splitterLocked);
    void triggerAnimationPlayback();
    void setAnimationPlayback(bool usingAnimation);

    void triggerOptions();

    // Layer helpers.
    void selectLayer(const QString &layerName, int layerIndex);
    // Finds \a layerName in the layer list view, verifies that it exists and that its name is \a layerName,
    // and sets layerDelegate to it.
    void verifyLayerName(const QString &layerName, QQuickItem **layerDelegate = nullptr);
    void makeCurrentAndRenameLayer(const QString &from, const QString &to);
    void changeLayerVisiblity(const QString &layerName, bool visible);

    void addAllProjectTypes();
    void addImageProjectTypes();
    void addActualProjectTypes();

    void createNewProject(Project::Type projectType, const QVariantMap &args = QVariantMap());
    void createNewTilesetProject(int tileWidth = 25, int tileHeight = 25,
        int tilesetTilesWide = -1, int tilesetTilesHigh = -1, bool transparentTilesetBackground = false);
    void createNewImageProject(int imageWidth = 256, int imageHeight = 256,
        bool transparentImageBackground = false);
    bool createNewLayeredImageProject(int imageWidth = 256, int imageHeight = 256,
        bool transparentImageBackground = false);
    void loadProject(const QUrl &url, const QString &expectedFailureMessage = QString());
    void updateVariables(bool isNewProject, Project::Type newProjectType);
    void discardChanges();
    void verifyErrorAndDismiss(const QString &expectedErrorMessage);
    void verifyNoErrorOrDismiss();

    void copyFileFromResourcesToTempProjectDir(const QString &baseName);
    void setupTempTilesetProjectDir();
    void setupTempLayeredImageProjectDir();
    void setupTempProjectDir(const QStringList &resourceFilesToCopy = QStringList(),
        QStringList *filesCopied = nullptr);

    void collapseAllPanels();
    Q_REQUIRED_RESULT bool isPanelExpanded(const QString &panelObjectName);
    void togglePanel(const QString &panelObjectName, bool expanded);
    void togglePanels(const QStringList &panelObjectNames, bool expanded);
    void expandColourPanel();

    void dragSplitViewHandle(const QString &splitViewObjectName, int index,
        const QPoint &newHandleCentreRelativeToSplitView, QPoint *oldHandleCentreRelativeToSplitView = nullptr);

    void switchMode(TileCanvas::Mode mode);
    void switchTool(ImageCanvas::Tool tool, InputType inputType = MouseInputType);
    void setPenForegroundColour(QString argbString);
    void panTopLeftTo(int x, int y);
    void panBy(int xDistance, int yDistance);
    void zoomTo(int zoomLevel);
    void zoomTo(int zoomLevel, const QPoint &pos);
    void changeCanvasSize(int width, int height, CloseDialogFlag closeDialog = CloseDialog);
    void changeImageSize(int width, int height);
    void changeToolSize(int size);
    void changeToolShape(ImageCanvas::ToolShape toolShape);
    void moveContents(int x, int y, bool onlyVisibleLayers);
    int sliderValue(QQuickItem *slider) const;
    void selectColourAtCursorPos();
    void drawPixelAtCursorPos();
    void drawTileAtCursorPos();
    void selectArea(const QRect &area);
    void dragSelection(const QPoint &newTopLeft);
    Q_REQUIRED_RESULT bool fuzzyColourCompare(const QColor &colour1, const QColor &colour2, int fuzz = 1);
    void fuzzyImageCompare(const QImage &image1, const QImage &image2);
    void everyPixelIs(const QImage &image, const QColor &colour);
    void compareSwatches(const Swatch &actualSwatch, const Swatch &expectedSwatch);
    void enableAutoSwatch();
    void verifySwatchViewDelegateExists(const QQuickItem *viewContentItem, const QColor &colour);
    QQuickItem *findSwatchViewDelegateAtIndex(int index);
    void addSwatchWithForegroundColour();
    void renameSwatchColour(int index, const QString &name);
    void deleteSwatchColour(int index);

    void addNewGuide(Qt::Orientation orientation, int position);

    QVector<Project::Type> allProjectTypes;
    QVector<ImageCanvas::PenToolRightClickBehaviour> allRightClickBehaviours;

    Application app;
    QQuickWindow *window;
    QQuickItem *overlay;
    QPointer<ProjectManager> projectManager;
    QPointer<Project> project;
    QPointer<ImageProject> imageProject;
    QPointer<TilesetProject> tilesetProject;
    QPointer<LayeredImageProject> layeredImageProject;
    QPointer<ImageCanvas> canvas;
    QPointer<ImageCanvas> imageCanvas;
    QPointer<TileCanvas> tileCanvas;
    QPointer<LayeredImageCanvas> layeredImageCanvas;

    QObject *fileMenuBarItem;

    QQuickItem *canvasSizeButton;
    QQuickItem *imageSizeButton;
    QQuickItem *modeToolButton;
    QQuickItem *penToolButton;
    QQuickItem *eyeDropperToolButton;
    QQuickItem *fillToolButton;
    QQuickItem *eraserToolButton;
    QQuickItem *selectionToolButton;
    QQuickItem *toolSizeButton;
    QQuickItem *toolShapeButton;
    QQuickItem *rotate90CcwToolButton;
    QQuickItem *rotate90CwToolButton;
    QQuickItem *flipHorizontallyToolButton;
    QQuickItem *flipVerticallyToolButton;
    QQuickItem *undoButton;
    QQuickItem *redoButton;
    QQuickItem *splitScreenToolButton;
    QQuickItem *lockSplitterToolButton;
    QQuickItem *penForegroundColourButton;
    QQuickItem *penBackgroundColourButton;
    QQuickItem *lighterButton;
    QQuickItem *darkerButton;
    QQuickItem *saturateButton;
    QQuickItem *desaturateButton;
    QQuickItem *swatchesPanel;
    QQuickItem *tilesetSwatchPanel;
    QQuickItem *tilesetSwatchFlickable;
    QQuickItem *newLayerButton;
    QQuickItem *duplicateLayerButton;
    QQuickItem *moveLayerDownButton;
    QQuickItem *moveLayerUpButton;
    QQuickItem *animationPlayPauseButton;

    QVector<ImageCanvas::Tool> mTools;

    QScopedPointer<QSignalSpy> projectCreationFailedSpy;

    QString tilesetBasename;
    QUrl tempTilesetUrl;
    QScopedPointer<QTemporaryDir> tempProjectDir;

    ImageGrabber imageGrabber;

    QPoint cursorPos;
    QPoint cursorWindowPos;

    QImage mCheckerImage;
};

#endif // TESTHELPER_H
