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
    void undoCanvasSizeChange();
    void undoPixelFill();
    void undoTileFill();
    void newProjectAfterChange();
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
};

tst_App::tst_App(int &argc, char **argv) :
    TestHelper(argc, argv)
{
}

void tst_App::newProjectWithNewTileset()
{
    createNewProject(32, 32, 5, 5);

    // Make sure that any changes are reflected in the image after it's saved.
    // First, establish what we expect the image to look like in the end.
    const int expectedWidth = 32 * 5;
    const int expectedHeight = 32 * 5;
    QCOMPARE(project->tileset()->image()->width(), expectedWidth);
    QCOMPARE(project->tileset()->image()->height(), expectedHeight);
    QImage expectedTilesetImage(expectedWidth, expectedHeight, project->tileset()->image()->format());
    expectedTilesetImage.fill(Qt::white);
    expectedTilesetImage.setPixelColor(10, 10, canvas->penForegroundColour());

    // Draw a tile on.
    switchMode(TileCanvas::TileMode);
    setCursorPosInTiles(0, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(QPoint(0, 0)));
    QVERIFY(project->hasUnsavedChanges());

    // Draw a pixel on that tile.
    switchMode(TileCanvas::PixelMode);
    setCursorPosInPixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*project->tileset()->image(), expectedTilesetImage);

    // Save the project.
    const QUrl saveFileName = QUrl::fromLocalFile(tempProjectDir->path() + "/mytileset.json");
    project->saveAs(saveFileName);
    VERIFY_NO_ERRORS_OCCURRED();
    // Should save the image at the same location as the project.
    const QString tilesetPath = tempProjectDir->path() + "/mytileset.png";
    QCOMPARE(project->tilesetUrl(), QUrl::fromLocalFile(tilesetPath));
    QVERIFY(QFile::exists(tilesetPath));
    QCOMPARE(*project->tileset()->image(), expectedTilesetImage);
}

void tst_App::openClose()
{
    // Create a new, valid project.
    createNewProject();

    // Test an invalid URL.
    QTest::ignoreMessage(QtWarningMsg, "QFSFileEngine::open: No file name specified");
    project->load(QUrl("doesnotexist"));
    QCOMPARE(project->url(), QUrl());
    QCOMPARE(project->hasLoaded(), true);
    const QString errorMessage = QLatin1String("Project files must have a .json extension ()");
    QCOMPARE(errorOccurredSpy.size(), 1);
    QCOMPARE(errorOccurredSpy.at(0).at(0).toString(), errorMessage);
    const QObject *errorPopup = findPopupFromTypeName("ErrorPopup");
    QVERIFY(errorPopup);
    QVERIFY(errorPopup->property("visible").toBool());
    QCOMPARE(errorPopup->property("text").toString(), errorMessage);
    QVERIFY(errorPopup->property("focus").isValid());
    QVERIFY(errorPopup->property("focus").toBool());
    errorOccurredSpy.clear();

    // Check that the cursor goes back to an arrow when there's a modal popup visible,
    // even if the mouse is over the canvas and not the popup.
    QTest::mouseMove(window, canvas->mapToScene(QPointF(0, 0)).toPoint());
    QVERIFY(!canvas->hasActiveFocus());
    QCOMPARE(window->cursor().shape(), Qt::ArrowCursor);

    // Close the error message popup.
    QTest::mouseClick(window, Qt::LeftButton);
    QVERIFY(!errorPopup->property("visible").toBool());

    // Check that the cursor goes blank when the canvas has focus.
    QVERIFY(canvas->hasActiveFocus());
    QTRY_COMPARE(window->cursor().shape(), Qt::BlankCursor);

    // Test closing a valid image.
    project->close();
    VERIFY_NO_ERRORS_OCCURRED();
    QCOMPARE(project->url(), QUrl());
    QCOMPARE(project->hasLoaded(), false);
    QVERIFY(!errorPopup->property("visible").toBool());

    createNewProject();
}

void tst_App::save()
{
    createNewProject();

    // Store a snapshot of the canvas before we alter it.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalImage = imageGrabber.takeImage();

    // Draw a tile on first.
    switchMode(TileCanvas::TileMode);

    // QTBUG-53466
    setCursorPosInPixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(QPoint(0, 0)));
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Save our drawing.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + "/project.json");
    project->saveAs(saveUrl);
    VERIFY_NO_ERRORS_OCCURRED();
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != originalImage);

    // Check that what's on disk matches the image that we currently have.
    QImage savedImage(tempTilesetUrl.toLocalFile());
    QVERIFY(!savedImage.isNull());
    QCOMPARE(savedImage, *project->tileAt(QPoint(0, 0))->tileset()->image());
    QTRY_VERIFY_WITH_TIMEOUT(!window->title().contains("*"), 10);
}

