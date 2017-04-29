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

#include <QCursor>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QtTest>
#include <QQuickItemGrabResult>
#include <QQuickWindow>

#include "application.h"
#include "applypixelpencommand.h"
#include "tilecanvas.h"
#include "project.h"
#include "testhelper.h"
#include "tileset.h"
#include "utils.h"

class tst_App : public TestHelper
{
    Q_OBJECT
    
public:
    tst_App(int &argc, char **argv);

private Q_SLOTS:
    void newProjectWithNewTileset();
    void openClose_data();
    void openClose();
    void save();
    void saveAsAndLoad();
    void keyboardShortcuts();
    void optionsCancelled();
    void showGrid();
    void undoPixels();
    void undoLargePixelPen();
    void undoTiles();
    void undoWithDuplicates();
    void undoTilesetCanvasSizeChange();
    void undoImageCanvasSizeChange();
    void undoPixelFill();
    void undoTileFill();
    void colours();
    void panes();
    void altEyedropper();
    void eyedropper();
    void zoomAndPan();
    void zoomAndCentre();
    void penWhilePannedAndZoomed_data();
    void penWhilePannedAndZoomed();
    void useTilesetSwatch();
    void tilesetSwatchContextMenu();
    void tilesetSwatchNavigation();
    void cursorShapeAfterClickingLighter();
//    void colourPickerHexField();
    void eraseImageCanvas();
    void selectionToolImageCanvas();
};

tst_App::tst_App(int &argc, char **argv) :
    TestHelper(argc, argv)
{
}

void tst_App::newProjectWithNewTileset()
{
    createNewTilesetProject(32, 32, 5, 5);

    // Make sure that any changes are reflected in the image after it's saved.
    // First, establish what we expect the image to look like in the end.
    const int expectedWidth = 32 * 5;
    const int expectedHeight = 32 * 5;
    QCOMPARE(tilesetProject->tileset()->image()->width(), expectedWidth);
    QCOMPARE(tilesetProject->tileset()->image()->height(), expectedHeight);
    QImage expectedTilesetImage(expectedWidth, expectedHeight, tilesetProject->tileset()->image()->format());
    expectedTilesetImage.fill(Qt::white);
    expectedTilesetImage.setPixelColor(10, 10, tileCanvas->penForegroundColour());

    // Draw a tile on.
    switchMode(TileCanvas::TileMode);
    setCursorPosInTiles(0, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(QPoint(0, 0)));
    QVERIFY(tilesetProject->hasUnsavedChanges());

    // Draw a pixel on that tile.
    switchMode(TileCanvas::PixelMode);
    setCursorPosInPixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileset()->image(), expectedTilesetImage);

    // Save the project.
    const QUrl saveFileName = QUrl::fromLocalFile(tempProjectDir->path() + "/mytileset.json");
    tilesetProject->saveAs(saveFileName);
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    // Should save the image at the same location as the project.
    const QString tilesetPath = tempProjectDir->path() + "/mytileset.png";
    QCOMPARE(tilesetProject->tilesetUrl(), QUrl::fromLocalFile(tilesetPath));
    QVERIFY(QFile::exists(tilesetPath));
    QCOMPARE(*tilesetProject->tileset()->image(), expectedTilesetImage);
}

void tst_App::openClose_data()
{
    addAllProjectTypes();
}

void tst_App::openClose()
{
    QFETCH(Project::Type, projectType);

    // Create a new, valid project.
    createNewProject(projectType);

    if (projectType == Project::TilesetType) {
        // Test an invalid tileset URL.
        QTest::ignoreMessage(QtWarningMsg, "QFSFileEngine::open: No file name specified");
        project->load(QUrl("doesnotexist"));
        QCOMPARE(project->url(), QUrl());
        // There was a project open before we attempted to load the invalid one.
        QCOMPARE(project->hasLoaded(), true);
        const QString errorMessage = QLatin1String("Tileset project files must have a .json extension ()");
        QCOMPARE(creationErrorOccurredSpy->size(), 1);
        QCOMPARE(creationErrorOccurredSpy->at(0).at(0).toString(), errorMessage);
        const QObject *errorPopup = findPopupFromTypeName("ErrorPopup");
        QVERIFY(errorPopup);
        QVERIFY(errorPopup->property("visible").toBool());
        QCOMPARE(errorPopup->property("text").toString(), errorMessage);
        QVERIFY(errorPopup->property("focus").isValid());
        QVERIFY(errorPopup->property("focus").toBool());
        creationErrorOccurredSpy->clear();

        // Check that the cursor goes back to an arrow when there's a modal popup visible,
        // even if the mouse is over the canvas and not the popup.
        QTest::mouseMove(window, canvas->mapToScene(QPointF(0, 0)).toPoint());
        QVERIFY(!canvas->hasActiveFocus());
        QCOMPARE(window->cursor().shape(), Qt::ArrowCursor);

        // Close the error message popup.
        QTest::mouseClick(window, Qt::LeftButton);
        QVERIFY(!errorPopup->property("visible").toBool());
    }

    // Check that the cursor goes blank when the canvas has focus.
    QVERIFY(canvas->hasActiveFocus());
    QTRY_COMPARE(window->cursor().shape(), Qt::BlankCursor);

    // Test closing a valid project.
    project->close();
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(project->url(), QUrl());
    QCOMPARE(project->hasLoaded(), false);

    // Hovering over the canvas should result in the default cursor being displayed.
    QTest::mouseMove(window, canvas->mapToScene(
        QPointF(canvas->width() / 2, canvas->height() / 2)).toPoint());
    QCOMPARE(window->cursor().shape(), Qt::ArrowCursor);

    createNewProject(projectType);
}

void tst_App::save()
{
    createNewTilesetProject();

    // Store a snapshot of the canvas before we alter it.
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalImage = imageGrabber.takeImage();

    // Draw a tile on first.
    switchMode(TileCanvas::TileMode);

    // QTBUG-53466
    setCursorPosInPixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(QPoint(0, 0)));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Save our drawing.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + "/project.json");
    tilesetProject->saveAs(saveUrl);
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != originalImage);

    // Check that what's on disk matches the image that we currently have.
    QImage savedImage(tempTilesetUrl.toLocalFile());
    QVERIFY(!savedImage.isNull());
    QCOMPARE(savedImage, *tilesetProject->tileAt(QPoint(0, 0))->tileset()->image());
    QTRY_VERIFY_WITH_TIMEOUT(!window->title().contains("*"), 10);
}

