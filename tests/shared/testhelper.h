/*
    Copyright 2019, Mitch Curtis

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

#define QVERIFY_NO_CREATION_ERRORS_OCCURRED() \
QVERIFY2(projectCreationFailedSpy->isEmpty(), \
    qPrintable(QString::fromLatin1("Unexpected error occurred: ") + \
    (!projectCreationFailedSpy->isEmpty() ? projectCreationFailedSpy->first().first().toString() : "")));

#define VERIFY_NO_CREATION_ERRORS_OCCURRED() \
VERIFY2(projectCreationFailedSpy->isEmpty(), \
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
    Q_REQUIRED_RESULT bool clearAndEnterText(QQuickItem *textField, const QString &text);
    Q_REQUIRED_RESULT bool selectComboBoxItem(const QString &comboBoxObjectName, int index);

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
    void setCursorPosInScenePixels(const QPoint &posInScenePixels, bool assertWithinWindow = true);
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
    Q_REQUIRED_RESULT bool triggerShortcut(const QString &objectName, const QString &sequenceAsString);
    Q_REQUIRED_RESULT bool triggerNewProject();
    Q_REQUIRED_RESULT bool triggerCloseProject();
    Q_REQUIRED_RESULT bool triggerSaveProject();
    Q_REQUIRED_RESULT bool triggerSaveProjectAs();
    Q_REQUIRED_RESULT bool triggerOpenProject();
    Q_REQUIRED_RESULT bool triggerRevert();

    Q_REQUIRED_RESULT bool triggerPaste();
    Q_REQUIRED_RESULT bool triggerFlipHorizontally();
    Q_REQUIRED_RESULT bool triggerFlipVertically();

    Q_REQUIRED_RESULT bool triggerSelectAll();

    Q_REQUIRED_RESULT bool triggerCentre();
    Q_REQUIRED_RESULT bool triggerGridVisible();
    Q_REQUIRED_RESULT bool triggerRulersVisible();
    Q_REQUIRED_RESULT bool triggerGuidesVisible();
    Q_REQUIRED_RESULT bool triggerSplitScreen();
    Q_REQUIRED_RESULT bool triggerSplitterLocked();
    Q_REQUIRED_RESULT bool setSplitterLocked(bool splitterLocked);
    Q_REQUIRED_RESULT bool triggerAnimationPlayback();
    Q_REQUIRED_RESULT bool setAnimationPlayback(bool usingAnimation);

    Q_REQUIRED_RESULT bool triggerOptions();

    // Layer helpers.
    Q_REQUIRED_RESULT bool selectLayer(const QString &layerName, int layerIndex);
    // Finds \a layerName in the layer list view, verifies that it exists and that its name is \a layerName,
    // and sets layerDelegate to it.
    Q_REQUIRED_RESULT bool verifyLayerName(const QString &layerName, QQuickItem **layerDelegate = nullptr);
    Q_REQUIRED_RESULT bool makeCurrentAndRenameLayer(const QString &from, const QString &to);
    Q_REQUIRED_RESULT bool changeLayerVisiblity(const QString &layerName, bool visible);

    void addAllProjectTypes();
    void addImageProjectTypes();
    void addActualProjectTypes();

    Q_REQUIRED_RESULT bool createNewProject(Project::Type projectType, const QVariantMap &args = QVariantMap());
    Q_REQUIRED_RESULT bool createNewTilesetProject(int tileWidth = 25, int tileHeight = 25,
        int tilesetTilesWide = -1, int tilesetTilesHigh = -1, bool transparentTilesetBackground = false);
    Q_REQUIRED_RESULT bool createNewImageProject(int imageWidth = 256, int imageHeight = 256,
        bool transparentImageBackground = false);
    Q_REQUIRED_RESULT bool createNewLayeredImageProject(int imageWidth = 256, int imageHeight = 256,
        bool transparentImageBackground = false);
    Q_REQUIRED_RESULT bool loadProject(const QUrl &url,
        const QRegularExpression &expectedFailureMessage = QRegularExpression());
    Q_REQUIRED_RESULT bool updateVariables(bool isNewProject, Project::Type newProjectType);
    Q_REQUIRED_RESULT bool discardChanges();
    Q_REQUIRED_RESULT bool verifyErrorAndDismiss(const QString &expectedErrorMessage);
    Q_REQUIRED_RESULT bool verifyNoErrorOrDismiss();

    Q_REQUIRED_RESULT bool copyFileFromResourcesToTempProjectDir(const QString &baseName);
    Q_REQUIRED_RESULT bool setupTempTilesetProjectDir();
    Q_REQUIRED_RESULT bool setupTempLayeredImageProjectDir();
    Q_REQUIRED_RESULT bool setupTempProjectDir(const QStringList &resourceFilesToCopy = QStringList(),
        QStringList *filesCopied = nullptr);

    Q_REQUIRED_RESULT bool collapseAllPanels();
    Q_REQUIRED_RESULT bool isPanelExpanded(const QString &panelObjectName);
    Q_REQUIRED_RESULT bool togglePanel(const QString &panelObjectName, bool expanded);
    Q_REQUIRED_RESULT bool togglePanels(const QStringList &panelObjectNames, bool expanded);
    Q_REQUIRED_RESULT bool expandAndResizePanel(const QString &panelObjectName);

    Q_REQUIRED_RESULT bool dragSplitViewHandle(const QString &splitViewObjectName, int index,
        const QPoint &newHandleCentreRelativeToSplitView, QPoint *oldHandleCentreRelativeToSplitView = nullptr);

    Q_REQUIRED_RESULT bool switchMode(TileCanvas::Mode mode);
    Q_REQUIRED_RESULT bool switchTool(ImageCanvas::Tool tool, InputType inputType = MouseInputType);
    Q_REQUIRED_RESULT bool setPenForegroundColour(QString argbString);
    Q_REQUIRED_RESULT bool panTopLeftTo(int x, int y);
    Q_REQUIRED_RESULT bool panBy(int xDistance, int yDistance);
    Q_REQUIRED_RESULT bool zoomTo(int zoomLevel);
    Q_REQUIRED_RESULT bool zoomTo(int zoomLevel, const QPoint &pos);
    Q_REQUIRED_RESULT bool changeCanvasSize(int width, int height, CloseDialogFlag closeDialog = CloseDialog);
    Q_REQUIRED_RESULT bool changeImageSize(int width, int height);
    Q_REQUIRED_RESULT bool changeToolSize(int size);
    Q_REQUIRED_RESULT bool changeToolShape(ImageCanvas::ToolShape toolShape);
    Q_REQUIRED_RESULT bool moveContents(int x, int y, bool onlyVisibleLayers);
    int sliderValue(QQuickItem *slider) const;
    Q_REQUIRED_RESULT bool selectColourAtCursorPos();
    Q_REQUIRED_RESULT bool drawPixelAtCursorPos();
    Q_REQUIRED_RESULT bool drawTileAtCursorPos();
    Q_REQUIRED_RESULT bool selectArea(const QRect &area);
    Q_REQUIRED_RESULT bool dragSelection(const QPoint &newTopLeft);
    Q_REQUIRED_RESULT bool fuzzyColourCompare(const QColor &colour1, const QColor &colour2, int fuzz = 1);
    Q_REQUIRED_RESULT bool fuzzyImageCompare(const QImage &image1, const QImage &image2);
    Q_REQUIRED_RESULT bool everyPixelIs(const QImage &image, const QColor &colour);
    Q_REQUIRED_RESULT bool compareSwatches(const Swatch &actualSwatch, const Swatch &expectedSwatch);
    Q_REQUIRED_RESULT bool enableAutoSwatch();
    Q_REQUIRED_RESULT bool swatchViewDelegateExists(const QQuickItem *viewContentItem, const QColor &colour);
    QQuickItem *findSwatchViewDelegateAtIndex(int index);
    Q_REQUIRED_RESULT bool addSwatchWithForegroundColour();
    Q_REQUIRED_RESULT bool renameSwatchColour(int index, const QString &name);
    Q_REQUIRED_RESULT bool deleteSwatchColour(int index);
    Q_REQUIRED_RESULT bool addNewGuide(Qt::Orientation orientation, int position);
    Q_REQUIRED_RESULT bool addSelectedColoursToTexturedFillSwatch();

    QByteArray failureMessage;

    QVector<Project::Type> allProjectTypes;
    QVector<ImageCanvas::PenToolRightClickBehaviour> allRightClickBehaviours;

    Application app;
    QQuickWindow *window = nullptr;
    QQuickItem *overlay = nullptr;
    QPointer<ProjectManager> projectManager;
    QPointer<Project> project;
    QPointer<ImageProject> imageProject;
    QPointer<TilesetProject> tilesetProject;
    QPointer<LayeredImageProject> layeredImageProject;
    QPointer<ImageCanvas> canvas;
    QPointer<ImageCanvas> imageCanvas;
    QPointer<TileCanvas> tileCanvas;
    QPointer<LayeredImageCanvas> layeredImageCanvas;

    QObject *fileMenuBarItem = nullptr;

    QQuickItem *toolBar = nullptr;
    QQuickItem *canvasSizeToolButton = nullptr;
    QQuickItem *imageSizeToolButton = nullptr;
    QQuickItem *cropToSelectionToolButton = nullptr;
    QQuickItem *modeToolButton = nullptr;
    QQuickItem *penToolButton = nullptr;
    QQuickItem *eyeDropperToolButton = nullptr;
    QQuickItem *fillToolButton = nullptr;
    QQuickItem *eraserToolButton = nullptr;
    QQuickItem *selectionToolButton = nullptr;
    QQuickItem *toolSizeButton = nullptr;
    QQuickItem *toolShapeButton = nullptr;
    QQuickItem *rotate90CcwToolButton = nullptr;
    QQuickItem *rotate90CwToolButton = nullptr;
    QQuickItem *flipHorizontallyToolButton = nullptr;
    QQuickItem *flipVerticallyToolButton = nullptr;
    QQuickItem *undoToolButton = nullptr;
    QQuickItem *redoToolButton = nullptr;
    QQuickItem *splitScreenToolButton = nullptr;
    QQuickItem *lockSplitterToolButton = nullptr;
    QQuickItem *penForegroundColourButton = nullptr;
    QQuickItem *penBackgroundColourButton = nullptr;
    QQuickItem *lighterButton = nullptr;
    QQuickItem *darkerButton = nullptr;
    QQuickItem *saturateButton = nullptr;
    QQuickItem *desaturateButton = nullptr;
    QQuickItem *swatchesPanel = nullptr;
    QQuickItem *tilesetSwatchPanel = nullptr;
    QQuickItem *tilesetSwatchFlickable = nullptr;
    QQuickItem *newLayerButton = nullptr;
    QQuickItem *duplicateLayerButton = nullptr;
    QQuickItem *moveLayerDownButton = nullptr;
    QQuickItem *moveLayerUpButton = nullptr;
    QQuickItem *animationPlayPauseButton = nullptr;
    QQuickItem *fullScreenToolButton = nullptr;

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
