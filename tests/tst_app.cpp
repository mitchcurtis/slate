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

#include <QClipboard>
#include <QCursor>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QtTest>
#include <QQuickItemGrabResult>
#include <QQuickWindow>

#include "application.h"
#include "applypixelpencommand.h"
#include "imagelayer.h"
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
    void saveTilesetProject();
    void saveAsAndLoadTilesetProject();
    void saveAsAndLoad_data();
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
    void colourPickerHexField();
    void eraseImageCanvas_data();
    void eraseImageCanvas();
    void selectionToolImageCanvas();
    void cancelSelectionToolImageCanvas();
    void moveSelectionImageCanvas_data();
    void moveSelectionImageCanvas();
    void moveSelectionWithKeysImageCanvas();
    void deleteSelectionImageCanvas_data();
    void deleteSelectionImageCanvas();
    void copyPaste_data();
    void copyPaste();
    void pasteFromExternalSource_data();
    void pasteFromExternalSource();
    void flipPastedImage();
    void fillImageCanvas_data();
    void fillImageCanvas();
    void greedyPixelFillImageCanvas_data();
    void greedyPixelFillImageCanvas();
    void pixelLineToolImageCanvas_data();
    void pixelLineToolImageCanvas();
    void rulersAndGuides_data();
    void rulersAndGuides();

    void addAndRemoveLayers();
    void layerVisibility();
    void moveLayerUpAndDown();
    void renameLayer();
    void saveAndLoadLayeredImageProject();
    void layerVisibilityAfterMoving();
    void undoAfterAddLayer();
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
    setCursorPosInScenePixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileset()->image(), expectedTilesetImage);

    // Save the project.
    const QUrl saveFileName = QUrl::fromLocalFile(tempProjectDir->path() + "/mytileset.stp");
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
        const QString errorMessage = QLatin1String("Tileset project files must have a .stp extension ()");
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
        QTest::mouseMove(window, canvas->mapToScene(QPointF(10, 10)).toPoint());
        QVERIFY(!canvas->hasActiveFocus());
        QCOMPARE(window->cursor().shape(), Qt::ArrowCursor);

        // Close the error message popup.
        // QTest::mouseClick(window, Qt::LeftButton) didn't work on mac after a couple of data row runs,
        // so we use the keyboard to close it instead.
        QTest::keyClick(window, Qt::Key_Escape);
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

void tst_App::saveTilesetProject()
{
    createNewTilesetProject();

    // Store a snapshot of the canvas before we alter it.
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalImage = imageGrabber.takeImage();

    // Draw a tile on first.
    switchMode(TileCanvas::TileMode);

    // QTBUG-53466
    setCursorPosInScenePixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(QPoint(0, 0)));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Save our drawing.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + "/project.stp");
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