void tst_App::saveAsAndLoad()
{
    createNewTilesetProject();

    // Save the untouched project.
    const QString originalProjectPath = tempProjectDir->path() + "/project.json";
    tilesetProject->saveAs(QUrl::fromLocalFile(originalProjectPath));
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
    QCOMPARE(tilesetProject->url().toLocalFile(), originalProjectPath);

    // Save the original project file contents, along with a screenshot.
    QByteArray originalProjectFileContents;
    {
        QFile file(originalProjectPath);
        QVERIFY2(file.open(QIODevice::ReadOnly), qPrintable(file.errorString()));
        originalProjectFileContents = file.readAll();
        QVERIFY(!originalProjectFileContents.isEmpty());
    }

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalCanvasImage = imageGrabber.takeImage();

    // Save the project to a new file.
    const QString savedProjectPath = tempProjectDir->path() + "/project2.json";
    tilesetProject->saveAs(QUrl::fromLocalFile(savedProjectPath));
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(tilesetProject->url().toLocalFile(), savedProjectPath);
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), originalCanvasImage);

    switchMode(TileCanvas::TileMode);

    // Draw on the canvas. We want to check that the latest
    // saved project is modified and not the original.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Save our project.
    tilesetProject->save();
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != originalCanvasImage);
    QTRY_VERIFY_WITH_TIMEOUT(!window->title().contains("*"), 10);

    // Check that what's on disk matches the tileset image that we currently have.
    const Tile *modifiedTile = tilesetProject->tileAt(QPoint(0, 0));
    QImage savedImage(modifiedTile->tileset()->fileName());
    QCOMPARE(savedImage, *modifiedTile->tileset()->image());

    // Check that what's on disk for the original project matches the original project that we have.
    {
        QFile file(originalProjectPath);
        QVERIFY(file.open(QIODevice::ReadOnly));
        QCOMPARE(file.readAll(), originalProjectFileContents);
    }

    // Check that what's on disk for the saved project doesn't match that of the original project.
    {
        QFile file(savedProjectPath);
        QVERIFY(file.open(QIODevice::ReadOnly));
        QVERIFY(file.readAll() != originalProjectFileContents);
    }

    // Loading the saved file.
    triggerCloseProject();
    QVERIFY(!tilesetProject->hasLoaded());

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage closedCanvasImage = imageGrabber.takeImage();

    tilesetProject->load(QUrl::fromLocalFile(savedProjectPath));
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != closedCanvasImage);
}

void tst_App::keyboardShortcuts()
{
    createNewTilesetProject();

    QTest::keyPress(window, Qt::Key_1);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);
    QTest::keyRelease(window, Qt::Key_1);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);

    QTest::keyPress(window, Qt::Key_2);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EyeDropperTool);
    QTest::keyRelease(window, Qt::Key_2);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyPress(window, Qt::Key_3);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EraserTool);
    QTest::keyRelease(window, Qt::Key_3);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EraserTool);

    // Open options dialog.
    triggerOptions();
    const QObject *optionsDialog = findPopupFromTypeName("OptionsDialog");
    QVERIFY(optionsDialog);
    QVERIFY(optionsDialog->property("visible").toBool());

    // Open the shortcuts tab.
    QQuickItem *shortcutsTabButton = optionsDialog->findChild<QQuickItem*>("shortcutsTabButton");
    QVERIFY(shortcutsTabButton);
    mouseEventOnCentre(shortcutsTabButton, MouseClick);

    // Give "New Project" shortcut editor focus.
    QQuickItem *newShortcutButton = optionsDialog->findChild<QQuickItem*>("newShortcutButton");
    QVERIFY(newShortcutButton);
    QCOMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    mouseEventOnCentre(newShortcutButton, MouseClick);

    QQuickItem *newShortcutEditor = optionsDialog->findChild<QQuickItem*>("newShortcutEditor");
    QVERIFY(newShortcutEditor);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    // The toolbutton should display the shortcut that has been entered so far (nothing).
    QCOMPARE(newShortcutButton->property("text").toString(), QString());

    // Begin inputting a shortcut.
    QTest::keyClick(window, Qt::Key_M, Qt::ControlModifier);
    QCOMPARE(newShortcutButton->property("text").toString(), QLatin1String("Ctrl+M"));

    // Cancel editing; shortcut shouldn't have changed.
    QTest::keyClick(window, Qt::Key_Escape);
    // The dialog should still be visible: QTBUG-57098
    QVERIFY(optionsDialog->property("visible").toBool());
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());

    // Give "New Project" shortcut editor focus again.
    mouseEventOnCentre(newShortcutButton, MouseClick);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    // Input another shortcut.
    QTest::keyClick(window, Qt::Key_U, Qt::ControlModifier);
    QCOMPARE(newShortcutButton->property("text").toString(), QLatin1String("Ctrl+U"));

    // Press Enter to accept it.
    QTest::keyClick(window, Qt::Key_Return);
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutButton->property("text").toString(), QLatin1String("Ctrl+U"));

    // There was an issue where entering the original shortcut (e.g. Ctrl+N) after
    // having changed it to a new one (e.g. Ctrl+U) would result in the now not-so-new one (Ctrl+U)
    // still being shown instead of the latest one (Ctrl+N).
    mouseEventOnCentre(newShortcutButton, MouseClick);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    QTest::keyClick(window, Qt::Key_N, Qt::ControlModifier);
    QCOMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QTest::keyClick(window, Qt::Key_Return);
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());

    // Close the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
}