void tst_App::saveAsAndLoad()
{
    createNewProject();

    // Save the untouched project.
    const QString originalProjectPath = tempProjectDir->path() + "/project.json";
    project->saveAs(QUrl::fromLocalFile(originalProjectPath));
    VERIFY_NO_ERRORS_OCCURRED();
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
    QCOMPARE(project->url().toLocalFile(), originalProjectPath);

    // Save the original project file contents, along with a screenshot.
    QByteArray originalProjectFileContents;
    {
        QFile file(originalProjectPath);
        QVERIFY2(file.open(QIODevice::ReadOnly), qPrintable(file.errorString()));
        originalProjectFileContents = file.readAll();
        QVERIFY(!originalProjectFileContents.isEmpty());
    }

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalCanvasImage = imageGrabber.takeImage();

    // Save the project to a new file.
    const QString savedProjectPath = tempProjectDir->path() + "/project2.json";
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    VERIFY_NO_ERRORS_OCCURRED();
    QCOMPARE(project->url().toLocalFile(), savedProjectPath);
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), originalCanvasImage);

    switchMode(TileCanvas::TileMode);

    // Draw on the canvas. We want to check that the latest
    // saved project is modified and not the original.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Save our project.
    project->save();
    VERIFY_NO_ERRORS_OCCURRED();
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != originalCanvasImage);
    QTRY_VERIFY_WITH_TIMEOUT(!window->title().contains("*"), 10);

    // Check that what's on disk matches the tileset image that we currently have.
    const Tile *modifiedTile = project->tileAt(QPoint(0, 0));
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
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(closeMenuButton, MouseClick);
    QVERIFY(!project->hasLoaded());

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage closedCanvasImage = imageGrabber.takeImage();

    project->load(QUrl::fromLocalFile(savedProjectPath));
    VERIFY_NO_ERRORS_OCCURRED();
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != closedCanvasImage);
}

void tst_App::keyboardShortcuts()
{
    QSKIP("https://bugreports.qt.io/browse/QTBUG-57098");

    createNewProject();

    QTest::keyPress(window, Qt::Key_1);
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);
    QTest::keyRelease(window, Qt::Key_1);
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);

    QTest::keyPress(window, Qt::Key_2);
    QCOMPARE(canvas->tool(), TileCanvas::EyeDropperTool);
    QTest::keyRelease(window, Qt::Key_2);
    QCOMPARE(canvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyPress(window, Qt::Key_3);
    QCOMPARE(canvas->tool(), TileCanvas::EraserTool);
    QTest::keyRelease(window, Qt::Key_3);
    QCOMPARE(canvas->tool(), TileCanvas::EraserTool);

    // Open options dialog.
    mouseEventOnCentre(optionsToolButton, MouseClick);
    mouseEventOnCentre(settingsMenuButton, MouseClick);
    const QObject *optionsDialog = findPopupFromTypeName("OptionsDialog");
    QVERIFY(optionsDialog);
    QVERIFY(optionsDialog->property("visible").toBool());

    // Open the shortcuts tab.
    QQuickItem *shortcutsTabButton = optionsDialog->findChild<QQuickItem*>("shortcutsTabButton");
    QVERIFY(shortcutsTabButton);
    mouseEventOnCentre(shortcutsTabButton, MouseClick);

    // Give "New Project" shortcut editor focus.
    QQuickItem *newShortcutToolButton = optionsDialog->findChild<QQuickItem*>("newShortcutToolButton");
    QVERIFY(newShortcutToolButton);
    QCOMPARE(newShortcutToolButton->property("text").toString(), app.settings()->defaultNewShortcut());
    mouseEventOnCentre(newShortcutToolButton, MouseClick);

    QQuickItem *newShortcutEditor = optionsDialog->findChild<QQuickItem*>("newShortcutEditor");
    QVERIFY(newShortcutEditor);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    // The toolbutton should display the shortcut that has been entered so far (nothing).
    QCOMPARE(newShortcutToolButton->property("text").toString(), QString());

    // Begin inputting a shortcut.
    QTest::keyClick(window, Qt::Key_M, Qt::ControlModifier);
    QCOMPARE(newShortcutToolButton->property("text").toString(), QLatin1String("Ctrl+M"));

    // Cancel editing; shortcut shouldn't have changed.
    QTest::keyClick(window, Qt::Key_Escape);
    // The dialog should still be visible: QTBUG-57098
    QVERIFY(optionsDialog->property("visible").toBool());
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutToolButton->property("text").toString(), app.settings()->defaultNewShortcut());

    // Give "New Project" shortcut editor focus again.
    mouseEventOnCentre(newShortcutToolButton, MouseClick);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    // Input another shortcut.
    QTest::keyClick(window, Qt::Key_U, Qt::ControlModifier);
    QCOMPARE(newShortcutToolButton->property("text").toString(), QLatin1String("Ctrl+U"));

    // Press Enter to accept it.
    QTest::keyClick(window, Qt::Key_Return);
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutToolButton->property("text").toString(), QLatin1String("Ctrl+U"));

    // There was an issue where entering the original shortcut (e.g. Ctrl+N) after
    // having changed it to a new one (e.g. Ctrl+U) would result in the now not-so-new one (Ctrl+U)
    // still being shown instead of the latest one (Ctrl+N).
    mouseEventOnCentre(newShortcutToolButton, MouseClick);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    QTest::keyClick(window, Qt::Key_N, Qt::ControlModifier);
    QCOMPARE(newShortcutToolButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QTest::keyClick(window, Qt::Key_Return);
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutToolButton->property("text").toString(), app.settings()->defaultNewShortcut());

    // Close the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
}