void tst_App::saveAsAndLoadTilesetProject()
{
    createNewTilesetProject();

    // Save the untouched project.
    const QString originalProjectPath = tempProjectDir->path() + "/project.stp";
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
    const QString savedProjectPath = tempProjectDir->path() + "/project2.stp";
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

void tst_App::saveAsAndLoad_data()
{
    addActualProjectTypes();
}

void tst_App::saveAsAndLoad()
{
    // Ensure that things that are common to all project types are saved,
    // like guides, pane offset and zoom, etc.

    QFETCH(Project::Type, projectType);
    QFETCH(QString, projectExtension);

    createNewProject(projectType);

    if (!canvas->rulersVisible()) {
        triggerRulersVisible();
        QCOMPARE(canvas->rulersVisible(), true);
    }

    if (!canvas->guidesVisible()) {
        triggerGuidesVisible();
        QCOMPARE(canvas->guidesVisible(), true);
    }

    QQuickItem *firstHorizontalRuler = canvas->findChild<QQuickItem*>("firstHorizontalRuler");
    QVERIFY(firstHorizontalRuler);
    const qreal rulerThickness = firstHorizontalRuler->height();

    // TODO: fix this failure so that we can test it properly
//    panTopLeftTo(rulerThickness, rulerThickness);
    canvas->firstPane()->setOffset(QPoint(rulerThickness, rulerThickness));

    // Drop a horizontal guide onto the canvas.
    setCursorPosInPixels(QPoint(50, rulerThickness / 2));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInPixels(QPoint(50, rulerThickness + 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);

    // Zoom in.
    setCursorPosInPixels(QPoint(0, 0));
    const int expectedZoomLevel = 5;
    CanvasPane *currentPane = canvas->currentPane();
    for (int i = 0; currentPane->zoomLevel() < expectedZoomLevel && i < expectedZoomLevel; ++i)
        wheelEvent(canvas, cursorWindowPos, 1);
    QCOMPARE(currentPane->zoomLevel(), expectedZoomLevel);

    QCOMPARE(canvas->isSplitScreen(), true);
    QTRY_COMPARE(canvas->firstPane()->size(), 0.5);
    QCOMPARE(canvas->secondPane()->size(), 0.5);

    // Store the original offsets, etc.
    const QPoint firstPaneOffset = canvas->firstPane()->offset();
    const int firstPaneZoomLevel = canvas->firstPane()->zoomLevel();
    const qreal firstPaneSize = canvas->firstPane()->size();
    const QPoint secondPaneOffset = canvas->secondPane()->offset();
    const int secondPaneZoomLevel = canvas->secondPane()->zoomLevel();
    const qreal secondPaneSize = canvas->secondPane()->size();

    // Save the project.
    const QString savedProjectPath = tempProjectDir->path() + "/saveAsAndLoad-project." + projectExtension;
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(project->url().toLocalFile(), savedProjectPath);

    // Close the project.
    triggerCloseProject();
    QVERIFY(!project->hasLoaded());

    // Load the saved file.
    project->load(QUrl::fromLocalFile(savedProjectPath));
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);
    QCOMPARE(canvas->firstPane()->offset(), firstPaneOffset);
    QCOMPARE(canvas->firstPane()->zoomLevel(), firstPaneZoomLevel);
    QCOMPARE(canvas->firstPane()->size(), firstPaneSize);
    QCOMPARE(canvas->secondPane()->offset(), secondPaneOffset);
    QCOMPARE(canvas->secondPane()->zoomLevel(), secondPaneZoomLevel);
    QCOMPARE(canvas->secondPane()->size(), secondPaneSize);
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
    tilesetProject->saveAs(QUrl::fromLocalFile(tempProjectDir->path() + "/project.stp"));
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
    setCursorPosInScenePixels(cursorPos + QPoint(0, 1));
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
    setCursorPosInScenePixels(toolSize, toolSize / 2);
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
    setCursorPosInScenePixels(cursorPos.x(), cursorPos.y() + 1);
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

    setCursorPosInScenePixels(0, 1);
    const QImage originalImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    QImage lastImage = originalImage;
    QTest::mouseMove(window, cursorWindowPos);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos)->pixelColor(cursorPos), tileCanvas->penForegroundColour());
    QVERIFY(*tilesetProject->tileAt(cursorPos)->tileset()->image() != lastImage);

    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    setCursorPosInScenePixels(0, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(tilesetProject->tileAt(cursorPos)->pixelColor(cursorPos), tileCanvas->penForegroundColour());
    QVERIFY(*tilesetProject->tileAt(cursorPos)->tileset()->image() != lastImage);

    // Go back over the same pixels.
    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    setCursorPosInScenePixels(0, 1);
    QTest::mouseMove(window, cursorWindowPos);
    tilesetProject->tileAt(cursorPos)->tileset()->image()->save("C:/dev/cur.png");
    lastImage.save("C:/dev/last.png");
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);

    setCursorPosInScenePixels(0, 2);
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
    setCursorPosInScenePixels(0, y);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    for (; x < tilesetProject->tileWidth(); ++x) {
        setCursorPosInScenePixels(x, y);
        QTest::mouseMove(window, cursorWindowPos);
        QCOMPARE(tilesetProject->tileAt(cursorPos)->pixelColor(cursorPos), tileCanvas->penForegroundColour());
    }
    // The last pixel is on the next tile.
    setCursorPosInScenePixels(++x, y);
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

    setCursorPosInScenePixels(250, 250);
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
    setCursorPosInScenePixels(0, 0);
    drawPixelAtCursorPos();

    setCursorPosInScenePixels(1, 0);
    drawPixelAtCursorPos();

    setCursorPosInScenePixels(1, 1);
    drawPixelAtCursorPos();

    setCursorPosInScenePixels(0, 1);
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
    setCursorPosInScenePixels(1, 0);
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

    setCursorPosInScenePixels(tileCanvas->width() / 2 + tilesetProject->tileWidth() * 1.5, 1);
    cursorPos.setX(cursorPos.x() - tileCanvas->width() / 2);
    lastTile = tilesetProject->tileAt(cursorPos);
    QVERIFY(!lastTile);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(tilesetProject->tileAt(cursorPos) != lastTile);

    // Remove split.
    triggerSplitScreen();
    QVERIFY(!app.settings()->isSplitScreen());
    QCOMPARE(tileCanvas->firstPane()->size(), 1.0);
    QCOMPARE(tileCanvas->secondPane()->size(), 0.0);

    // Add it back again.
    triggerSplitScreen();
    QVERIFY(app.settings()->isSplitScreen());
    QCOMPARE(tileCanvas->firstPane()->size(), 0.5);
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
    for (int i = Project::TilesetType; i <= Project::LayeredImageType; ++i) {
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
        setCursorPosInScenePixels(100, 100, false);
        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        // Use base class project since this code also covers layered image projects.
        QVERIFY(project->hasUnsavedChanges());

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

    // Ensure that the tileset swatch flickable has the correct contentY.
    QQuickItem *tilesetSwatchFlickable = tilesetSwatch->findChild<QQuickItem*>("tilesetSwatchFlickable");
    QVERIFY(tilesetSwatchFlickable);
    QVERIFY(tilesetSwatchFlickable->property("contentY").isValid());
    QCOMPARE(tilesetSwatchFlickable->property("contentY").toReal(), 0.0);

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

void tst_App::colourPickerHexField()
{
    createNewImageProject();

    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    QVERIFY(hexTextField);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penForegroundColour().name());

    const QColor originalPenColour = canvas->penForegroundColour();

    mouseEventOnCentre(hexTextField, MouseClick);
    QCOMPARE(hexTextField->property("activeFocus").toBool(), true);

    keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_Backspace);
    QCOMPARE(hexTextField->property("text").toString(), QString());
    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

    QTest::keyClick(window, Qt::Key_A);
    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a"));
    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

    // Invalid character.
    QTest::keyClick(window, Qt::Key_Z);
    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a"));
    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

    QTest::keyClick(window, Qt::Key_1);
    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a1"));
    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

    QTest::keyClick(window, Qt::Key_2);
    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a12"));
    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

    QTest::keyClick(window, Qt::Key_3);
    QTest::keyClick(window, Qt::Key_4);
    QTest::keyClick(window, Qt::Key_5);
    QCOMPARE(hexTextField->property("text").toString(), QLatin1String("a12345"));
    QCOMPARE(canvas->penForegroundColour(), originalPenColour);

    QTest::keyClick(window, Qt::Key_Enter);
    QCOMPARE(canvas->penForegroundColour(), QColor("#a12345"));
    // Accepting the input should give the canvas focus.
    QCOMPARE(canvas->hasActiveFocus(), true);

    // Give the text field focus again.
    mouseEventOnCentre(hexTextField, MouseClick);
    QCOMPARE(hexTextField->property("activeFocus").toBool(), true);

    // Clear the field.
    keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_Backspace);
    QCOMPARE(hexTextField->property("text").toString(), QString());

    // Cancel the changes.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->penForegroundColour(), QColor("#a12345"));
    // Accepting the input should give the canvas focus.
    QCOMPARE(canvas->hasActiveFocus(), true);
}

