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
#include <QPainter>
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
#include "projectmanager.h"
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
    void repeatedNewProject_data();
    void repeatedNewProject();
    void openClose_data();
    void openClose();
    void saveTilesetProject();
    void saveAsAndLoadTilesetProject();
    void saveAsAndLoad_data();
    void saveAsAndLoad();
    void animationPlayback();
    void keyboardShortcuts();
    void optionsShortcutCancelled();
    void optionsTransparencyCancelled();
    void showGrid();
    void undoPixels();
    void undoLargePixelPen();
    void undoTiles();
    void undoWithDuplicates();
    void undoTilesetCanvasSizeChange();
    void undoImageCanvasSizeChange();
    void undoImageSizeChange();
    void undoLayeredImageSizeChange();
    void undoPixelFill();
    void undoTileFill();
    void undoThickPen();
    void colours_data();
    void colours();
    void colourPickerSaturationHex();
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
    void colourPickerHexFieldTranslucent();
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
    void undoCopyPasteWithTransparency();
    void pasteFromExternalSource_data();
    void pasteFromExternalSource();
    void flipPastedImage();
    void selectionEdgePan_data();
    void selectionEdgePan();
    void panThenMoveSelection();

    void fillImageCanvas_data();
    void fillImageCanvas();
    void fillLayeredImageCanvas();
    void greedyPixelFillImageCanvas_data();
    void greedyPixelFillImageCanvas();
    void texturedFill_data();
    void texturedFill();
    void pixelLineToolImageCanvas_data();
    void pixelLineToolImageCanvas();
    void pixelLineToolTransparent_data();
    void pixelLineToolTransparent();
    void rulersAndGuides_data();
    void rulersAndGuides();
    void recentFiles();

    void addAndRemoveLayers();
    void layerVisibility();
    void moveLayerUpAndDown();
    void mergeLayerUpAndDown();
    void renameLayers();
    void saveAndLoadLayeredImageProject();
    void layerVisibilityAfterMoving();
//    void undoAfterAddLayer();
    void selectionConfirmedWhenSwitchingLayers();
    void autoExport();
    void exportFileNamedLayers();
    void disableToolsWhenLayerHidden();
    void undoMoveContents();
    void undoMoveContentsOfVisibleLayers();
};

typedef QVector<Project::Type> ProjectTypeVector;

tst_App::tst_App(int &argc, char **argv) :
    TestHelper(argc, argv)
{
}

void tst_App::newProjectWithNewTileset()
{
    QVERIFY2(createNewTilesetProject(32, 32, 5, 5), failureMessage);

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
    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);
    setCursorPosInTiles(0, 0);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(QPoint(0, 0)));
    QVERIFY(tilesetProject->hasUnsavedChanges());

    // Draw a pixel on that tile.
    QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);
    setCursorPosInScenePixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileset()->image(), expectedTilesetImage);

    // Save the project.
    const QUrl saveFileName = QUrl::fromLocalFile(tempProjectDir->path() + "/mytileset.stp");
    tilesetProject->saveAs(saveFileName);
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    // Should save the image at the same location as the project.
    const QString tilesetPath = tempProjectDir->path() + "/mytileset.png";
    QCOMPARE(tilesetProject->tilesetUrl(), QUrl::fromLocalFile(tilesetPath));
    QVERIFY(QFile::exists(tilesetPath));
    QCOMPARE(*tilesetProject->tileset()->image(), expectedTilesetImage);
}

void tst_App::repeatedNewProject_data()
{
    QTest::addColumn<ProjectTypeVector>("projectTypes");

    QTest::newRow("TilesetType, ImageType") << (ProjectTypeVector() << Project::TilesetType << Project::ImageType);
    QTest::newRow("ImageType, LayeredImageType") << (ProjectTypeVector() << Project::ImageType << Project::LayeredImageType);
    QTest::newRow("LayeredImageType, TilesetType") << (ProjectTypeVector() << Project::LayeredImageType << Project::TilesetType);
}

void tst_App::repeatedNewProject()
{
    QFETCH(ProjectTypeVector, projectTypes);

    foreach (auto projectType, projectTypes) {
        // Shouldn't crash on repeated opening of new projects.
        QVERIFY2(createNewProject(projectType), failureMessage);
    }
}

void tst_App::openClose_data()
{
    addAllProjectTypes();
}

void tst_App::openClose()
{
    QFETCH(Project::Type, projectType);

    // Create a new, valid project.
    QVERIFY2(createNewProject(projectType), failureMessage);

    if (projectType == Project::TilesetType) {
        // Test an invalid tileset URL.
        QTest::ignoreMessage(QtWarningMsg, "QFSFileEngine::open: No file name specified");
        const QUrl badUrl("doesnotexist");

        QSignalSpy creationFailedSpy(projectManager.data(), SIGNAL(creationFailed(QString)));
        QVERIFY(creationFailedSpy.isValid());

        projectManager->beginCreation(Project::TilesetType);
        projectManager->temporaryProject()->load(badUrl);
        projectManager->completeCreation();

        // The spy was reset, so we use creationFailed() to check the error.
        QCOMPARE(creationErrorOccurredSpy->size(), 0);

        QCOMPARE(project->url(), QUrl());
        // There was a project open before we attempted to load the invalid one.
        QCOMPARE(project->hasLoaded(), true);
        const QString errorMessage = QLatin1String("Tileset project files must have a .stp extension ()");
        QCOMPARE(creationFailedSpy.size(), 1);
        QCOMPARE(creationFailedSpy.at(0).at(0).toString(), errorMessage);
        const QObject *errorPopup = findPopupFromTypeName("ErrorPopup");
        QVERIFY(errorPopup);
        QVERIFY(errorPopup->property("visible").toBool());
        QCOMPARE(errorPopup->property("text").toString(), errorMessage);
        QVERIFY(errorPopup->property("focus").isValid());
        QVERIFY(errorPopup->property("focus").toBool());

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
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(project->url(), QUrl());
    QCOMPARE(project->hasLoaded(), false);

    // Hovering over the canvas should result in the default cursor being displayed.
    QTest::mouseMove(window, canvas->mapToScene(
        QPointF(canvas->width() / 2, canvas->height() / 2)).toPoint());
    QCOMPARE(window->cursor().shape(), Qt::ArrowCursor);

    QVERIFY2(createNewProject(projectType), failureMessage);
}

void tst_App::saveTilesetProject()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    // Store a snapshot of the canvas before we alter it.
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalImage = imageGrabber.takeImage();

    // Draw a tile on first.
    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

    // QTBUG-53466
    setCursorPosInScenePixels(10, 10);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(QPoint(0, 0)));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Save our drawing.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + "/project.stp");
    tilesetProject->saveAs(saveUrl);
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
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
    QVERIFY2(createNewTilesetProject(), failureMessage);

    // Save the untouched project.
    const QString originalProjectPath = tempProjectDir->path() + "/project.stp";
    tilesetProject->saveAs(QUrl::fromLocalFile(originalProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
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
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(tilesetProject->url().toLocalFile(), savedProjectPath);
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), originalCanvasImage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
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

    // Load the saved file.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!tilesetProject->hasLoaded());

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage closedCanvasImage = imageGrabber.takeImage();

    tilesetProject->load(QUrl::fromLocalFile(savedProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
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

    QVERIFY2(createNewProject(projectType), failureMessage);

    if (!canvas->rulersVisible()) {
        QVERIFY2(triggerRulersVisible(), failureMessage);
        QCOMPARE(canvas->rulersVisible(), true);
    }

    if (!canvas->guidesVisible()) {
        QVERIFY2(triggerGuidesVisible(), failureMessage);
        QCOMPARE(canvas->guidesVisible(), true);
    }

    QQuickItem *firstHorizontalRuler = canvas->findChild<QQuickItem*>("firstHorizontalRuler");
    QVERIFY(firstHorizontalRuler);
    const qreal rulerThickness = firstHorizontalRuler->height();

    // TODO: fix this failure so that we can test it properly
//    QVERIFY2(panTopLeftTo(rulerThickness, rulerThickness), failureMessage);
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
    QVERIFY2(zoomTo(5), failureMessage);

    // Ensure that the splitter is not locked.
    QCOMPARE(canvas->isSplitScreen(), true);
    QVERIFY2(setSplitterLocked(false), failureMessage);

    // Resize the first pane to make it smaller.
    const QPoint splitterCentre(canvas->width() / 2, canvas->height() / 2);
    QTest::mouseMove(window, splitterCentre);
    QCOMPARE(window->cursor().shape(), Qt::SplitHCursor);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, splitterCentre);
    QTest::mouseMove(window, QPoint(canvas->width() / 4, canvas->height() / 2));
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, QPoint(canvas->width() / 4, canvas->height() / 2));
    QVERIFY(qAbs(canvas->firstPane()->size() - 0.25) < 0.001);
    QVERIFY(qAbs(canvas->secondPane()->size() - 0.75) < 0.001);

    // Store the original offsets, etc.
    const QPoint firstPaneOffset = canvas->firstPane()->offset();
    const int firstPaneZoomLevel = canvas->firstPane()->integerZoomLevel();
    const qreal firstPaneSize = canvas->firstPane()->size();
    const QPoint secondPaneOffset = canvas->secondPane()->offset();
    const int secondPaneZoomLevel = canvas->secondPane()->integerZoomLevel();
    const qreal secondPaneSize = canvas->secondPane()->size();

    // Save the project.
    const QString savedProjectPath = tempProjectDir->path() + "/saveAsAndLoad-project." + projectExtension;
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(project->url().toLocalFile(), savedProjectPath);

    // Close the project.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!project->hasLoaded());

    // Load the saved file.
    QVERIFY2(loadProject(QUrl::fromLocalFile(savedProjectPath)), failureMessage);
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);
    QCOMPARE(canvas->firstPane()->offset(), firstPaneOffset);
    QCOMPARE(canvas->firstPane()->integerZoomLevel(), firstPaneZoomLevel);
    QCOMPARE(canvas->firstPane()->size(), firstPaneSize);
    QCOMPARE(canvas->secondPane()->offset(), secondPaneOffset);
    QCOMPARE(canvas->secondPane()->integerZoomLevel(), secondPaneZoomLevel);
    QCOMPARE(canvas->secondPane()->size(), secondPaneSize);

    if (projectType == Project::ImageType || projectType == Project::LayeredImageType) {
        // Test that the save shortcut works by drawing and then saving.
        setCursorPosInScenePixels(0, 0);
        QTest::mouseMove(window, cursorWindowPos);
        QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY(project->hasUnsavedChanges());

        QVERIFY2(triggerSaveProject(), failureMessage);
        QVERIFY(!project->hasUnsavedChanges());
    }
}