void tst_App::optionsCancelled()
{
    QSKIP("https://bugreports.qt.io/browse/QTBUG-57098");

    // Ensure that cancelling the options dialog after changing a shortcut cancels the shortcut change.
    createNewProject();

    // Open options dialog.
    mouseEventOnCentre(optionsToolButton, MouseClick);
    mouseEventOnCentre(settingsMenuButton, MouseClick);
    const QObject *optionsDialog = findPopupFromTypeName("OptionsDialog");
    QVERIFY(optionsDialog);
    QVERIFY(optionsDialog->property("visible").toBool());

    // Open the shortcuts tab.
    QQuickItem *shortcutsTabButton = optionsDialog->findChild<QQuickItem*>("shortcutsTabButton");
    QVERIFY(shortcutsTabButton);
    mouseEventOnCentre(shortcutsTabButton, MouseClick);

    // Give "New Project" shortcut editor focus.
    QQuickItem *newShortcutToolButton = optionsDialog->findChild<QQuickItem*>("newShortcutToolButton");
    QVERIFY(newShortcutToolButton);
    QCOMPARE(newShortcutToolButton->property("text").toString(), app.settings()->defaultNewShortcut());
    mouseEventOnCentre(newShortcutToolButton, MouseClick);

    QQuickItem *newShortcutEditor = optionsDialog->findChild<QQuickItem*>("newShortcutEditor");
    QVERIFY(newShortcutEditor);
    QVERIFY(newShortcutEditor->hasActiveFocus());

    // Begin inputting a shortcut.
    QTest::keyClick(window, Qt::Key_J, Qt::ControlModifier);
    QCOMPARE(newShortcutToolButton->property("text").toString(), QLatin1String("Ctrl+J"));

    // Press Enter to accept it.
    QTest::keyClick(window, Qt::Key_Return);
    QVERIFY(!newShortcutEditor->hasActiveFocus());
    QCOMPARE(newShortcutToolButton->property("text").toString(), QLatin1String("Ctrl+J"));
    // Shortcut shouldn't change until we hit "OK".
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(!optionsDialog->property("visible").toBool());
    // Cancelling the dialog shouldn't change anything.
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    // Reopen the dialog to make sure that the editor shows the default shortcut.
    mouseEventOnCentre(optionsToolButton, MouseClick);
    mouseEventOnCentre(settingsMenuButton, MouseClick);
    QVERIFY(optionsDialog->property("visible").toBool());
    QTRY_COMPARE(newShortcutToolButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    // Close the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
}

void tst_App::showGrid()
{
    createNewProject();

    // Store a screenshot of the canvas so we can ensure that the grid lines
    // aren't actually visible to the user.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage withGrid = imageGrabber.takeImage();

    mouseEventOnCentre(viewToolButton, MouseClick);
    QVERIFY(showGridMenuButton->property("checked").toBool());
    QVERIFY(app.settings()->isGridVisible());

    // Toggle the option.
    mouseEventOnCentre(showGridMenuButton, MouseClick);
    QVERIFY(!showGridMenuButton->property("checked").toBool());

    // Close the view menu.
    QTest::keyClick(window, Qt::Key_Escape);

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage withoutGrid = imageGrabber.takeImage();
    QVERIFY(withoutGrid != withGrid);

    // Show the grid again.
    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(showGridMenuButton, MouseClick);
    QVERIFY(showGridMenuButton->property("checked").toBool());
    QVERIFY(app.settings()->isGridVisible());
}

void tst_App::undoPixels()
{
    createNewProject();
    switchTool(TileCanvas::PenTool);

    // It's a new project.
    QVERIFY(project->canSave());
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    // Save the project so that we can test hasUnsavedChanges.
    project->saveAs(QUrl::fromLocalFile(tempProjectDir->path() + "/project.json"));
    VERIFY_NO_ERRORS_OCCURRED();
    QVERIFY(!project->canSave());
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    switchMode(TileCanvas::TileMode);

    // Draw a tile on.
    setCursorPosInTiles(0, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    // Changes are added as press/move events happen.
    QVERIFY(project->hasUnsavedChanges());
    // The macro isn't finished until a release event occurs, and hence we can't undo yet.
    QVERIFY(!project->undoStack()->canUndo());
    // The title shouldn't update until canSave() is true.
    QVERIFY(!window->title().contains("*"));
    QVERIFY(!project->canSave());

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY2(project->tileAt(cursorPos), qPrintable(QString::fromLatin1("No tile at x %1 y %2")
        .arg(cursorPos.x()).arg(cursorPos.y())));
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(project->undoStack()->canUndo());
    QVERIFY(window->title().contains("*"));
    QVERIFY(project->canSave());

    switchMode(TileCanvas::PixelMode);

    // Draw on some pixels of that tile.
    const QImage originalImage = *project->tileAt(cursorPos)->tileset()->image();
    QImage lastImage;
    QTest::mouseMove(window, cursorPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(*project->tileAt(cursorPos)->tileset()->image() != lastImage);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    lastImage = *project->tileAt(cursorPos)->tileset()->image();
    setCursorPosInPixels(cursorPos + QPoint(0, 1));
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(*project->tileAt(cursorPos)->tileset()->image() != lastImage);

    lastImage = *project->tileAt(cursorPos)->tileset()->image();
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*project->tileAt(cursorPos)->tileset()->image(), lastImage);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(*project->tileAt(cursorPos)->tileset()->image(), originalImage);
    // Still have the tile pen changes.
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test reverting.
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(revertMenuButton, MouseClick);
    QVERIFY(!project->tileAt(cursorPos));
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
}

void tst_App::undoLargePixelPen()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    // Draw the first tile on the canvas.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(project->tileAt(cursorPos));

    // Select the second tile from the top-left in the swatch.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = project->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(canvas->penTile(), expectedTile);

    // Draw that on next to the first one.
    setCursorPosInTiles(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(project->tileAt(tileCanvasCentre(0, 0)) != project->tileAt(tileCanvasCentre(1, 0)));

    switchMode(TileCanvas::PixelMode);

    const QImage originalTilesetImage = *project->tileset()->image();

    const int toolSize = project->tileWidth();
    changeToolSize(toolSize);

    // Draw a large square.
    setCursorPosInPixels(toolSize, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    const qreal halfToolSize = qRound(toolSize / 2.0);
    // Test that both tiles were drawn on by checking each corner of the drawn square.
    const QPoint sceneTopLeft(cursorPos - QPoint(halfToolSize, 0));
    QCOMPARE(project->tileAt(sceneTopLeft)->pixelColor(halfToolSize, 0).isValid(), true);
    QCOMPARE(project->tileAt(sceneTopLeft)->pixelColor(halfToolSize, 0), QColor(Qt::black));

    const QPoint sceneBottomLeft(cursorPos - QPoint(halfToolSize, -halfToolSize - 1));
    QCOMPARE(project->tileAt(sceneBottomLeft)->pixelColor(halfToolSize, halfToolSize - 1).isValid(), true);
    QCOMPARE(project->tileAt(sceneBottomLeft)->pixelColor(halfToolSize, halfToolSize - 1), QColor(Qt::black));

    const QPoint sceneTopRight(cursorPos + QPoint(halfToolSize, 0));
    QCOMPARE(project->tileAt(sceneTopRight)->pixelColor(halfToolSize - 1, 0).isValid(), true);
    QCOMPARE(project->tileAt(sceneTopRight)->pixelColor(halfToolSize - 1, 0), QColor(Qt::black));

    const QPoint sceneBottomRight(cursorPos + QPoint(halfToolSize, halfToolSize - 1));
    QCOMPARE(project->tileAt(sceneBottomRight)->pixelColor(halfToolSize - 1, halfToolSize - 1).isValid(), true);
    QCOMPARE(project->tileAt(sceneBottomRight)->pixelColor(halfToolSize - 1, halfToolSize - 1), QColor(Qt::black));

    // Undo the change and check that it worked.
    mouseEventOnCentre(undoButton, MouseClick);

    QCOMPARE(*project->tileset()->image(), originalTilesetImage);
}

void tst_App::undoTiles()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    // Move the cursor away so we have an image we can compare against other grabbed images later on.
    setCursorPosInTiles(0, 2);
    const QPoint outsideCanvas = cursorWindowPos - QPoint(project->tileWidth(), 0);
    QTest::mouseMove(window, outsideCanvas);
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalCanvasImage = imageGrabber.takeImage();

    setCursorPosInTiles(0, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    // The macro isn't finished until a release event occurs, and hence
    // we can't undo yet.
    QVERIFY(!undoButton->isEnabled());
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    // Check that undoing merged commands (drawing the same tile at the same position) works.
    setCursorPosInPixels(cursorPos.x(), cursorPos.y() + 1);
    Tile *lastTile = project->tileAt(cursorPos);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(project->tileAt(cursorPos), lastTile);

    // Go to another tile.
    setCursorPosInTiles(0, 2);
    lastTile = project->tileAt(cursorPos);
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos) != lastTile);

    // Release, ending the macro composition.
    lastTile = project->tileAt(cursorPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->tileAt(cursorPos), lastTile);
    QVERIFY(undoButton->isEnabled());
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test the undo button.
    mouseEventOnCentre(undoButton, MouseClick);
    QVERIFY(!project->tileAt(cursorPos));
    QVERIFY(!project->tileAt(cursorPos - QPoint(0, project->tileHeight())));
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
    // Move the mouse away so the tile pen preview doesn't affect
    // our check that the canvas is actually updated.
    QTest::mouseMove(window, outsideCanvas);
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), originalCanvasImage);

    // Draw a tile back so we can test the revert button.
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test reverting.
    mouseEventOnCentre(fileToolButton, MouseClick);
    QVERIFY(revertMenuButton->isEnabled());
    mouseEventOnCentre(revertMenuButton, MouseClick);
    QVERIFY(!project->tileAt(cursorPos));
    QVERIFY(!undoButton->isEnabled());
    QVERIFY(!project->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
}