void tst_App::optionsCancelled()
{
    // Ensure that cancelling the options dialog after changing a shortcut cancels the shortcut change.
    createNewTilesetProject();

    // Open options dialog.
    triggerOptions();
    const QObject *optionsDialog = findPopupFromTypeName("OptionsDialog");
    QVERIFY(optionsDialog);
    QVERIFY(optionsDialog->property("visible").toBool());

    // Open the shortcuts tab.
    QQuickItem *shortcutsTabButton = optionsDialog->findChild<QQuickItem*>("shortcutsTabButton");
    QVERIFY(shortcutsTabButton);
    mouseEventOnCentre(shortcutsTabButton, MouseClick);

    // Give "New Project" shortcut editor focus.
    QQuickItem *newShortcutButton = optionsDialog->findChild<QQuickItem*>("newShortcutButton");
    QVERIFY(newShortcutButton);
    QCOMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    mouseEventOnCentre(newShortcutButton, MouseClick);

    QQuickItem *newShortcutEditor = optionsDialog->findChild<QQuickItem*>("newShortcutEditor");
    QVERIFY(newShortcutEditor);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    // Begin inputting a shortcut.
    QTest::keyClick(window, Qt::Key_J, Qt::ControlModifier);
    QCOMPARE(newShortcutButton->property("text").toString(), QLatin1String("Ctrl+J"));

    // Press Enter to accept it.
    QTest::keyClick(window, Qt::Key_Return);
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutButton->property("text").toString(), QLatin1String("Ctrl+J"));
    // Shortcut shouldn't change until we hit "OK".
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(!optionsDialog->property("visible").toBool());
    // Cancelling the dialog shouldn't change anything.
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    // Reopen the dialog to make sure that the editor shows the default shortcut.
    triggerOptions();
    QVERIFY(optionsDialog->property("visible").toBool());
    QTRY_COMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    // Close the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
}

void tst_App::showGrid()
{
    createNewTilesetProject();

    // Store a screenshot of the canvas so we can ensure that the grid lines
    // aren't actually visible to the user.
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage withGrid = imageGrabber.takeImage();

    QVERIFY(app.settings()->isGridVisible());
    // Toggle the option.
    triggerGridVisible();
    QVERIFY(!app.settings()->isGridVisible());

    // Close the view menu.
    QTest::keyClick(window, Qt::Key_Escape);

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage withoutGrid = imageGrabber.takeImage();
    QVERIFY(withoutGrid != withGrid);

    // Show the grid again.
    triggerGridVisible();
    QVERIFY(app.settings()->isGridVisible());
}

void tst_App::undoPixels()
{
    createNewTilesetProject();
    switchTool(TileCanvas::PenTool);

    // It's a new project.
    QVERIFY(tilesetProject->canSave());
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    // Save the project so that we can test hasUnsavedChanges.
    tilesetProject->saveAs(QUrl::fromLocalFile(tempProjectDir->path() + "/project.json"));
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!tilesetProject->canSave());
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    switchMode(TileCanvas::TileMode);

    // Draw a tile on.
    setCursorPosInTiles(0, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    // Changes are added as press/move events happen.
    QVERIFY(tilesetProject->hasUnsavedChanges());
    // The macro isn't finished until a release event occurs, and hence we can't undo yet.
    QVERIFY(!tilesetProject->undoStack()->canUndo());
    // The title shouldn't update until canSave() is true.
    QVERIFY(!window->title().contains("*"));
    QVERIFY(!tilesetProject->canSave());

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY2(tilesetProject->tileAt(cursorPos), qPrintable(QString::fromLatin1("No tile at x %1 y %2")
        .arg(cursorPos.x()).arg(cursorPos.y())));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(tilesetProject->undoStack()->canUndo());
    QVERIFY(window->title().contains("*"));
    QVERIFY(tilesetProject->canSave());

    switchMode(TileCanvas::PixelMode);

    // Draw on some pixels of that tile.
    const QImage originalImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    QImage lastImage;
    QTest::mouseMove(window, cursorPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(*tilesetProject->tileAt(cursorPos)->tileset()->image() != lastImage);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    setCursorPosInPixels(cursorPos + QPoint(0, 1));
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(*tilesetProject->tileAt(cursorPos)->tileset()->image() != lastImage);

    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), originalImage);
    // Still have the tile pen changes.
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test reverting.
    triggerRevert();
    QVERIFY(!tilesetProject->tileAt(cursorPos));
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
}

void tst_App::undoLargePixelPen()
{
    createNewTilesetProject();

    switchMode(TileCanvas::TileMode);

    // Draw the first tile on the canvas.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(tilesetProject->tileAt(cursorPos));

    // Select the second tile from the top-left in the swatch.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = tilesetProject->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(tileCanvas->penTile(), expectedTile);

    // Draw that on next to the first one.
    setCursorPosInTiles(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(tilesetProject->tileAt(tileCanvasCentre(0, 0)) != tilesetProject->tileAt(tileCanvasCentre(1, 0)));

    switchMode(TileCanvas::PixelMode);

    const QImage originalTilesetImage = *tilesetProject->tileset()->image();

    const int toolSize = tilesetProject->tileWidth();
    changeToolSize(toolSize);

    // Draw a large square.
    setCursorPosInPixels(toolSize, toolSize / 2);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    const qreal halfToolSize = qRound(toolSize / 2.0);
    // Test that both tiles were drawn on by checking each corner of the drawn square.
    const QPoint sceneTopLeft(cursorPos - QPoint(halfToolSize, 0));
    QCOMPARE(tilesetProject->tileAt(sceneTopLeft)->pixelColor(halfToolSize, 0).isValid(), true);
    QCOMPARE(tilesetProject->tileAt(sceneTopLeft)->pixelColor(halfToolSize, 0), QColor(Qt::black));

    const QPoint sceneBottomLeft(cursorPos - QPoint(halfToolSize, halfToolSize - 1));
    QCOMPARE(tilesetProject->tileAt(sceneBottomLeft)->pixelColor(halfToolSize, halfToolSize - 1).isValid(), true);
    QCOMPARE(tilesetProject->tileAt(sceneBottomLeft)->pixelColor(halfToolSize, halfToolSize - 1), QColor(Qt::black));

    const QPoint sceneTopRight(cursorPos + QPoint(halfToolSize, 0));
    QCOMPARE(tilesetProject->tileAt(sceneTopRight)->pixelColor(halfToolSize - 1, 0).isValid(), true);
    QCOMPARE(tilesetProject->tileAt(sceneTopRight)->pixelColor(halfToolSize - 1, 0), QColor(Qt::black));

    const QPoint sceneBottomRight(cursorPos + QPoint(halfToolSize, halfToolSize - 1));
    QCOMPARE(tilesetProject->tileAt(sceneBottomRight)->pixelColor(halfToolSize - 1, halfToolSize - 1).isValid(), true);
    QCOMPARE(tilesetProject->tileAt(sceneBottomRight)->pixelColor(halfToolSize - 1, halfToolSize - 1), QColor(Qt::black));

    // Undo the change and check that it worked.
    mouseEventOnCentre(undoButton, MouseClick);

    QCOMPARE(*tilesetProject->tileset()->image(), originalTilesetImage);
}

void tst_App::undoTiles()
{
    createNewTilesetProject();

    switchMode(TileCanvas::TileMode);

    // Move the cursor away so we have an image we can compare against other grabbed images later on.
    setCursorPosInTiles(0, 2);
    const QPoint outsideCanvas = cursorWindowPos - QPoint(tilesetProject->tileWidth(), 0);
    QTest::mouseMove(window, outsideCanvas);
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalCanvasImage = imageGrabber.takeImage();

    setCursorPosInTiles(0, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    // The macro isn't finished until a release event occurs, and hence
    // we can't undo yet.
    QVERIFY(!undoButton->isEnabled());
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    // Check that undoing merged commands (drawing the same tile at the same position) works.
    setCursorPosInPixels(cursorPos.x(), cursorPos.y() + 1);
    Tile *lastTile = tilesetProject->tileAt(cursorPos);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos), lastTile);

    // Go to another tile.
    setCursorPosInTiles(0, 2);
    lastTile = tilesetProject->tileAt(cursorPos);
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos) != lastTile);

    // Release, ending the macro composition.
    lastTile = tilesetProject->tileAt(cursorPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos), lastTile);
    QVERIFY(undoButton->isEnabled());
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test the undo button.
    mouseEventOnCentre(undoButton, MouseClick);
    QVERIFY(!tilesetProject->tileAt(cursorPos));
    QVERIFY(!tilesetProject->tileAt(cursorPos - QPoint(0, tilesetProject->tileHeight())));
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
    // Move the mouse away so the tile pen preview doesn't affect
    // our check that the canvas is actually updated.
    QTest::mouseMove(window, outsideCanvas);
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), originalCanvasImage);

    // Draw a tile back so we can test the revert button.
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test reverting.
    triggerRevert();
    QVERIFY(!tilesetProject->tileAt(cursorPos));
    QVERIFY(!undoButton->isEnabled());
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
}