void tst_App::eraseImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::eraseImageCanvas()
{
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    switchTool(ImageCanvas::EraserTool);
    changeToolSize(1);

    // Make sure that the edges of the canvas can be erased.
    setCursorPosInScenePixels(project->widthInPixels() - 1, 0);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    QCOMPARE(canvas->currentProjectImage()->pixelColor(cursorPos), QColor(Qt::transparent));
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

QDebug operator<<(QDebug debug, const SelectionData &data)
{
    debug << "press:" << data.pressScenePos
          << "release:" << data.releaseScenePos
          << "expected area:" << data.expectedSelectionArea;
    return debug;
}

QString selectionDataToString(const SelectionData &data)
{
    QString string;
    QDebug stringBuilder(&string);
    stringBuilder << data;
    return string;
}

// Can't call this toString(); interferes with testlib code
QString rectToString(const QRect &rect)
{
    QString string;
    QDebug stringBuilder(&string);
    stringBuilder << rect;
    return string;
}

QByteArray selectionAreaFailureMessage(ImageCanvas *canvas, const SelectionData &selectionData, const QRect &expectedArea)
{
    return qPrintable(QString::fromLatin1("Data: %1 \n      Actual area: %2\n    Expected area: %3")
        .arg(selectionDataToString(selectionData), rectToString(canvas->selectionArea()), rectToString(expectedArea)));
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
    selectionData << SelectionData(QPoint(0, 0), QPoint(256, 256), QRect(0, 0, 256, 256));
    // TODO - these fail:
//    selectionData << SelectionData(QPoint(30, 30), QPoint(0, 0), QRect(0, 0, 30, 30));
//    selectionData << SelectionData(QPoint(256, 256), QPoint(246, 246), QRect(246, 246, 10, 10));

    foreach (const SelectionData &data, selectionData) {
        // Pressing outside the canvas should make the selection start at {0, 0}.
        setCursorPosInScenePixels(data.pressScenePos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        const QPoint boundExpectedPressPos = QPoint(qBound(0, data.pressScenePos.x(), project->widthInPixels()),
            qBound(0, data.pressScenePos.y(), project->heightInPixels()));
        const QRect expectedPressArea = QRect(boundExpectedPressPos, QSize(0, 0));
        QVERIFY2(canvas->selectionArea() == expectedPressArea, selectionAreaFailureMessage(canvas, data, expectedPressArea));

        setCursorPosInScenePixels(data.releaseScenePos);
        QTest::mouseMove(window, cursorWindowPos);
        QVERIFY2(canvas->selectionArea() == data.expectedSelectionArea, selectionAreaFailureMessage(canvas, data, data.expectedSelectionArea));

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY2(canvas->selectionArea() == data.expectedSelectionArea, selectionAreaFailureMessage(canvas, data, data.expectedSelectionArea));

        // Cancel the selection so that we can do the next one.
        switchTool(ImageCanvas::PenTool);
        QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));
        switchTool(ImageCanvas::SelectionTool);
    }
}