void tst_App::animationPlayback()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QCOMPARE(layeredImageProject->isUsingAnimation(), false);

    QVERIFY2(setAnimationPlayback(true), failureMessage);

    // Open the settings popup to modify the settings slightly.
    QQuickItem *animationPanelSettingsToolButton = window->findChild<QQuickItem*>("animationPanelSettingsToolButton");
    QVERIFY(animationPanelSettingsToolButton);
    mouseEventOnCentre(animationPanelSettingsToolButton, MouseClick);

    QObject *animationSettingsPopup = findPopupFromTypeName("AnimationSettingsPopup");
    QVERIFY(animationSettingsPopup);
    QTRY_COMPARE(animationSettingsPopup->property("visible").toBool(), true);

    // Increase FPS.
    QQuickItem *animationFpsSpinBox = window->findChild<QQuickItem*>("animationFpsSpinBox");
    QVERIFY(animationFpsSpinBox);
    QCOMPARE(animationFpsSpinBox->property("value").toInt(), 4);

    mouseEvent(animationFpsSpinBox, QPoint(animationFpsSpinBox->width() - 10,
        animationFpsSpinBox->height() / 2), MouseClick);
    QCOMPARE(animationFpsSpinBox->property("value").toInt(), 4 + 1);

    // Increase frame x.
    QQuickItem *animationFrameXSpinBox = window->findChild<QQuickItem*>("animationFrameXSpinBox");
    QVERIFY(animationFrameXSpinBox);
    QCOMPARE(animationFrameXSpinBox->property("value").toInt(), 0);

    mouseEvent(animationFrameXSpinBox, QPoint(animationFrameXSpinBox->width() - 10,
        animationFrameXSpinBox->height() / 2), MouseClick);
    QCOMPARE(animationFrameXSpinBox->property("value").toInt(), 1);

    // Increase frame y.
    QQuickItem *animationFrameYSpinBox = window->findChild<QQuickItem*>("animationFrameYSpinBox");
    QVERIFY(animationFrameYSpinBox);
    QCOMPARE(animationFrameYSpinBox->property("value").toInt(), 0);

    mouseEvent(animationFrameYSpinBox, QPoint(animationFrameYSpinBox->width() - 10,
        animationFrameYSpinBox->height() / 2), MouseClick);
    QCOMPARE(animationFrameYSpinBox->property("value").toInt(), 1);

    // Increase frame width.
    QQuickItem *animationFrameWidthSpinBox = window->findChild<QQuickItem*>("animationFrameWidthSpinBox");
    QVERIFY(animationFrameWidthSpinBox);
    QCOMPARE(animationFrameWidthSpinBox->property("value").toInt(), 256 / 4);

    mouseEvent(animationFrameWidthSpinBox, QPoint(animationFrameWidthSpinBox->width() - 10,
        animationFrameWidthSpinBox->height() / 2), MouseClick);
    QCOMPARE(animationFrameWidthSpinBox->property("value").toInt(), 256 / 4 + 1);

    // Increase frame height.
    QQuickItem *animationFrameHeightSpinBox = window->findChild<QQuickItem*>("animationFrameHeightSpinBox");
    QVERIFY(animationFrameHeightSpinBox);
    QCOMPARE(animationFrameHeightSpinBox->property("value").toInt(), 256);

    mouseEvent(animationFrameHeightSpinBox, QPoint(animationFrameHeightSpinBox->width() - 10,
        animationFrameHeightSpinBox->height() / 2), MouseClick);
    QCOMPARE(animationFrameHeightSpinBox->property("value").toInt(), 256 + 1);

    // Increase frame count.
    QQuickItem *animationFrameCountSpinBox = window->findChild<QQuickItem*>("animationFrameCountSpinBox");
    QVERIFY(animationFrameCountSpinBox);
    QCOMPARE(animationFrameCountSpinBox->property("value").toInt(), 4);

    mouseEvent(animationFrameCountSpinBox, QPoint(animationFrameCountSpinBox->width() - 10,
        animationFrameCountSpinBox->height() / 2), MouseClick);
    QCOMPARE(animationFrameCountSpinBox->property("value").toInt(), 4 + 1);

    // Click in the middle of the slider to increase the sacle.
    QQuickItem *animationPreviewScaleSlider = window->findChild<QQuickItem*>("animationPreviewScaleSlider");
    QVERIFY(animationPreviewScaleSlider);
    mouseEventOnCentre(animationPreviewScaleSlider, MouseClick);
    const qreal modifiedScaleValue = layeredImageProject->animationPlayback()->scale();
    QVERIFY(modifiedScaleValue > 1.0);

    // Accept and close the settings popup.
    QQuickItem *saveButton = findDialogButtonFromText(animationSettingsPopup, "Save");
    QVERIFY(saveButton);
    mouseEventOnCentre(saveButton, MouseClick);
    QTRY_COMPARE(animationSettingsPopup->property("visible").toBool(), false);
    QCOMPARE(animationFpsSpinBox->property("value").toInt(), 4 + 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameWidth(), 256 / 4 + 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameHeight(), 256 + 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameCount(), 4 + 1);

    mouseEventOnCentre(animationPlayPauseButton, MouseClick);
    QCOMPARE(layeredImageProject->animationPlayback()->isPlaying(), true);
    QCOMPARE(layeredImageProject->animationPlayback()->currentFrameIndex(), 0);

    // Let it play a bit.
    QTRY_VERIFY(layeredImageProject->animationPlayback()->currentFrameIndex() > 0);

    // Save.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/animationStuffSaved.slp"));
    layeredImageProject->saveAs(saveUrl);
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!layeredImageProject->hasUnsavedChanges());

    // Close.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!layeredImageProject->hasLoaded());
    QCOMPARE(layeredImageProject->isUsingAnimation(), false);

    // Load the saved file and check that our custom settings were remembered.
    layeredImageProject->load(saveUrl);
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(layeredImageProject->isUsingAnimation(), true);
    QCOMPARE(animationFpsSpinBox->property("value").toInt(), 4 + 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameX(), 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameY(), 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameWidth(), 256 / 4 + 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameHeight(), 256 + 1);
    QCOMPARE(layeredImageProject->animationPlayback()->frameCount(), 4 + 1);
    QCOMPARE(layeredImageProject->animationPlayback()->scale(), modifiedScaleValue);
}

void tst_App::keyboardShortcuts()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

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
    QVERIFY2(triggerOptions(), failureMessage);
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

void tst_App::optionsShortcutCancelled()
{
    // Ensure that cancelling the options dialog after changing a shortcut cancels the shortcut change.
    QVERIFY2(createNewTilesetProject(), failureMessage);

    // Open options dialog.
    QVERIFY2(triggerOptions(), failureMessage);
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
    QVERIFY2(triggerOptions(), failureMessage);
    QVERIFY(optionsDialog->property("visible").toBool());
    QTRY_COMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    // Close the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
}

void tst_App::optionsTransparencyCancelled()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Open options dialog.
    QVERIFY2(triggerOptions(), failureMessage);
    const QObject *optionsDialog = findPopupFromTypeName("OptionsDialog");
    QVERIFY(optionsDialog);
    QVERIFY(optionsDialog->property("visible").toBool());

    // Open the general tab.
    QQuickItem *generalTabButton = optionsDialog->findChild<QQuickItem*>("generalTabButton");
    QVERIFY(generalTabButton);
    mouseEventOnCentre(generalTabButton, MouseClick);

    // Give "checkerColour1TextField" focus.
    QQuickItem *checkerColour1TextField = optionsDialog->findChild<QQuickItem*>("checkerColour1TextField");
    QVERIFY(checkerColour1TextField);
    QCOMPARE(checkerColour1TextField->property("text").toString(), app.settings()->checkerColour1().name().right(6));
    mouseEventOnCentre(checkerColour1TextField, MouseClick);
    QVERIFY(checkerColour1TextField->hasActiveFocus());

    // Input a colour.
    QTest::keySequence(window, QKeySequence(QKeySequence::SelectAll));
    QTest::keyClick(window, Qt::Key_1);
    QTest::keyClick(window, Qt::Key_2);
    QTest::keyClick(window, Qt::Key_3);
    QTest::keyClick(window, Qt::Key_4);
    QTest::keyClick(window, Qt::Key_5);
    QTest::keyClick(window, Qt::Key_6);
    QCOMPARE(checkerColour1TextField->property("text").toString(), QLatin1String("123456"));

    // Press Enter to accept it.
    QTest::keyClick(window, Qt::Key_Return);
    QCOMPARE(checkerColour1TextField->property("text").toString(), QLatin1String("123456"));
    // Shortcut shouldn't change until we hit "OK".
    QCOMPARE(app.settings()->checkerColour1(), app.settings()->defaultCheckerColour1());

    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(!optionsDialog->property("visible").toBool());
    // Cancelling the dialog shouldn't change anything.
    QCOMPARE(app.settings()->checkerColour1(), app.settings()->defaultCheckerColour1());

    // Reopen the dialog to make sure that the editor shows the default value.
    QVERIFY2(triggerOptions(), failureMessage);
    QVERIFY(optionsDialog->property("visible").toBool());
    QTRY_COMPARE(checkerColour1TextField->property("text").toString(), app.settings()->defaultCheckerColour1().name().right(6));
    QCOMPARE(app.settings()->checkerColour1(), app.settings()->defaultCheckerColour1());

    // Close the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
}