// Test going back over the same pixels several times.
void tst_App::undoWithDuplicates()
{
    createNewTilesetProject();

    switchMode(TileCanvas::TileMode);

    // Draw on a tile so that we can operate on its pixels.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    switchMode(TileCanvas::PixelMode);

    setCursorPosInPixels(0, 1);
    const QImage originalImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    QImage lastImage = originalImage;
    QTest::mouseMove(window, cursorWindowPos);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos)->pixelColor(cursorPos), tileCanvas->penForegroundColour());
    QVERIFY(*tilesetProject->tileAt(cursorPos)->tileset()->image() != lastImage);

    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    setCursorPosInPixels(0, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos)->pixelColor(cursorPos), tileCanvas->penForegroundColour());
    QVERIFY(*tilesetProject->tileAt(cursorPos)->tileset()->image() != lastImage);

    // Go back over the same pixels.
    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    setCursorPosInPixels(0, 1);
    QTest::mouseMove(window, cursorWindowPos);
    tilesetProject->tileAt(cursorPos)->tileset()->image()->save("C:/dev/cur.png");
    lastImage.save("C:/dev/last.png");
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);

    setCursorPosInPixels(0, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);

    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), originalImage);
    // Still have the tile change.
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Now test that going over the same pixels on the same tile but in a different scene
    // position doesn't result in those pixels not being undone.
    switchMode(TileCanvas::TileMode);

    // Draw on another tile next to the existing one.
    setCursorPosInTiles(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));

    switchMode(TileCanvas::PixelMode);

    int x = 0;
    const int y = tilesetProject->tileHeight() / 2;
    setCursorPosInPixels(0, y);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    for (; x < tilesetProject->tileWidth(); ++x) {
        setCursorPosInPixels(x, y);
        QTest::mouseMove(window, cursorWindowPos);
        QCOMPARE(tilesetProject->tileAt(cursorPos)->pixelColor(cursorPos), tileCanvas->penForegroundColour());
    }
    // The last pixel is on the next tile.
    setCursorPosInPixels(++x, y);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos)->pixelColor(cursorPos - QPoint(tilesetProject->tileWidth(), 0)), tileCanvas->penForegroundColour());

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), originalImage);
    // Still have the tile change.
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));
}

void tst_App::undoTilesetCanvasSizeChange()
{
    createNewTilesetProject();

    switchMode(TileCanvas::TileMode);

    setCursorPosInTiles(8, 9);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos), tileCanvas->penTile());

    setCursorPosInTiles(9, 9);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos), tileCanvas->penTile());

    const QVector<int> originalTiles = tilesetProject->tiles();

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preSizeChangeCanvasSnapshot = imageGrabber.takeImage();

    changeCanvasSize(9, 9);
    QVERIFY(tilesetProject->tiles() != originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 9 * 9);

    // Ensure that the canvas was repainted after the size change.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != preSizeChangeCanvasSnapshot);

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(tilesetProject->tiles(), originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 10 * 10);

    // Check that neither of the following assert.
    changeCanvasSize(10, 9);
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(tilesetProject->tiles(), originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 10 * 10);

    changeCanvasSize(9, 10);
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(tilesetProject->tiles(), originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 10 * 10);

    changeCanvasSize(12, 12);
    QCOMPARE(tilesetProject->tiles().size(), 12 * 12);
    QCOMPARE(tilesetProject->tiles().last(), -1);
}

void tst_App::undoImageCanvasSizeChange()
{
    createNewImageProject();

    QCOMPARE(imageProject->widthInPixels(), 256);
    QCOMPARE(imageProject->heightInPixels(), 256);

    setCursorPosInPixels(250, 250);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageProject->image()->pixelColor(cursorPos), imageCanvas->penForegroundColour());

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preSizeChangeCanvasSnapshot = imageGrabber.takeImage();

    changeCanvasSize(200, 200);

    // Ensure that the canvas was repainted after the size change.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != preSizeChangeCanvasSnapshot);

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(imageProject->image()->size(), QSize(256, 256));
}