void tst_App::cancelSelectionToolImageCanvas()
{
    createNewImageProject();

    switchTool(ImageCanvas::SelectionTool);

    // Select an area.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(10, 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 10, 10));

    // Switching tools should clear the selection.
    switchTool(ImageCanvas::PenTool);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));

    switchTool(ImageCanvas::SelectionTool);
    QCOMPARE(project->hasUnsavedChanges(), false);

    // Select an area.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(10, 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 10, 10));
    // Hover to the centre of it
    setCursorPosInScenePixels(QPoint(5, 5));
    QTest::mouseMove(window, cursorWindowPos);
    // The cursor shape should change.
    QCOMPARE(window->cursor().shape(), Qt::SizeAllCursor);

    // Pressing outside of the selection should clear it.
    setCursorPosInScenePixels(QPoint(20, 20));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::BlankCursor);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    // ImageCanvas has to assume that it might still be selection at this point.
    QCOMPARE(canvas->selectionArea(), QRect(20, 20, 0, 0));
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));
    QCOMPARE(project->hasUnsavedChanges(), false);

    // Select an area.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(10, 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 10, 10));

    // Pressing escape should clear the selection.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));
}

void tst_App::moveSelectionImageCanvas_data()
{
    QTest::addColumn<Project::Type>("projectType");
    QTest::addColumn<bool>("transparentBackground");

    for (int i = Project::ImageType; i <= Project::LayeredImageType; ++i) {
        const Project::Type type = static_cast<Project::Type>(i);
        const QString typeAsString = Project::typeToString(type);

        QTest::newRow(qPrintable(QString::fromLatin1("%1, white background").arg(typeAsString))) << type << false;
        QTest::newRow(qPrintable(QString::fromLatin1("%1, transparent background").arg(typeAsString))) << type << true;
    }
}

void tst_App::moveSelectionImageCanvas()
{
    QFETCH(Project::Type, projectType);
    QFETCH(bool, transparentBackground);

    if (projectType == Project::ImageType)
        createNewImageProject(256, 256, transparentBackground);
    else if (projectType == Project::LayeredImageType)
        createNewLayeredImageProject(256, 256, transparentBackground);
    else
        QFAIL("Test doesn't support this project type");

    if (transparentBackground) {
        QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::transparent));
        QCOMPARE(canvas->currentProjectImage()->pixelColor(255, 255), QColor(Qt::transparent));
    }

    const QColor backgroundColour = transparentBackground ? Qt::transparent : Qt::white;

    // Draw a square of black pixels.
    switchTool(ImageCanvas::PenTool);
    changeToolSize(5);
    setCursorPosInScenePixels(2, 2);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));

    changeToolSize(1);
    switchTool(ImageCanvas::SelectionTool);

    // Select an area.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(5, 5));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 5, 5));

    // Drag the selection somewhere else.
    setCursorPosInScenePixels(QPoint(2, 2));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(20, 20));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(18, 18, 5, 5));

    // The project's actual image contents shouldn't change until the move has been confirmed.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
    QImage iii = *canvas->currentProjectImage();
    QCOMPARE(canvas->currentProjectImage()->pixelColor(18, 18), backgroundColour);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(22, 22), backgroundColour);

    // Clear the selection.
    switchTool(ImageCanvas::PenTool);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::transparent));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(18, 18), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(22, 22), QColor(Qt::black));

    switchTool(ImageCanvas::SelectionTool);

    // Undo the selection move.
    QVERIFY(canvas->hasActiveFocus());
    triggerShortcut("undoShortcut", app.settings()->undoShortcut());
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
    QEXPECT_FAIL("ImageType, transparent background", "TODO", Abort);
    QEXPECT_FAIL("LayeredImageType, transparent background", "TODO", Abort);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(18, 18), backgroundColour);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(22, 22), backgroundColour);

    // Select the same area again.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(5, 5));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 5, 5));

    // Drag the selection half way down past its original position.
    setCursorPosInScenePixels(QPoint(2, 2));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(2, 4));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 2, 5, 5));
    // The project's actual image contents shouldn't change until the move has been confirmed.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));

    // "Undo" the selection move. The selection move was never confirmed, so this
    // will take the ImageCanvas-shortcutOverride path.
    triggerShortcut("undoShortcut", app.settings()->undoShortcut());
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
}