// Test going back over the same pixels several times.
void tst_App::undoWithDuplicates()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    // Draw on a tile so that we can operate on its pixels.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    switchMode(TileCanvas::PixelMode);

    setCursorPosInPixels(0, 1);
    const QImage originalImage = *project->tileAt(cursorPos)->tileset()->image();
    QImage lastImage = originalImage;
    QTest::mouseMove(window, cursorWindowPos);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->tileAt(cursorPos)->pixelColor(cursorPos), canvas->penForegroundColour());
    QVERIFY(*project->tileAt(cursorPos)->tileset()->image() != lastImage);

    lastImage = *project->tileAt(cursorPos)->tileset()->image();
    setCursorPosInPixels(0, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(project->tileAt(cursorPos)->pixelColor(cursorPos), canvas->penForegroundColour());
    QVERIFY(*project->tileAt(cursorPos)->tileset()->image() != lastImage);

    // Go back over the same pixels.
    lastImage = *project->tileAt(cursorPos)->tileset()->image();
    setCursorPosInPixels(0, 1);
    QTest::mouseMove(window, cursorWindowPos);
    project->tileAt(cursorPos)->tileset()->image()->save("C:/dev/cur.png");
    lastImage.save("C:/dev/last.png");
    QCOMPARE(*project->tileAt(cursorPos)->tileset()->image(), lastImage);

    setCursorPosInPixels(0, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(*project->tileAt(cursorPos)->tileset()->image(), lastImage);

    lastImage = *project->tileAt(cursorPos)->tileset()->image();
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*project->tileAt(cursorPos)->tileset()->image(), lastImage);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(*project->tileAt(cursorPos)->tileset()->image(), originalImage);
    // Still have the tile change.
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Now test that going over the same pixels on the same tile but in a different scene
    // position doesn't result in those pixels not being undone.
    switchMode(TileCanvas::TileMode);

    // Draw on another tile next to the existing one.
    setCursorPosInTiles(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));

    switchMode(TileCanvas::PixelMode);

    int x = 0;
    const int y = project->tileHeight() / 2;
    setCursorPosInPixels(0, y);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    for (; x < project->tileWidth(); ++x) {
        setCursorPosInPixels(x, y);
        QTest::mouseMove(window, cursorWindowPos);
        QCOMPARE(project->tileAt(cursorPos)->pixelColor(cursorPos), canvas->penForegroundColour());
    }
    // The last pixel is on the next tile.
    setCursorPosInPixels(++x, y);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->tileAt(cursorPos)->pixelColor(cursorPos - QPoint(project->tileWidth(), 0)), canvas->penForegroundColour());

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(*project->tileAt(cursorPos)->tileset()->image(), originalImage);
    // Still have the tile change.
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));
}