void tst_App::showGrid()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    // Store a screenshot of the canvas so we can ensure that the grid lines
    // aren't actually visible to the user.
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage withGrid = imageGrabber.takeImage();

    QVERIFY(app.settings()->isGridVisible());
    // Toggle the option.
    QVERIFY2(triggerGridVisible(), failureMessage);
    QVERIFY(!app.settings()->isGridVisible());

    // Close the view menu.
    QTest::keyClick(window, Qt::Key_Escape);

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage withoutGrid = imageGrabber.takeImage();
    QVERIFY(withoutGrid != withGrid);

    // Show the grid again.
    QVERIFY2(triggerGridVisible(), failureMessage);
    QVERIFY(app.settings()->isGridVisible());
}

void tst_App::undoPixels()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);
    QVERIFY2(switchTool(TileCanvas::PenTool), failureMessage);

    // It's a new project.
    QVERIFY(tilesetProject->canSave());
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    // Save the project so that we can test hasUnsavedChanges.
    tilesetProject->saveAs(QUrl::fromLocalFile(tempProjectDir->path() + "/project.stp"));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!tilesetProject->canSave());
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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

    QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);

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
    QVERIFY2(triggerRevert(), failureMessage);
    QVERIFY(!tilesetProject->tileAt(cursorPos));
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
}

void tst_App::undoLargePixelPen()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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

    QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);

    const QImage originalTilesetImage = *tilesetProject->tileset()->image();

    const int toolSize = tilesetProject->tileWidth();
    QVERIFY2(changeToolSize(toolSize), failureMessage);

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
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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
    QVERIFY2(triggerRevert(), failureMessage);
    QVERIFY(!tilesetProject->tileAt(cursorPos));
    QVERIFY(!undoButton->isEnabled());
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
}

// Test going back over the same pixels several times.
void tst_App::undoWithDuplicates()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

    // Draw on a tile so that we can operate on its pixels.
    setCursorPosInTiles(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);

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
    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

    // Draw on another tile next to the existing one.
    setCursorPosInTiles(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->tileAt(cursorPos));

    QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);

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
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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

    QVERIFY2(changeCanvasSize(9, 9), failureMessage);
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
    QVERIFY2(changeCanvasSize(10, 9), failureMessage);
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(tilesetProject->tiles(), originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 10 * 10);

    QVERIFY2(changeCanvasSize(9, 10), failureMessage);
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(tilesetProject->tiles(), originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 10 * 10);

    QVERIFY2(changeCanvasSize(12, 12), failureMessage);
    QCOMPARE(tilesetProject->tiles().size(), 12 * 12);
    QCOMPARE(tilesetProject->tiles().last(), -1);
}

void tst_App::undoImageCanvasSizeChange()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    QCOMPARE(imageProject->widthInPixels(), 256);
    QCOMPARE(imageProject->heightInPixels(), 256);

    // Draw something near the bottom right.
    setCursorPosInScenePixels(250, 250);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageProject->image()->pixelColor(cursorPos), imageCanvas->penForegroundColour());

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preSizeChangeCanvasSnapshot = imageGrabber.takeImage();

    // Change the size so that our drawing is removed.
    QVERIFY2(changeCanvasSize(200, 200), failureMessage);

    // Ensure that the canvas was repainted after the size change.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY(imageGrabber.takeImage() != preSizeChangeCanvasSnapshot);

    // Undo it.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(imageProject->image()->size(), QSize(256, 256));
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), preSizeChangeCanvasSnapshot);
}

void tst_App::undoImageSizeChange()
{
    QVERIFY2(createNewImageProject(12, 12), failureMessage);

    QVERIFY2(changeToolSize(4), failureMessage);

    setCursorPosInScenePixels(2, 2);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageProject->image()->pixelColor(QPoint(0, 0)), imageCanvas->penForegroundColour());
    QCOMPARE(imageProject->image()->pixelColor(QPoint(3, 3)), imageCanvas->penForegroundColour());

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preSizeChangeCanvasSnapshot = imageGrabber.takeImage();

    QVERIFY2(changeImageSize(6, 6), failureMessage);

    // The contents should have been scaled down by 50%.
    QCOMPARE(imageProject->image()->pixelColor(QPoint(0, 0)), imageCanvas->penForegroundColour());
    QCOMPARE(imageProject->image()->pixelColor(QPoint(1, 1)), imageCanvas->penForegroundColour());
    QCOMPARE(imageProject->image()->pixelColor(QPoint(2, 2)), QColor(Qt::white));

    // Move the mouse back so the image comparison works.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseMove(window, cursorWindowPos);

    // Ensure that the canvas was repainted after the size change.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preUndoSnapshot = imageGrabber.takeImage();
    QVERIFY(preUndoSnapshot != preSizeChangeCanvasSnapshot);

    // Undo the size change.
    mouseEventOnCentre(undoButton, MouseClick);

    QCOMPARE(imageProject->image()->size(), QSize(12, 12));
    QCOMPARE(imageProject->image()->pixelColor(QPoint(0, 0)), imageCanvas->penForegroundColour());
    QCOMPARE(imageProject->image()->pixelColor(QPoint(3, 3)), imageCanvas->penForegroundColour());

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage postUndoSnapshot = imageGrabber.takeImage();
    QCOMPARE(postUndoSnapshot, preSizeChangeCanvasSnapshot);
}

void tst_App::undoLayeredImageSizeChange()
{
    QVERIFY2(createNewLayeredImageProject(12, 12), failureMessage);

    QVERIFY2(changeToolSize(4), failureMessage);

    setCursorPosInScenePixels(2, 2);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(3, 3), QColor(Qt::black));

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    ImageLayer *layer1 = layeredImageProject->layerAt(1);
    ImageLayer *layer2 = layeredImageProject->layerAt(0);

    // Select the new layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw on the new layer.
    setCursorPosInScenePixels(6, 2);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer2->image()->pixelColor(4, 0), QColor(Qt::red));
    QCOMPARE(layer2->image()->pixelColor(7, 3), QColor(Qt::red));

    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preSizeChangeCanvasSnapshot = imageGrabber.takeImage();

    QVERIFY2(changeImageSize(6, 6), failureMessage);

    // The contents of both layers should have been scaled down by 50%.
    QCOMPARE(layer1->image()->pixelColor(QPoint(0, 0)), QColor(Qt::black));
    QCOMPARE(layer1->image()->pixelColor(QPoint(1, 1)), QColor(Qt::black));
    QCOMPARE(layer1->image()->pixelColor(QPoint(0, 2)), QColor(Qt::white));

    QCOMPARE(layer2->image()->pixelColor(QPoint(2, 0)), QColor(Qt::red));
    QCOMPARE(layer2->image()->pixelColor(QPoint(2, 1)), QColor(Qt::red));
    QCOMPARE(layer2->image()->pixelColor(QPoint(2, 2)), QColor(Qt::transparent));

    // Move the mouse back so the image comparison works.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseMove(window, cursorWindowPos);

    // Ensure that the canvas was repainted after the size change.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preUndoSnapshot = imageGrabber.takeImage();
    QVERIFY(preUndoSnapshot != preSizeChangeCanvasSnapshot);

    // Undo the size change.
    mouseEventOnCentre(undoButton, MouseClick);

    QCOMPARE(layer1->image()->size(), QSize(12, 12));
    QCOMPARE(layer1->image()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(layer1->image()->pixelColor(3, 3), QColor(Qt::black));

    QCOMPARE(layer2->image()->pixelColor(4, 0), QColor(Qt::red));
    QCOMPARE(layer2->image()->pixelColor(7, 3), QColor(Qt::red));

    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage postUndoSnapshot = imageGrabber.takeImage();
    QCOMPARE(postUndoSnapshot, preSizeChangeCanvasSnapshot);
}

void tst_App::undoPixelFill()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    setCursorPosInScenePixels(1, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    setCursorPosInScenePixels(1, 1);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    setCursorPosInScenePixels(0, 1);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

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
    QVERIFY2(switchTool(TileCanvas::FillTool), failureMessage);
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
    QVERIFY2(createNewTilesetProject(), failureMessage);

    // Draw a block of tiles.
    setCursorPosInTiles(0, 0);
    QVERIFY2(drawTileAtCursorPos(), failureMessage);

    setCursorPosInTiles(1, 0);
    QVERIFY2(drawTileAtCursorPos(), failureMessage);

    const Tile *targetTile = tilesetProject->tileAt(cursorPos);
    QVERIFY(targetTile);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(0, 0)), tileCanvas->penTile());
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(1, 0)), tileCanvas->penTile());

    // Try to fill it. The whole block should be filled.
    QVERIFY2(switchTool(TileCanvas::FillTool), failureMessage);

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

void tst_App::undoThickPen()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    QVERIFY2(changeToolSize(2), failureMessage);

    // First, try a single click.
    setCursorPosInScenePixels(QPoint(1, 1));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 1), QColor(Qt::black));

    // Undo it.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 0), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 1), QColor(Qt::white));

    // Next, try dragging.
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 1), QColor(Qt::black));

    setCursorPosInScenePixels(QPoint(1, 2));
//    QTest::qWait(2000);
    QTest::mouseMove(window, cursorWindowPos);
//    QTest::qWait(2000);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
//    QTest::qWait(30000);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 2), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 2), QColor(Qt::black));

    // Undo it.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 0), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 2), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 2), QColor(Qt::white));
}

void tst_App::colours_data()
{
    addAllProjectTypes();
}