void tst_App::moveSelectionWithKeysImageCanvas()
{
    createNewImageProject(256, 256);

    // Draw a square of black pixels.
    switchTool(ImageCanvas::PenTool);
    changeToolSize(5);
    setCursorPosInScenePixels(2, 2);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageProject->image()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(imageProject->image()->pixelColor(4, 4), QColor(Qt::black));

    changeToolSize(1);
    switchTool(ImageCanvas::SelectionTool);

    // Select an area.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(5, 5));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 5, 5));

    // Move the selection somewhere else.
    QTest::keyClick(window, Qt::Key_Right);
    QCOMPARE(canvas->selectionArea(), QRect(1, 0, 5, 5));

    // The project's actual image contents shouldn't change until the move has been confirmed.
    QCOMPARE(imageProject->image()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(imageProject->image()->pixelColor(4, 4), QColor(Qt::black));

    // Clear the selection, confirming the move.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(imageProject->image()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(imageProject->image()->pixelColor(0, 4), QColor(Qt::transparent));
    QCOMPARE(imageProject->image()->pixelColor(1, 0), QColor(Qt::black));
    QCOMPARE(imageProject->image()->pixelColor(5, 4), QColor(Qt::black));
}

void tst_App::deleteSelectionImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::deleteSelectionImageCanvas()
{
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    switchTool(ImageCanvas::SelectionTool);

    // Select an area.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(10, 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 10, 10));

    QTest::keyClick(window, Qt::Key_Delete);
    // The selection should be cleared.
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));

    const QImage deletedPortion = canvas->currentProjectImage()->copy(0, 0, 10, 10);
    for (int y = 0; y < deletedPortion.size().height(); ++y) {
        for (int x = 0; x < deletedPortion.size().width(); ++x) {
            QCOMPARE(deletedPortion.pixelColor(x, y), QColor(Qt::transparent));
        }
    }
}

void tst_App::copyPaste_data()
{
    addImageProjectTypes();
}

void tst_App::copyPaste()
{
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    // Make comparing grabbed image pixels easier.
    panTopLeftTo(0, 0);

    // Draw a square of black pixels.
    switchTool(ImageCanvas::PenTool);
    changeToolSize(5);
    setCursorPosInScenePixels(12, 12);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(10, 10), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(14, 14), QColor(Qt::black));

    changeToolSize(1);
    switchTool(ImageCanvas::SelectionTool);

    // Select an area.
    setCursorPosInScenePixels(QPoint(10, 10));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(15, 15));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(10, 10, 5, 5));

    keySequence(window, QKeySequence::Copy);
    QCOMPARE(QGuiApplication::clipboard()->image(), canvas->currentProjectImage()->copy(10, 10, 5, 5));

    // Paste. The project's image shouldn't change until the paste selection is confirmed.
    keySequence(window, QKeySequence::Paste);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::white));
    QCOMPARE(canvas->hasSelection(), true);
    QCOMPARE(canvas->selectionArea(), QRect(10, 10, 5, 5));
    // However, the selection preview image should be visible...
    // Note: currently this won't change, so it's commented out.
    // If the location of the pasted image ever changes, uncomment this so that it's tested.
    //QVERIFY(imageGrabber.requestImage(canvas));
    //QTRY_VERIFY(imageGrabber.isReady());
    //QCOMPARE(imageGrabber.takeImage().pixelColor(2, 2), QColor(Qt::black));

    // Undo the paste while it's still selected.
    //keySequence(window, app.settings()->undoShortcut());
    //QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::white));
    //QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::white));
    //QCOMPARE(canvas->hasSelection(), false);
    //QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));

    // Redo the paste. There shouldn't be any selection, but the image should have been applied.
    //keySequence(window, app.settings()->redoShortcut());
    //QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    //QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
    //QCOMPARE(canvas->hasSelection(), false);
    //QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));
}

void tst_App::pasteFromExternalSource_data()
{
    addImageProjectTypes();
}

void tst_App::pasteFromExternalSource()
{
    // When pasting while a tool other than the selection tool is active,
    // the selection tool should be made active.
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    QCOMPARE(canvas->tool(), ImageCanvas::PenTool);

    QImage image(32, 32, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::blue);
    qGuiApp->clipboard()->setImage(image);

    keySequence(window, QKeySequence::Paste);
    QCOMPARE(canvas->tool(), ImageCanvas::SelectionTool);
    QCOMPARE(canvas->hasSelection(), true);

    // Confirm the selection.
    setCursorPosInScenePixels(33, 33);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, 100);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::blue));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(31, 31), QColor(Qt::blue));
}

void tst_App::flipPastedImage()
{
    // TODO
}

void tst_App::fillImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::fillImageCanvas()
{
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    // A fill on a canvas of this size would previously trigger a stack overflow
    // using a recursive algorithm.
    changeCanvasSize(90, 90);

    // Fill the canvas with black.
    switchTool(ImageCanvas::FillTool);
    setCursorPosInScenePixels(0, 0);
    mouseEvent(canvas, cursorWindowPos, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(project->widthInPixels() - 1,
                                                       project->heightInPixels() - 1), QColor(Qt::black));
}

void tst_App::greedyPixelFillImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::greedyPixelFillImageCanvas()
{
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    changeCanvasSize(40, 40);

    // Draw 4 separate pixels.
    setCursorPosInScenePixels(4, 4);
    drawPixelAtCursorPos();

    setCursorPosInScenePixels(35, 4);
    drawPixelAtCursorPos();

    setCursorPosInScenePixels(35, 35);
    drawPixelAtCursorPos();

    setCursorPosInScenePixels(4, 35);
    drawPixelAtCursorPos();

    switchTool(ImageCanvas::FillTool);
    canvas->setPenForegroundColour(Qt::blue);
    setCursorPosInScenePixels(4, 4);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::keyPress(window, Qt::Key_Shift);
    // For some reason there must be a delay in order for the shift modifier to work.
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, 100);
    QTest::keyRelease(window, Qt::Key_Shift);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::blue));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(35, 4), QColor(Qt::blue));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(35, 35), QColor(Qt::blue));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 35), QColor(Qt::blue));

    // Undo it.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(35, 4), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(35, 35), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 35), QColor(Qt::black));
}