void tst_App::undoPixelFill()
{
    createNewTilesetProject();

    switchMode(TileCanvas::TileMode);

    // Select a blank tile to draw on.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = tilesetProject->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(tileCanvas->penTile(), expectedTile);

    // Draw the tile on so that we can operate on its pixels.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Draw a block of tiles.
    setCursorPosInPixels(0, 0);
    drawPixelAtCursorPos();

    setCursorPosInPixels(1, 0);
    drawPixelAtCursorPos();

    setCursorPosInPixels(1, 1);
    drawPixelAtCursorPos();

    setCursorPosInPixels(0, 1);
    drawPixelAtCursorPos();

    const Tile *targetTile = tilesetProject->tileAt(cursorPos);
    QVERIFY(targetTile);
    const QColor black = QColor(Qt::black);
    QCOMPARE(targetTile->pixelColor(0, 0), black);
    QCOMPARE(targetTile->pixelColor(1, 0), black);
    QCOMPARE(targetTile->pixelColor(1, 1), black);
    QCOMPARE(targetTile->pixelColor(0, 1), black);

    // Try to fill it. The whole thing should be filled.
    // We do it from the top right because there was a bug where fills
    // wouldn't go downwards.
    switchTool(TileCanvas::FillTool);
    setCursorPosInPixels(1, 0);
    const QColor red = QColor(Qt::red);
    tileCanvas->setPenForegroundColour(red);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(targetTile->pixelColor(0, 0), red);
    QCOMPARE(targetTile->pixelColor(1, 0), red);
    QCOMPARE(targetTile->pixelColor(1, 1), red);
    QCOMPARE(targetTile->pixelColor(0, 1), red);

    // Undo it.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(targetTile->pixelColor(0, 0), black);
    QCOMPARE(targetTile->pixelColor(1, 0), black);
    QCOMPARE(targetTile->pixelColor(1, 1), black);
    QCOMPARE(targetTile->pixelColor(0, 1), black);
}

void tst_App::undoTileFill()
{
    createNewTilesetProject();

    // Draw a block of tiles.
    setCursorPosInTiles(0, 0);
    drawTileAtCursorPos();

    setCursorPosInTiles(1, 0);
    drawTileAtCursorPos();

    const Tile *targetTile = tilesetProject->tileAt(cursorPos);
    QVERIFY(targetTile);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(0, 0)), tileCanvas->penTile());
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(1, 0)), tileCanvas->penTile());

    // Try to fill it. The whole block should be filled.
    switchTool(TileCanvas::FillTool);

    // Select the second tile from the top-left in the swatch.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *replacementTile = tilesetProject->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(tileCanvas->penTile(), replacementTile);

    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(0, 0)), replacementTile);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(1, 0)), replacementTile);

    // Undo it.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(0, 0)), targetTile);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(1, 0)), targetTile);
}

void tst_App::colours()
{
    createNewTilesetProject();
    QCOMPARE(tileCanvas->penForegroundColour(), QColor(Qt::black));
    QCOMPARE(tileCanvas->penBackgroundColour(), QColor(Qt::white));

    // First try changing the foreground colour.
    mouseEvent(penForegroundColourButton, QPoint(1, 1), MouseClick);

    QQuickItem *saturationLightnessPicker = window->findChild<QQuickItem*>("saturationLightnessPicker");
    QVERIFY(saturationLightnessPicker);

    // Choose a colour.
    mouseEventOnCentre(saturationLightnessPicker, MouseClick);
    const QColor expectedColour = QColor("#c04141");
    fuzzyColourCompare(tileCanvas->penForegroundColour(), expectedColour);
    // Background colour shouldn't be affected.
    QCOMPARE(tileCanvas->penBackgroundColour(), QColor(Qt::white));

    // Now try changing the background colour.
    mouseEvent(penBackgroundColourButton,
        QPoint(penBackgroundColourButton->width() - 1, penBackgroundColourButton->width() - 1), MouseClick);

    // Choose a colour.
    mouseEvent(saturationLightnessPicker, QPointF(saturationLightnessPicker->width() * 0.25,
        saturationLightnessPicker->height() * 0.25), MouseClick);
    QVERIFY(tileCanvas->penBackgroundColour() != QColor(Qt::white));
    // Foreground colour shouldn't be affected.
    fuzzyColourCompare(tileCanvas->penForegroundColour(), expectedColour);

    // Hex field should represent background colour when selected.
    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    QVERIFY(hexTextField);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), tileCanvas->penBackgroundColour().name());

    // Hex field should represent foreground colour when selected.
    mouseEventOnCentre(penForegroundColourButton, MouseClick);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), tileCanvas->penForegroundColour().name());
}

void tst_App::panes()
{
    createNewTilesetProject();
    QVERIFY(tileCanvas->firstPane());
    QCOMPARE(tileCanvas->firstPane()->size(), 0.5);
    QVERIFY(tileCanvas->secondPane());
    QCOMPARE(tileCanvas->secondPane()->size(), 0.5);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);

    switchMode(TileCanvas::TileMode);

    setCursorPosInTiles(0, 0);
    Tile *lastTile = tilesetProject->tileAt(cursorPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));
    QVERIFY(tilesetProject->tileAt(cursorPos) != lastTile);

    setCursorPosInPixels(tileCanvas->width() / 2 + tilesetProject->tileWidth() * 1.5, 1);
    cursorPos.setX(cursorPos.x() - tileCanvas->width() / 2);
    lastTile = tilesetProject->tileAt(cursorPos);
    QVERIFY(!lastTile);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(tilesetProject->tileAt(cursorPos) != lastTile);

    // Remove split.
    triggerSplitScreen();
    QVERIFY(!app.settings()->isSplitScreen());
    QVERIFY(!tileCanvas->secondPane());
    QVERIFY(tileCanvas->firstPane());
    QCOMPARE(tileCanvas->firstPane()->size(), 1.0);

    // Add it back again.
    triggerSplitScreen();
    QVERIFY(app.settings()->isSplitScreen());
    QVERIFY(tileCanvas->firstPane());
    QCOMPARE(tileCanvas->firstPane()->size(), 0.5);
    QVERIFY(tileCanvas->secondPane());
    QCOMPARE(tileCanvas->secondPane()->size(), 0.5);
}