void tst_App::colours()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);
    QCOMPARE(canvas->penForegroundColour(), QColor(Qt::black));
    QCOMPARE(canvas->penBackgroundColour(), QColor(Qt::white));

    QQuickItem *hueSlider = window->findChild<QQuickItem*>("hueSlider");
    QVERIFY(hueSlider);
    QCOMPARE(hueSlider->property("hue").toReal(), 0.0);

    // First try changing the foreground colour.
    mouseEvent(penForegroundColourButton, QPoint(1, 1), MouseClick);

    QQuickItem *saturationLightnessPicker = window->findChild<QQuickItem*>("saturationLightnessPicker");
    QVERIFY(saturationLightnessPicker);

    // Choose a colour.
    mouseEventOnCentre(saturationLightnessPicker, MouseClick);
    const QColor expectedColour = QColor("#c04141");
    QVERIFY2(fuzzyColourCompare(canvas->penForegroundColour(), expectedColour), failureMessage);
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
    QVERIFY2(fuzzyColourCompare(canvas->penForegroundColour(), expectedColour), failureMessage);

    // Hex field should represent background colour when selected.
    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    QVERIFY(hexTextField);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penBackgroundColour().name());

    // Hex field should represent foreground colour when selected.
    mouseEventOnCentre(penForegroundColourButton, MouseClick);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penForegroundColour().name());

    // TODO: fix issue where hue slider handle is missing
    // For now, we work around it.
    mouseEvent(hueSlider, QPointF(hueSlider->width() / 2, hueSlider->height() / 2), MouseClick);

    // Test that the "Lighter" button works.
    QColor oldColour = canvas->penForegroundColour();
    mouseEventOnCentre(lighterButton, MouseClick);
    QVERIFY(canvas->penForegroundColour().lightnessF() > oldColour.lightnessF());

    // Test that the "Darker" button works.
    oldColour = canvas->penForegroundColour();
    mouseEventOnCentre(darkerButton, MouseClick);
    QVERIFY(canvas->penForegroundColour().lightnessF() < oldColour.lightnessF());

    // Test that the "Saturate" button works.
    oldColour = canvas->penForegroundColour();
    mouseEventOnCentre(saturateButton, MouseClick);
    QVERIFY(canvas->penForegroundColour().saturationF() > oldColour.saturationF());

    // Test that the "Desaturate" button works.
    oldColour = canvas->penForegroundColour();
    mouseEventOnCentre(desaturateButton, MouseClick);
    QVERIFY(canvas->penForegroundColour().saturationF() < oldColour.saturationF());
}

// Test that two colours that only differ by saturation
// show the correct colour in the hex text field.
void tst_App::colourPickerSaturationHex()
{
    QVERIFY2(createNewImageProject(2, 1), failureMessage);

    const QColor colour1 = QColor("#fbf7ea");
    const QColor colour2 = QColor("#fffbee");

    // Draw with one colour.
    imageCanvas->setPenForegroundColour(colour1);
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageProject->image()->pixelColor(0, 0), colour1);

    // Now draw with a colour that only varies in saturation from the previous colour.
    imageCanvas->setPenForegroundColour(colour2);
    setCursorPosInScenePixels(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageProject->image()->pixelColor(1, 0), colour2);

    // Now select the first colour with the eyedropper, and ensure that the hex text is correct.
    QVERIFY2(switchTool(ImageCanvas::EyeDropperTool), failureMessage);
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageCanvas->penForegroundColour(), colour1);

    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    QVERIFY(hexTextField);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), colour1.name());
}

void tst_App::panes()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);
    QVERIFY(tileCanvas->firstPane());
    QCOMPARE(tileCanvas->firstPane()->size(), 0.5);
    QVERIFY(tileCanvas->secondPane());
    QCOMPARE(tileCanvas->secondPane()->size(), 0.5);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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
    QVERIFY2(triggerSplitScreen(), failureMessage);
    QVERIFY(!canvas->isSplitScreen());
    QCOMPARE(tileCanvas->firstPane()->size(), 1.0);
    QCOMPARE(tileCanvas->secondPane()->size(), 0.0);

    // Add it back again.
    QVERIFY2(triggerSplitScreen(), failureMessage);
    QVERIFY(canvas->isSplitScreen());
    QCOMPARE(tileCanvas->firstPane()->size(), 0.5);
    QCOMPARE(tileCanvas->secondPane()->size(), 0.5);
}

void tst_App::altEyedropper()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);

    QTest::keyPress(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyRelease(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);

    QTest::keyPress(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::EyeDropperTool);

    QTest::keyRelease(window, Qt::Key_Alt);
    QCOMPARE(tileCanvas->tool(), TileCanvas::PenTool);
}

void tst_App::eyedropper()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

    setCursorPosInTiles(1, 1);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    Tile *originalTile = tilesetProject->tileAt(cursorPos);
    QVERIFY(originalTile);
    const QPoint pixelPos = QPoint(tilesetProject->tileWidth() / 2, tilesetProject->tileHeight() / 2);
    QVERIFY(tileCanvas->penForegroundColour() != originalTile->pixelColor(pixelPos));

    QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);
    QVERIFY2(switchTool(TileCanvas::EyeDropperTool), failureMessage);

    QColor lastForegroundColour = tileCanvas->penForegroundColour();
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tileCanvas->penForegroundColour() != lastForegroundColour);
    // TODO: no idea why this doesn't work.. the positions are both {12, 12}.
//    QCOMPARE(canvas->penForegroundColour(), originalTile->pixelColor(pixelPos));

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Test panning.
    QVERIFY2(panBy(50, 0), failureMessage);

    // Test zoom.
    QVERIFY2(zoomTo(2, tileSceneCentre(5, 5)), failureMessage);
}

void tst_App::zoomAndCentre()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    // Pan to some non-centered location.
    QVERIFY2(panTopLeftTo(-100, -100), failureMessage);

    const CanvasPane *currentPane = tileCanvas->currentPane();
    QCOMPARE(currentPane, tileCanvas->firstPane());

    // Zoom in.
    QVERIFY2(zoomTo(5, tileSceneCentre(5, 5)), failureMessage);

    QVERIFY2(triggerCentre(), failureMessage);
    const QPoint expectedOffset(
        currentPane->size() * tileCanvas->width() / 2 - (tilesetProject->widthInPixels() * currentPane->integerZoomLevel()) / 2,
        tileCanvas->height() / 2 - (tilesetProject->heightInPixels() * currentPane->integerZoomLevel()) / 2);
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

    QVERIFY2(createNewProject(projectType), failureMessage);
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);
    QVERIFY2(panBy(xDistance, yDistance), failureMessage);

    if (zoomLevel > 1) {
        for (int i = 0; i < zoomLevel - canvas->currentPane()->integerZoomLevel(); ++i) {
            wheelEvent(canvas, tileSceneCentre(5, 5), 1);
        }
        QCOMPARE(canvas->currentPane()->integerZoomLevel(), zoomLevel);
    } else if (zoomLevel < 1) {
        for (int i = 0; i < qAbs(zoomLevel - canvas->currentPane()->integerZoomLevel()); ++i) {
            wheelEvent(canvas, tileSceneCentre(5, 5), -1);
        }
        QCOMPARE(canvas->currentPane()->integerZoomLevel(), zoomLevel);
    }

    if (projectType == Project::TilesetType) {
        QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

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
    QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);
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
//    QVERIFY2(createNewTilesetProject(), failureMessage);

//    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAt(0, 0));

//    // Test clicking outside of the menu to cancel it.

//    const Tile *originallySelectedTile = tileCanvas->penTile();
//    // Open the context menu.
//    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
//    QObject *tilesetContextMenu = window->findChild<QObject*>("tilesetContextMenu");
//    QVERIFY(tilesetContextMenu);
//    QVERIFY(tilesetContextMenu->property("visible").toBool());

//    const QPoint outsidePos = tilesetTileSceneCentre(tilesetProject->tileset()->tilesWide() - 1, 0);
//    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, outsidePos);
//    QVERIFY(!tilesetContextMenu->property("visible").toBool());
//    // The selected tile shouldn't have changed.
//    QCOMPARE(tileCanvas->penTile(), originallySelectedTile);

//    // Test duplicating a tile.

//    // Open the context menu.
//    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
//    QVERIFY(tilesetContextMenu->property("visible").toBool());

//    // Click the "duplicate" menu item.
//    mouseEventOnCentre(duplicateTileMenuItem, MouseClick);
//    QCOMPARE(tileCanvas->penTile(), originallySelectedTile);
//    QVERIFY(!tilesetContextMenu->property("visible").toBool());

//    // Duplicate the tile.
//    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, tilesetTileSceneCentre(1, 1));
//    // It should be the tile we right clicked on.
//    QCOMPARE(tilesetProject->tilesetTileAt(0, 0), originallySelectedTile);
//    // The selected tile shouldn't have changed.
//    QCOMPARE(tileCanvas->penTile(), originallySelectedTile);

//    // Test rotating a tile left.

//    // Draw the tile that we're rotating onto the canvas and then
//    // take a snapshot of the canvas to make sure that it's actually updated.
//    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);
//    setCursorPosInTiles(0, 0);
//    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
//    QVERIFY(tilesetProject->tileAt(cursorPos));
//    QVERIFY(imageGrabber.requestImage(tileCanvas));
//    QTRY_VERIFY(imageGrabber.isReady());
//    QImage lastCanvasSnapshot = imageGrabber.takeImage();

//    // Open the context menu.
//    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
//    QVERIFY(tilesetContextMenu->property("visible").toBool());

//    // Click the "rotate left" menu item.
//    const QImage originalTileImage = tileCanvas->penTile()->image();
//    mouseEventOnCentre(rotateTileLeftMenuItem, MouseClick);
//    QVERIFY(!tilesetContextMenu->property("visible").toBool());
//    QCOMPARE(Utils::rotate(tileCanvas->penTile()->image(), 90), originalTileImage);

//    QVERIFY(imageGrabber.requestImage(tileCanvas));
//    QTRY_VERIFY(imageGrabber.isReady());
//    QImage currentImage = imageGrabber.takeImage();
//    QVERIFY(currentImage != lastCanvasSnapshot);
//    lastCanvasSnapshot = currentImage;

//    // Test rotating a tile right.

//    // Open the context menu.
//    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, tilesetTileSceneCentre(0, 0));
//    QVERIFY(tilesetContextMenu->property("visible").toBool());

//    // Click the "rotate right" menu item.
//    mouseEventOnCentre(rotateTileRightMenuItem, MouseClick);
//    QVERIFY(!tilesetContextMenu->property("visible").toBool());
//    QCOMPARE(tileCanvas->penTile()->image(), originalTileImage);

//    QVERIFY(imageGrabber.requestImage(tileCanvas));
//    QTRY_VERIFY(imageGrabber.isReady());
//    currentImage = imageGrabber.takeImage();
//    QVERIFY(currentImage != lastCanvasSnapshot);
}

void tst_App::tilesetSwatchNavigation()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

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
    QVERIFY2(createNewImageProject(), failureMessage);

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

void tst_App::colourPickerHexFieldTranslucent()
{
    QVERIFY2(setPenForegroundColour("#88ff0000"), failureMessage);
}