void tst_App::pixelLineToolImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::pixelLineToolImageCanvas()
{
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    switchTool(ImageCanvas::PenTool);

    // Draw the start of the line.
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(project->hasUnsavedChanges(), true);

    // Draw the line itself.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::keyPress(window, Qt::Key_Shift);
    // For some reason there must be a delay in order for the shift modifier to work.
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, 100);
    QTest::keyRelease(window, Qt::Key_Shift);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::black));

    // Undo the line.
    mouseEventOnCentre(undoButton, MouseClick);
    // The initial press has to still be there.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::white));

    // Redo the line.
    mouseEventOnCentre(redoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::black));

    // Draw another line.
    setCursorPosInScenePixels(0, 4);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::keyPress(window, Qt::Key_Shift);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, 100);
    QTest::keyRelease(window, Qt::Key_Shift);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 3), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 4), QColor(Qt::black));

    // Undo the second line.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 3), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 4), QColor(Qt::white));

    // Undo the first line.
    mouseEventOnCentre(undoButton, MouseClick);
    // The initial press has to still be there.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::white));

    // Undo the inital press.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(project->hasUnsavedChanges(), false);
}

void tst_App::rulersAndGuides_data()
{
    addAllProjectTypes();
}

void tst_App::rulersAndGuides()
{
    QFETCH(Project::Type, projectType);

    createNewProject(projectType);

    triggerRulersVisible();
    QCOMPARE(app.settings()->areRulersVisible(), true);

    QQuickItem *firstHorizontalRuler = canvas->findChild<QQuickItem*>("firstHorizontalRuler");
    QVERIFY(firstHorizontalRuler);
    const qreal rulerThickness = firstHorizontalRuler->height();

    // Pan so that the top left of the canvas is at the rulers' corners.
    panTopLeftTo(rulerThickness, rulerThickness);

    // A guide should only be added when dropped outside of the ruler.
    setCursorPosInPixels(QPoint(50, rulerThickness / 2));
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(!canvas->pressedRuler());

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(canvas->pressedRuler());

    setCursorPosInPixels(QPoint(50, rulerThickness - 2));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(!canvas->pressedRuler());
    QCOMPARE(project->guides().size(), 0);
    QCOMPARE(project->undoStack()->canUndo(), false);

    // Drop a horizontal guide onto the canvas.
    setCursorPosInPixels(QPoint(50, rulerThickness / 2));
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(!canvas->pressedRuler());

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(canvas->pressedRuler());

    setCursorPosInPixels(QPoint(50, rulerThickness + 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(!canvas->pressedRuler());
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);
    QCOMPARE(project->undoStack()->canUndo(), true);

    // Undo.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(project->guides().size(), 0);
    QCOMPARE(project->undoStack()->canUndo(), false);

    // Redo.
    mouseEventOnCentre(redoButton, MouseClick);
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);
    QCOMPARE(project->undoStack()->canUndo(), true);

    // The cursor should change when over an existing guide.
    // For some reason it is necessary to move the mouse away like this...
    // works fine in actual application usage.
    QTest::mouseMove(window, cursorWindowPos + QPoint(1, 0));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::OpenHandCursor);

    // Move it.
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->pressedGuideIndex(), 0);
    QCOMPARE(window->cursor().shape(), Qt::ClosedHandCursor);

    setCursorPosInPixels(QPoint(50, rulerThickness + 20));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->guides().first().position(), 20);
    QCOMPARE(window->cursor().shape(), Qt::OpenHandCursor);

    // Undo.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);
    QTest::mouseMove(window, cursorWindowPos + QPoint(1, 0));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::BlankCursor);

    // Redo.
    mouseEventOnCentre(redoButton, MouseClick);
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 20);
    QTest::mouseMove(window, cursorWindowPos + QPoint(1, 0));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::OpenHandCursor);

    // Delete it by dragging it onto the ruler.
    setCursorPosInPixels(QPoint(50, rulerThickness + 20));
    QTest::mouseMove(window, cursorWindowPos);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->pressedGuideIndex(), 0);
    QCOMPARE(window->cursor().shape(), Qt::ClosedHandCursor);

    setCursorPosInPixels(QPoint(50, rulerThickness / 2));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(project->guides().isEmpty(), true);
    QCOMPARE(window->cursor().shape(), Qt::ArrowCursor);
}