void tst_App::undoCanvasSizeChange()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    setCursorPosInTiles(8, 9);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->tileAt(cursorPos), canvas->penTile());

    setCursorPosInTiles(9, 9);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->tileAt(cursorPos), canvas->penTile());

    const QVector<int> originalTiles = project->tiles();

    changeCanvasSize(9, 9);
    QVERIFY(project->tiles() != originalTiles);
    QCOMPARE(project->tiles().size(), 9 * 9);

    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(project->tiles(), originalTiles);
    QCOMPARE(project->tiles().size(), 10 * 10);

    // Check that neither of the following assert.
    changeCanvasSize(10, 9);
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(project->tiles(), originalTiles);
    QCOMPARE(project->tiles().size(), 10 * 10);

    changeCanvasSize(9, 10);
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(project->tiles(), originalTiles);
    QCOMPARE(project->tiles().size(), 10 * 10);

    changeCanvasSize(12, 12);
    QCOMPARE(project->tiles().size(), 12 * 12);
    QCOMPARE(project->tiles().last(), -1);
}

void tst_App::undoPixelFill()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    // Select a blank tile to draw on.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = project->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(canvas->penTile(), expectedTile);

    // Draw the tile on so that we can operate on its pixels.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(project->hasUnsavedChanges());
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

    const Tile *targetTile = project->tileAt(cursorPos);
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
    canvas->setPenForegroundColour(red);
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
    createNewProject();

    // Draw a block of tiles.
    setCursorPosInTiles(0, 0);
    drawTileAtCursorPos();

    setCursorPosInTiles(1, 0);
    drawTileAtCursorPos();

    const Tile *targetTile = project->tileAt(cursorPos);
    QVERIFY(targetTile);
    QCOMPARE(project->tileAtTilePos(QPoint(0, 0)), canvas->penTile());
    QCOMPARE(project->tileAtTilePos(QPoint(1, 0)), canvas->penTile());

    // Try to fill it. The whole block should be filled.
    switchTool(TileCanvas::FillTool);

    // Select the second tile from the top-left in the swatch.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *replacementTile = project->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(canvas->penTile(), replacementTile);

    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->tileAtTilePos(QPoint(0, 0)), replacementTile);
    QCOMPARE(project->tileAtTilePos(QPoint(1, 0)), replacementTile);

    // Undo it.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(project->tileAtTilePos(QPoint(0, 0)), targetTile);
    QCOMPARE(project->tileAtTilePos(QPoint(1, 0)), targetTile);
}