void tst_App::eraseImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::eraseImageCanvas()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::EraserTool), failureMessage);
    QVERIFY2(changeToolSize(1), failureMessage);

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
    QVERIFY2(createNewImageProject(), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

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
        QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
        QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));
        QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);
    }
}

void tst_App::cancelSelectionToolImageCanvas()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    // Select an area.
    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(10, 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 10, 10));

    // Switching tools should clear the selection.
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));

    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);
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
        QVERIFY2(createNewImageProject(256, 256, transparentBackground), failureMessage);
    else if (projectType == Project::LayeredImageType)
        QVERIFY2(createNewLayeredImageProject(256, 256, transparentBackground), failureMessage);
    else
        QFAIL("Test doesn't support this project type");

    if (transparentBackground) {
        QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::transparent));
        QCOMPARE(canvas->currentProjectImage()->pixelColor(255, 255), QColor(Qt::transparent));
    }

    const QColor backgroundColour = transparentBackground ? Qt::transparent : Qt::white;

    // Draw a square of black pixels.
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    QVERIFY2(changeToolSize(5), failureMessage);
    setCursorPosInScenePixels(2, 2);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));

    QVERIFY2(changeToolSize(1), failureMessage);
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

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
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::transparent));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(18, 18), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(22, 22), QColor(Qt::black));

    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    // Undo the selection move.
    QVERIFY(canvas->hasActiveFocus());
    QVERIFY2(triggerShortcut("undoShortcut", app.settings()->undoShortcut()), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
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
    QVERIFY2(triggerShortcut("undoShortcut", app.settings()->undoShortcut()), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
}

void tst_App::moveSelectionWithKeysImageCanvas()
{
    QVERIFY2(createNewImageProject(256, 256), failureMessage);

    // Draw a square of black pixels.
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    QVERIFY2(changeToolSize(5), failureMessage);
    setCursorPosInScenePixels(2, 2);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(imageProject->image()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(imageProject->image()->pixelColor(4, 4), QColor(Qt::black));

    QVERIFY2(changeToolSize(1), failureMessage);
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

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

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

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

    QVERIFY2(createNewProject(projectType), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Draw a square of black pixels.
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    QVERIFY2(changeToolSize(5), failureMessage);
    setCursorPosInScenePixels(12, 12);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(10, 10), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(14, 14), QColor(Qt::black));

    QVERIFY2(changeToolSize(1), failureMessage);
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    // Select an area.
    setCursorPosInScenePixels(QPoint(10, 10));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(15, 15));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(10, 10, 5, 5));

    // Copy it.
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

void tst_App::undoCopyPasteWithTransparency()
{
    QVERIFY2(createNewLayeredImageProject(256, 256, true), failureMessage);

    // Draw something low enough that pasting something won't overlap it.
    setCursorPosInScenePixels(1, 10);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 10), QColor(Qt::black));

    // Select it.
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    setCursorPosInScenePixels(QPoint(0, 9));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(QPoint(3, 12));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    const QRect copyRect(QRect(0, 9, 3, 3));
    QCOMPARE(canvas->selectionArea(), copyRect);

    // Copy it.
    keySequence(window, QKeySequence::Copy);
    QCOMPARE(QGuiApplication::clipboard()->image(), canvas->currentProjectImage()->copy(copyRect));

    // Deselect so that we paste at the top left.
    setCursorPosInScenePixels(100, 100);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(!canvas->hasSelection());

    // Paste.
    keySequence(window, QKeySequence::Paste);
    QCOMPARE(canvas->hasSelection(), true);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 3, 3));

    // Deselect.
    setCursorPosInScenePixels(100, 100);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(!canvas->hasSelection());
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 10), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));

    // Undo the paste. It should disappear.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 10), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::transparent));
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

    QVERIFY2(createNewProject(projectType), failureMessage);

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
    QVERIFY2(createNewImageProject(), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    QImage image(32, 32, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::blue);

    QPainter painter(&image);
    painter.fillRect(0, 0, image.width() / 2, 10, QColor(Qt::red));
    qGuiApp->clipboard()->setImage(image);

    QVERIFY2(triggerPaste(), failureMessage);
    QCOMPARE(canvas->tool(), ImageCanvas::SelectionTool);
    QCOMPARE(canvas->hasSelection(), true);

    QVERIFY2(triggerFlipVertically(), failureMessage);
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage snapshotFlippedVertically = imageGrabber.takeImage();
    QCOMPARE(snapshotFlippedVertically.pixelColor(image.width() * 0.25, image.height() - 5), QColor(Qt::red));

    QVERIFY2(triggerFlipHorizontally(), failureMessage);
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage snapshotFlippedHorizontally = imageGrabber.takeImage();
    QCOMPARE(snapshotFlippedHorizontally.pixelColor(image.width() * 0.75, image.height() - 5), QColor(Qt::red));
}

void tst_App::selectionEdgePan_data()
{
    QTest::addColumn<QPoint>("selectionStartPos");
    QTest::addColumn<QPoint>("selectionEndPos");
    // The direction the panning is happening in. 0 if not going past the edge.
    QTest::addColumn<QPoint>("panDirection");
    QTest::addColumn<QRect>("expectedSelectionArea");

    const QPoint startPos(100, 100);
    const int largeDistance = qMax(window->width(), window->height());

    // "reverse" selections end up with slightly different coordinates.
    QTest::newRow("top-left") << startPos << QPoint(-largeDistance, -largeDistance) << QPoint(-1, -1) << QRect(0, 0, 101, 101);
    QTest::newRow("top") << startPos << QPoint(0, -largeDistance) << QPoint(0, -1) << QRect(0, 0, 101, 101);
    QTest::newRow("top-right") << startPos << QPoint(largeDistance, -largeDistance) << QPoint(1, -1) << QRect(100, 0, 256 - 100, 101);
    QTest::newRow("right") << startPos << QPoint(largeDistance, 0) << QPoint(1, 0) << QRect(100, 0, 256 - 100, 101);
    QTest::newRow("bottom-right") << startPos << QPoint(largeDistance, largeDistance) << QPoint(1, 1) << QRect(100, 100, 256 - 100, 256 - 100);
    QTest::newRow("bottom") << startPos << QPoint(256, largeDistance) << QPoint(0, 1) << QRect(100, 100, 256 - 100, 256 - 100);
    QTest::newRow("bottom-left") << startPos << QPoint(-largeDistance, largeDistance) << QPoint(-1, 1) << QRect(0, 100, 101, 256 - 100);
    QTest::newRow("left") << startPos << QPoint(-largeDistance, 0) << QPoint(-1, 0) << QRect(0, 0, 101, 101);
}

void tst_App::selectionEdgePan()
{
    QFETCH(QPoint, selectionStartPos);
    QFETCH(QPoint, selectionEndPos);
    QFETCH(QPoint, panDirection);
    QFETCH(QRect, expectedSelectionArea);

    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    const QPoint originalOffset = canvas->currentPane()->offset();

    // Test that the canvas is panned when the mouse goes past the edge when creating a selection.
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    setCursorPosInScenePixels(selectionStartPos.x(), selectionStartPos.y(), false);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    // Add an extra move otherwise the pan isn't started for some reason.
    QTest::mouseMove(window, cursorWindowPos + QPoint(1, 1));

    setCursorPosInScenePixels(selectionEndPos.x(), selectionEndPos.y(), false);
    // The current pane shouldn't change at all during this test.
    QCOMPARE(canvas->currentPane(), canvas->firstPane());
    // Uncomment when https://bugreports.qt.io/browse/QTBUG-67702 is fixed
//    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*Mouse event at .* occurs outside of target window.*"));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());
    QVERIFY(canvas->hasSelection());
    QCOMPARE(canvas->selectionArea(), expectedSelectionArea);

    if (panDirection.x() != 0) {
        // We don't want try to know how far it will pan, as we don't
        // want to rely on implementation details. So, choose a sufficiently
        // high number as the maximum amount of iterations.
        QPoint movedOffset = originalOffset - panDirection;
        for (int i = 0; i < 10000 && movedOffset != canvas->currentPane()->offset(); ++i) {
            movedOffset -= panDirection;
        }
        QCOMPARE(canvas->currentPane()->offset().x(), movedOffset.x());
    } else {
        QCOMPARE(canvas->currentPane()->offset().x(), originalOffset.x());
    }

    if (panDirection.y() != 0) {
        QPoint movedOffset = originalOffset - panDirection;
        for (int i = 0; i < 10000 && movedOffset != canvas->currentPane()->offset(); ++i) {
            movedOffset -= panDirection;
        }
        QCOMPARE(canvas->currentPane()->offset().y(), movedOffset.y());
    } else {
        QCOMPARE(canvas->currentPane()->offset().y(), originalOffset.y());
    }

    // Uncomment when https://bugreports.qt.io/browse/QTBUG-67702 is fixed
//    QTest::ignoreMessage(QtWarningMsg, QRegularExpression(".*Mouse event at .* occurs outside of target window.*"));
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
}

// https://github.com/mitchcurtis/slate/issues/50
// When panning while having an active selection, and then releasing the space bar
// and hence starting a drag, there would be an assertion failure (Q_ASSERT(mPotentiallySelecting))
// in ImageCanvas::updateSelectionArea().
void tst_App::panThenMoveSelection()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    setCursorPosInScenePixels(100, 100);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    // Add an extra move otherwise the pan isn't started for some reason.
    QTest::mouseMove(window, cursorWindowPos + QPoint(1, 1));

    setCursorPosInScenePixels(120, 120);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(layeredImageCanvas->hasSelection());

    // Start panning.
    QTest::keyPress(window, Qt::Key_Space);
    QCOMPARE(window->cursor().shape(), Qt::OpenHandCursor);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::ClosedHandCursor);

    // Stop panning but don't release the mouse.
    QTest::keyRelease(window, Qt::Key_Space);

    // Move the mouse slightly and it would previously crash with an assertion failure.
    QTest::mouseMove(window, cursorWindowPos + QPoint(1, 1));

    // Release to finish up.
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
}