void tst_App::addAndRemoveLayers()
{
    createNewLayeredImageProject();

    panTopLeftTo(0, 0);

    ImageLayer *expectedCurrentLayer = layeredImageProject->currentLayer();

    // Draw a blue square at {10, 10}.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), Qt::blue);

    // Ensure that the blue square is visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBlueDot = imageGrabber.takeImage();
    QCOMPARE(grabWithBlueDot.pixelColor(10, 10), Qt::blue);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    // The current layer shouldn't change..
    QCOMPARE(layeredImageProject->currentLayer(), expectedCurrentLayer);
    // ..but its index should, as new layers are added above all others.
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 2"));
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));

    // Select the new layer.
    selectLayer("Layer 2", 0);

    // Draw a red dot on the new layer.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(20, 20), Qt::red);

    // Ensure that both dots are visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBothDots = imageGrabber.takeImage();
    QCOMPARE(grabWithBothDots.pixelColor(10, 10), Qt::blue);
    QCOMPARE(grabWithBothDots.pixelColor(20, 20), Qt::red);

    {
        // Ensure that what the user sees is correct.
        QQuickItem *layerListView = window->findChild<QQuickItem*>("layerListView");
        QVERIFY(layerListView);
        QCOMPARE(layerListView->property("count"), 2);

        QQuickItem *layer1Delegate = nullptr;
        verifyLayerName(QLatin1String("Layer 1"), &layer1Delegate);
        QQuickItem *layer2Delegate = nullptr;
        verifyLayerName(QLatin1String("Layer 2"), &layer2Delegate);
        // The second layer was added last, so it should be at the top of the list.
        QVERIFY(layer1Delegate->y() > layer2Delegate->z());
    }

    // Select the original layer.
    selectLayer("Layer 1", 1);

    // Delete the original layer.
    expectedCurrentLayer = layeredImageProject->layerAt(0);

    QQuickItem *deleteLayerButton = window->findChild<QQuickItem*>("deleteLayerButton");
    QVERIFY(deleteLayerButton);
    mouseEventOnCentre(deleteLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 1);
    QCOMPARE(layeredImageProject->currentLayer(), expectedCurrentLayer);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 2"));

    // The blue dot should no longer be visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithRedDot = imageGrabber.takeImage();
    // It's going to be the checkered pattern, so we just make sure it's not blue rather than
    // trying to compare to something.
    QVERIFY(grabWithRedDot.pixelColor(10, 10) != Qt::blue);
    QCOMPARE(grabWithRedDot.pixelColor(20, 20), Qt::red);

    // Undo the deletion.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    verifyLayerName(QLatin1String("Layer 1"));

    // Both dots should be visible again.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), grabWithBothDots);
}

void tst_App::layerVisibility()
{
    createNewLayeredImageProject();

    // Make comparing grabbed image pixels easier.
    panTopLeftTo(0, 0);

    // Draw a blue square at {10, 10}.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), Qt::blue);

    // Ensure that the blue square is visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBlueDot = imageGrabber.takeImage();
    QCOMPARE(grabWithBlueDot.pixelColor(10, 10), Qt::blue);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    selectLayer("Layer 2", 0);

    // Draw a red dot at the same position.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), Qt::red);

    // Ensure that it's visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithRedDot = imageGrabber.takeImage();
    QCOMPARE(grabWithRedDot.pixelColor(10, 10), Qt::red);

    // Hide the current layer.
    QQuickItem *layer2Delegate = nullptr;
    verifyLayerName("Layer 2", &layer2Delegate);
    QQuickItem *layer2VisibilityCheckBox = layer2Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer2VisibilityCheckBox);
    mouseEventOnCentre(layer2VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);

    // Ensure that the layer has been hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithRedDotHidden = imageGrabber.takeImage();
    QCOMPARE(grabWithRedDotHidden.pixelColor(10, 10), Qt::blue);

    // Undo the visibility change.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), true);
    // The canvas should look as it did before it was hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), grabWithRedDot);
}

void tst_App::moveLayerUpAndDown()
{
    createNewLayeredImageProject();

    QCOMPARE(moveLayerDownButton->isEnabled(), false);
    QCOMPARE(moveLayerUpButton->isEnabled(), false);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    // It should be possible to move the lowest layer up but not down.
    QCOMPARE(moveLayerDownButton->isEnabled(), false);
    QCOMPARE(moveLayerUpButton->isEnabled(), true);

    // Make the new layer the current layer.
    selectLayer("Layer 2", 0);

    // It should be possible to move the highest layer down but not up.
    QCOMPARE(moveLayerDownButton->isEnabled(), true);
    QCOMPARE(moveLayerUpButton->isEnabled(), false);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 3);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    // It should be possible to move the middle layer both up and down.
    QCOMPARE(moveLayerDownButton->isEnabled(), true);
    QCOMPARE(moveLayerUpButton->isEnabled(), true);

    // Move the current layer up.
    mouseEventOnCentre(moveLayerUpButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 2"));

    // Move the current layer down.
    mouseEventOnCentre(moveLayerDownButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));

    // TODO: draw a different-coloured pixel on each layer and do screen grab comparisons
}