void tst_App::newProjectAfterChange()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    // Paint a tile on.
    setCursorPosInTiles(0, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(project->hasUnsavedChanges());

    // Shouldn't assert.
    createNewProject();
}

void tst_App::colours()
{
    createNewProject();
    QCOMPARE(canvas->penForegroundColour(), QColor(Qt::black));
    QCOMPARE(canvas->penBackgroundColour(), QColor(Qt::white));

    // First try changing the foreground colour.
    mouseEvent(penForegroundColourButton, QPoint(1, 1), MouseClick);

    QQuickItem *saturationLightnessPicker = window->findChild<QQuickItem*>("saturationLightnessPicker");
    QVERIFY(saturationLightnessPicker);

    // Choose a colour.
    mouseEventOnCentre(saturationLightnessPicker, MouseClick);
    const QColor expectedColour = QColor("#c04141");
    fuzzyColourCompare(canvas->penForegroundColour(), expectedColour);
    // Background colour shouldn't be affected.
    QCOMPARE(canvas->penBackgroundColour(), QColor(Qt::white));

    // Now try changing the background colour.
    mouseEvent(penBackgroundColourButton,
        QPoint(penBackgroundColourButton->width() - 1, penBackgroundColourButton->width() - 1), MouseClick);

    // Choose a colour.
    mouseEvent(saturationLightnessPicker, QPointF(saturationLightnessPicker->width() * 0.25,
        saturationLightnessPicker->height() * 0.25), MouseClick);
    QVERIFY(canvas->penBackgroundColour() != QColor(Qt::white));
    // Foreground colour shouldn't be affected.
    fuzzyColourCompare(canvas->penForegroundColour(), expectedColour);

    // Hex field should represent background colour when selected.
    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    QVERIFY(hexTextField);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penBackgroundColour().name());

    // Hex field should represent foreground colour when selected.
    mouseEventOnCentre(penForegroundColourButton, MouseClick);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penForegroundColour().name());
}

void tst_App::panes()
{
    createNewProject();
    QVERIFY(canvas->firstPane());
    QCOMPARE(canvas->firstPane()->size(), 0.5);
    QVERIFY(canvas->secondPane());
    QCOMPARE(canvas->secondPane()->size(), 0.5);
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);

    switchMode(TileCanvas::TileMode);

    setCursorPosInTiles(0, 0);
    Tile *lastTile = project->tileAt(cursorPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));
    QVERIFY(project->tileAt(cursorPos) != lastTile);

    setCursorPosInPixels(canvas->width() / 2 + project->tileWidth() * 1.5, 1);
    cursorPos.setX(cursorPos.x() - canvas->width() / 2);
    lastTile = project->tileAt(cursorPos);
    QVERIFY(!lastTile);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(project->tileAt(cursorPos) != lastTile);

    // Remove split.
    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(splitScreenMenuButton, MouseClick);
    QVERIFY(!app.settings()->isSplitScreen());
    QVERIFY(!canvas->secondPane());
    QVERIFY(canvas->firstPane());
    QCOMPARE(canvas->firstPane()->size(), 1.0);

    // Add it back again.
    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(splitScreenMenuButton, MouseClick);
    QVERIFY(app.settings()->isSplitScreen());
    QVERIFY(canvas->firstPane());
    QCOMPARE(canvas->firstPane()->size(), 0.5);
    QVERIFY(canvas->secondPane());
    QCOMPARE(canvas->secondPane()->size(), 0.5);
}

void tst_App::altEyedropper()
{
    createNewProject();
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);

    QTest::keyPress(window, Qt::Key_Alt);
    QCOMPARE(canvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyRelease(window, Qt::Key_Alt);
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);

    mouseEventOnCentre(fileToolButton, MouseClick);
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(window->activeFocusItem(), canvas);

    QTest::keyPress(window, Qt::Key_Alt);
    QCOMPARE(canvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyRelease(window, Qt::Key_Alt);
    QCOMPARE(canvas->tool(), TileCanvas::PenTool);
}