void tst_App::fillImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::fillImageCanvas()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    // A fill on a canvas of this size would previously trigger a stack overflow
    // using a recursive algorithm.
    QVERIFY2(changeCanvasSize(90, 90), failureMessage);

    // Fill the canvas with black.
    QVERIFY2(switchTool(ImageCanvas::FillTool), failureMessage);
    setCursorPosInScenePixels(0, 0);
    mouseEvent(canvas, cursorWindowPos, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(project->widthInPixels() - 1,
                                                       project->heightInPixels() - 1), QColor(Qt::black));
}

void tst_App::fillLayeredImageCanvas()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QVERIFY(layeredImageProject->currentLayerIndex() == 1);
    ImageLayer *layer1 = layeredImageProject->layerAt(1);
    ImageLayer *layer2 = layeredImageProject->layerAt(0);

    // Switch to the fill tool.
    QVERIFY2(switchTool(ImageCanvas::FillTool), failureMessage);

    // Fill layer 1.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer1->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layer1->image()->pixelColor(255, 255), QColor(Qt::red));

    // Select the new layer (make it current).
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Undo. It should affect layer 1.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layer1->image()->pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(layer1->image()->pixelColor(255, 255), QColor(Qt::white));
    QCOMPARE(layer2->image()->pixelColor(0, 0), QColor(Qt::transparent));
}

void tst_App::greedyPixelFillImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::greedyPixelFillImageCanvas()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(changeCanvasSize(40, 40), failureMessage);

    // Draw 4 separate pixels.
    setCursorPosInScenePixels(4, 4);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    setCursorPosInScenePixels(35, 4);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    setCursorPosInScenePixels(35, 35);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    setCursorPosInScenePixels(4, 35);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::FillTool), failureMessage);
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

void tst_App::texturedFill_data()
{
    addImageProjectTypes();
}

void tst_App::texturedFill()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(changeCanvasSize(90, 90), failureMessage);

    // TODO: switch tools via the popup menu
//    QVERIFY2(switchTool(ImageCanvas::TexturedFillTool), failureMessage);
    canvas->setTool(ImageCanvas::TexturedFillTool);
    QCOMPARE(canvas->lastFillToolUsed(), ImageCanvas::TexturedFillTool);

    QVERIFY2(setPenForegroundColour("#123456"), failureMessage);

    // Open the settings dialog.
    QObject *settingsDialog = window->findChild<QObject*>("texturedFillSettingsDialog");
    QVERIFY(settingsDialog);
    QVERIFY(QMetaObject::invokeMethod(settingsDialog, "open"));
    QVERIFY(settingsDialog->property("visible").toBool());

    QQuickItem *hueVarianceCheckBox = settingsDialog->findChild<QQuickItem*>("hueVarianceCheckBox");
    QVERIFY(hueVarianceCheckBox);
    QCOMPARE(hueVarianceCheckBox->property("checked").toBool(), false);

    QQuickItem *saturationVarianceCheckBox = settingsDialog->findChild<QQuickItem*>("saturationVarianceCheckBox");
    QVERIFY(saturationVarianceCheckBox);
    QCOMPARE(saturationVarianceCheckBox->property("checked").toBool(), false);

    QQuickItem *lightnessVarianceCheckBox = settingsDialog->findChild<QQuickItem*>("lightnessVarianceCheckBox");
    QVERIFY(lightnessVarianceCheckBox);
    QCOMPARE(lightnessVarianceCheckBox->property("checked").toBool(), true);

    // Change some settings.
    QVERIFY(hueVarianceCheckBox->setProperty("checked", QVariant(true)));
    QVERIFY(saturationVarianceCheckBox->setProperty("checked", QVariant(true)));
    QVERIFY(lightnessVarianceCheckBox->setProperty("checked", QVariant(false)));

    // Cancel the dialog..
    QQuickItem *texturedFillSettingsCancelButton = settingsDialog->findChild<QQuickItem*>("texturedFillSettingsCancelButton");
    mouseEventOnCentre(texturedFillSettingsCancelButton, MouseClick);
    QVERIFY(!settingsDialog->property("visible").toBool());

    // .. and then open it again.
    QVERIFY(QMetaObject::invokeMethod(settingsDialog, "open"));
    QVERIFY(settingsDialog->property("visible").toBool());

    // The original settings should be restored.
    QVERIFY(hueVarianceCheckBox->setProperty("checked", QVariant(false)));
    QVERIFY(saturationVarianceCheckBox->setProperty("checked", QVariant(false)));
    QVERIFY(lightnessVarianceCheckBox->setProperty("checked", QVariant(true)));

    // Open the settings dialog again.
    QVERIFY(settingsDialog->property("visible").toBool());
    QCOMPARE(hueVarianceCheckBox->property("checked").toBool(), false);
    QCOMPARE(saturationVarianceCheckBox->property("checked").toBool(), false);
    QCOMPARE(lightnessVarianceCheckBox->property("checked").toBool(), true);

    // Confirm the changes.
    QQuickItem *texturedFillSettingsDialogOkButton = settingsDialog->findChild<QQuickItem*>("texturedFillSettingsDialogOkButton");
    QVERIFY(texturedFillSettingsDialogOkButton);
    mouseEventOnCentre(texturedFillSettingsDialogOkButton, MouseClick);
    QVERIFY(!settingsDialog->property("visible").toBool());

    // Fill the canvas with the default settings.
    setCursorPosInScenePixels(0, 0);
    mouseEvent(canvas, cursorWindowPos, MouseClick);
    // Ensure that there is some variation to the colours.
    bool hasVariation = false;
    for (int y = 0; y < canvas->currentProjectImage()->height() && !hasVariation; ++y) {
        for (int x = 0; x < canvas->currentProjectImage()->width() && !hasVariation; ++x) {
            const QColor colour = canvas->currentProjectImage()->pixelColor(x, y);
            // (the background is white by default)
            hasVariation = colour != QColor(Qt::black) && colour != QColor(Qt::white);
        }
    }
    QVERIFY(hasVariation);
}

void tst_App::pixelLineToolImageCanvas_data()
{
    addImageProjectTypes();
}

void tst_App::pixelLineToolImageCanvas()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);

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

void tst_App::pixelLineToolTransparent_data()
{
    addImageProjectTypes();
}

void tst_App::pixelLineToolTransparent()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);

    const QColor translucentRed = QColor::fromRgba(0x88ff0000);
    QVERIFY2(setPenForegroundColour("#88ff0000"), failureMessage);

    // Draw the start of the line.
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), translucentRed);
    QCOMPARE(project->hasUnsavedChanges(), true);

    // Draw the line itself.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::keyPress(window, Qt::Key_Shift);
    // For some reason there must be a delay in order for the shift modifier to work.
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, 100);
    QTest::keyRelease(window, Qt::Key_Shift);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), translucentRed);

    // Undo the line.
    mouseEventOnCentre(undoButton, MouseClick);
    // The initial press has to still be there.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::white));

    // Redo the line.
    mouseEventOnCentre(redoButton, MouseClick);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), translucentRed);\
}

void tst_App::rulersAndGuides_data()
{
    addAllProjectTypes();
}

void tst_App::rulersAndGuides()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(triggerRulersVisible(), failureMessage);
    QCOMPARE(app.settings()->areRulersVisible(), true);

    QQuickItem *firstHorizontalRuler = canvas->findChild<QQuickItem*>("firstHorizontalRuler");
    QVERIFY(firstHorizontalRuler);
    const qreal rulerThickness = firstHorizontalRuler->height();

    // Pan so that the top left of the canvas is at the rulers' corners.
    QVERIFY2(panTopLeftTo(rulerThickness, rulerThickness), failureMessage);

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

void tst_App::recentFiles()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Should be no recent files until the new project is saved.
    QObject *recentFilesInstantiator = window->findChild<QObject*>("recentFilesInstantiator");
    QVERIFY(recentFilesInstantiator);
    QCOMPARE(recentFilesInstantiator->property("count").toInt(), 0);

    // Save.
    project->saveAs(QUrl::fromLocalFile(tempProjectDir->path() + "/recentFiles.png"));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(recentFilesInstantiator->property("count").toInt(), 1);

    // Get the recent file menu item from the instantiator and ensure its text is correct.
    {
        QObject *recentFileMenuItem = nullptr;
        QVERIFY(QMetaObject::invokeMethod(recentFilesInstantiator, "objectAt", Qt::DirectConnection,
            Q_RETURN_ARG(QObject*, recentFileMenuItem), Q_ARG(int, 0)));
        QVERIFY(recentFileMenuItem);
        QCOMPARE(recentFileMenuItem->property("text").toString(), project->url().path());
    }

    // Can't click platform types from tests, so clear the recent items manually.
    app.settings()->clearRecentFiles();

    QCOMPARE(recentFilesInstantiator->property("count").toInt(), 0);
    {
        QObject *recentFileMenuItem = nullptr;
        QVERIFY(QMetaObject::invokeMethod(recentFilesInstantiator, "objectAt", Qt::DirectConnection,
            Q_RETURN_ARG(QObject*, recentFileMenuItem), Q_ARG(int, 0)));
        QVERIFY(!recentFileMenuItem);
    }
}

void tst_App::addAndRemoveLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    ImageLayer *expectedCurrentLayer = layeredImageProject->currentLayer();

    // Draw a blue square at {10, 10}.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), QColor(Qt::blue));

    // Ensure that the blue square is visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBlueDot = imageGrabber.takeImage();
    QCOMPARE(grabWithBlueDot.pixelColor(10, 10), QColor(Qt::blue));

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
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot on the new layer.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(20, 20), QColor(Qt::red));

    // Ensure that both dots are visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBothDots = imageGrabber.takeImage();
    QCOMPARE(grabWithBothDots.pixelColor(10, 10), QColor(Qt::blue));
    QCOMPARE(grabWithBothDots.pixelColor(20, 20), QColor(Qt::red));

    {
        // Ensure that what the user sees is correct.
        QQuickItem *layerListView = window->findChild<QQuickItem*>("layerListView");
        QVERIFY(layerListView);
        QCOMPARE(layerListView->property("count").toInt(), 2);

        QQuickItem *layer1Delegate = nullptr;
        QVERIFY2(verifyLayerName(QLatin1String("Layer 1"), &layer1Delegate), failureMessage);
        QQuickItem *layer2Delegate = nullptr;
        QVERIFY2(verifyLayerName(QLatin1String("Layer 2"), &layer2Delegate), failureMessage);
        // The second layer was added last, so it should be at the top of the list.
        QVERIFY(layer1Delegate->y() > layer2Delegate->z());
    }

    // Select the original layer.
    QVERIFY2(selectLayer("Layer 1", 1), failureMessage);

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
    QVERIFY(grabWithRedDot.pixelColor(10, 10) != QColor(Qt::blue));
    QCOMPARE(grabWithRedDot.pixelColor(20, 20), QColor(Qt::red));

    // Undo the deletion.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QVERIFY2(verifyLayerName(QLatin1String("Layer 1")), failureMessage);

    // Both dots should be visible again.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), grabWithBothDots);
}