void tst_App::renameLayer()
{
    createNewLayeredImageProject();

    QQuickItem *delegate = nullptr;
    verifyLayerName("Layer 1", &delegate);
    QCOMPARE(delegate->property("checked").toBool(), true);

    QQuickItem *nameTextField = delegate->findChild<QQuickItem*>("layerNameTextField");
    QVERIFY(nameTextField);

    // A single click should not give the text field focus.
    mouseEventOnCentre(nameTextField, MouseClick);
    QCOMPARE(nameTextField->hasActiveFocus(), false);

    // A double click should.
    mouseEventOnCentre(nameTextField, MouseDoubleClick);
    QCOMPARE(nameTextField->hasActiveFocus(), true);

    // Enter some text.
    QTest::keyClick(window, Qt::Key_Z);
    QCOMPARE(nameTextField->property("text").toString(), QLatin1String("Layer 1z"));
    // The layer name shouldn't change until it's confirmed.
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));

    // Escape should cancel whatever we were inputting.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(nameTextField->property("text").toString(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));

    // Enter some text and confirm the changes.
    mouseEventOnCentre(nameTextField, MouseDoubleClick);
    QCOMPARE(nameTextField->hasActiveFocus(), true);
    QTest::keyClick(window, Qt::Key_2);
    QCOMPARE(nameTextField->property("text").toString(), QLatin1String("Layer 12"));
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));
    QTest::keyClick(window, Qt::Key_Enter);
    QCOMPARE(nameTextField->property("text").toString(), QLatin1String("Layer 12"));
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 12"));

    // Undo the name change.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));
}

void tst_App::saveAndLoadLayeredImageProject()
{
    createNewLayeredImageProject();

    // Make comparing grabbed image pixels easier.
    panTopLeftTo(0, 0);

    // Draw a blue square.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), Qt::blue);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    selectLayer("Layer 2", 0);

    // Draw a red dot.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(20, 20), Qt::red);

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabBeforeSaving = imageGrabber.takeImage();
    QCOMPARE(grabBeforeSaving.pixelColor(10, 10), Qt::blue);
    QCOMPARE(grabBeforeSaving.pixelColor(20, 20), Qt::red);

    // Save.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + "/layeredimageproject.slp");
    layeredImageProject->saveAs(saveUrl);
    VERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!layeredImageProject->hasUnsavedChanges());

    // Close.
    triggerCloseProject();
    QVERIFY(!layeredImageProject->hasLoaded());
    QCOMPARE(layeredImageProject->layerCount(), 0);
    // The layer panel shouldn't show any layers.
    QQuickItem *layerListView = window->findChild<QQuickItem*>("layerListView");
    QVERIFY(layerListView);
    QCOMPARE(layerListView->property("count").toInt(), 0);

    // Load the saved file.
    layeredImageProject->load(saveUrl);
    VERIFY_NO_CREATION_ERRORS_OCCURRED();

    panTopLeftTo(0, 0);

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabAfterSaving = imageGrabber.takeImage();
    QCOMPARE(grabAfterSaving.pixelColor(10, 10), Qt::blue);
    QCOMPARE(grabAfterSaving.pixelColor(20, 20), Qt::red);
}

void tst_App::layerVisibilityAfterMoving()
{
    createNewLayeredImageProject();

    // Make comparing grabbed image pixels easier.
    panTopLeftTo(0, 0);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    selectLayer("Layer 2", 0);

    // Draw a red dot.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(20, 20), Qt::red);

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabBeforeSaving = imageGrabber.takeImage();
    QCOMPARE(grabBeforeSaving.pixelColor(0, 0), Qt::white);
    QCOMPARE(grabBeforeSaving.pixelColor(20, 20), Qt::red);

    // Move it below Layer 1.
    mouseEventOnCentre(moveLayerDownButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));

    // Hide Layer 1. It should show the red square and a transparent background.
    QQuickItem *layer1Delegate = nullptr;
    verifyLayerName("Layer 1", &layer1Delegate);
    QQuickItem *layer1VisibilityCheckBox = layer1Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer1VisibilityCheckBox);
    mouseEventOnCentre(layer1VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->layerAt(0)->isVisible(), false);

    // Ensure that the layer has been hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Hidden = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Hidden.pixelColor(20, 20), Qt::red);

    // Show Layer 1. It should show only white.
    mouseEventOnCentre(layer1VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), true);
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Visible = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Visible.pixelColor(0, 0), Qt::white);
    QCOMPARE(grabWithLayer1Visible.pixelColor(20, 20), Qt::white);

    // Make Layer 1 the current layer.
    selectLayer("Layer 1", 0);

    // Move Layer 1 back down. The red square should be visible on a white background.
    mouseEventOnCentre(moveLayerDownButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Below = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Below.pixelColor(0, 0), Qt::white);
    QCOMPARE(grabWithLayer1Below.pixelColor(20, 20), Qt::red);
}

void tst_App::undoAfterAddLayer()
{
    createNewLayeredImageProject();

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->hasUnsavedChanges(), true);
    QCOMPARE(undoButton->isEnabled(), true);
}

int main(int argc, char *argv[])
{
    tst_App test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "tst_app.moc"
