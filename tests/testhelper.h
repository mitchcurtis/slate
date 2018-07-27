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

#define QVERIFY_NO_CREATION_ERRORS_OCCURRED() \
QVERIFY2(creationErrorOccurredSpy->isEmpty(), \
    qPrintable(QString::fromLatin1("Unexpected error occurred: ") + \
    (!creationErrorOccurredSpy->isEmpty() ? creationErrorOccurredSpy->first().first().toString() : "")));

#define VERIFY_NO_CREATION_ERRORS_OCCURRED() \
VERIFY2(creationErrorOccurredSpy->isEmpty(), \
    qPrintable(QString::fromLatin1("Unexpected error occurred: ") + \
    (!creationErrorOccurredSpy->isEmpty() ? creationErrorOccurredSpy->first().first().toString() : "")));

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
    void mouseEventOnCentre(QQuickItem *item, TestMouseEventType eventType);
    void mouseEvent(QQuickItem *item, const QPointF &localPos, TestMouseEventType eventType,
        Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(), int delay = -1);
    void wheelEvent(QQuickItem *item, const QPoint &localPos, const int degrees);

    QObject *findPopupFromTypeName(const QString &typeName) const;
    QQuickItem *findDialogButtonFromText(const QObject *dialog, const QString &text);
    QQuickItem *findDialogButtonFromObjectName(const QObject *dialog, const QString &objectName);
    QQuickItem *findListViewChild(QQuickItem *listView, const QString &childObjectName) const;
    QQuickItem *findListViewChild(const QString &listViewObjectName, const QString &childObjectName) const;
    static QQuickItem *findChildWithText(QQuickItem *item, const QString &text);

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
    // Replace with result of QTBUG-53381 (if it ever gets added).
    void keySequence(QWindow *window, QKeySequence sequence);
    int digits(int number);
    int digitAt(int number, int index);

    enum InputType {
        MouseInputType,
        KeyboardInputType
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
    // and returns the delegate for convenience.
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
    Q_REQUIRED_RESULT bool loadProject(const QUrl &url);
    Q_REQUIRED_RESULT bool updateVariables(bool isNewProject, Project::Type newProjectType);

    Q_REQUIRED_RESULT bool setupTempTilesetProjectDir();
    Q_REQUIRED_RESULT bool setupTempLayeredImageProjectDir();
    Q_REQUIRED_RESULT bool setupTempProjectDir(const QStringList &resourceFilesToCopy = QStringList(),
        QStringList *filesCopied = nullptr);

    Q_REQUIRED_RESULT bool switchMode(TileCanvas::Mode mode);
    Q_REQUIRED_RESULT bool switchTool(ImageCanvas::Tool tool, InputType inputType = MouseInputType);
    Q_REQUIRED_RESULT bool setPenForegroundColour(QString argbString);
    Q_REQUIRED_RESULT bool panTopLeftTo(int x, int y);
    Q_REQUIRED_RESULT bool panBy(int xDistance, int yDistance);
    Q_REQUIRED_RESULT bool zoomTo(int zoomLevel);
    Q_REQUIRED_RESULT bool zoomTo(int zoomLevel, const QPoint &pos);
    Q_REQUIRED_RESULT bool changeCanvasSize(int width, int height);
    Q_REQUIRED_RESULT bool changeImageSize(int width, int height);
    Q_REQUIRED_RESULT bool changeToolSize(int size);
    Q_REQUIRED_RESULT bool moveContents(int x, int y, bool onlyVisibleLayers);
    int sliderValue(QQuickItem *slider) const;
    Q_REQUIRED_RESULT bool drawPixelAtCursorPos();
    Q_REQUIRED_RESULT bool drawTileAtCursorPos();
    Q_REQUIRED_RESULT bool fuzzyColourCompare(const QColor &colour1, const QColor &colour2, int fuzz = 1);
    Q_REQUIRED_RESULT bool fuzzyImageCompare(const QImage &image1, const QImage &image2);

    QByteArray failureMessage;

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
    QQuickItem *tilesetSwatch;
    QQuickItem *tilesetSwatchFlickable;
    QQuickItem *newLayerButton;
    QQuickItem *moveLayerDownButton;
    QQuickItem *moveLayerUpButton;
    QQuickItem *animationPlayPauseButton;

    QScopedPointer<QSignalSpy> creationErrorOccurredSpy;

    QString tilesetBasename;
    QUrl tempTilesetUrl;
    QScopedPointer<QTemporaryDir> tempProjectDir;

    ImageGrabber imageGrabber;

    QPoint cursorPos;
    QPoint cursorWindowPos;

    QImage mCheckerImage;
};

#endif // TESTHELPER_H