void tst_App::layerVisibility()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Draw a blue square at {10, 10}.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), QColor(Qt::blue));

    // Ensure that the blue square is visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBlueDot = imageGrabber.takeImage();
    QCOMPARE(grabWithBlueDot.pixelColor(10, 10), QColor(Qt::blue));

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot at the same position.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), QColor(Qt::red));

    // Ensure that it's visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithRedDot = imageGrabber.takeImage();
    QCOMPARE(grabWithRedDot.pixelColor(10, 10), QColor(Qt::red));

    // Hide the current layer.
    QQuickItem *layer2Delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 2", &layer2Delegate), failureMessage);
    QQuickItem *layer2VisibilityCheckBox = layer2Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer2VisibilityCheckBox);
    mouseEventOnCentre(layer2VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);

    // Ensure that the layer has been hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithRedDotHidden = imageGrabber.takeImage();
    QCOMPARE(grabWithRedDotHidden.pixelColor(10, 10), QColor(Qt::blue));

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
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

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
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

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
    QCOMPARE(moveLayerDownButton->isEnabled(), true);
    QCOMPARE(moveLayerUpButton->isEnabled(), false);

    // Move the current layer down.
    mouseEventOnCentre(moveLayerDownButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));
    QCOMPARE(moveLayerDownButton->isEnabled(), true);
    QCOMPARE(moveLayerUpButton->isEnabled(), true);

    // TODO: draw a different-coloured pixel on each layer and do screen grab comparisons
}

void tst_App::mergeLayerUpAndDown()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QObject *mergeLayerDownMenuItem = window->findChild<QObject*>("mergeLayerDownMenuItem");
    QVERIFY(mergeLayerDownMenuItem);

    QObject *mergeLayerUpMenuItem = window->findChild<QObject*>("mergeLayerUpMenuItem");
    QVERIFY(mergeLayerUpMenuItem);

    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), false);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);

    // Draw something on Layer 1.
    setCursorPosInScenePixels(0, 0);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::red));

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    // It should be possible to merge the lowest layer up but not down.
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), false);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);

    // Make the new layer the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw something on Layer 2.
    setCursorPosInScenePixels(1, 0);
    layeredImageCanvas->setPenForegroundColour(Qt::green);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(1, 0), QColor(Qt::green));

    // It should be possible to merge the highest layer down but not up.
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 3);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    // It should be possible to merge the middle layer both up and down.
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);

    // Make the new layer the current layer.
    QVERIFY2(selectLayer("Layer 3", 0), failureMessage);

    // Draw something on Layer 3.
    setCursorPosInScenePixels(2, 0);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Make Layer 2 the current layer.
    QVERIFY2(selectLayer("Layer 2", 1), failureMessage);

    // Merge the current layer down. Don't have a shortcut for merging, and we don't really need one.
    layeredImageProject->mergeCurrentLayerDown();
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    // Photoshop uses the lower layer's name, so we'll do that too.
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), false);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(1, 0), QColor(Qt::green));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Merge the current layer up. That leaves us with one layer containing everything.
    layeredImageProject->mergeCurrentLayerUp();
    QCOMPARE(layeredImageProject->layerCount(), 1);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    // Photoshop uses the lower layer's name, so we'll do that too.
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 3"));
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), false);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(1, 0), QColor(Qt::green));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Undo the last merge so that we're back at having two layers.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 3"));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(1, 0), QColor(Qt::green));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Undo the first merge so that we have all three layers again.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 3);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);
    QCOMPARE(layeredImageProject->layerAt(2)->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 3"));
    QCOMPARE(layeredImageProject->layerAt(2)->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(1, 0), QColor(Qt::green));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(2, 0), QColor(Qt::blue));
}

void tst_App::renameLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QQuickItem *delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 1", &delegate), failureMessage);
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
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Draw a blue square.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), QColor(Qt::blue));

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(20, 20), QColor(Qt::red));

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabBeforeSaving = imageGrabber.takeImage();
    QCOMPARE(grabBeforeSaving.pixelColor(10, 10), QColor(Qt::blue));
    QCOMPARE(grabBeforeSaving.pixelColor(20, 20), QColor(Qt::red));

    // Select a layer with a non-zero index so that we can check that it's saved.
    QVERIFY2(selectLayer("Layer 1", 1), failureMessage);

    // Save.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + "/layeredimageproject.slp");
    layeredImageProject->saveAs(saveUrl);
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!layeredImageProject->hasUnsavedChanges());

    // Close.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!layeredImageProject->hasLoaded());
    QCOMPARE(layeredImageProject->layerCount(), 0);
    // The layer panel shouldn't show any layers.
    QQuickItem *layerListView = window->findChild<QQuickItem*>("layerListView");
    QVERIFY(layerListView);
    QCOMPARE(layerListView->property("count").toInt(), 0);

    // Load the saved file.
    layeredImageProject->load(saveUrl);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->currentLayer()->name(), "Layer 1");
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabAfterSaving = imageGrabber.takeImage();
    QCOMPARE(grabAfterSaving.pixelColor(10, 10), QColor(Qt::blue));
    QCOMPARE(grabAfterSaving.pixelColor(20, 20), QColor(Qt::red));
}

void tst_App::layerVisibilityAfterMoving()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(20, 20), QColor(Qt::red));

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabBeforeSaving = imageGrabber.takeImage();
    QCOMPARE(grabBeforeSaving.pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(grabBeforeSaving.pixelColor(20, 20), QColor(Qt::red));

    // Move it below Layer 1.
    mouseEventOnCentre(moveLayerDownButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));

    // Hide Layer 1. It should show the red square and a transparent background.
    QQuickItem *layer1Delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 1", &layer1Delegate), failureMessage);
    QQuickItem *layer1VisibilityCheckBox = layer1Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer1VisibilityCheckBox);
    mouseEventOnCentre(layer1VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->layerAt(0)->isVisible(), false);

    // Ensure that the layer has been hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Hidden = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Hidden.pixelColor(20, 20), QColor(Qt::red));

    // Show Layer 1. It should show only white.
    mouseEventOnCentre(layer1VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), true);
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Visible = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Visible.pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(grabWithLayer1Visible.pixelColor(20, 20), QColor(Qt::white));

    // Make Layer 1 the current layer.
    QVERIFY2(selectLayer("Layer 1", 0), failureMessage);

    // Move Layer 1 back down. The red square should be visible on a white background.
    mouseEventOnCentre(moveLayerDownButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Below = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Below.pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(grabWithLayer1Below.pixelColor(20, 20), QColor(Qt::red));
}

//void tst_App::undoAfterAddLayer()
//{
//    QVERIFY2(createNewLayeredImageProject(), failureMessage);

//    // Add a new layer.
//    mouseEventOnCentre(newLayerButton, MouseClick);
//    QCOMPARE(layeredImageProject->layerCount(), 2);
//    QCOMPARE(layeredImageProject->hasUnsavedChanges(), true);
//    QCOMPARE(undoButton->isEnabled(), true);

//    // TODO
//}

// https://github.com/mitchcurtis/slate/issues/6
void tst_App::selectionConfirmedWhenSwitchingLayers()
{
    // Copy an image onto the clipboard.
    QImage clipboardContents(100, 200, QImage::Format_ARGB32_Premultiplied);
    clipboardContents.fill(Qt::red);
    qGuiApp->clipboard()->setImage(clipboardContents);

    // Create a new layered image project with the dimensions of the clipboard contents.
    QVERIFY2(createNewLayeredImageProject(100, 200), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Create a new layer and make it the active layer
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->hasUnsavedChanges(), true);
    QCOMPARE(undoButton->isEnabled(), true);
    // Sanity check that the names are correct. Newly added layers go to the top,
    // and hence have a lower index.
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 2"));
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->layerAt(1)->isVisible(), true);

    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Paste the image into that layer
    QVERIFY2(triggerPaste(), failureMessage);
    QCOMPARE(canvas->hasSelection(), true);

    // Switching layers should cause the selection to be confirmed.
    QVERIFY2(selectLayer("Layer 1", 1), failureMessage);
    QCOMPARE(canvas->hasSelection(), false);

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage snapshotAfterSwitchingLayers = imageGrabber.takeImage();
    QCOMPARE(snapshotAfterSwitchingLayers.pixelColor(0, 0), QColor(Qt::red));
}