void tst_App::altEyedropper()
{
    createNewTilesetProject();
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);

    QTest::keyPress(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyRelease(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);

#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(fileToolButton, MouseClick);
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(window->activeFocusItem(), tileCanvas.data());
#endif

    QTest::keyPress(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyRelease(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);
}

void tst_App::eyedropper()
{
    createNewTilesetProject();

    switchMode(TileCanvas::TileMode);

    setCursorPosInTiles(1, 1);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    Tile *originalTile = tilesetProject->tileAt(cursorPos);
    QVERIFY(originalTile);
    const QPoint pixelPos = QPoint(tilesetProject->tileWidth() / 2, tilesetProject->tileHeight() / 2);
    QVERIFY(tileCanvas->penForegroundColour() != originalTile->pixelColor(pixelPos));

    switchMode(TileCanvas::PixelMode);
    switchTool(TileCanvas::EyeDropperTool);

    QColor lastForegroundColour = tileCanvas->penForegroundColour();
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tileCanvas->penForegroundColour() != lastForegroundColour);
    // TODO: no idea why this doesn't work.. the positions are both {12, 12}.
//    QCOMPARE(canvas->penForegroundColour(), originalTile->pixelColor(pixelPos));

    switchMode(TileCanvas::TileMode);

    // Choose another tile from the swatch.
    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAt(0, 0));

    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = tilesetProject->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(tileCanvas->penTile(), expectedTile);

    setCursorPosInTiles(1, 1);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tileCanvas->penTile(), originalTile);
}

void tst_App::zoomAndPan()
{
    createNewTilesetProject();

    panTopLeftTo(0, 0);

    // Test panning.
    panBy(50, 0);

    // Test zoom.
    QPoint zoomPos = tileSceneCentre(5, 5);
    wheelEvent(tileCanvas, zoomPos, 1);
    QCOMPARE(tileCanvas->currentPane()->zoomLevel(), 2);
}

void tst_App::zoomAndCentre()
{
    createNewTilesetProject();

    // Pan to some non-centered location.
    panTopLeftTo(-100, -100);

    const CanvasPane *currentPane = tileCanvas->currentPane();
    QCOMPARE(currentPane, tileCanvas->firstPane());

    // Zoom in.
    QPoint zoomPos = tileSceneCentre(5, 5);
    const int expectedZoomLevel = 5;
    for (int i = 0; currentPane->zoomLevel() < expectedZoomLevel && i < expectedZoomLevel; ++i)
        wheelEvent(tileCanvas, zoomPos, 1);
    QCOMPARE(currentPane->zoomLevel(), expectedZoomLevel);

    triggerCentre();
    const QPoint expectedOffset(
        currentPane->size() * tileCanvas->width() / 2 - (tilesetProject->widthInPixels() * currentPane->zoomLevel()) / 2,
        tileCanvas->height() / 2 - (tilesetProject->heightInPixels() * currentPane->zoomLevel()) / 2);
    // A one pixel difference was introduced here at some point.. not sure why, but it's not important.
    const int xDiff = qAbs(currentPane->offset().x() - expectedOffset.x());
    const int yDiff = qAbs(currentPane->offset().y() - expectedOffset.y());
    QVERIFY(xDiff <= 1);
    QVERIFY(yDiff <= 1);
}

void tst_App::penWhilePannedAndZoomed_data()
{
    QTest::addColumn<Project::Type>("projectType");
    QTest::addColumn<int>("xDistance");
    QTest::addColumn<int>("yDistance");
    QTest::addColumn<int>("zoomLevel");

    // TODO: test with zoom
    for (int i = Project::TilesetType; i <= Project::ImageType; ++i) {
        const Project::Type type = static_cast<Project::Type>(i);
        const QString typeAsString = Project::typeToString(type);
        QTest::newRow(qPrintable(QString::fromLatin1("%1, {40, 0}, 1").arg(typeAsString))) << type << 40 << 0 << 1;
        QTest::newRow(qPrintable(QString::fromLatin1("%1, {0, 40}, 2").arg(typeAsString))) << type << 0 << 40 << 1;
        QTest::newRow(qPrintable(QString::fromLatin1("%1, {40, 40}, 3").arg(typeAsString))) << type << 40 << 40 << 1;
        QTest::newRow(qPrintable(QString::fromLatin1("%1, {-40, 0}, 3").arg(typeAsString))) << type << -40 << 0 << 1;
        QTest::newRow(qPrintable(QString::fromLatin1("%1, {0, 0}, 2").arg(typeAsString))) << type << 0 << -40 << 1;
        QTest::newRow(qPrintable(QString::fromLatin1("%1, {-40, -40}, 2").arg(typeAsString))) << type << -40 << -40 << 1;
    }
}

void tst_App::penWhilePannedAndZoomed()
{
    QFETCH(Project::Type, projectType);
    QFETCH(int, xDistance);
    QFETCH(int, yDistance);
    QFETCH(int, zoomLevel);

    createNewProject(projectType);
    panTopLeftTo(0, 0);
    panBy(xDistance, yDistance);

    if (zoomLevel > 1) {
        for (int i = 0; i < zoomLevel - canvas->currentPane()->zoomLevel(); ++i) {
            wheelEvent(canvas, tileSceneCentre(5, 5), 1);
        }
        QCOMPARE(canvas->currentPane()->zoomLevel(), zoomLevel);
    } else if (zoomLevel < 1) {
        for (int i = 0; i < qAbs(zoomLevel - canvas->currentPane()->zoomLevel()); ++i) {
            wheelEvent(canvas, tileSceneCentre(5, 5), -1);
        }
        QCOMPARE(canvas->currentPane()->zoomLevel(), zoomLevel);
    }

    if (projectType == Project::TilesetType) {
        switchMode(TileCanvas::TileMode);

        // Draw a tile on.
        setCursorPosInTiles(4, 4);
        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY(tilesetProject->hasUnsavedChanges());

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY2(tilesetProject->tileAt(cursorPos), qPrintable(QString::fromLatin1("No tile at x %1 y %2")
                                                               .arg(cursorPos.x()).arg(cursorPos.y())));
    } else {
        // Draw a pixel on.
        // TOOD: x=100 y=100 is a quick, hard-coded, hacky coordinate.
        // If the test fails, it's probably because of this not being big enough/too big. Do it properly.
        setCursorPosInPixels(100, 100, false);
        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY(imageProject->hasUnsavedChanges());

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
//        QVERIFY2(imageProject->tileAt(cursorPos), qPrintable(QString::fromLatin1("No tile at x %1 y %2")
//                                                               .arg(cursorPos.x()).arg(cursorPos.y())));
    }
}