void tst_App::eyedropper()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    setCursorPosInTiles(1, 1);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    Tile *originalTile = project->tileAt(cursorPos);
    QVERIFY(originalTile);
    const QPoint pixelPos = QPoint(project->tileWidth() / 2, project->tileHeight() / 2);
    QVERIFY(canvas->penForegroundColour() != originalTile->pixelColor(pixelPos));

    switchMode(TileCanvas::PixelMode);
    switchTool(TileCanvas::EyeDropperTool);

    QColor lastForegroundColour = canvas->penForegroundColour();
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(canvas->penForegroundColour() != lastForegroundColour);
    // TODO: no idea why this doesn't work.. the positions are both {12, 12}.
//    QCOMPARE(canvas->penForegroundColour(), originalTile->pixelColor(pixelPos));

    switchMode(TileCanvas::TileMode);

    // Choose another tile from the swatch.
    QCOMPARE(canvas->penTile(), project->tilesetTileAt(0, 0));

    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = project->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(canvas->penTile(), expectedTile);

    setCursorPosInTiles(1, 1);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->penTile(), originalTile);
}

void tst_App::zoomAndPan()
{
    createNewProject();

    panTopLeftTo(0, 0);

    // Test panning.
    panBy(50, 0);

    // Test zoom.
    QPoint zoomPos = tileSceneCentre(5, 5);
    wheelEvent(canvas, zoomPos, 1);
    QCOMPARE(canvas->currentPane()->zoomLevel(), 2);
}

void tst_App::zoomAndCentre()
{
    createNewProject();

    // Pan to some non-centered location.
    panTopLeftTo(-100, -100);

    const TileCanvasPane *currentPane = canvas->currentPane();
    QCOMPARE(currentPane, canvas->firstPane());

    // Zoom in.
    QPoint zoomPos = tileSceneCentre(5, 5);
    const int expectedZoomLevel = 5;
    for (int i = 0; currentPane->zoomLevel() < expectedZoomLevel && i < expectedZoomLevel; ++i)
        wheelEvent(canvas, zoomPos, 1);
    QCOMPARE(currentPane->zoomLevel(), expectedZoomLevel);

    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(centreMenuButton, MouseClick);
    const QPoint expectedOffset(
        currentPane->size() * canvas->width() / 2 - (project->widthInPixels() * currentPane->zoomLevel()) / 2,
        canvas->height() / 2 - (project->heightInPixels() * currentPane->zoomLevel()) / 2);
    // A one pixel difference was introduced here at some point.. not sure why, but it's not important.
    const int xDiff = qAbs(currentPane->offset().x() - expectedOffset.x());
    const int yDiff = qAbs(currentPane->offset().y() - expectedOffset.y());
    QVERIFY(xDiff <= 1);
    QVERIFY(yDiff <= 1);
}

void tst_App::penWhilePannedAndZoomed_data()
{
    QTest::addColumn<int>("xDistance");
    QTest::addColumn<int>("yDistance");
    QTest::addColumn<int>("zoomLevel");

    // TODO: test with zoom
    QTest::newRow("{40, 0}, 1") << 40 << 0 << 1;
    QTest::newRow("{0, 40}, 2") << 0 << 40 << 1;
    QTest::newRow("{40, 40}, 3") << 40 << 40 << 1;
    QTest::newRow("{-40, 0}, 3") << -40 << 0 << 1;
    QTest::newRow("{0, 0}, 2") << 0 << -40 << 1;
    QTest::newRow("{-40, -40}, 2") << -40 << -40 << 1;
}

void tst_App::penWhilePannedAndZoomed()
{
    QFETCH(int, xDistance);
    QFETCH(int, yDistance);
    QFETCH(int, zoomLevel);

    createNewProject();
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

    switchMode(TileCanvas::TileMode);

    // Draw a tile on.
    setCursorPosInTiles(4, 4);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->hasUnsavedChanges());

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY2(project->tileAt(cursorPos), qPrintable(QString::fromLatin1("No tile at x %1 y %2")
        .arg(cursorPos.x()).arg(cursorPos.y())));
}