void tst_App::autoExport()
{
    // Create a new layered image project with the dimensions of the clipboard contents.
    QVERIFY2(createNewLayeredImageProject(10, 10), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    QCOMPARE(layeredImageProject->isAutoExportEnabled(), false);

    // Don't have a shortcut for it yet, so have to change it manually, but we can still
    // check that the menus update accordingly.
    QObject *autoExportMenuItem = window->findChild<QObject*>("autoExportMenuItem");
    QVERIFY(autoExportMenuItem);
    QCOMPARE(autoExportMenuItem->property("checked").toBool(), false);

    layeredImageProject->setAutoExportEnabled(true);
    QCOMPARE(autoExportMenuItem->property("checked").toBool(), true);

    QCOMPARE(layeredImageProject->canSave(), true);

    // Save the project so that the auto-export is triggered.
    const QString savedProjectPath = tempProjectDir->path() + "/autoExport-project.slp";
    layeredImageProject->saveAs(QUrl::fromLocalFile(savedProjectPath));

    // The image file should exist now.
    const QString autoExportFilePath = LayeredImageProject::autoExportFilePath(layeredImageProject->url());
    QVERIFY(QFile::exists(autoExportFilePath));

    QImage exportedImage(autoExportFilePath);
    QVERIFY(!exportedImage.isNull());

    QImage expectedExportedImage(10, 10, QImage::Format_ARGB32);
    expectedExportedImage.fill(Qt::white);
    QCOMPARE(exportedImage, expectedExportedImage);

    // Disable auto-export.
    layeredImageProject->setAutoExportEnabled(false);
    QCOMPARE(autoExportMenuItem->property("checked").toBool(), false);

    // Draw something.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(2, 2), QColor(Qt::black));

    // Save again.
    layeredImageProject->saveAs(QUrl::fromLocalFile(savedProjectPath));

    // No export should have happened and so the exported image shouldn't have changed.
    exportedImage = QImage(autoExportFilePath);
    QCOMPARE(exportedImage, expectedExportedImage);
}

void tst_App::exportFileNamedLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    layeredImageProject->setAutoExportEnabled(true);

    // Draw a red dot.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::red));

    // Add some new layers.
    mouseEventOnCentre(newLayerButton, MouseClick);
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 3);
    ImageLayer *layer1 = layeredImageProject->layerAt(2);
    ImageLayer *layer2 = layeredImageProject->layerAt(1);
    ImageLayer *layer3 = layeredImageProject->layerAt(0);

    // Select Layer 2.
    QVERIFY2(selectLayer("Layer 2", 1), failureMessage);

    // Draw a green dot on layer 2.
    layeredImageCanvas->setPenForegroundColour(Qt::green);
    setCursorPosInScenePixels(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer2->image()->pixelColor(1, 0), QColor(Qt::green));

    // Select Layer 3.
    QVERIFY2(selectLayer("Layer 3", 0), failureMessage);

    // Draw a blue dot on layer 3.
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    setCursorPosInScenePixels(2, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer3->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Give the layer a name so that it's saved as a PNG.
    QVERIFY2(makeCurrentAndRenameLayer("Layer 2", "[test] Layer 2"), failureMessage);

    // Save the project so that auto export is triggered and the images saved.
    const QString savedProjectPath = tempProjectDir->path() + "/exportFileNameLayers-project.slp";
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();

    // Check that the "main" image (combined from regular layers) is saved.
    const QString exportedImagePath = tempProjectDir->path() + "/exportFileNameLayers-project.png";
    QVERIFY(QFile::exists(exportedImagePath));
    QImage exportedImage(exportedImagePath);
    // TODO: formats are wrong when comparing whole images, but I don't know why.
    QCOMPARE(exportedImage.pixelColor(0, 0), layer1->image()->pixelColor(0, 0));
    QCOMPARE(exportedImage.pixelColor(2, 0), layer3->image()->pixelColor(2, 0));

    // Check that the one file-named layer was saved as a separate image.
    const QString exportedTestLayerImagePath = tempProjectDir->path() + "/test.png";
    QVERIFY(QFile::exists(exportedTestLayerImagePath));
    QImage exportedLayerImage(exportedTestLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(1, 0), layer2->image()->pixelColor(1, 0));

    // Remove the image so that we can re-test it being exported.
    QVERIFY(QFile::remove(exportedTestLayerImagePath));
    QVERIFY(!QFile::exists(exportedTestLayerImagePath));

    // Hide that file-named layer; it should still be exported.
    QVERIFY2(changeLayerVisiblity("[test] Layer 2", false), failureMessage);

    // Save to export.
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(QFile::exists(exportedTestLayerImagePath));
    exportedLayerImage = QImage(exportedTestLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(1, 0), layer2->image()->pixelColor(1, 0));

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 4);
    ImageLayer *layer4 = layeredImageProject->layerAt(0);

    // Rename it so it's in the same group as "test".
    QVERIFY2(makeCurrentAndRenameLayer("Layer 4", "[test] Layer 4"), failureMessage);

    // Draw a dot on it.
    layeredImageCanvas->setPenForegroundColour(Qt::darkMagenta);
    setCursorPosInScenePixels(3, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer4->image()->pixelColor(3, 0), QColor(Qt::darkMagenta));

    // Export again. Both layers should have been exported to the same image.
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(QFile::exists(exportedTestLayerImagePath));
    exportedLayerImage = QImage(exportedTestLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(1, 0), layer2->image()->pixelColor(1, 0));
    QCOMPARE(exportedLayerImage.pixelColor(3, 0), layer4->image()->pixelColor(3, 0));

    // Add another new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 5);
    ImageLayer *layer5 = layeredImageProject->layerAt(0);

    // Rename it so that it uses the project as a prefix.
    QVERIFY2(makeCurrentAndRenameLayer("Layer 5", "[%p-blah] Layer 5"), failureMessage);

    // Now we have the following layers (x = hidden):
    // - [%p-blah] Layer 5
    // - [test] Layer 4
    // - Layer 3
    // x [test] Layer 2
    // - Layer 1

    // Draw a dot on it as usual, so that we can verify it exports correctly.
    layeredImageCanvas->setPenForegroundColour(Qt::darkBlue);
    setCursorPosInScenePixels(4, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer5->image()->pixelColor(4, 0), QColor(Qt::darkBlue));

    // Export it.
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    const QString exportedBlahLayerImagePath = tempProjectDir->path() + "/exportFileNameLayers-project-blah.png";
    QVERIFY(QFile::exists(exportedBlahLayerImagePath));
    exportedLayerImage = QImage(exportedBlahLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(4, 0), layer5->image()->pixelColor(4, 0));

    // Add [no-export] to each layer without a prefix so that they're not exported.
    QVERIFY2(makeCurrentAndRenameLayer("Layer 1", "[no-export] Layer 1"), failureMessage);
    QVERIFY2(makeCurrentAndRenameLayer("Layer 3", "[no-export] Layer 3"), failureMessage);

    // Now we have the following layers:
    // - [%p-blah] Layer 5
    // - [test] Layer 4
    // - [no-export] Layer 3
    // x [test] Layer 2
    // - [no-export] Layer 1

    // Remove the image so that we can re-test it being exported.
    QVERIFY(QFile::remove(exportedImagePath));
    QVERIFY(!QFile::exists(exportedImagePath));

    // Now it shouldn't be exported.
    project->saveAs(QUrl::fromLocalFile(savedProjectPath));
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QVERIFY(!QFile::exists(exportedImagePath));
}

void tst_App::disableToolsWhenLayerHidden()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // The cursor should be normal.
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::BlankCursor);

    QQuickItem *layer1Delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 1", &layer1Delegate), failureMessage);
    QQuickItem *layer1VisibilityCheckBox = layer1Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer1VisibilityCheckBox);

    const QVector<ImageCanvas::Tool> tools {
        ImageCanvas::PenTool,
        ImageCanvas::EyeDropperTool,
        ImageCanvas::EraserTool,
        ImageCanvas::FillTool,
        ImageCanvas::SelectionTool/*,
        ImageCanvas::CropTool TODO: not implemented yet*/
    };

    foreach (ImageCanvas::Tool tool, tools) {
        // Hide the layer.
        mouseEventOnCentre(layer1VisibilityCheckBox, MouseClick);
        QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);
        // Qt::ForbiddenCursor shouldn't be displayed yet.
        // The cursor should be disabled for each tool.
        QVERIFY2(window->cursor().shape() == Qt::ArrowCursor,
            qPrintable(QString::fromLatin1("Expected Qt::ArrowCursor for tool %1, but got %2")
                .arg(Utils::enumToString(tool)).arg(Utils::enumToString(window->cursor().shape()))));

        // Switch tool.
        QVERIFY2(switchTool(tool), failureMessage);

        // Move onto the canvas. The cursor should be disabled for each tool.
        setCursorPosInScenePixels(0, 0);
        QTest::mouseMove(window, cursorWindowPos);
        // TODO: ForbiddenCursor
        QVERIFY2(window->cursor().shape() == Qt::ForbiddenCursor,
            qPrintable(QString::fromLatin1("Expected Qt::ForbiddenCursor for tool %1, but got %2")
                .arg(Utils::enumToString(tool)).arg(Utils::enumToString(window->cursor().shape()))));

        // Make the layer visible again.
        mouseEventOnCentre(layer1VisibilityCheckBox, MouseClick);
        QCOMPARE(layeredImageProject->currentLayer()->isVisible(), true);

        // The cursor should not be ForbiddenCursor now.
        QVERIFY(window->cursor().shape() != Qt::ForbiddenCursor);
    }

    // Hide the layer.
    mouseEventOnCentre(layer1VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);

    // Ensure that we can't actually do anything when the cursor is disabled.
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    setCursorPosInScenePixels(10, 10);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), QColor(Qt::white));
}

void tst_App::undoMoveContents()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Draw a red dot.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::red));

    // Move the contents down.
    QVERIFY2(moveContents(1, 3, false), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 2), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(1, 2), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(1, 3), QColor(Qt::red));

    // Undo.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::red));
}

void tst_App::undoMoveContentsOfVisibleLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Draw a red dot.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::red));

    // Add a new layer.
    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    ImageLayer *layer1 = layeredImageProject->layerAt(1);
    ImageLayer *layer2 = layeredImageProject->layerAt(0);

    // Select the new layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a blue dot on layer 2.
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    setCursorPosInScenePixels(1, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer2->image()->pixelColor(1, 0), QColor(Qt::blue));

    // Hide layer 2.
    QQuickItem *layer2Delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 2", &layer2Delegate), failureMessage);
    QQuickItem *layer2VisibilityCheckBox = layer2Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer2VisibilityCheckBox);
    mouseEventOnCentre(layer2VisibilityCheckBox, MouseClick);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);

    // Move the contents down. Only layer 1 should have been moved.
    QVERIFY2(moveContents(0, 1, true), failureMessage);
    QCOMPARE(layer1->image()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(layer1->image()->pixelColor(0, 1), QColor(Qt::red));
    QCOMPARE(layer2->image()->pixelColor(1, 0), QColor(Qt::blue));

    // Undo.
    mouseEventOnCentre(undoButton, MouseClick);
    QCOMPARE(layer1->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layer1->image()->pixelColor(0, 1), QColor(Qt::white));
    QCOMPARE(layer2->image()->pixelColor(1, 0), QColor(Qt::blue));
}

int main(int argc, char *argv[])
{
    tst_App test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "tst_app.moc"