void tst_App::useTilesetSwatch()
{
    createNewTilesetProject();

    switchMode(TileCanvas::TileMode);

    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAt(0, 0));

    // Select the second tile from the top-left in the swatch.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = tilesetProject->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(tileCanvas->penTile(), expectedTile);

    // Draw it on the canvas.
    setCursorPosInTiles(1, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QCOMPARE(tilesetProject->tileAt(cursorPos), expectedTile);

    // Draw some pixels on the tile we just painted onto the canvas.
    switchMode(TileCanvas::PixelMode);
    // Make sure that the pixel's colour will actually change.
    const QPoint pixelPos = QPoint(tilesetProject->tileWidth() / 2, tilesetProject->tileHeight() / 2);
    QVERIFY(tileCanvas->penForegroundColour() != expectedTile->tileset()->image()->pixelColor(pixelPos));

    // Take a snapshot of the swatch to make sure that it's actually updated.
    QVERIFY(imageGrabber.requestImage(tilesetSwatch));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalTilesetSnapshot = imageGrabber.takeImage();

    const QImage originalTilesetImage = *expectedTile->tileset()->image();
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(*expectedTile->tileset()->image() != originalTilesetImage);

    QVERIFY(imageGrabber.requestImage(tilesetSwatch));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != originalTilesetSnapshot);
}

void tst_App::tilesetSwatchContextMenu()
{
#ifdef NON_NATIVE_MENUS
    createNewTilesetProject();

    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAt(0, 0));

    // Test clicking outside of the menu to cancel it.

    const Tile *originallySelectedTile = tileCanvas->penTile();
    // Open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
    QObject *tilesetContextMenu = window->findChild<QObject*>("tilesetContextMenu");
    QVERIFY(tilesetContextMenu);
    QVERIFY(tilesetContextMenu->property("visible").toBool());

    const QPoint outsidePos = tilesetTileSceneCentre(tilesetProject->tileset()->tilesWide() - 1, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, outsidePos);
    QVERIFY(!tilesetContextMenu->property("visible").toBool());
    // The selected tile shouldn't have changed.
    QCOMPARE(tileCanvas->penTile(), originallySelectedTile);

    // Test duplicating a tile.

    // Open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
    QVERIFY(tilesetContextMenu->property("visible").toBool());

    // Click the "duplicate" menu item.
    mouseEventOnCentre(duplicateTileMenuButton, MouseClick);
    QCOMPARE(tileCanvas->penTile(), originallySelectedTile);
    QVERIFY(!tilesetContextMenu->property("visible").toBool());

    // Duplicate the tile.
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 1));
    // It should be the tile we right clicked on.
    QCOMPARE(tilesetProject->tilesetTileAt(0, 0), originallySelectedTile);
    // The selected tile shouldn't have changed.
    QCOMPARE(tileCanvas->penTile(), originallySelectedTile);

    // Test rotating a tile left.

    // Draw the tile that we're rotating onto the canvas and then
    // take a snapshot of the canvas to make sure that it's actually updated.
    switchMode(TileCanvas::TileMode);
    setCursorPosInTiles(0, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QImage lastCanvasSnapshot = imageGrabber.takeImage();

    // Open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
    QVERIFY(tilesetContextMenu->property("visible").toBool());

    // Click the "rotate left" menu item.
    const QImage originalTileImage = tileCanvas->penTile()->image();
    mouseEventOnCentre(rotateTileLeftMenuButton, MouseClick);
    QVERIFY(!tilesetContextMenu->property("visible").toBool());
    QCOMPARE(Utils::rotate(tileCanvas->penTile()->image(), 90), originalTileImage);

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QImage currentImage = imageGrabber.takeImage();
    QVERIFY(currentImage != lastCanvasSnapshot);
    lastCanvasSnapshot = currentImage;

    // Test rotating a tile right.

    // Open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
    QVERIFY(tilesetContextMenu->property("visible").toBool());

    // Click the "rotate right" menu item.
    mouseEventOnCentre(rotateTileRightMenuButton, MouseClick);
    QVERIFY(!tilesetContextMenu->property("visible").toBool());
    QCOMPARE(tileCanvas->penTile()->image(), originalTileImage);

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    currentImage = imageGrabber.takeImage();
    QVERIFY(currentImage != lastCanvasSnapshot);
#endif
}

void tst_App::tilesetSwatchNavigation()
{
    createNewTilesetProject();

    const Qt::Key leftKey = Qt::Key_A;
    const Qt::Key rightKey = Qt::Key_D;
    const Qt::Key upKey = Qt::Key_W;
    const Qt::Key downKey = Qt::Key_S;

    QPoint tilePos(0, 0);
    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));

    // Already at left edge; shouldn't do anything.
    QTest::keyClick(window, leftKey);
    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));

    // Move all the way to the right edge.
    while (++tilePos.rx() < tilesetProject->tileset()->tilesWide()) {
        QTest::keyClick(window, rightKey);
        QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));
    }
    // The loop leaves us with an illegal position.
    --tilePos.rx();
    QCOMPARE(tilePos, QPoint(tilesetProject->tileset()->tilesWide() - 1, 0));

    // Already at right edge; shouldn't do anything.
    QTest::keyClick(window, rightKey);
    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));


    // Move all the way to the bottom edge.
    while (++tilePos.ry() < tilesetProject->tileset()->tilesHigh()) {
        QTest::keyClick(window, downKey);
        QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));
    }
    // The loop leaves us with an illegal position.
    --tilePos.ry();
    QCOMPARE(tilePos, QPoint(tilesetProject->tileset()->tilesWide() - 1, tilesetProject->tileset()->tilesHigh() - 1));

    // Already at bottom edge; shouldn't do anything.
    QTest::keyClick(window, downKey);
    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));


    // Move all the way to the top edge.
    while (--tilePos.ry() >= 0) {
        QTest::keyClick(window, upKey);
        QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));
    }
    // The loop leaves us with an illegal position.
    ++tilePos.ry();
    QCOMPARE(tilePos, QPoint(tilesetProject->tileset()->tilesWide() - 1, 0));

    // Already at top edge; shouldn't do anything.
    QTest::keyClick(window, upKey);
    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAtTilePos(tilePos));
}