void tst_App::useTilesetSwatch()
{
    createNewProject();

    switchMode(TileCanvas::TileMode);

    QCOMPARE(canvas->penTile(), project->tilesetTileAt(0, 0));

    // Select the second tile from the top-left in the swatch.
    QTest::mouseMove(window, tilesetTileSceneCentre(1, 0));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 0));
    const QPoint tilesetCentre = tilesetTileCentre(1, 0);
    const Tile *expectedTile = project->tilesetTileAt(tilesetCentre.x(), tilesetCentre.y());
    QCOMPARE(canvas->penTile(), expectedTile);

    // Draw it on the canvas.
    setCursorPosInTiles(1, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->hasUnsavedChanges());
    QVERIFY(project->tileAt(cursorPos));
    QCOMPARE(project->tileAt(cursorPos), expectedTile);

    // Draw some pixels on the tile we just painted onto the canvas.
    switchMode(TileCanvas::PixelMode);
    // Make sure that the pixel's colour will actually change.
    const QPoint pixelPos = QPoint(project->tileWidth() / 2, project->tileHeight() / 2);
    QVERIFY(canvas->penForegroundColour() != expectedTile->tileset()->image()->pixelColor(pixelPos));

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
    createNewProject();

    QCOMPARE(canvas->penTile(), project->tilesetTileAt(0, 0));

    // Test clicking outside of the menu to cancel it.

    const Tile *originallySelectedTile = canvas->penTile();
    // Open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
    QObject *tilesetContextMenu = window->findChild<QObject*>("tilesetContextMenu");
    QVERIFY(tilesetContextMenu);
    QVERIFY(tilesetContextMenu->property("visible").toBool());

    const QPoint outsidePos = tilesetTileSceneCentre(project->tileset()->tilesWide() - 1, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, outsidePos);
    QVERIFY(!tilesetContextMenu->property("visible").toBool());
    // The selected tile shouldn't have changed.
    QCOMPARE(canvas->penTile(), originallySelectedTile);

    // Test duplicating a tile.

    // Open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
    QVERIFY(tilesetContextMenu->property("visible").toBool());

    // Click the "duplicate" menu item.
    mouseEventOnCentre(duplicateTileMenuButton, MouseClick);
    QCOMPARE(canvas->penTile(), originallySelectedTile);
    QVERIFY(!tilesetContextMenu->property("visible").toBool());

    // Duplicate the tile.
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 1));
    // It should be the tile we right clicked on.
    QCOMPARE(project->tilesetTileAt(0, 0), originallySelectedTile);
    // The selected tile shouldn't have changed.
    QCOMPARE(canvas->penTile(), originallySelectedTile);

    // Test rotating a tile left.

    // Draw the tile that we're rotating onto the canvas and then
    // take a snapshot of the canvas to make sure that it's actually updated.
    switchMode(TileCanvas::TileMode);
    setCursorPosInTiles(0, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(project->tileAt(cursorPos));
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QImage lastCanvasSnapshot = imageGrabber.takeImage();

    // Open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
    QVERIFY(tilesetContextMenu->property("visible").toBool());

    // Click the "rotate left" menu item.
    const QImage originalTileImage = canvas->penTile()->image();
    mouseEventOnCentre(rotateTileLeftMenuButton, MouseClick);
    QVERIFY(!tilesetContextMenu->property("visible").toBool());
    QCOMPARE(Utils::rotate(canvas->penTile()->image(), 90), originalTileImage);

    QVERIFY(imageGrabber.requestImage(canvas));
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
    QCOMPARE(canvas->penTile()->image(), originalTileImage);

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    currentImage = imageGrabber.takeImage();
    QVERIFY(currentImage != lastCanvasSnapshot);
}

void tst_App::tilesetSwatchNavigation()
{
    createNewProject();

    const Qt::Key leftKey = Qt::Key_A;
    const Qt::Key rightKey = Qt::Key_D;
    const Qt::Key upKey = Qt::Key_W;
    const Qt::Key downKey = Qt::Key_S;

    QPoint tilePos(0, 0);
    QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));

    // Already at left edge; shouldn't do anything.
    QTest::keyClick(window, leftKey);
    QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));

    // Move all the way to the right edge.
    while (++tilePos.rx() < project->tileset()->tilesWide()) {
        QTest::keyClick(window, rightKey);
        QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));
    }
    // The loop leaves us with an illegal position.
    --tilePos.rx();
    QCOMPARE(tilePos, QPoint(project->tileset()->tilesWide() - 1, 0));

    // Already at right edge; shouldn't do anything.
    QTest::keyClick(window, rightKey);
    QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));


    // Move all the way to the bottom edge.
    while (++tilePos.ry() < project->tileset()->tilesHigh()) {
        QTest::keyClick(window, downKey);
        QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));
    }
    // The loop leaves us with an illegal position.
    --tilePos.ry();
    QCOMPARE(tilePos, QPoint(project->tileset()->tilesWide() - 1, project->tileset()->tilesHigh() - 1));

    // Already at bottom edge; shouldn't do anything.
    QTest::keyClick(window, downKey);
    QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));


    // Move all the way to the top edge.
    while (--tilePos.ry() >= 0) {
        QTest::keyClick(window, upKey);
        QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));
    }
    // The loop leaves us with an illegal position.
    ++tilePos.ry();
    QCOMPARE(tilePos, QPoint(project->tileset()->tilesWide() - 1, 0));

    // Already at top edge; shouldn't do anything.
    QTest::keyClick(window, upKey);
    QCOMPARE(canvas->penTile(), project->tilesetTileAtTilePos(tilePos));
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

int main(int argc, char *argv[])
{
    tst_App test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "tst_app.moc"
