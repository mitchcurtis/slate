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
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>

#include "application.h"
#include "imagecanvas.h"
#include "imageproject.h"
#include "tilecanvas.h"
#include "tilesetproject.h"
#include "project.h"
#include "tilesetproject.h"

class ProjectManager;

#define VERIFY_NO_CREATION_ERRORS_OCCURRED() \
QVERIFY2(creationErrorOccurredSpy->isEmpty(), \
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

    void resetCreationErrorSpy();

protected:
    enum TestMouseEventType
    {
        MousePress,
        MouseRelease,
        MouseClick,
    };
    void mouseEventOnCentre(QQuickItem *item, TestMouseEventType eventType);
    void mouseEvent(QQuickItem *item, const QPointF &localPos, TestMouseEventType eventType);
    void wheelEvent(QQuickItem *item, const QPoint &localPos, const int degrees);

    QObject *findPopupFromTypeName(const QString &typeName) const;
    QQuickItem *findDialogButton(const QObject *dialog, const QString &text);
    // Returns the position of the cursor in a tile's local coordinates.
    QPoint mapToTile(const QPoint &cursorPos) const;
    QPoint tileSceneCentre(int xPosInTiles, int yPosInTiles) const;
    QPoint tileCanvasCentre(int xPosInTiles, int yPosInTiles) const;
    QPointF canvasCentre() const;
    QPoint canvasSceneCentre() const;
    QPoint firstPaneSceneCentre() const;
    void setCursorPosInTiles(int xPosInTiles, int yPosInTiles);
    void setCursorPosInPixels(int xPosInPixels, int yPosInPixels, bool assertWithinWindow = true);
    void setCursorPosInPixels(const QPoint &posInPixels);
    QPoint tilesetTileCentre(int xPosInTiles, int yPosInTiles) const;
    QPoint tilesetTileSceneCentre(int xPosInTiles, int yPosInTiles) const;
    // Replace with result of QTBUG-53381 (if it ever gets added).
    void keySequence(QWindow *window, QKeySequence sequence);
    int digits(int number);
    int digitAt(int number, int index);

    void addAllProjectTypes();
    void createNewProject(Project::Type projectType, const QVariantMap &args = QVariantMap());
    void createNewTilesetProject(int tileWidth = 25, int tileHeight = 25,
        int tilesetTilesWide = -1, int tilesetTilesHigh = -1, bool transparentTilesetBackground = false);
    void createNewImageProject(int imageWidth = 256, int imageHeight = 256,
        bool transparentImageBackground = false);
    void setupTempTilesetProjectDir();
    void switchMode(TileCanvas::Mode mode);
    void switchTool(ImageCanvas::Tool tool);
    void panTopLeftTo(int x, int y);
    void panBy(int xDistance, int yDistance);
    void changeCanvasSize(int width, int height);
    void changeToolSize(int size);
    int sliderValue(QQuickItem *slider) const;
    void drawPixelAtCursorPos();
    void drawTileAtCursorPos();
    void fuzzyColourCompare(const QColor &colour1, const QColor &colour2, int fuzz = 1);
    void fuzzyImageCompare(const QImage &image1, const QImage &image2);

    Application app;
    QQuickWindow *window;
    QQuickItem *overlay;
    QPointer<ProjectManager> projectManager;
    QPointer<Project> project;
    QPointer<ImageProject> imageProject;
    QPointer<TilesetProject> tilesetProject;
    QPointer<ImageCanvas> canvas;
    QPointer<ImageCanvas> imageCanvas;
    QPointer<TileCanvas> tileCanvas;
    QQuickItem *fileToolButton;
    QQuickItem *optionsToolButton;
    QQuickItem *viewToolButton;
    QQuickItem *newMenuButton;
    QQuickItem *closeMenuButton;
    QQuickItem *openMenuButton;
    QQuickItem *saveMenuButton;
    QQuickItem *saveAsMenuButton;
    QQuickItem *settingsMenuButton;
    QQuickItem *revertMenuButton;
    QQuickItem *centreMenuButton;
    QQuickItem *showGridMenuButton;
    QQuickItem *splitScreenMenuButton;
    QQuickItem *canvasSizeButton;
    QQuickItem *modeToolButton;
    QQuickItem *penToolButton;
    QQuickItem *eyeDropperToolButton;
    QQuickItem *fillToolButton;
    QQuickItem *eraserToolButton;
    QQuickItem *toolSizeButton;
    QQuickItem *undoButton;
    QQuickItem *redoButton;
    QQuickItem *penForegroundColourButton;
    QQuickItem *penBackgroundColourButton;
    QQuickItem *tilesetSwatch;
    QQuickItem *duplicateTileMenuButton;
    QQuickItem *rotateTileLeftMenuButton;
    QQuickItem *rotateTileRightMenuButton;

    QScopedPointer<QSignalSpy> creationErrorOccurredSpy;

    QString tilesetBasename;
    QUrl tempTilesetUrl;
    QScopedPointer<QTemporaryDir> tempProjectDir;

    ImageGrabber imageGrabber;

    QPoint cursorPos;
    QPoint cursorWindowPos;
};

#endif // TESTHELPER_H