void tst_App::cursorShapeAfterClickingLighter()
{
//    createNewProject();

//    QQuickItem *lighterButton = window->findChild<QQuickItem*>("lighterButton");
//    QVERIFY(lighterButton);
//    mouseEventOnCentre(lighterButton, MouseClick);

//    setCursorPosInTiles(0, 0);
    //    QTRY_COMPARE(window->cursor().shape(), Qt::BlankCursor);
}

//void tst_App::colourPickerHexField()
//{
//    createNewProject();

//    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
//    QVERIFY(hexTextField);
//    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penForegroundColour().name());

//    const QColor originalPenColour = canvas->penForegroundColour();

//    mouseEventOnCentre(hexTextField, MouseClick);
//    QCOMPARE(hexTextField->property("activeFocus").toBool(), true);

//    keySequence(window, QKeySequence::SelectAll);
//    QTest::keyClick(window, Qt::Key_Backspace);
//    QCOMPARE(hexTextField->property("text").toString(), QString());
//    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

//    QTest::keyClick(window, Qt::Key_A);
//    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a"));
//    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

//    // Invalid character.
//    QTest::keyClick(window, Qt::Key_Z);
//    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a"));
//    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

//    QTest::keyClick(window, Qt::Key_1);
//    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a1"));
//    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

//    QTest::keyClick(window, Qt::Key_2);
//    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a12"));
//    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

//    QTest::keyClick(window, Qt::Key_3);
//    QTest::keyClick(window, Qt::Key_4);
//    QTest::keyClick(window, Qt::Key_5);
//    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a12345"));
//    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

//    QTest::keyClick(window, Qt::Key_Enter);
//    // TODO: SaturationLightnessPicker::setColor()'s RGB => HSL transformation
//    // is altering the colour value that was input.
//    QCOMPARE(canvas->penForegroundColour(), QColor("a12345"));
//}

void tst_App::eraseImageCanvas()
{
    createNewImageProject();

    switchTool(ImageCanvas::EraserTool);
    changeToolSize(1);

    // Make sure that the edges of the canvas can be erased.
    setCursorPosInPixels(project->widthInPixels() - 1, 0);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    QCOMPARE(imageProject->image()->pixelColor(cursorPos), QColor(Qt::transparent));
}

struct SelectionData
{
    SelectionData(const QPoint &pressScenePos = QPoint(),
            const QPoint &releaseScenePos = QPoint(),
            const QRect &expectedSelectionArea = QRect()) :
        pressScenePos(pressScenePos),
        releaseScenePos(releaseScenePos),
        expectedSelectionArea(expectedSelectionArea)
    {
    }

    QString toString() const;

    QPoint pressScenePos;
    QPoint releaseScenePos;
    QRect expectedSelectionArea;
};

QDebug Q_QUICK_EXPORT operator<<(QDebug debug, const SelectionData &data)
{
    debug << "press:" << data.pressScenePos
          << "release:" << data.releaseScenePos
          << "expected area:" << data.expectedSelectionArea;
    return debug;
}

QString toString(const SelectionData &data)
{
    QString string;
    QDebug stringBuilder(&string);
    stringBuilder << data;
    return string;
}

QString toString(const QRect &rect)
{
    QString string;
    QDebug stringBuilder(&string);
    stringBuilder << rect;
    return string;
}

QByteArray selectionAreaFailureMessage(ImageCanvas *canvas, const SelectionData &selectionData, const QRect &expectedArea)
{
    return qPrintable(QString::fromLatin1("Data: %1 \n      Actual area: %2\n    Expected area: %3")
        .arg(toString(selectionData), toString(canvas->selectionArea()), toString(expectedArea)));
}

void tst_App::selectionToolImageCanvas()
{
    createNewImageProject();

    switchTool(ImageCanvas::SelectionTool);

    // We don't want to use a _data() function for this, because we don't need
    // to create a new project every time.
    QVector<SelectionData> selectionData;
    selectionData << SelectionData(QPoint(-10, -10), QPoint(10, 10), QRect(0, 0, 10, 10));
    selectionData << SelectionData(QPoint(-10, 0), QPoint(10, 10), QRect(0, 0, 10, 10));
    selectionData << SelectionData(QPoint(0, -10), QPoint(10, 10), QRect(0, 0, 10, 10));
    selectionData << SelectionData(QPoint(project->widthInPixels() + 10, project->heightInPixels() + 10),
        QPoint(project->widthInPixels() - 10, project->widthInPixels() - 10),
        QRect(project->widthInPixels() - 10, project->widthInPixels() - 10, 10, 10));

    foreach (const SelectionData &data, selectionData) {
        // Pressing outside the canvas should make the selection start at {0, 0}.
        setCursorPosInPixels(data.pressScenePos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        const QPoint boundExpectedPressPos = QPoint(qBound(0, data.pressScenePos.x(), project->widthInPixels()),
            qBound(0, data.pressScenePos.y(), project->heightInPixels()));
        const QRect expectedPressArea = QRect(boundExpectedPressPos, QSize(0, 0));
        QVERIFY2(canvas->selectionArea() == expectedPressArea, selectionAreaFailureMessage(canvas, data, expectedPressArea));

        setCursorPosInPixels(data.releaseScenePos);
        QTest::mouseMove(window, cursorWindowPos);
        QVERIFY2(canvas->selectionArea() == data.expectedSelectionArea, selectionAreaFailureMessage(canvas, data, data.expectedSelectionArea));

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY2(canvas->selectionArea() == data.expectedSelectionArea, selectionAreaFailureMessage(canvas, data, data.expectedSelectionArea));
    }

    // Switching tools should clear the selection.
//    switchTool(ImageCanvas::PenTool);
//    QCOMPARE(canvas->selectionArea(), QRect());
}

int main(int argc, char *argv[])
{
    tst_App test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "tst_app.moc"
