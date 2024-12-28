/*
    Copyright 2023, Mitch Curtis

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
#include <QLoggingCategory>
#include <QPainter>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QtTest>
#include <QQuickItemGrabResult>
#include <QQuickWindow>
#include <QtQuickTest>

// Need this otherwise we get linker errors.
extern "C" {
#include "bitmap/bmp.h"
#include "bitmap/misc/gif.h"
}

#include "application.h"
#include "applypixelpencommand.h"
#include "imagelayer.h"
#include "imageutils.h"
#include "tilecanvas.h"
#include "probabilityswatch.h"
#include "project.h"
#include "projectmanager.h"
#include "qtutils.h"
#include "swatch.h"
#include "testhelper.h"
#include "tileset.h"

Q_LOGGING_CATEGORY(lcModels, "tests.models")

class tst_App : public TestHelper
{
    Q_OBJECT

public:
    tst_App(int &argc, char **argv);

private Q_SLOTS:
    // Project management.
    void newProjectWithNewTileset();
    void repeatedNewProject_data();
    void repeatedNewProject();
    void openClose_data();
    void openClose();
    void saveTilesetProject();
    void saveAsAndLoadTilesetProject();
    void saveAsAndLoad_data();
    void saveAsAndLoad();
    void versionCheck_data();
    void versionCheck();
    void loadTilesetProjectWithInvalidTileset();
    void loadLayeredImageProjectAfterTilesetProject();
    void loadInvalidProjects_data();
    void loadInvalidProjects();
    void recentFiles();
    void newProjectSizeFromClipboard_data();
    void newProjectSizeFromClipboard();
    void splitViewStateAcrossProjects();
    void saveOnPrompt();
    void loadPaneFractionalOffset();

    // Tools, misc.
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
    void undoRearrangeContentsIntoGridChange_data();
    void undoRearrangeContentsIntoGridChange();
    void undoPixelFill();
    void undoTileFill();
    void undoThickSquarePen();
    void undoThickRoundPen();
    void penSubpixelPosition();
    void penSubpixelPositionWithThickBrush_data();
    void penSubpixelPositionWithThickBrush();
    void colours_data();
    void colours();
    void colourPickerSaturationHex();
    void panes();
    void altEyedropper();
    void eyedropper();
    void eyedropperBackgroundColour();
    void zoomAndPan();
    void zoomAndCentre();
    void penWhilePannedAndZoomed_data();
    void penWhilePannedAndZoomed();
    void centrePanes();
    void useTilesetSwatch();
    void tilesetSwatchContextMenu();
    void tilesetSwatchNavigation();
    void cursorShapeAfterClickingLighter();
    void colourPickerHexField();
    void colourPickerHexFieldTranslucent();
    void eraseImageCanvas_data();
    void eraseImageCanvas();
    void splitterSettingsMouse_data();
    void splitterSettingsMouse();
    void fillImageCanvas_data();
    void fillImageCanvas();
    void fillLayeredImageCanvas();
    void greedyPixelFillImageCanvas_data();
    void greedyPixelFillImageCanvas();
    void texturedFillVariance_data();
    void texturedFillVariance();
    void texturedFillSwatch_data();
    void texturedFillSwatch();
    void pixelLineToolImageCanvas_data();
    void pixelLineToolImageCanvas();
    void pixelLineToolTransparent_data();
    void pixelLineToolTransparent();
    void lineMiddleMouseButton();
    void penToolRightClickBehaviour_data();
    void penToolRightClickBehaviour();
    void splitScreenRendering();
    void formatNotModifiable();
    void models();

    // Rulers, guides, notes, etc.
    void rulersAndGuides_data();
    void rulersAndGuides();
    void rulersSplitScreen();
    void addAndDeleteMultipleGuides();
    void loadDuplicateGuides();
    void notes_data();
    void notes();
    void dragNoteWithoutMoving();
    void saveAndLoadNotes();

    // Swatches.
    void autoSwatch_data();
    void autoSwatch();
    void autoSwatchGridViewContentY();
    void autoSwatchPasteConfirmation();
    void swatches();
    void importSwatches_data();
    void importSwatches();

    // Selection-related stuff.
    void selectionToolImageCanvas();
    void selectionToolTileCanvas();
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
    void undoAfterMovedPaste();
    void undoPasteAcrossLayers_data();
    void undoPasteAcrossLayers();
    void flipPastedImage();
    void flipOnTransparentBackground();
    void panThenMoveSelection();
    void selectionCursorGuide();
    void rotateSelection_data();
    void rotateSelection();
    void rotateSelectionAtEdge_data();
    void rotateSelectionAtEdge();
    void rotateSelectionTransparentBackground_data();
    void rotateSelectionTransparentBackground();
    void hueSaturation_data();
    void hueSaturation();
    void opacityDialog_data();
    void opacityDialog();
    void cropToSelection_data();
    void cropToSelection();

    // Animation.
    void animationPlayback_data();
    void animationPlayback();
    void playNonLoopingAnimationTwice();
    void animationGifExport();
    void newAnimations_data();
    void newAnimations();
    void duplicateAnimations_data();
    void duplicateAnimations();
    void saveAnimations();
    void clickOnCurrentAnimation();
    void renameAnimation();
    void reverseAnimation();
    void animationFrameWidthTooLarge();
    void animationPreviewUpdated();
    void seekAnimation();
    void animationFrameMarkers();

    // Layers.
    void addAndRemoveLayers();
    void newLayerIndex();
    void layerVisibility();
    void moveLayerUpAndDown();
    void mergeLayerUpAndDown();
    void renameLayers();
    void duplicateLayers();
    void saveAndLoadLayeredImageProject();
    void layerVisibilityAfterMoving();
//    void undoAfterAddLayer();
    void selectionConfirmedWhenSwitchingLayers();
    void newLayerAfterMovingSelection();
    void undoAfterMovingTwoSelections();
    void autoExport();
    void exportFileNamedLayers();
    void disableToolsWhenLayerHidden();
    void undoMoveContents();
    void undoMoveContentsOfVisibleLayers();
    void selectNextLayer();
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
    QImage expectedTilesetImage(ImageUtils::filledImage(expectedWidth, expectedHeight, Qt::white));
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
    QVERIFY2(compareImages(*tilesetProject->tileset()->image(), expectedTilesetImage), failureMessage);

    // Save the project.
    const QUrl saveFileName = QUrl::fromLocalFile(tempProjectDir->path() + "/mytileset.stp");
    QVERIFY(tilesetProject->saveAs(saveFileName));
    // Should save the image at the same location as the project.
    const QString tilesetPath = tempProjectDir->path() + "/mytileset.png";
    QCOMPARE(tilesetProject->tilesetUrl(), QUrl::fromLocalFile(tilesetPath));
    QVERIFY(QFile::exists(tilesetPath));
    QVERIFY2(compareImages(*tilesetProject->tileset()->image(), expectedTilesetImage), failureMessage);
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

        const QUrl badUrl("doesnotexist.stp");
        const QRegularExpression errorMessage(QLatin1String("Failed to open tileset project's STP file at "));
        QVERIFY2(loadProject(badUrl, errorMessage), failureMessage);

        // There was a project open before we attempted to load the invalid one.
        QCOMPARE(project->hasLoaded(), true);
    }

    // Check that the cursor goes blank when the canvas has focus.
    QVERIFY2(canvas->hasActiveFocus(), activeFocusFailureMessage(canvas));
    // Move the mouse a bit to trigger a cursor update since mContainsMouse was reset.
    QTest::mouseMove(window, canvas->mapToScene(
        QPointF(canvas->width() / 2, canvas->height() / 2)).toPoint());
    QTRY_COMPARE(window->cursor().shape(), Qt::BlankCursor);

    // Test closing a valid project.
    project->close();
    QVERIFY_NO_CREATION_ERRORS_OCCURRED();
    QCOMPARE(project->url(), QUrl());
    QCOMPARE(project->hasLoaded(), false);

    // Some tool buttons need a project open...
    QCOMPARE(canvasSizeToolButton->property("enabled").toBool(), false);
    // ... while others don't.
    QCOMPARE(fullScreenToolButton->property("enabled").toBool(), true);

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
    QVERIFY(tilesetProject->saveAs(saveUrl));
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
    QSKIP("SplitView part fails; TODO: check if still failing with Qt 6");

    QVERIFY2(createNewTilesetProject(), failureMessage);

    // Save the untouched project.
    const QString originalProjectPath = tempProjectDir->path() + "/project.stp";
    QVERIFY(tilesetProject->saveAs(QUrl::fromLocalFile(originalProjectPath)));
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
    QVERIFY(tilesetProject->saveAs(QUrl::fromLocalFile(savedProjectPath)));
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
    QVERIFY(tilesetProject->save());
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

    QVERIFY2(loadProject(QUrl::fromLocalFile(savedProjectPath)), failureMessage);
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
    QSKIP("SplitView part fails; TODO: check if still failing with Qt 6");

    // Ensure that things that are common to all project types are saved,
    // like guides, pane offset and zoom, etc.

    QFETCH(Project::Type, projectType);
    QFETCH(QString, projectExtension);

    QVERIFY2(createNewProject(projectType), failureMessage);

    // TestHelper hides rulers for new projects.
    QCOMPARE(canvas->areRulersVisible(), false);
    QCOMPARE(canvas->areGuidesVisible(), true);
    QCOMPARE(canvas->areGuidesLocked(), false);

    // Add an opaque red to the swatch.
    canvas->setPenForegroundColour(Qt::red);
    QVERIFY2(addSwatchWithForegroundColour(), failureMessage);
    // Add a translucent red to the swatch.
    const QColor translucentRed = QColor::fromRgba(0xaaff0000);
    canvas->setPenForegroundColour(translucentRed);
    QVERIFY2(addSwatchWithForegroundColour(), failureMessage);
    // Having this visible interferes with the rest of the test, and since I'm too lazy
    // to check why and it's not necessary to have it open, just close it.
    QVERIFY(swatchesPanel->setProperty("expanded", QVariant(false)));

    // Temporarily show rulers so we can drag some guides out.
    if (!canvas->areRulersVisible()) {
        QVERIFY2(triggerRulersVisible(), failureMessage);
        QCOMPARE(canvas->areRulersVisible(), true);
    }

    QQuickItem *firstHorizontalRuler = findChildItem(canvas, "firstHorizontalRuler");
    QVERIFY(firstHorizontalRuler);
    const qreal rulerThickness = firstHorizontalRuler->height();

    // TODO: fix this failure so that we can test it properly
//    QVERIFY2(panTopLeftTo(rulerThickness, rulerThickness), failureMessage);
    canvas->firstPane()->setIntegerOffset(QPoint(rulerThickness, rulerThickness));

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

    // Test SplitView state serialisation.
    // First, resize the panel column.
    QPointer<QQuickItem> panelSplitView = window->findChild<QQuickItem*>("panelSplitView");
    QVERIFY(panelSplitView);
    const qreal defaultPanelSplitViewWidth = panelSplitView->width();
    QPointer<QQuickItem> mainSplitViewHandle = findSplitViewHandle("mainSplitView", 0);
    QVERIFY(mainSplitViewHandle);
    const QPoint mainSplitViewHandleCentreAfterMoving = QPoint(
        panelSplitView->width() / 2 - 10, panelSplitView->height() / 2);
    QPoint mainSplitViewHandleCentre;
    QVERIFY2(dragSplitViewHandle("mainSplitView", 0,
        mainSplitViewHandleCentreAfterMoving, &mainSplitViewHandleCentre), failureMessage);
    const qreal resizedPanelSplitViewWidth = panelSplitView->width();
    QVERIFY(resizedPanelSplitViewWidth > defaultPanelSplitViewWidth);

    // Set these to the opposite of the defaults to tet that they're serialised properly.
    QVERIFY2(triggerGuidesVisible(), failureMessage);
    QCOMPARE(canvas->areGuidesVisible(), false);
    QVERIFY2(triggerRulersVisible(), failureMessage);
    QCOMPARE(canvas->areRulersVisible(), false);
    QVERIFY2(clickButton(lockGuidesToolButton), failureMessage);
    QCOMPARE(canvas->areGuidesLocked(), true);

    // Store the expected pane offsets, etc.
    // Do it after resizing the splitview to avoid it affecting it.
    const QPoint firstPaneOffset = canvas->firstPane()->integerOffset();
    const int firstPaneZoomLevel = canvas->firstPane()->integerZoomLevel();
    const qreal firstPaneSize = canvas->firstPane()->size();
    const QPoint secondPaneOffset = canvas->secondPane()->integerOffset();
    const int secondPaneZoomLevel = canvas->secondPane()->integerZoomLevel();
    const qreal secondPaneSize = canvas->secondPane()->size();

    // Save the project.
    const QString savedProjectPath = tempProjectDir->path() + "/saveAsAndLoad-project." + projectExtension;
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));
    QCOMPARE(project->url().toLocalFile(), savedProjectPath);
    QCOMPARE(project->modificationVersion().toString(), qApp->applicationVersion());

    // Resize the SplitView back to its old proportions so that we
    // can check that the state is actually restored,
    // as the view won't be destroyed between saving and loading,
    // so it will keep its values if we don't do this.
    QVERIFY2(dragSplitViewHandle("mainSplitView", 0, mainSplitViewHandleCentre), failureMessage);
    // We were getting some failures where the actual was 240.333333333 where the expected was 240.
    // Such small differences don't matter.
    QCOMPARE(int(panelSplitView->width()), int(defaultPanelSplitViewWidth));

    // Close the project.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!project->hasLoaded());

    // Load the saved file.
    QVERIFY2(loadProject(QUrl::fromLocalFile(savedProjectPath)), failureMessage);

    QCOMPARE(project->modificationVersion(), QVersionNumber::fromString(qApp->applicationVersion()));

    // Test SplitView state serialisation.
    panelSplitView = window->findChild<QQuickItem*>("panelSplitView");
    QVERIFY(panelSplitView);
    QTRY_COMPARE_WITH_TIMEOUT(panelSplitView->width(), resizedPanelSplitViewWidth, 100);

    if (projectType == Project::LayeredImageType) {
        // Test that the save shortcut works by drawing and then saving.
        setCursorPosInScenePixels(0, 0);
        QVERIFY2(drawPixelAtCursorPos(), failureMessage);

        QVERIFY2(triggerSaveProject(), failureMessage);
        QVERIFY(!project->hasUnsavedChanges());
    }

    // Check guides and panes.
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);
    QCOMPARE(canvas->firstPane()->integerOffset(), firstPaneOffset);
    QCOMPARE(canvas->firstPane()->integerZoomLevel(), firstPaneZoomLevel);
    QCOMPARE(canvas->firstPane()->size(), firstPaneSize);
    QCOMPARE(canvas->secondPane()->integerOffset(), secondPaneOffset);
    QCOMPARE(canvas->secondPane()->integerZoomLevel(), secondPaneZoomLevel);
    QCOMPARE(canvas->secondPane()->size(), secondPaneSize);
    QCOMPARE(canvas->areGuidesVisible(), false);
    QCOMPARE(canvas->areGuidesLocked(), true);
    QCOMPARE(canvas->areRulersVisible(), false);

    QVector<SwatchColour> expectedSwatchColours;
    expectedSwatchColours.append(SwatchColour(QString(), Qt::red));
    expectedSwatchColours.append(SwatchColour(QString(), translucentRed));
    if (project->swatch()->colours() != expectedSwatchColours) {
        QString message;
        QDebug stream(&message);
        stream << "\n    Actual: " << project->swatch()->colours()
               << "\n    Expected: " << expectedSwatchColours;
        QFAIL(qPrintable(message));
    }
}

enum ValidationCheck {
    NoExtraValidation = 0x0,
    HasVersion = 0x1,
    HasAnimation = 0x2
};

Q_DECLARE_FLAGS(ValidationChecks, ValidationCheck)
Q_DECLARE_METATYPE(ValidationCheck)
Q_DECLARE_METATYPE(ValidationChecks)

void tst_App::versionCheck_data()
{
    QTest::addColumn<QString>("projectFileName");
    QTest::addColumn<QVersionNumber>("version");
    QTest::addColumn<ValidationChecks>("validation");

    QTest::newRow("version-check-v0.2.1.slp") << QString::fromLatin1("version-check-v0.2.1.slp")
        << QVersionNumber(0, 2, 1) << ValidationChecks(NoExtraValidation);

    QTest::newRow("version-check-v0.8.0.slp") << QString::fromLatin1("version-check-v0.8.0.slp")
        << QVersionNumber(0, 8, 0) << ValidationChecks(NoExtraValidation);

    // 0.9 and newer should have a version number saved in the file.
    // 0.9 only had one animation. We should support it by converting it to the first animation in our list of animations.
    QTest::newRow("version-check-v0.9.0.slp") << QString::fromLatin1("version-check-v0.9.0.slp")
        << QVersionNumber(0, 9, 0) << ValidationChecks(HasVersion | HasAnimation);
}

// Tests that old project files (within reason) can still be loaded.
void tst_App::versionCheck()
{
    QFETCH(QString, projectFileName);
    QFETCH(QVersionNumber, version);
    QFETCH(ValidationChecks, validation);

    // Ensure that we have a temporary directory.
    if (projectManager->projectTypeForFileName(projectFileName) == Project::LayeredImageType)
        QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);
    else
        QVERIFY2(setupTempTilesetProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Try to load the project; there shouldn't be any errors.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    QVERIFY2(loadProject(QUrl::fromLocalFile(absolutePath)), failureMessage);

    if (validation.testFlag(HasVersion)) {
        QCOMPARE(project->creationVersion(), version);
        QCOMPARE(project->modificationVersion(), version);
    }

    if (validation.testFlag(HasAnimation)) {
        // Only LayeredImageProject supports saving animations.
        QVERIFY(layeredImageProject);
        QVERIFY(layeredImageProject->isUsingAnimation());
        const auto animationSystem = layeredImageProject->animationSystem();
        QCOMPARE(animationSystem->animationCount(), 1);
    }
}

void tst_App::loadTilesetProjectWithInvalidTileset()
{
    // Set up a temporary directory for the test.
    QVERIFY2(setupTempTilesetProjectDir(), failureMessage);

    // Copy the files we need to our temporary directory.
    const QString projectFileName = QLatin1String("invalid-tileset.stp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Load it. It shouldn't crash.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    const QRegularExpression expectedFailureMessage(QLatin1String("Failed to open project's tileset at /nope/nope/nope"));
    QVERIFY2(loadProject(QUrl::fromLocalFile(absolutePath), expectedFailureMessage), failureMessage);
}

// There was an issue where there would be a large gap between
// the swatches and layers panels where the tileset swatch panel
// used to be after switching to an .slp after having an .stp open.
void tst_App::loadLayeredImageProjectAfterTilesetProject()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QQuickItem *layersLoader = window->findChild<QQuickItem*>("layersLoader");
    QVERIFY(layersLoader);
    QVERIFY(swatchesPanel);
    const int splitViewHandleSize = 4;
    QTRY_COMPARE(layersLoader->y(), swatchesPanel->y() + swatchesPanel->height() + splitViewHandleSize);
}

void tst_App::loadInvalidProjects_data()
{
    QTest::addColumn<QString>("projectFileName");
    QTest::addColumn<QRegularExpression>("expectedErrorMessage");

    QTest::newRow("loadInvalidProjects-empty.slp")
        << QString::fromLatin1("loadInvalidProjects-empty.slp")
        << QRegularExpression("Layered image project file is missing a \"project\" key.*");
    QTest::newRow("loadInvalidProjects-empty.stp")
        << QString::fromLatin1("loadInvalidProjects-empty.stp")
        << QRegularExpression("Tileset project file is missing a \"project\" key.*");
}

void tst_App::loadInvalidProjects()
{
    QFETCH(QString, projectFileName);
    QFETCH(QRegularExpression, expectedErrorMessage);

    // Ensure that we have a temporary directory.
    if (projectManager->projectTypeForFileName(projectFileName) == Project::LayeredImageType)
        QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);
    else
        QVERIFY2(setupTempTilesetProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Try to load the project; there should be an error.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    QVERIFY2(loadProject(QUrl::fromLocalFile(absolutePath), expectedErrorMessage), failureMessage);
}

void tst_App::recentFiles()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Should be no recent files until the new project is saved.
    QObject *recentFilesInstantiator = window->findChild<QObject*>("recentFilesInstantiator");
    QVERIFY(recentFilesInstantiator);
    QCOMPARE(recentFilesInstantiator->property("count").toInt(), 0);

    // Save.
    QVERIFY(project->saveAs(QUrl::fromLocalFile(tempProjectDir->path() + "/recentFiles.png")));
    QCOMPARE(recentFilesInstantiator->property("count").toInt(), 1);

    // Get the recent file menu item from the instantiator and ensure its text is correct.
    {
        QObject *recentFileMenuItem = nullptr;
        QVERIFY(QMetaObject::invokeMethod(recentFilesInstantiator, "objectAt", Qt::DirectConnection,
            Q_RETURN_ARG(QObject*, recentFileMenuItem), Q_ARG(int, 0)));
        QVERIFY(recentFileMenuItem);
        QCOMPARE(recentFileMenuItem->property("text").toString(), project->url().toLocalFile());
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

void tst_App::newProjectSizeFromClipboard_data()
{
    QTest::addColumn<Project::Type>("projectType");
    QTest::addColumn<QImage>("clipboardImage");

    const QImage clipboardImage = ImageUtils::filledImage(100, 200, Qt::red);

    QTest::newRow("ImageType, 100x200") << Project::ImageType << clipboardImage;
    QTest::newRow("ImageType, (none)") << Project::ImageType << QImage();
    QTest::newRow("LayeredImageType, 100x200") << Project::LayeredImageType << clipboardImage;
    QTest::newRow("LayeredImageType, (none)") << Project::LayeredImageType << QImage();
}

void tst_App::newProjectSizeFromClipboard()
{
    // If there is an image in the clipboard, use it to suggest a project size.
    QFETCH(Project::Type, projectType);
    QFETCH(QImage, clipboardImage);

    qGuiApp->clipboard()->setImage(clipboardImage);

    // Don't want to use createNewLayeredImageProject() here, because we need to test the new project popup.
    QVERIFY2(triggerNewProject(), failureMessage);

    const QObject *newProjectPopup = findOpenPopupFromTypeName("NewProjectPopup");
    QVERIFY(newProjectPopup);
    QTRY_VERIFY(newProjectPopup->property("opened").toBool());
    QVERIFY2(newProjectPopup->property("activeFocus").toBool(),
        qPrintable(QString::fromLatin1("NewProjectPopup doesn't have active focus (%1 does)")
            .arg(window->activeFocusItem()->objectName())));

    QString newProjectButtonObjectName;
    if (projectType == Project::ImageType)
        newProjectButtonObjectName = QLatin1String("imageProjectButton");
    else
        newProjectButtonObjectName = QLatin1String("layeredImageProjectButton");

    // Click on the appropriate project type button.
    QQuickItem *tilesetProjectButton = newProjectPopup->findChild<QQuickItem*>(newProjectButtonObjectName);
    QVERIFY(tilesetProjectButton);

    QVERIFY2(clickButton(tilesetProjectButton), failureMessage);
    QCOMPARE(tilesetProjectButton->property("checked").toBool(), true);

    QTRY_COMPARE(newProjectPopup->property("visible").toBool(), false);

    // Now the new project popup should be visible.
    QObject *newImageProjectPopup = nullptr;
    QVERIFY2(ensureNewImageProjectPopupVisible(projectType, &newImageProjectPopup), failureMessage);

    // Ensure that the width and height values match the clipboard data.
    QQuickItem *imageWidthSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageWidthSpinBox");
    QVERIFY(imageWidthSpinBox);
    if (!clipboardImage.isNull())
        QCOMPARE(imageWidthSpinBox->property("value").toInt(), clipboardImage.width());
    else
        QCOMPARE(imageWidthSpinBox->property("value").toInt(), 256);

    QQuickItem *imageHeightSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageHeightSpinBox");
    QVERIFY(imageHeightSpinBox);
    if (!clipboardImage.isNull())
        QCOMPARE(imageHeightSpinBox->property("value").toInt(), clipboardImage.height());
    else
        QCOMPARE(imageHeightSpinBox->property("value").toInt(), 256);

    QTest::keyClick(window, Qt::Key_Escape);
    QTRY_VERIFY(!newImageProjectPopup->property("visible").toBool());
}

void tst_App::splitViewStateAcrossProjects()
{
    QSKIP("SplitView part fails; TODO: check if still failing with Qt 6");

    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Make the panel split item larger.
    QPointer<QQuickItem> mainSplitView = window->findChild<QQuickItem*>("mainSplitView");
    QVERIFY(mainSplitView);
    QPointer<QQuickItem> panelSplitView = window->findChild<QQuickItem*>("panelSplitView");
    QVERIFY(panelSplitView);
    const qreal defaultPanelSplitItemWidth = panelSplitView->width();
    QPointer<QQuickItem> mainSplitViewHandle = findSplitViewHandle("mainSplitView", 0);
    QVERIFY(mainSplitViewHandle);
    const QPoint mainSplitViewHandleCentreAfterMoving = QPoint(
        mainSplitView->width() / 2, mainSplitView->height() / 2);
    QVERIFY2(dragSplitViewHandle("mainSplitView", 0, mainSplitViewHandleCentreAfterMoving), failureMessage);
    const qreal resizedPanelSplitItemWidth = panelSplitView->width();
    QVERIFY(resizedPanelSplitItemWidth > defaultPanelSplitItemWidth);

    // Save the project with the new split size.
    QVERIFY(layeredImageProject->canSave());
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/splitViewStateAcrossProjects.slp"));
    QVERIFY(layeredImageProject->saveAs(saveUrl));
    QVERIFY(!layeredImageProject->hasUnsavedChanges());

    // Create a new project. It should have the default panel split item size.
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QCOMPARE(panelSplitView->width(), defaultPanelSplitItemWidth);
}

// Tests that saving changes when prompted actually saves those changes.
// Note that we can only test this for projects that were already saved,
// as we can't interact with a native save dialog.
void tst_App::saveOnPrompt()
{
    if (offscreenPlatform)
        QSKIP("Doesn't work with offscreen platform");

    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/saveOnPrompt.slp"));
    QVERIFY(layeredImageProject->saveAs(saveUrl));
    QVERIFY(!layeredImageProject->hasUnsavedChanges());

    setCursorPosInScenePixels(1, 1);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY2(saveChanges(), failureMessage);
}

// Tests that pane offset is correctly read from a project file when either coordinate is not a whole number.
void tst_App::loadPaneFractionalOffset()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    const QString projectFileName = QLatin1String("loadPaneFractionalOffset.slp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Load the project and make sure the offset is correct.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    const QUrl projectUrl = QUrl::fromLocalFile(absolutePath);
    QVERIFY2(loadProject(projectUrl), failureMessage);
    QCOMPARE(canvas->firstPane()->offset(), QPointF(257, 235));
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

    // Open the behaviour tab of the options dialog.
    QObject *optionsDialog = nullptr;
    QVERIFY2(openOptionsTab("behaviourTabButton", &optionsDialog), failureMessage);

    // Bring the "New Project" shortcut row into view.
    QVERIFY2(ensureScrollViewChildVisible("behaviourScrollView", "newShortcutRow"), failureMessage);

    // Give "New Project" shortcut editor focus.
    QQuickItem *newShortcutButton = optionsDialog->findChild<QQuickItem*>("newShortcutButton");
    QVERIFY(newShortcutButton);
    QCOMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QVERIFY2(clickButton(newShortcutButton), failureMessage);

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
    QVERIFY2(clickButton(newShortcutButton), failureMessage);
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
    QVERIFY2(clickButton(newShortcutButton), failureMessage);
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

    // Open the behaviour tab of the options dialog.
    QObject *optionsDialog = nullptr;
    QVERIFY2(openOptionsTab("behaviourTabButton", &optionsDialog), failureMessage);

    // Give "New Project" shortcut editor focus.
    QQuickItem *newShortcutButton = optionsDialog->findChild<QQuickItem*>("newShortcutButton");
    QVERIFY(newShortcutButton);
    QCOMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QVERIFY2(clickButton(newShortcutButton), failureMessage);

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
    QTRY_VERIFY(!optionsDialog->property("visible").toBool());
    // Cancelling the dialog shouldn't change anything.
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    // Reopen the dialog to make sure that the editor shows the default shortcut.
    QVERIFY2(triggerOptions(), failureMessage);
    QTRY_VERIFY(optionsDialog->property("opened").toBool());
    QTRY_COMPARE(newShortcutButton->property("text").toString(), app.settings()->defaultNewShortcut());
    QCOMPARE(app.settings()->newShortcut(), app.settings()->defaultNewShortcut());

    // Close the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
}

void tst_App::optionsTransparencyCancelled()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Open the appearance tab of the options dialog.
    QObject *optionsDialog = nullptr;
    QVERIFY2(openOptionsTab("appearanceTabButton", &optionsDialog), failureMessage);

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
    QTRY_VERIFY(!optionsDialog->property("visible").toBool());
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

    QVERIFY(tileCanvas->isGridVisible());
    // Toggle the option.
    QVERIFY2(triggerGridVisible(), failureMessage);
    QVERIFY(!tileCanvas->isGridVisible());

    // Close the view menu.
    QTest::keyClick(window, Qt::Key_Escape);

    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage withoutGrid = imageGrabber.takeImage();
    QVERIFY(withoutGrid != withGrid);

    // Show the grid again.
    QVERIFY2(triggerGridVisible(), failureMessage);
    QVERIFY(tileCanvas->isGridVisible());
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
    QVERIFY(tilesetProject->saveAs(QUrl::fromLocalFile(tempProjectDir->path() + "/project.stp")));
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

    // With the mouse pressed, move the cursor down by one pixel to draw more,
    // checking that the tile's image changes.
    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    setCursorPosInScenePixels(cursorPos + QPoint(0, 1));
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(*tilesetProject->tileAt(cursorPos)->tileset()->image() != lastImage);

    // Now release the mouse and finish the drawing. Nothing should have changed.
    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Undo the changes.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
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
    QVERIFY2(togglePanel("tilesetSwatchPanel", true), failureMessage);

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
    QVERIFY2(clickButton(undoToolButton), failureMessage);

    QCOMPARE(*tilesetProject->tileset()->image(), originalTilesetImage);
}

void tst_App::undoTiles()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

    // For some reason, the second pane isn't updated after the centering occurs.
    // There is a noticeable delay when creating a new tileset project
    // (with splitscreen as the default) too, but it's not a huge deal...
    QTest::mouseMove(window, QPoint(window->width() * 0.66, window->height() * 0.5));
    // Wait for the pane centering update to be painted.
    QTest::qWait(100);

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
    QVERIFY(!undoToolButton->isEnabled());
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
    QVERIFY(undoToolButton->isEnabled());
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test the undo button.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QVERIFY(!tilesetProject->tileAt(cursorPos));
    QVERIFY(!tilesetProject->tileAt(cursorPos - QPoint(0, tilesetProject->tileHeight())));
    QVERIFY(!tilesetProject->hasUnsavedChanges());
    QVERIFY(!window->title().contains("*"));
    // Move the mouse away so the tile pen preview doesn't affect
    // our check that the canvas is actually updated.
    QTest::mouseMove(window, outsideCanvas);
    QVERIFY(imageGrabber.requestImage(tileCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grab = imageGrabber.takeImage();
    QCOMPARE(grab, originalCanvasImage);

    // Draw a tile back so we can test the revert button.
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    // Test reverting.
    QVERIFY2(triggerRevert(), failureMessage);
    QVERIFY(!tilesetProject->tileAt(cursorPos));
    QVERIFY(!undoToolButton->isEnabled());
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
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);

    setCursorPosInScenePixels(0, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);

    lastImage = *tilesetProject->tileAt(cursorPos)->tileset()->image();
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(*tilesetProject->tileAt(cursorPos)->tileset()->image(), lastImage);
    QVERIFY(tilesetProject->hasUnsavedChanges());
    QVERIFY(window->title().contains("*"));

    QVERIFY2(clickButton(undoToolButton), failureMessage);
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

    QVERIFY2(clickButton(undoToolButton), failureMessage);
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

    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(tilesetProject->tiles(), originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 10 * 10);

    // Check that neither of the following assert.
    QVERIFY2(changeCanvasSize(10, 9), failureMessage);
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(tilesetProject->tiles(), originalTiles);
    QCOMPARE(tilesetProject->tiles().size(), 10 * 10);

    QVERIFY2(changeCanvasSize(9, 10), failureMessage);
    QVERIFY2(clickButton(undoToolButton), failureMessage);
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
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
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

    const QImage originalContents = project->exportedImage();

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preSizeChangeCanvasSnapshot = imageGrabber.takeImage();

    QVERIFY2(changeImageSize(6, 6), failureMessage);

    // The contents should have been scaled down by 50%.
    const QImage expectedResizedContents = ImageUtils::resizeContents(originalContents, 6, 6);
    QVERIFY2(compareImages(project->exportedImage(), expectedResizedContents), failureMessage);

    // Move the mouse back so the image comparison works.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseMove(window, cursorWindowPos);

    // Ensure that the canvas was repainted after the size change.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preUndoSnapshot = imageGrabber.takeImage();
    QVERIFY(preUndoSnapshot != preSizeChangeCanvasSnapshot);

    // Undo the size change.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QVERIFY2(compareImages(project->exportedImage(), originalContents), failureMessage);

    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage postUndoSnapshot = imageGrabber.takeImage();
    QCOMPARE(postUndoSnapshot, preSizeChangeCanvasSnapshot);
}

void tst_App::undoLayeredImageSizeChange()
{
    QVERIFY2(createNewLayeredImageProject(12, 12), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QVERIFY2(changeToolSize(4), failureMessage);

    // Draw a 4x4 square with its centre at 2, 2.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(3, 3), QColor(Qt::black));

    // Add a new layer.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    ImageLayer *layer2 = layeredImageProject->layerAt(0);

    // Select the new layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw on the new layer.
    setCursorPosInScenePixels(6, 2);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(layer2->image()->pixelColor(4, 0), QColor(Qt::red));
    QCOMPARE(layer2->image()->pixelColor(7, 3), QColor(Qt::red));

    const QImage originalContents = project->exportedImage();

    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preSizeChangeCanvasSnapshot = imageGrabber.takeImage();

    QVERIFY2(changeImageSize(6, 6), failureMessage);

    // The contents of both layers should have been scaled down by 50%.
    const QImage expectedResizedContents = ImageUtils::resizeContents(originalContents, 6, 6);
    QVERIFY2(compareImages(project->exportedImage(), expectedResizedContents), failureMessage);

    // Move the mouse back so the image comparison works.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseMove(window, cursorWindowPos);

    // Ensure that the canvas was repainted after the size change.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage preUndoSnapshot = imageGrabber.takeImage();
    QVERIFY(preUndoSnapshot != preSizeChangeCanvasSnapshot);

    // Undo the size change.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QVERIFY2(compareImages(project->exportedImage(), originalContents), failureMessage);

    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage postUndoSnapshot = imageGrabber.takeImage();
    QCOMPARE(postUndoSnapshot, preSizeChangeCanvasSnapshot);
}

void tst_App::undoRearrangeContentsIntoGridChange_data()
{
    QTest::addColumn<QString>("projectPath");
    QTest::addColumn<QString>("expectedExportedImagePath");
    QTest::addColumn<int>("cellWidth");
    QTest::addColumn<int>("cellHeight");
    QTest::addColumn<int>("columns");
    QTest::addColumn<int>("rows");

    QTest::newRow("ImageType, grid-4x4 to 8x8")
        << "grid-4x4.png" << ":/resources/grid-4x4-to-8x8.png" << 8 << 8 << 8 << 8;
    QTest::newRow("LayeredImageType, grid-4x4 to 8x8")
        << "grid-4x4.slp" << ":/resources/grid-4x4-to-8x8.png" << 8 << 8 << 8 << 8;
}

void tst_App::undoRearrangeContentsIntoGridChange()
{
    QFETCH(QString, projectPath);
    QFETCH(QString, expectedExportedImagePath);
    QFETCH(int, cellWidth);
    QFETCH(int, cellHeight);
    QFETCH(int, columns);
    QFETCH(int, rows);

    QVERIFY2(setupTempProjectDir(), failureMessage);
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectPath), failureMessage);

    const QUrl projectUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1Char('/') + projectPath);
    QVERIFY2(loadProject(projectUrl), failureMessage);

    QVERIFY2(rearrangeContentsIntoGrid(cellWidth, cellHeight, columns, rows), failureMessage);

    const QImage expectedExportedImage(expectedExportedImagePath);
    QVERIFY2(!expectedExportedImage.isNull(), qPrintable(QString::fromLatin1(
        "Failed to open expectedExportedImage at %1").arg(expectedExportedImagePath)));
    QVERIFY2(compareImages(project->exportedImage(), expectedExportedImage), failureMessage);
}

void tst_App::undoPixelFill()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);
    QVERIFY2(togglePanel("tilesetSwatchPanel", true), failureMessage);

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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(targetTile->pixelColor(0, 0), black);
    QCOMPARE(targetTile->pixelColor(1, 0), black);
    QCOMPARE(targetTile->pixelColor(1, 1), black);
    QCOMPARE(targetTile->pixelColor(0, 1), black);
}

void tst_App::undoTileFill()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);
    QVERIFY2(togglePanel("tilesetSwatchPanel", true), failureMessage);

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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(0, 0)), targetTile);
    QCOMPARE(tilesetProject->tileAtTilePos(QPoint(1, 0)), targetTile);
}

void tst_App::undoThickSquarePen()
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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
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
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 2), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 2), QColor(Qt::black));

    // Undo it.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 0), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 2), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 2), QColor(Qt::white));
}

void tst_App::undoThickRoundPen()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    QVERIFY2(changeToolSize(4), failureMessage);
    QVERIFY2(changeToolShape(ImageCanvas::CircleToolShape), failureMessage);

    QImage expectedClickImage(":/resources/undoThickRoundPen-1.png");
    QVERIFY(!expectedClickImage.isNull());

    QImage undoneImage(4, 4, QImage::Format_ARGB32);
    undoneImage.fill(Qt::white);

    // First, try a single click.
    setCursorPosInScenePixels(QPoint(2, 2));
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->copy(QRect(0, 0, 4, 4)), expectedClickImage);

    // Undo it.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->copy(QRect(0, 0, 4, 4)), undoneImage);

    QImage expectedDragImage(":/resources/undoThickRoundPen-2.png");
    QVERIFY(!expectedDragImage.isNull());

    undoneImage = QImage(5, 5, QImage::Format_ARGB32);
    undoneImage.fill(Qt::white);

    // Next, try dragging.
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    setCursorPosInScenePixels(QPoint(3, 3));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->copy(QRect(0, 0, 5, 5)), expectedDragImage);

    // Undo it.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->copy(QRect(0, 0, 5, 5)), undoneImage);
}

void tst_App::penSubpixelPosition()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    // Ensure that the first pane is current by hovering it.
    setCursorPosInScenePixels(QPoint(1, 1));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());

    // For some reason it crashes on macOS here, so do it manually.
    canvas->currentPane()->setZoomLevel(30);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());

    // Ensure that we can see the top left corner as a sanity check while visually debugging.
    QVERIFY2(panTopLeftTo(100, 100), failureMessage);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());

    /*
        This test ensures that the pen tool draws at the correct position
        when the canvas is zoomed in. Specifically, the centre of the brush's crosshair
        should be the centre of the drawn pixel. In the diagram below, the centre
        of the crosshair is "between" {0, 0} and {1, 1}. Each check moves it one pixel
        diagonally to make sure the correct pixel is drawn in.

              0         1
         +---------+---------+
         |         |         |
       0 |         |         |
         |         |         |
         +---------o---------+
         |         |         |
       1 |         |         |
         |         |         |
         +---------+---------+
    */
    setCursorPosInScenePixels(QPoint(1, 1));
    const QPoint originalPos = cursorWindowPos;
    QTest::mouseMove(window, cursorWindowPos);

    /*
        {0, 0} should be filled in since the centre of the crosshair is now over it.

              0         1
         +---------+---------+
         |         |         |
       0 |         |         |
         |       o |         |
         +---------|---------+
         |         |         |
       1 |         |         |
         |         |         |
         +---------+---------+
    */
    cursorWindowPos = originalPos + QPoint(-1, -1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));

    /*
        {1, 0} should be filled in since the centre of the crosshair is now over it.

              0         1
         +---------+---------+
         |         |         |
       0 |         |         |
         |         | o       |
         +---------|---------+
         |         |         |
       1 |         |         |
         |         |         |
         +---------+---------+
    */
    cursorWindowPos = originalPos + QPoint(1, -1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 0), QColor(Qt::black));

    /*
        {1, 1} should be filled in since the centre of the crosshair is now over it.

              0         1
         +---------+---------+
         |         |         |
       0 |         |         |
         |         |         |
         +---------|---------+
         |         | o       |
       1 |         |         |
         |         |         |
         +---------+---------+
    */
    cursorWindowPos = originalPos + QPoint(1, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));

    /*
        {0, 1} should be filled in since the centre of the crosshair is now over it.

              0         1
         +---------+---------+
         |         |         |
       0 |         |         |
         |         |         |
         +---------|---------+
         |       o |         |
       1 |         |         |
         |         |         |
         +---------+---------+
    */
    cursorWindowPos = originalPos + QPoint(-1, 1);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 1), QColor(Qt::black));
}

void tst_App::penSubpixelPositionWithThickBrush_data()
{
    QTest::addColumn<ImageCanvas::ToolShape>("toolShape");
    QTest::addColumn<QString>("expectedImagePath");

    QTest::newRow("square") << ImageCanvas::SquareToolShape
        << ":/resources/penSubpixelPositionWithThickBrush-square.png";
    QTest::newRow("circle") << ImageCanvas::CircleToolShape
        << ":/resources/penSubpixelPositionWithThickBrush-circle.png";
}

// Same as penSubpixelPosition() except with a thicker brush.
void tst_App::penSubpixelPositionWithThickBrush()
{
    QFETCH(ImageCanvas::ToolShape, toolShape);
    QFETCH(QString, expectedImagePath);

    QVERIFY2(createNewImageProject(4, 4), failureMessage);

    QVERIFY2(changeToolShape(toolShape), failureMessage);
    QVERIFY2(changeToolSize(4), failureMessage);

    // Ensure that the first pane is current by hovering it.
    setCursorPosInScenePixels(QPoint(1, 1));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());

    // For some reason it crashes on macOS here, so do it manually.
    canvas->currentPane()->setZoomLevel(30);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());

    // Ensure that we can see the top left corner as a sanity check while visually debugging.
    QVERIFY2(panTopLeftTo(100, 100), failureMessage);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());

    // Set up the original position.
    setCursorPosInScenePixels(QPoint(2, 2));
    const QPoint originalPos = cursorWindowPos;
    QTest::mouseMove(window, cursorWindowPos);

    QVector<QPoint> mousePositions;
    mousePositions << originalPos + QPoint(-1, -1);
    mousePositions << originalPos + QPoint(1, -1);
    mousePositions << originalPos + QPoint(1, 1);
    mousePositions << originalPos + QPoint(-1, 1);

    const QImage expectedImage(expectedImagePath);
    QVERIFY(!expectedImage.isNull());

    for (const QPoint &mousePosition : std::as_const(mousePositions)) {
        cursorWindowPos = mousePosition;
        QTest::mouseMove(window, cursorWindowPos);
        QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QCOMPARE(canvas->currentProjectImage()->copy(QRect(0, 0, 4, 4)), expectedImage);
    }
}

void tst_App::colours_data()
{
    addAllProjectTypes();
}

void tst_App::colours()
{
    QSKIP("SplitView part fails; TODO: check if still failing with Qt 6");

    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);
    QVERIFY2(expandAndResizePanel("colourPanel"), failureMessage);
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
    QVERIFY2(clickButton(penForegroundColourButton), failureMessage);
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penForegroundColour().name());

    // TODO: fix issue where hue slider handle is missing
    // For now, we work around it.
    mouseEvent(hueSlider, QPointF(hueSlider->width() / 2, hueSlider->height() / 2), MouseClick);

    // Ensure that the buttons in the panel are visible.
    QQuickItem *colourPanelFlickable = window->findChild<QQuickItem*>("colourPanelFlickable");
    QVERIFY(colourPanelFlickable);
    QVERIFY2(ensureFlickableChildVisible(colourPanelFlickable, lighterButton), failureMessage);

    // Test that the "Lighter" button works.
    QColor oldColour = canvas->penForegroundColour();
    QVERIFY2(clickButton(lighterButton), failureMessage);
    QVERIFY(canvas->penForegroundColour().lightnessF() > oldColour.lightnessF());

    // Test that the "Darker" button works.
    oldColour = canvas->penForegroundColour();
    QVERIFY2(clickButton(darkerButton), failureMessage);
    QVERIFY(canvas->penForegroundColour().lightnessF() < oldColour.lightnessF());

    // Test that the "Saturate" button works.
    oldColour = canvas->penForegroundColour();
    QVERIFY2(clickButton(saturateButton), failureMessage);
    QVERIFY(canvas->penForegroundColour().saturationF() > oldColour.saturationF());

    // Test that the "Desaturate" button works.
    oldColour = canvas->penForegroundColour();
    QVERIFY2(clickButton(desaturateButton), failureMessage);
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
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Now draw with a colour that only varies in saturation from the previous colour.
    imageCanvas->setPenForegroundColour(colour2);
    setCursorPosInScenePixels(1, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

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
    QVERIFY2(togglePanel("tilesetSwatchPanel", true), failureMessage);

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

// Check that the eyedropper applies the colour to the background colour if it's selected (#75).
void tst_App::eyedropperBackgroundColour()
{
    QVERIFY2(createNewImageProject(), failureMessage);
    QVERIFY2(togglePanel("colourPanel", true), failureMessage);

    // Draw a black pixel.
    setCursorPosInScenePixels(QPoint(10, 10));
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Make the background colour active.
    mouseEvent(penBackgroundColourButton,
        QPoint(penBackgroundColourButton->width() - 1, penBackgroundColourButton->width() - 1), MouseClick);

    // Select the black pixel; the background colour should then be black.
    QVERIFY2(switchTool(ImageCanvas::EyeDropperTool), failureMessage);
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->penBackgroundColour(), QColor(Qt::black));
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
    const int xDiff = qAbs(currentPane->integerOffset().x() - expectedOffset.x());
    const int yDiff = qAbs(currentPane->integerOffset().y() - expectedOffset.y());
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

void tst_App::centrePanes()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    QCOMPARE(canvas->isSplitScreen(), true);
    QCOMPARE(canvas->firstPane()->size(), 0.5);
    QCOMPARE(canvas->secondPane()->size(), 0.5);

    CanvasPane *firstPane = canvas->firstPane();
    CanvasPane *secondPane = canvas->secondPane();

    QVERIFY2(panTopLeftTo(100, 100), failureMessage);
    // zoomTo() wasn't working, so set the zoom levels manually.
    firstPane->setZoomLevel(2);
    secondPane->setZoomLevel(4);

    QVERIFY2(triggerCentre(), failureMessage);
    QCOMPARE(firstPane->integerOffset(), canvas->centredPaneOffset(0));
    QCOMPARE(secondPane->integerOffset(), canvas->centredPaneOffset(1));
}

void tst_App::useTilesetSwatch()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);
    QVERIFY2(togglePanel("tilesetSwatchPanel", true), failureMessage);

    QVERIFY2(switchMode(TileCanvas::TileMode), failureMessage);

    QCOMPARE(tileCanvas->penTile(), tilesetProject->tilesetTileAt(0, 0));

    // Ensure that the tileset swatch flickable has the correct contentY.
    QQuickItem *tilesetSwatchFlickable = tilesetSwatchPanel->findChild<QQuickItem*>("tilesetSwatchFlickable");
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
    QVERIFY(imageGrabber.requestImage(tilesetSwatchPanel));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage originalTilesetSnapshot = imageGrabber.takeImage();

    const QImage originalTilesetImage = *expectedTile->tileset()->image();
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(*expectedTile->tileset()->image() != originalTilesetImage);

    QVERIFY(imageGrabber.requestImage(tilesetSwatchPanel));
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
//    QVERIFY2(clickButton(duplicateTileMenuItem), failureMessage);
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
//    QVERIFY2(clickButton(rotateTileLeftMenuItem), failureMessage);
//    QVERIFY(!tilesetContextMenu->property("visible").toBool());
//    QCOMPARE(ImageUtils::rotate(tileCanvas->penTile()->image(), 90), originalTileImage);

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
//    QVERIFY2(clickButton(rotateTileRightMenuItem), failureMessage);
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
//    QVERIFY2(clickButton(lighterButton), failureMessage);

//    setCursorPosInTiles(0, 0);
    //    QTRY_COMPARE(window->cursor().shape(), Qt::BlankCursor);
}

void tst_App::colourPickerHexField()
{
    QVERIFY2(createNewImageProject(), failureMessage);
    QVERIFY2(togglePanel("colourPanel", true), failureMessage);

    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    QVERIFY(hexTextField);
    QCOMPARE(canvas->penForegroundColour(), QColor(Qt::black));
    QCOMPARE(hexTextField->property("text").toString().prepend("#"), canvas->penForegroundColour().name());

    const QColor originalPenColour = canvas->penForegroundColour();

    mouseEventOnCentre(hexTextField, MouseClick);
    ENSURE_ACTIVE_FOCUS(hexTextField);

    QTest::keySequence(window, QKeySequence::SelectAll);
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
    QTest::keySequence(window, QKeySequence::SelectAll);
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

void tst_App::splitterSettingsMouse_data()
{
    addAllProjectTypes();
}

void tst_App::splitterSettingsMouse()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    // Split screen is on by default for auto tests.
    QCOMPARE(canvas->isSplitScreen(), true);
    QCOMPARE(canvas->firstPane()->size(), 0.5);
    QCOMPARE(canvas->secondPane()->size(), 0.5);
    QCOMPARE(splitScreenToolButton->isEnabled(), true);
    QCOMPARE(splitScreenToolButton->property("checked").toBool(), true);
    // The splitter is always locked by default.
    QCOMPARE(canvas->splitter()->isEnabled(), false);
    QCOMPARE(lockSplitterToolButton->isEnabled(), true);
    QCOMPARE(lockSplitterToolButton->property("checked").toBool(), true);

    // Turn split screen off.
    QVERIFY2(clickButton(splitScreenToolButton), failureMessage);
    QCOMPARE(canvas->isSplitScreen(), false);
    QCOMPARE(canvas->firstPane()->size(), 1.0);
    QCOMPARE(canvas->secondPane()->size(), 0.0);
    QCOMPARE(splitScreenToolButton->isEnabled(), true);
    QCOMPARE(splitScreenToolButton->property("checked").toBool(), false);
    // The lock splitter tool button should be disabled but retain its original value.
    QCOMPARE(canvas->splitter()->isEnabled(), false);
    QCOMPARE(lockSplitterToolButton->isEnabled(), false);
    QCOMPARE(lockSplitterToolButton->property("checked").toBool(), true);

    // Turn split screen back on.
    QVERIFY2(clickButton(splitScreenToolButton), failureMessage);
    QCOMPARE(canvas->isSplitScreen(), true);
    QCOMPARE(canvas->firstPane()->size(), 0.5);
    QCOMPARE(canvas->secondPane()->size(), 0.5);
    QCOMPARE(splitScreenToolButton->isEnabled(), true);
    QCOMPARE(splitScreenToolButton->property("checked").toBool(), true);
    QCOMPARE(canvas->splitter()->isEnabled(), false);
    QCOMPARE(lockSplitterToolButton->isEnabled(), true);
    QCOMPARE(lockSplitterToolButton->property("checked").toBool(), true);

    // Enable the splitter.
    QVERIFY2(clickButton(lockSplitterToolButton), failureMessage);
    QCOMPARE(canvas->isSplitScreen(), true);
    QCOMPARE(canvas->firstPane()->size(), 0.5);
    QCOMPARE(canvas->secondPane()->size(), 0.5);
    QCOMPARE(splitScreenToolButton->isEnabled(), true);
    QCOMPARE(splitScreenToolButton->property("checked").toBool(), true);
    QCOMPARE(canvas->splitter()->isEnabled(), true);
    QCOMPARE(lockSplitterToolButton->isEnabled(), true);
    QCOMPARE(lockSplitterToolButton->property("checked").toBool(), false);
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
    QCOMPARE(canvas->currentProjectImage()->pixelColor(
        project->widthInPixels() - 1, project->heightInPixels() - 1), QColor(Qt::black));
}

void tst_App::fillLayeredImageCanvas()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    // Add a new layer.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(35, 4), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(35, 35), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 35), QColor(Qt::black));
}

void tst_App::texturedFillVariance_data()
{
    addImageProjectTypes();
}

void tst_App::texturedFillVariance()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(changeCanvasSize(90, 90), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::TexturedFillTool), failureMessage);
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
    QVERIFY2(clickButton(texturedFillSettingsCancelButton), failureMessage);
    QTRY_VERIFY(!settingsDialog->property("visible").toBool());

    // .. and then open it again.
    QVERIFY(QMetaObject::invokeMethod(settingsDialog, "open"));
    QTRY_VERIFY(settingsDialog->property("opened").toBool());

    // The original settings should be restored.
    QVERIFY(hueVarianceCheckBox->setProperty("checked", QVariant(false)));
    QVERIFY(saturationVarianceCheckBox->setProperty("checked", QVariant(false)));
    QVERIFY(lightnessVarianceCheckBox->setProperty("checked", QVariant(true)));

    // Open the settings dialog again.
    QTRY_VERIFY(settingsDialog->property("opened").toBool());
    QCOMPARE(hueVarianceCheckBox->property("checked").toBool(), false);
    QCOMPARE(saturationVarianceCheckBox->property("checked").toBool(), false);
    QCOMPARE(lightnessVarianceCheckBox->property("checked").toBool(), true);

    // Confirm the changes.
    QQuickItem *texturedFillSettingsDialogOkButton = settingsDialog->findChild<QQuickItem*>("texturedFillSettingsDialogOkButton");
    QVERIFY(texturedFillSettingsDialogOkButton);
    QVERIFY2(clickButton(texturedFillSettingsDialogOkButton), failureMessage);
    QTRY_VERIFY(!settingsDialog->property("visible").toBool());

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

void tst_App::texturedFillSwatch_data()
{
    addImageProjectTypes();
}

void tst_App::texturedFillSwatch()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(changeCanvasSize(90, 90), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::TexturedFillTool), failureMessage);
    QCOMPARE(canvas->lastFillToolUsed(), ImageCanvas::TexturedFillTool);

    // Draw some pixels.
    const QColor colour1("#0000ff");
    QVERIFY2(setPenForegroundColour(colour1.name()), failureMessage);
    setCursorPosInScenePixels(89, 89);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    const QColor colour2("#ff0000");
    QVERIFY2(setPenForegroundColour(colour2.name()), failureMessage);
    setCursorPosInScenePixels(89, 88);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Select the pixels that we just drew so that we can use them for the texture fill swatch.
    QVERIFY2(selectArea(QRect(89, 88, 1, 2)), failureMessage);

    // Add the colours from the selection manually since we can't open the menu.
    QVERIFY2(addSelectedColoursToTexturedFillSwatch(), failureMessage);

    // Decrease probability of colour2 (red).
    QObject *settingsDialog = window->findChild<QObject*>("texturedFillSettingsDialog");
    QVERIFY(settingsDialog);
    QQuickItem *texturedFillSwatchListView = settingsDialog->findChild<QQuickItem*>("texturedFillSwatchListView");
    QVERIFY(texturedFillSwatchListView);
    QQuickItem *swatchDelegate = viewItemAtIndex(texturedFillSwatchListView, 0);
    QVERIFY(swatchDelegate);
    QCOMPARE(texturedFillSwatchListView->property("count").toInt(), 2);

    QVERIFY(swatchDelegate);
    QQuickItem *probabilitySlider = swatchDelegate->findChild<QQuickItem*>("texturedFillSwatchColourProbabilitySlider");
    QVERIFY(probabilitySlider);
    mouseEvent(probabilitySlider, QPoint(probabilitySlider->width() * 0.25, probabilitySlider->height() / 2), MouseClick);
    const qreal sliderValue = probabilitySlider->property("value").toReal();
    QVERIFY(sliderValue < 0.5);

    QQuickItem *texturedFillSettingsPreviewItem = settingsDialog->findChild<QQuickItem*>("texturedFillSettingsPreviewItem");
    QVERIFY(texturedFillSettingsPreviewItem);
    auto previewParamaters = texturedFillSettingsPreviewItem->property("parameters").value<TexturedFillParameters*>();
    QCOMPARE(previewParamaters->swatch()->probabilities().at(0), sliderValue);

    // Confirm the changes.
    QQuickItem *texturedFillSettingsDialogOkButton = settingsDialog->findChild<QQuickItem*>("texturedFillSettingsDialogOkButton");
    QVERIFY(texturedFillSettingsDialogOkButton);
    QVERIFY2(clickButton(texturedFillSettingsDialogOkButton), failureMessage);
    QTRY_VERIFY(!settingsDialog->property("visible").toBool());

    // Switch back from the selection tool to the textured fill tool and clear the selection.
//    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY2(switchTool(ImageCanvas::TexturedFillTool), failureMessage);
    QVERIFY(!canvas->hasSelection());

    // Fill the canvas.
    setCursorPosInScenePixels(0, 0);
    mouseEvent(canvas, cursorWindowPos, MouseClick);
    // Ensure that there is some variation to the colours.
    int colour1Count = 0;
    int colour2Count = 0;
    for (int y = 0; y < canvas->currentProjectImage()->height(); ++y) {
        for (int x = 0; x < canvas->currentProjectImage()->width(); ++x) {
            const QColor colour = canvas->currentProjectImage()->pixelColor(x, y);
            // (the background is white by default)
            if (colour == colour1)
                ++colour1Count;
            else if (colour == colour2)
                ++colour2Count;
        }
    }
    QVERIFY(colour1Count > 0);
    QVERIFY(colour2Count > 0);
    const int totalPixels = 90 * 90;
    const qreal colour1occurrence = colour1Count / qreal(totalPixels);
    const qreal colour2occurrence = colour2Count / qreal(totalPixels);
    QVERIFY2(colour1occurrence - colour2occurrence > 0.25, qPrintable(QString::fromLatin1(
        "Expected occurrence of colour1 (%1) to be at least 25% higher than colour2 (%2)")
            .arg(colour1occurrence).arg(colour2occurrence)));

    // Add the same colours again; they shouldn't be added because they're duplicates.
    QVERIFY2(selectArea(QRect(89, 88, 1, 2)), failureMessage);
    QVERIFY2(addSelectedColoursToTexturedFillSwatch(), failureMessage);
    QVERIFY2(clickButton(texturedFillSettingsDialogOkButton), failureMessage);
    QTRY_VERIFY(!settingsDialog->property("visible").toBool());
    QCOMPARE(canvas->texturedFillParameters()->swatch()->colours().size(), 2);
    QCOMPARE(canvas->texturedFillParameters()->swatch()->probabilities().size(), 2);
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

    QQuickItem *lineLengthLabel = window->findChild<QQuickItem*>("lineLengthLabel");
    QVERIFY(lineLengthLabel);
    QQuickItem *lineAngleLabel = window->findChild<QQuickItem*>("lineAngleLabel");
    QVERIFY(lineAngleLabel);

    // Draw the start of the line.
    setCursorPosInScenePixels(0, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);
    QCOMPARE(canvas->isLineVisible(), false);
    QCOMPARE(lineLengthLabel->isVisible(), false);
    QCOMPARE(lineAngleLabel->isVisible(), false);

    // Hold shift to start the line-drawing process and
    // get the line length/angle labels visible.
    QTest::keyPress(window, Qt::Key_Shift);
    QCOMPARE(canvas->isLineVisible(), true);
    QCOMPARE(lineLengthLabel->isVisible(), true);
    QCOMPARE(lineAngleLabel->isVisible(), true);
    QCOMPARE(lineLengthLabel->property("text").toString(), "0");
    QCOMPARE(lineAngleLabel->property("text").toString(), "0.00");

    // Draw the line itself.
    setCursorPosInScenePixels(2, 2);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(lineLengthLabel->property("text").toString(), "2");
    QCOMPARE(lineAngleLabel->property("text").toString(), "315.00");
    // For some reason there must be a delay in order for the shift modifier to work.
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, 100);
    QCOMPARE(canvas->isLineVisible(), true);
    QCOMPARE(lineLengthLabel->isVisible(), true);
    QCOMPARE(lineAngleLabel->isVisible(), true);
    QTest::keyRelease(window, Qt::Key_Shift);
    QCOMPARE(canvas->isLineVisible(), false);
    QCOMPARE(lineLengthLabel->isVisible(), false);
    QCOMPARE(lineAngleLabel->isVisible(), false);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::black));

    // Undo the line.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    // The initial press has to still be there.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::white));

    // Redo the line.
    QVERIFY2(clickButton(redoToolButton), failureMessage);
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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 3), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 4), QColor(Qt::white));

    // Undo the first line.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    // The initial press has to still be there.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::white));

    // Undo the inital press.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    // The initial press has to still be there.
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), QColor(Qt::white));

    // Redo the line.
    QVERIFY2(clickButton(redoToolButton), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), translucentRed);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(2, 2), translucentRed);
}

void tst_App::lineMiddleMouseButton()
{
    QVERIFY2(createNewProject(Project::LayeredImageType), failureMessage);
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);

    // Draw a black pixel.
    setCursorPosInScenePixels(10, 10);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Middle-click once.
    QTest::mouseClick(window, Qt::MiddleButton, Qt::NoModifier, cursorWindowPos);

    // Change foreground colour to red so we can tell that the line preview is rendered.
    canvas->setPenForegroundColour(Qt::red);
    // Hold shift; the line preview should be black, not white.
    QTest::keyPress(window, Qt::Key_Shift);
    // If the test fails before we can release, we don't want to affect future tests.
    auto releaseShift = qScopeGuard([=](){ QTest::keyRelease(window, Qt::Key_Shift); });
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithRedPixel = imageGrabber.takeImage();
    QCOMPARE(grabWithRedPixel.pixelColor(10, 10), QColor(Qt::red));
}

void tst_App::penToolRightClickBehaviour_data()
{
    QTest::addColumn<Project::Type>("projectType");
    QTest::addColumn<ImageCanvas::PenToolRightClickBehaviour>("penToolRightClickBehaviour");

    for (const auto projectType : std::as_const(allProjectTypes)) {
        const QString typeString = Project::typeToString(projectType);

        for (const auto behaviour : std::as_const(allRightClickBehaviours)) {
            const QMetaEnum behaviourMetaEnum = QMetaEnum::fromType<ImageCanvas::PenToolRightClickBehaviour>();
            const QString behaviourString = behaviourMetaEnum.valueToKey(behaviour);

            const QString tag = typeString + QLatin1Char(',') + behaviourString;
            QTest::newRow(qPrintable(tag)) << projectType << behaviour;
        }
    }
}

void tst_App::penToolRightClickBehaviour()
{
    QFETCH(Project::Type, projectType);
    QFETCH(ImageCanvas::PenToolRightClickBehaviour, penToolRightClickBehaviour);

    QVERIFY2(createNewProject(projectType), failureMessage);
    QCOMPARE(app.settings()->penToolRightClickBehaviour(), ImageCanvas::PenToolRightClickAppliesEraser);

    // Zoom in to make visual debugging easier.
    canvas->setSplitScreen(false);
    canvas->currentPane()->setZoomLevel(48);

    // Open the behaviour tab of the options dialog.
    QObject *optionsDialog = nullptr;
    QVERIFY2(openOptionsTab("behaviourTabButton", &optionsDialog), failureMessage);

    // Ensure that penToolRightClickBehaviourComboBox is visible in the options dialog.
    QVERIFY2(ensureScrollViewChildVisible("behaviourScrollView", "penToolRightClickBehaviourComboBox"), failureMessage);

    // Open penToolRightClickBehaviourComboBox's popup.
    QQuickItem *penToolRightClickBehaviourComboBox = optionsDialog->findChild<QQuickItem*>("penToolRightClickBehaviourComboBox");
    QVERIFY(penToolRightClickBehaviourComboBox);
    mouseEventOnCentre(penToolRightClickBehaviourComboBox, MouseClick);
    QVERIFY2(penToolRightClickBehaviourComboBox->hasActiveFocus(), qPrintable(QtUtils::toString(window->activeFocusItem())));
    QObject *penToolRightClickBehaviourComboBoxPopup = penToolRightClickBehaviourComboBox->property("popup").value<QObject*>();
    QVERIFY(penToolRightClickBehaviourComboBoxPopup);
    QTRY_COMPARE(penToolRightClickBehaviourComboBoxPopup->property("opened").toBool(), true);
    QCOMPARE(penToolRightClickBehaviourComboBox->property("currentIndex").toInt(), app.settings()->penToolRightClickBehaviour());
    QCOMPARE(penToolRightClickBehaviourComboBox->property("highlightedIndex").toInt(), app.settings()->penToolRightClickBehaviour());

    // Move down the list until we find the value that we're interested in.
    for (int i = 0; i < penToolRightClickBehaviour; ++i) {
        QTest::keyClick(window, Qt::Key_Down);
        QCOMPARE(penToolRightClickBehaviourComboBox->property("highlightedIndex").toInt(), i + 1);
    }

    // Press space to accept it.
    QTest::keyClick(window, Qt::Key_Space);
    QCOMPARE(penToolRightClickBehaviourComboBox->property("currentIndex").toInt(), penToolRightClickBehaviour);
    // The setting shouldn't change until we hit "OK".
    QCOMPARE(app.settings()->penToolRightClickBehaviour(), app.settings()->defaultPenToolRightClickBehaviour());

    // Accept the dialog to save the changes.
    QVERIFY(QMetaObject::invokeMethod(optionsDialog, "accept"));
    QTRY_VERIFY(!optionsDialog->property("visible").toBool());
    QCOMPARE(app.settings()->penToolRightClickBehaviour(), penToolRightClickBehaviour);

    // For RightClickAppliesBackgroundColour.
    canvas->setPenBackgroundColour(Qt::red);

    setCursorPosInScenePixels(0, 0);

    if (projectType == Project::TilesetType) {
        setCursorPosInScenePixels(0, 0);
        QVERIFY2(drawTileAtCursorPos(), failureMessage);
        QVERIFY2(switchMode(TileCanvas::PixelMode), failureMessage);
    }

    // Right-click and check that the correct tool was used.
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, cursorWindowPos);

    const QImage projectImage = projectType == Project::TilesetType
        ? *tilesetProject->tileAt(cursorPos)->tileset()->image() : *canvas->currentProjectImage();

    switch (penToolRightClickBehaviour) {
    case ImageCanvas::PenToolRightClickAppliesEraser:
        QCOMPARE(projectImage.pixelColor(0, 0), Qt::transparent);
        break;
    case ImageCanvas::PenToolRightClickAppliesEyeDropper:
        QCOMPARE(canvas->penForegroundColour(), projectImage.pixelColor(0, 0));
        break;
    case ImageCanvas::PenToolRightClickAppliesBackgroundColour:
        QCOMPARE(projectImage.pixelColor(0, 0), Qt::red);
        break;
    }
}

void tst_App::splitScreenRendering()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY(canvas->isSplitScreen());

    // Both panes should be visible and have rendered their white backgrounds.
    const QQuickItem *firstPaneItem = findChildItem(layeredImageCanvas, "layeredImageCanvasPaneItem0");
    QVERIFY(firstPaneItem);
    QVERIFY(firstPaneItem->isVisible());

    const QQuickItem *secondPaneItem = findChildItem(layeredImageCanvas, "layeredImageCanvasPaneItem1");
    QVERIFY(secondPaneItem);
    QVERIFY(secondPaneItem->isVisible());

    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage canvasGrab = imageGrabber.takeImage();
    QCOMPARE(canvasGrab.pixelColor(layeredImageCanvas->width() * 0.25, layeredImageCanvas->height() / 2), QColor(Qt::white));
    QCOMPARE(canvasGrab.pixelColor(layeredImageCanvas->width() * 0.75, layeredImageCanvas->height() / 2), QColor(Qt::white));
}

// Distinct from a read-only file, this test checks that the UI prevents images with formats like Format_Indexed8
// from being modified, as QPainter doesn't support it.
void tst_App::formatNotModifiable()
{
    QVERIFY2(setupTempProjectDir(), failureMessage);
    QVERIFY2(copyFileFromResourcesToTempProjectDir("indexed-8-format.png"), failureMessage);

    // Load the image.
    const QUrl projectUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/indexed-8-format.png"));
    QVERIFY2(loadProject(projectUrl), failureMessage);

    auto toolsForbiddenReasonLabel = window->findChild<QQuickItem*>("toolsForbiddenReasonLabel");
    QVERIFY(toolsForbiddenReasonLabel);
    QVERIFY(toolsForbiddenReasonLabel->isVisible());
    QCOMPARE(toolsForbiddenReasonLabel->property("text").toString(),
        "Image cannot be edited because its format is indexed 8-bit, which does not support modification.");
}

// Hint: enable the tests.models and qt.modeltest logging categories when debugging failures here.
void tst_App::models()
{
    auto models = window->findChildren<QAbstractItemModel*>();
    for (const auto model : models) {
        qCDebug(lcModels) << "testing model" << model << model->hasChildren();
        QAbstractItemModelTester modelTester(model);
    }
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
    QCOMPARE(canvas->areRulersVisible(), true);

    QQuickItem *firstHorizontalRuler = findChildItem(canvas, "firstHorizontalRuler");
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
    QVERIFY2(addNewGuide(10, Qt::Horizontal), failureMessage);

    // Undo.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(project->guides().size(), 0);
    QCOMPARE(project->undoStack()->canUndo(), false);

    // Redo.
    QVERIFY2(clickButton(redoToolButton), failureMessage);
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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(project->guides().size(), 1);
    QCOMPARE(project->guides().first().position(), 10);
    QTest::mouseMove(window, cursorWindowPos + QPoint(1, 0));
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::BlankCursor);

    // Redo.
    QVERIFY2(clickButton(redoToolButton), failureMessage);
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

    // Shouldn't be possible to create a guide when Guides Locked is checked.
    canvas->setGuidesLocked(true);

    // Try to drag a guide out.
    setCursorPosInPixels(QPoint(50, rulerThickness / 2));
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(!canvas->pressedRuler());

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(!canvas->pressedRuler());

    setCursorPosInPixels(QPoint(50, rulerThickness + 10));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(!canvas->pressedRuler());
    QCOMPARE(project->guides().size(), 0);

    canvas->setGuidesLocked(false);
}

void tst_App::rulersSplitScreen()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY(canvas->isSplitScreen());
    QVERIFY(!canvas->areRulersVisible());

    // Rulers are not visible by default, so they shouldn't be visible when enabling split-screen.
    const QQuickItem *firstHorizontalRuler = findChildItem(canvas, "firstHorizontalRuler");
    QVERIFY(firstHorizontalRuler);
    QVERIFY(!firstHorizontalRuler->isVisible());

    const QQuickItem *firstVerticalRuler = findChildItem(canvas, "firstVerticalRuler");
    QVERIFY(firstVerticalRuler);
    QVERIFY(!firstVerticalRuler->isVisible());

    const QQuickItem *secondHorizontalRuler = findChildItem(canvas, "secondHorizontalRuler");
    QVERIFY(secondHorizontalRuler);
    QVERIFY(!secondHorizontalRuler->isVisible());

    const QQuickItem *secondVerticalRuler = findChildItem(canvas, "secondVerticalRuler");
    QVERIFY(secondVerticalRuler);
    QVERIFY(!secondVerticalRuler->isVisible());
}

void tst_App::addAndDeleteMultipleGuides()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QVERIFY2(triggerRulersVisible(), failureMessage);
    QCOMPARE(canvas->areRulersVisible(), true);

    QVERIFY2(addNewGuides(63, 65), failureMessage);

    // Try to add the same guides again; it shouldn't add any duplicates.
    QVERIFY2(addNewGuides(63, 65, AddNewGuidesFlag::ExpectAllDuplicates), failureMessage);

    // Remove all guides.
    canvas->removeAllGuides();
    QVector<Guide> guides;
    QCOMPARE(QtUtils::toString(project->guides()), QtUtils::toString(guides));
}

void tst_App::loadDuplicateGuides()
{
    QVERIFY2(setupTempProjectDir(), failureMessage);
    QVERIFY2(copyFileFromResourcesToTempProjectDir("duplicate-guides.slp"), failureMessage);

    QTest::ignoreMessage(QtWarningMsg, "Project contains duplicate guides; they will be removed");

    const QUrl projectUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/duplicate-guides.slp"));
    QVERIFY2(loadProject(projectUrl), failureMessage);
    const QVector<Guide> actualGuides = project->guides();
    const QVector<Guide> expectedUniqueGuides = QtUtils::uniqueValues(actualGuides);
    QVERIFY2(actualGuides == expectedUniqueGuides, "Expected all loaded guides to be unique");
}

void tst_App::notes_data()
{
    addImageProjectTypes();
}

void tst_App::notes()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::NoteTool), failureMessage);

    // Begin creating a new note.
    setCursorPosInScenePixels(11, 12);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTRY_VERIFY(findOpenPopupFromTypeName("NoteDialog"));
    const QObject *noteDialog = findOpenPopupFromTypeName("NoteDialog");
    QTRY_VERIFY(noteDialog->property("opened").toBool());

    // Type in some stuff.
    QQuickItem *noteDialogTextField = noteDialog->findChild<QQuickItem*>("noteDialogTextField");
    QVERIFY(noteDialogTextField);
    QVERIFY(noteDialogTextField->hasActiveFocus());
    QTest::keyClick(window, Qt::Key_Z);

    QQuickItem *noteDialogXTextField = noteDialog->findChild<QQuickItem*>("noteDialogXTextField");
    QVERIFY(noteDialogXTextField);
    QTest::keyClick(window, Qt::Key_Tab);
    QVERIFY(noteDialogXTextField->hasActiveFocus());
    QTest::keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_3);

    QQuickItem *noteDialogYTextField = noteDialog->findChild<QQuickItem*>("noteDialogYTextField");
    QVERIFY(noteDialogYTextField);
    QTest::keyClick(window, Qt::Key_Tab);
    QVERIFY(noteDialogYTextField->hasActiveFocus());
    QTest::keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_4);

    // Cancel the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
    QTRY_VERIFY(!noteDialog->property("visible").toBool());

    // Move the mouse to verify that the position is correct upon opening a creation dialog.
    setCursorPosInScenePixels(13, 14);
    QTest::mouseMove(window, cursorWindowPos);

    // Open it again.
    QVERIFY2(addNewNoteAtCursorPos("test"), failureMessage);

    // Undo the creation of the note.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(project->notes().size(), 0);

    // Redo it.
    QVERIFY2(clickButton(redoToolButton), failureMessage);
    QCOMPARE(project->notes().size(), 1);
    QCOMPARE(project->notes().at(0).position(), QPoint(13, 14));
    QCOMPARE(project->notes().at(0).text(), QLatin1String("test"));
    QVERIFY(!project->notes().at(0).size().isEmpty());

    // Drag the note somewhere else. Make sure to click in the centre to ensure
    // that it respects the press offset.
    QPoint draggedNotePosition;
    {
        const Note note = project->notes().first();
        const QPoint noteSize(note.size().width(), note.size().height());
        const QPoint dragDistance(10, 10);
        QPoint noteCentrePos = note.position() + noteSize / 2;
        setCursorPosInScenePixels(noteCentrePos);
        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QCOMPARE(canvas->pressedNoteIndex(), 0);
        setCursorPosInScenePixels(noteCentrePos + dragDistance);
        QTest::mouseMove(window, cursorWindowPos);
        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QCOMPARE(canvas->pressedNoteIndex(), -1);
        draggedNotePosition = noteCentrePos + dragDistance - noteSize / 2;
        QCOMPARE(project->notes().at(0).position(), draggedNotePosition);
    }

    // Edit the note via the dialog. First, open the context menu.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, cursorWindowPos);
    QTRY_VERIFY(findOpenPopupFromTypeName("NoteContextMenu"));
    const QObject *noteContextMenu = findOpenPopupFromTypeName("NoteContextMenu");
    QTRY_VERIFY(noteContextMenu->property("opened").toBool());

    QQuickItem *editMenuItem = noteContextMenu->findChild<QQuickItem*>("noteContextMenuEditMenuItem");
    QVERIFY(editMenuItem);
    QVERIFY2(clickButton(editMenuItem), failureMessage);
    QTRY_VERIFY(noteDialog->property("opened").toBool());
    QVERIFY(noteDialogTextField->hasActiveFocus());
    QTest::keyClick(window, Qt::Key_1);
    QCOMPARE(noteDialogTextField->property("text").toString(), QLatin1String("test1"));

    QTest::keyClick(window, Qt::Key_Tab);
    QVERIFY(noteDialogXTextField->hasActiveFocus());
    QTest::keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_8);

    QTest::keyClick(window, Qt::Key_Tab);
    QVERIFY(noteDialogYTextField->hasActiveFocus());
    QTest::keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_9);

    // Cancel the dialog.
    QTest::keyClick(window, Qt::Key_Escape);
    QTRY_VERIFY(!noteDialog->property("visible").toBool());

    // Open it again. It shouldn't have the text that was previously entered,
    // but the position of the note that we're editing.
    QTest::mouseClick(window, Qt::RightButton, Qt::NoModifier, cursorWindowPos);
    QTRY_VERIFY(noteContextMenu->property("opened").toBool());
    QVERIFY2(clickButton(editMenuItem), failureMessage);
    QTRY_VERIFY(noteDialog->property("opened").toBool());
    QCOMPARE(noteDialogTextField->property("text").toString(), QLatin1String("test"));
    QCOMPARE(noteDialogXTextField->property("text").toString(), QString::number(draggedNotePosition.x()));
    QCOMPARE(noteDialogYTextField->property("text").toString(), QString::number(draggedNotePosition.y()));

    // Edit it again.
    QVERIFY(noteDialogTextField->hasActiveFocus());
    const QSize oldNoteSize = project->notes().at(0).size();
    QTest::keyClick(window, Qt::Key_0);
    QCOMPARE(noteDialogTextField->property("text").toString(), QLatin1String("test0"));

    QTest::keyClick(window, Qt::Key_Tab);
    QVERIFY(noteDialogXTextField->hasActiveFocus());
    QTest::keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_8);
    QTest::keyClick(window, Qt::Key_1);

    QTest::keyClick(window, Qt::Key_Tab);
    QVERIFY(noteDialogYTextField->hasActiveFocus());
    QTest::keySequence(window, QKeySequence::SelectAll);
    QTest::keyClick(window, Qt::Key_9);
    QTest::keyClick(window, Qt::Key_1);

    // Accept the dialog.
    QTest::keyClick(window, Qt::Key_Return);
    QTRY_VERIFY(!noteDialog->property("visible").toBool());
    QCOMPARE(project->notes().at(0).position(), QPoint(81, 91));
    QCOMPARE(project->notes().at(0).text(), QLatin1String("test0"));
    QVERIFY(!project->notes().at(0).size().isEmpty());
    // The edited text is longer than the old one, so the note box should grow.
    QVERIFY(project->notes().at(0).size().width() > oldNoteSize.width());
}

// While zoomed in, dragging a note without actually moving it
// should not result in the edit dialog opening, only clicks should.
void tst_App::dragNoteWithoutMoving()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::NoteTool), failureMessage);

    // Zoom in so we can see what happens when debugging.
    canvas->setSplitScreen(false);
    canvas->currentPane()->setZoomLevel(5);
    canvas->firstPane()->setOffset(QPointF(686.8, 283.8));

    // Create a new note.
    setCursorPosInScenePixels(0, 0);
    QVERIFY2(addNewNoteAtCursorPos("test"), failureMessage);

    const Note oldNote = project->notes().at(0);
    setCursorPosInScenePixels(1, 1);
    QTest::mouseMove(window, cursorWindowPos);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY2(canvas->pressedNoteIndex() == 0, qPrintable(QString::fromLatin1(
        "Expected note at index %1 to be pressed, but %2 is pressed instead")
            .arg(0).arg(canvas->pressedNoteIndex())));

    cursorWindowPos.rx() += 3;
    QTest::mouseMove(window, cursorWindowPos);

    QSignalSpy noteModificationRequestedSpy(canvas.data(), SIGNAL(noteModificationRequested(int)));
    QVERIFY(noteModificationRequestedSpy.isValid());

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(canvas->pressedNoteIndex() == -1);
    const QPoint expectedPosition = oldNote.position();
    QCOMPARE(project->notes().at(0).position(), expectedPosition);
    QCOMPARE(noteModificationRequestedSpy.count(), 0);
}

void tst_App::saveAndLoadNotes()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY(canvas->areNotesVisible());

    QVERIFY2(switchTool(ImageCanvas::NoteTool), failureMessage);

    // Create some notes.
    setCursorPosInScenePixels(0, 0);
    QVERIFY2(addNewNoteAtCursorPos("note1"), failureMessage);

    setCursorPosInScenePixels(100, 100);
    QVERIFY2(addNewNoteAtCursorPos("note2"), failureMessage);

    setCursorPosInScenePixels(200, 200);
    QVERIFY2(addNewNoteAtCursorPos("note3"), failureMessage);

    // Save a snapshot of the rendered notes to compare against later.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage canvasGrab = imageGrabber.takeImage();

    // Change the value of notesVisible so that we can test that it's
    // saved and loaded in the project's UI state.
    QVERIFY2(clickButton(showNotesToolButton), failureMessage);
    QVERIFY(!canvas->areNotesVisible());
    QVERIFY(!showNotesToolButton->property("checked").toBool());

    // Check that the notes are no longer rendered.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY2(imageGrabber.takeImage() != canvasGrab, "Notes were not rendered as expected after loading");

    // Save the project.
    const QString savedProjectPath = tempProjectDir->path() + "/saveAndLoadNotes-project.slp";
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));

    // Close the project.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!project->hasLoaded());

    // Load the saved file.
    QVERIFY2(loadProject(QUrl::fromLocalFile(savedProjectPath)), failureMessage);
    QCOMPARE(project->notes().at(0).position(), QPoint(0, 0));
    QCOMPARE(project->notes().at(1).position(), QPoint(100, 100));
    QCOMPARE(project->notes().at(2).position(), QPoint(200, 200));
    QVERIFY(!canvas->areNotesVisible());
    QVERIFY(!showNotesToolButton->property("checked").toBool());

    // Check that the notes are still not rendered.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY2(imageGrabber.takeImage() != canvasGrab, "Notes were not rendered as expected after loading");

    // Show notes.
    QVERIFY2(clickButton(showNotesToolButton), failureMessage);
    QVERIFY(canvas->areNotesVisible());
    QVERIFY(showNotesToolButton->property("checked").toBool());

    // Check that the notes are rendered.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY2(imageGrabber.takeImage() == canvasGrab, "Notes were not rendered as expected after showing them");

    // Save it again, but this time with notes visible.
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));

    // Hide notes.
    QVERIFY2(clickButton(showNotesToolButton), failureMessage);
    QVERIFY(!canvas->areNotesVisible());
    QVERIFY(!showNotesToolButton->property("checked").toBool());

    // Load it again.
    QVERIFY2(loadProject(QUrl::fromLocalFile(savedProjectPath)), failureMessage);
    QCOMPARE(project->notes().at(0).position(), QPoint(0, 0));
    QCOMPARE(project->notes().at(1).position(), QPoint(100, 100));
    QCOMPARE(project->notes().at(2).position(), QPoint(200, 200));
    QVERIFY(canvas->areNotesVisible());
    QVERIFY(showNotesToolButton->property("checked").toBool());
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QVERIFY2(imageGrabber.takeImage() == canvasGrab, "Notes were not rendered as expected after loading");
}

void tst_App::autoSwatch_data()
{
    addAllProjectTypes();
}

void tst_App::autoSwatch()
{
    QFETCH(Project::Type, projectType);

    const bool isTilesetProject = projectType == Project::TilesetType;

    QVERIFY2(createNewProject(projectType), failureMessage);
    QVERIFY2(enableAutoSwatch(), failureMessage);

    QQuickItem *autoSwatchGridView = window->findChild<QQuickItem*>("autoSwatchGridView");
    QVERIFY(autoSwatchGridView);

    QQuickItem *viewContentItem = autoSwatchGridView->property("contentItem").value<QQuickItem*>();
    QVERIFY(viewContentItem);

    // Check that the auto swatch has at least one colour at startup.
    int originalAutoSwatchColourCount = 0;
    if (isTilesetProject) {
        // The tileset we're using in the tests has a bunch of colours.
        QTRY_VERIFY(autoSwatchGridView->property("count").toInt() > 1);

        setCursorPosInScenePixels(0, 0);
        QVERIFY2(drawTileAtCursorPos(), failureMessage);
    } else {
        // White background = one white colour in the auto swatch.
        QTRY_COMPARE(autoSwatchGridView->property("count").toInt(), 1);
    }
    originalAutoSwatchColourCount = autoSwatchGridView->property("count").toInt();

    // White background for image projects by default, and the test tileset has a white background too.
    QVERIFY2(swatchViewDelegateExists(viewContentItem, Qt::white), failureMessage);

    // Draw a pixel with a colour that we know we haven't used yet.
    setCursorPosInScenePixels(0, 0);
    canvas->setPenForegroundColour(Qt::cyan);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // There should be one more colour in the swatch.
    QTRY_COMPARE(autoSwatchGridView->property("count").toInt(), originalAutoSwatchColourCount + 1);
    QVERIFY2(swatchViewDelegateExists(viewContentItem, Qt::white), failureMessage);
    QVERIFY2(swatchViewDelegateExists(viewContentItem, Qt::cyan), failureMessage);

    // Undo the drawing; the swatch should revert to its previous contents.
    QVERIFY2(triggerShortcut("undoShortcut", app.settings()->undoShortcut()), failureMessage);
    QTRY_COMPARE(autoSwatchGridView->property("count").toInt(), originalAutoSwatchColourCount);
    QVERIFY2(swatchViewDelegateExists(viewContentItem, Qt::white), failureMessage);
}

// The contentY should not be reset between changes.
void tst_App::autoSwatchGridViewContentY()
{
    QSKIP("Flaky experimental stuff");

    QVERIFY2(createNewImageProject(16, 16, false), failureMessage);

    QVERIFY2(enableAutoSwatch(), failureMessage);

    // Paste the image with lots of colours in.
    QImage colourfulImage(":/resources/test-colourful.png");
    QVERIFY(!colourfulImage.isNull());
    qGuiApp->clipboard()->setImage(colourfulImage);
    QVERIFY2(triggerPaste(), failureMessage);
    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(imageProject->hasUnsavedChanges());

    // Ensure that it can be flicked.
    QQuickItem *autoSwatchGridView = window->findChild<QQuickItem*>("autoSwatchGridView");
    QVERIFY(autoSwatchGridView);
    QTRY_COMPARE(autoSwatchGridView->property("count").toInt(), 255);
    const qreal contentHeight = autoSwatchGridView->property("contentHeight").toReal();
    QVERIFY(contentHeight > autoSwatchGridView->height());
    // "flick" by setting contentY.
    const qreal expectedContentY = contentHeight - autoSwatchGridView->height();
    QVERIFY(expectedContentY > 0);
    QVERIFY(autoSwatchGridView->setProperty("contentY", QVariant(expectedContentY)));

    // Draw a pixel with a colour that doesn't exist in the image.
    // The model should be reset and the view along with it, but
    // we want to make sure that the contentY stays the same to avoid
    // interrupting the user.
    canvas->setPenForegroundColour(Qt::cyan);
    setCursorPosInScenePixels(0, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);
    QQuickItem *viewContentItem = autoSwatchGridView->property("contentItem").value<QQuickItem*>();
    QVERIFY(viewContentItem);
    QTRY_VERIFY2(swatchViewDelegateExists(viewContentItem, Qt::cyan), failureMessage);
    QTRY_COMPARE(autoSwatchGridView->property("contentY").toReal(), expectedContentY);
}

void tst_App::autoSwatchPasteConfirmation()
{
    QVERIFY2(createNewImageProject(16, 16, false), failureMessage);

    QVERIFY2(enableAutoSwatch(), failureMessage);

    // Paste the image with lots of colours in.
    QImage colourfulImage(":/resources/test-colourful.png");
    QVERIFY(!colourfulImage.isNull());
    qGuiApp->clipboard()->setImage(colourfulImage);
    QVERIFY2(triggerPaste(), failureMessage);

    // Switch tool to confirm pasted selection.
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    QVERIFY(imageProject->hasUnsavedChanges());

    // The auto swatch view should eventually be filled.
    QQuickItem *autoSwatchGridView = window->findChild<QQuickItem*>("autoSwatchGridView");
    QVERIFY(autoSwatchGridView);
    QTRY_COMPARE(autoSwatchGridView->property("count").toInt(), 256);
}

void tst_App::swatches()
{
    QVERIFY2(createNewLayeredImageProject(16, 16, false), failureMessage);

    // Not necessary to have the colour panel visible, but helps when debugging.
    QVERIFY2(togglePanel("colourPanel", true), failureMessage);
    QVERIFY2(togglePanel("swatchesPanel", true), failureMessage);

    // Paste an image in.
    QImage colourfulImage(":/resources/test-colourful.png");
    QVERIFY(!colourfulImage.isNull());
    qGuiApp->clipboard()->setImage(colourfulImage);
    QVERIFY2(triggerPaste(), failureMessage);

    // Select some colours from the image, adding a new swatch colour for each one.
    QVERIFY2(switchTool(ImageCanvas::EyeDropperTool), failureMessage);
    for (int x = 0; x < colourfulImage.width(); ++x) {
        setCursorPosInScenePixels(x, 0);
        QVERIFY2(selectColourAtCursorPos(), failureMessage);
        QVERIFY2(addSwatchWithForegroundColour(), failureMessage);
    }

    // Rename one.
    QVERIFY2(renameSwatchColour(0, QLatin1String("test")), failureMessage);

    // Export them. Can't interact with native dialogs here, so we just do it directly.
    QSignalSpy errorSpy(project.data(), SIGNAL(errorOccurred(QString)));
    QVERIFY(errorSpy.isValid());
    const QUrl swatchUrl(QUrl::fromLocalFile(tempProjectDir->path() + "/swatch.json"));
    project->exportSwatch(swatchUrl);
    QVERIFY(errorSpy.isEmpty());

    // Delete them all.
    for (int i = project->swatch()->colours().size() - 1; i >= 0; --i) {
        QVERIFY2(deleteSwatchColour(i), failureMessage);
    }
    QVERIFY(project->swatch()->colours().isEmpty());

    // Import them.
    project->importSwatch(Project::SlateSwatch, swatchUrl);
    QVERIFY(errorSpy.isEmpty());
    QVERIFY(!project->swatch()->colours().isEmpty());
    // Ensure that the user can see them too.
    QVERIFY(findSwatchViewDelegateAtIndex(0));
    QVERIFY(findSwatchViewDelegateAtIndex(project->swatch()->colours().size() - 1));
}

void tst_App::importSwatches_data()
{
    QTest::addColumn<Project::SwatchImportFormat>("swatchImportFormat");
    QTest::addColumn<QString>("swatchFileName");
    QTest::addColumn<QString>("expectedErrorMessage");
    QTest::addColumn<Swatch*>("expectedSwatch");

    Swatch *expectedSwatch = new Swatch(this);
    expectedSwatch->addColour(QString(), QColor::fromRgba(0xAA4800FF));
    QTest::newRow("paint.net-valid")
        << Project::PaintNetSwatch
        << QString::fromLatin1("swatch-paint.net-valid-1.txt")
        << QString()
        << expectedSwatch;

    expectedSwatch = nullptr;
    QTest::newRow("paint.net-invalid")
        << Project::PaintNetSwatch
        << QString::fromLatin1("swatch-paint.net-invalid-1.txt")
        << QString::fromLatin1("Invalid colour ZZFFFFFF at line 3 of paint.net swatch file")
        << expectedSwatch;
}

void tst_App::importSwatches()
{
    QFETCH(Project::SwatchImportFormat, swatchImportFormat);
    QFETCH(QString, swatchFileName);
    QFETCH(QString, expectedErrorMessage);
    QFETCH(Swatch*, expectedSwatch);

    // Don't want a bunch of swatches hanging around unnecessarily,
    // so delete them at the end of this test.
    QScopedPointer<Swatch> swatchGuard(expectedSwatch);

    QVERIFY2(createNewLayeredImageProject(16, 16, false), failureMessage);

    // Not necessary to have the colour panel visible, but helps when debugging.
    QVERIFY2(togglePanel("colourPanel", true), failureMessage);
    QVERIFY2(togglePanel("swatchesPanel", true), failureMessage);

    // Paste an image in.
    QImage colourfulImage(":/resources/test-colourful.png");
    QVERIFY(!colourfulImage.isNull());
    qGuiApp->clipboard()->setImage(colourfulImage);
    QVERIFY2(triggerPaste(), failureMessage);

    // Select a colour from the image, adding a new swatch.
    // We'll use this to verify that:
    // - Failed imports don't modify the existing swatch
    // - Successful imports replace the existing swatch
    QVERIFY2(switchTool(ImageCanvas::EyeDropperTool), failureMessage);
    setCursorPosInScenePixels(0, 0);
    QVERIFY2(selectColourAtCursorPos(), failureMessage);
    QVERIFY2(addSwatchWithForegroundColour(), failureMessage);

    // Copy the files we need to our temporary directory.
    QVERIFY2(copyFileFromResourcesToTempProjectDir(swatchFileName), failureMessage);
    const QString swatchFilePath = tempProjectDir->path() + "/" + swatchFileName;
    project->importSwatch(swatchImportFormat, QUrl::fromLocalFile(swatchFilePath));
    if (expectedErrorMessage.isEmpty()) {
        // Valid swatch.
        QVERIFY2(verifyNoErrorOrDismiss(), failureMessage);
        QVERIFY2(compareSwatches(*project->swatch(), *expectedSwatch), failureMessage);
    } else {
        // Invalid swatch.
        QVERIFY2(verifyErrorAndDismiss(expectedErrorMessage), failureMessage);
        // Since it was invalid, it shouldn't affect the existing swatch.
        QVector<SwatchColour> swatchColours = project->swatch()->colours();
        QCOMPARE(swatchColours.size(), 1);
        QCOMPARE(swatchColours.first().colour(), QColor::fromRgb(0xff0001));
    }
}

struct SelectionData
{
    SelectionData(const QPoint &pressScenePos = QPoint(),
            const QPoint &releaseScenePos = QPoint(),
            const QRect &expectedSelectionArea = QRect(),
            const QPoint &paneZoomCentre = QPoint(-1, -1),
            const int paneZoomLevel = 0) :
        pressScenePos(pressScenePos),
        expectedPressScenePos(pressScenePos),
        releaseScenePos(releaseScenePos),
        expectedSelectionArea(expectedSelectionArea),
        paneZoomCentre(paneZoomCentre),
        paneZoomLevel(paneZoomLevel)
    {
    }

    QString toString() const;

    QPoint pressScenePos;
    // Defaults to pressScenePos, but should be specified when e.g. snapping.
    QPoint expectedPressScenePos;
    QPoint releaseScenePos;
    QRect expectedSelectionArea;
    QPoint paneZoomCentre;
    int paneZoomLevel;
    ImageCanvas::SnapFlags snapFlags;
    QVector<Guide> guides;
};

QDebug operator<<(QDebug debug, const SelectionData &data)
{
    QDebugStateSaver stateSaver(debug);
    debug.nospace();
    debug << "press=" << data.pressScenePos
          << " release=" << data.releaseScenePos
          << " expected area=" << data.expectedSelectionArea
          << " pane zoom centre=" << data.paneZoomCentre
          << " pane zoom level=" << data.paneZoomLevel
          << " snap flags=" << data.snapFlags
          << " guides=" << data.guides;
    return debug;
}

QVector<SelectionData> selectionToolImageCanvasData()
{
    // Non-snapping selection data.
    // SelectionData's params are: press             release         expected sel.    zoom centre     zoom
    QVector<SelectionData> selectionData;
    selectionData << SelectionData(QPoint(-10, -10), QPoint(10, 10), QRect(0, 0, 10, 10));
    selectionData << SelectionData(QPoint(-10, 0), QPoint(10, 10), QRect(0, 0, 10, 10));
    selectionData << SelectionData(QPoint(0, -10), QPoint(10, 10), QRect(0, 0, 10, 10));
    selectionData << SelectionData(QPoint(0, 0), QPoint(256, 256), QRect(0, 0, 256, 256));
    selectionData << SelectionData(QPoint(30, 30), QPoint(0, 0), QRect(0, 0, 30, 30), QPoint(15, 15), 4);
    selectionData << SelectionData(QPoint(256, 256), QPoint(246, 246), QRect(246, 246, 10, 10));

    // Test data involving snapping.
    {
        // Snap press x position to a vertical guide.
        SelectionData dataRow(QPoint(20, 20), QPoint(50, 50), QRect(10, 20, 40, 30));
        dataRow.expectedPressScenePos = QPoint(10, 20);
        dataRow.snapFlags = ImageCanvas::SnapToGuides;
        dataRow.guides = {{ 10, Qt::Vertical }};
        selectionData << dataRow;
    }
    {
        // Snap press y position to a horizontal guide.
        SelectionData dataRow(QPoint(20, 20), QPoint(50, 50), QRect(20, 10, 30, 40));
        dataRow.expectedPressScenePos = QPoint(20, 10);
        dataRow.snapFlags = ImageCanvas::SnapToGuides;
        dataRow.guides = {{ 10, Qt::Horizontal }};
        selectionData << dataRow;
    }
    {
        // Snap press x and y position to a vertical and horizontal guide.
        SelectionData dataRow(QPoint(20, 20), QPoint(50, 50), QRect(10, 10, 40, 40));
        dataRow.expectedPressScenePos = QPoint(10, 10);
        dataRow.snapFlags = ImageCanvas::SnapToGuides;
        dataRow.guides = {{ 10, Qt::Vertical }, { 10, Qt::Horizontal }};
        selectionData << dataRow;
    }
    {
        // Snap release x and y position to a vertical and horizontal guide
        // (select from bottom-right to top-left).
        SelectionData dataRow(QPoint(50, 50), QPoint(20, 20), QRect(10, 10, 40, 40));
        dataRow.expectedPressScenePos = QPoint(50, 50);
        dataRow.snapFlags = ImageCanvas::SnapToGuides;
        dataRow.guides = {{ 10, Qt::Vertical }, { 10, Qt::Horizontal }};
        selectionData << dataRow;
    }
    {
        // Snap press x position to the left edge of the canvas.
        SelectionData dataRow(QPoint(10, 20), QPoint(50, 50), QRect(0, 20, 50, 30));
        dataRow.expectedPressScenePos = QPoint(0, 20);
        dataRow.snapFlags = ImageCanvas::SnapToCanvasEdges;
        selectionData << dataRow;
    }
    {
        // Snap press y position to the top edge of the canvas.
        SelectionData dataRow(QPoint(20, 10), QPoint(50, 50), QRect(20, 0, 30, 50));
        dataRow.expectedPressScenePos = QPoint(20, 0);
        dataRow.snapFlags = ImageCanvas::SnapToCanvasEdges;
        selectionData << dataRow;
    }
    {
        // Snap release x and y position to the bottom-right corner of the canvas.
        // The default canvas size is 256 x 256.
        SelectionData dataRow(QPoint(216, 216), QPoint(246, 246), QRect(216, 216, 40, 40));
        dataRow.snapFlags = ImageCanvas::SnapToCanvasEdges;
        selectionData << dataRow;
    }

    return selectionData;
}

void tst_App::selectionToolImageCanvas()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    // We don't want to use a _data() function for this, because we don't need
    // to create a new project every time and it's much faster this way.
    const QVector<SelectionData> selectionData = selectionToolImageCanvasData();

    // For debugging.
    const bool debug = false;
    const int eventDelay = debug ? 1000 : 0;

    const QPoint defaultPaneZoomCentre = QPoint(project->size().width() / 2, project->size().height() / 2);
    const int defaultPaneZoom = canvas->firstPane()->integerZoomLevel();

    const auto firstPaneDefaultZoom = canvas->firstPane()->zoomLevel();
    const auto firstPaneDefaultOffset = canvas->firstPane()->offset();
    const auto secondPaneDefaultZoom = canvas->secondPane()->zoomLevel();
    const auto secondPaneDefaultOffset = canvas->secondPane()->offset();

    for (const SelectionData &data : std::as_const(selectionData)) {
        auto selectionDataFailureMessage = [&](const QString &failedAction) -> QByteArray {
            return qPrintable(QString::fromLatin1("Failed to %1 for data: %2\n      %3")
                .arg(failedAction, QtUtils::toString(data), failureMessage));
        };

        // Remove, create, and show/hide guides, if necessary.
        canvas->removeAllGuides();
        if (!data.guides.isEmpty()) {
            if (!canvas->areGuidesVisible())
                QVERIFY2(triggerGuidesVisible(), failureMessage);

            for (const auto &guide : std::as_const(data.guides)) {
                QVERIFY2(addNewGuide(guide.position(), guide.orientation()),
                    selectionDataFailureMessage(QString::fromLatin1("add guide %1").arg(QtUtils::toString(guide))));
            }
        } else {
            if (canvas->areGuidesVisible())
                QVERIFY2(triggerGuidesVisible(), failureMessage);
        }

        // Ensure that guides are now locked so we don't accidentally drag them.
        if (!data.guides.isEmpty() && !canvas->areGuidesLocked()) {
            QVERIFY2(clickButton(lockGuidesToolButton), failureMessage);
            QVERIFY(canvas->areGuidesLocked());
        }

        // Enable/disable snapping.
        canvas->setSnapSelectionsTo(data.snapFlags);

        // Zoom/pan.
        const bool customPaneZoomLevel = data.paneZoomLevel != 0;
        if (customPaneZoomLevel) {
            setCursorPosInScenePixels(customPaneZoomLevel ? data.paneZoomCentre : defaultPaneZoomCentre);
            QTest::mouseMove(window, cursorWindowPos, eventDelay);
            if (debug)
                QTest::qWait(eventDelay);

            QVERIFY2(zoomTo(customPaneZoomLevel ? data.paneZoomLevel : defaultPaneZoom), failureMessage);

            if (debug)
                QTest::qWait(eventDelay);
        } else {
            canvas->firstPane()->setOffset(firstPaneDefaultOffset);
            canvas->firstPane()->setZoomLevel(firstPaneDefaultZoom);
            canvas->secondPane()->setOffset(secondPaneDefaultOffset);
            canvas->secondPane()->setZoomLevel(secondPaneDefaultZoom);
        }

        auto selectionAreaFailureMessage = [&](const QRect &expectedArea, const QString &failedAction) -> QByteArray {
            return qPrintable(QString::fromLatin1("Selection area is incorrect after %1 for data %2:" \
                    "\n      Actual area: %3\n    Expected area: %4").arg(
                failedAction,
                QtUtils::toString(selectionData),
                QtUtils::toString(canvas->selectionArea()),
                QtUtils::toString(expectedArea)));
        };

        // Press.
        setCursorPosInScenePixels(data.pressScenePos);
        // Pressing outside the canvas should result in the press position being clamped.
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, eventDelay);
        const QPoint boundExpectedPressPos = QPoint(qBound(0, data.expectedPressScenePos.x(), project->widthInPixels()),
            qBound(0, data.expectedPressScenePos.y(), project->heightInPixels()));
        const QRect expectedPressArea = QRect(boundExpectedPressPos, QSize(0, 0));
        if (debug && canvas->selectionArea() != expectedPressArea) {
            const auto imageGrabPath = QDir().absolutePath() + "/selectionToolImageCanvas-press-"
                + QtUtils::toString(data.expectedSelectionArea) + "-window-grab.png";
            qDebug() << "Saving window's image grab to:\n" << imageGrabPath;
            QVERIFY(window->grabWindow().save(imageGrabPath));
        }
        QVERIFY2(canvas->selectionArea() == expectedPressArea,
            selectionAreaFailureMessage(expectedPressArea, "pressing mouse"));
        // The cursor should remain Qt::BlankCursor until the mouse is released,
        // so that the crosshair is visible.
        QCOMPARE(window->cursor().shape(), Qt::BlankCursor);

        // Drag (move).
        setCursorPosInScenePixels(data.releaseScenePos);
        QTest::mouseMove(window, cursorWindowPos, eventDelay);
        if (debug && canvas->selectionArea() != data.expectedSelectionArea) {
            const auto imageGrabPath = QDir().absolutePath() + "/selectionToolImageCanvas-move-"
                + QtUtils::toString(data.expectedSelectionArea) + "-window-grab.png";
            qDebug() << "Saving window's image grab to:\n" << imageGrabPath;
            QVERIFY(window->grabWindow().save(imageGrabPath));
        }
        QVERIFY2(canvas->selectionArea() == data.expectedSelectionArea,
            selectionAreaFailureMessage(data.expectedSelectionArea, "moving mouse"));
        QCOMPARE(window->cursor().shape(), Qt::BlankCursor);

        // Release.
        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, eventDelay);
        QVERIFY2(canvas->selectionArea() == data.expectedSelectionArea,
            selectionAreaFailureMessage(data.expectedSelectionArea, "releasing mouse"));

        // Cancel the selection so that we can do the next one.
        QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
        QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));
        QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);
    }
}

void tst_App::selectionToolTileCanvas()
{
    QVERIFY2(createNewTilesetProject(), failureMessage);

    // The selection tool currently doesn't work for tileset projects.
    QQuickItem *selectionToolButton = window->findChild<QQuickItem*>("selectionToolButton");
    QVERIFY(selectionToolButton);
    QVERIFY(!selectionToolButton->isVisible());

    QVERIFY2(!switchTool(ImageCanvas::SelectionTool, KeyboardInputType), failureMessage);
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
    QVERIFY2(everyPixelIs(deletedPortion, Qt::transparent), failureMessage);

    // Undo the deletion.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    const QImage undeletedPortion = canvas->currentProjectImage()->copy(0, 0, 10, 10);
    QVERIFY2(everyPixelIs(undeletedPortion, Qt::white), failureMessage);

    // Press delete/backspace without having anything selected;
    // it should remove the contents of the image/layer.
    QTest::keyClick(window, Qt::Key_Delete);
    QVERIFY2(everyPixelIs(*canvas->currentProjectImage(), Qt::transparent), failureMessage);
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
    QTest::keySequence(window, QKeySequence::Copy);
    QCOMPARE(QGuiApplication::clipboard()->image(), canvas->currentProjectImage()->copy(10, 10, 5, 5));

    // Deselect.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->hasSelection(), false);

    // Fill the black rectangle with red so we know that the paste works.
    canvas->setPenForegroundColour(Qt::red);
    QVERIFY2(switchTool(ImageCanvas::FillTool), failureMessage);
    setCursorPosInScenePixels(12, 12);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(10, 10), QColor(Qt::red));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(14, 14), QColor(Qt::red));

    // Paste. The project's image shouldn't change until the paste selection is confirmed.
    QTest::keySequence(window, QKeySequence::Paste);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(10, 10), QColor(Qt::red));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(14, 14), QColor(Qt::red));
    QCOMPARE(canvas->hasSelection(), true);
    QCOMPARE(canvas->selectionArea(), QRect(10, 10, 5, 5));
    // However, the selection preview image should be visible...
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage().pixelColor(12, 12), QColor(Qt::black));

    // Undo the paste while it's still selected.
    //QTest::keySequence(window, app.settings()->undoShortcut());
    //QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::white));
    //QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::white));
    //QCOMPARE(canvas->hasSelection(), false);
    //QCOMPARE(canvas->selectionArea(), QRect(0, 0, 0, 0));

    // Redo the paste. There shouldn't be any selection, but the image should have been applied.
    //QTest::keySequence(window, app.settings()->redoShortcut());
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
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // This test was written when pasting an image would always put it at the
    // top-left of the canvas. The test used to select {0,9 3x3} and copy it,
    // and then paste it. To ensure the integrity of the test, we manually
    // set the clipboard image so that it's still pasted at the top-left.
    QImage image = ImageUtils::filledImage(3, 3);
    image.setPixelColor(1, 1, Qt::black);
    qGuiApp->clipboard()->setImage(image);

    // Paste.
    QTest::keySequence(window, QKeySequence::Paste);
    QCOMPARE(canvas->hasSelection(), true);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 3, 3));

    // Deselect.
    setCursorPosInScenePixels(100, 100);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(!canvas->hasSelection());
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 10), QColor(Qt::black));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(1, 1), QColor(Qt::black));

    // Undo the paste. It should disappear.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
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

    QImage image = ImageUtils::filledImage(32, 32, Qt::blue);
    qGuiApp->clipboard()->setImage(image);

    QTest::keySequence(window, QKeySequence::Paste);
    QCOMPARE(canvas->tool(), ImageCanvas::SelectionTool);
    QCOMPARE(canvas->hasSelection(), true);

    // Confirm the selection.
    setCursorPosInScenePixels(33, 33);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos, 100);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::blue));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(31, 31), QColor(Qt::blue));
}

void tst_App::undoAfterMovedPaste()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    canvas->setSplitScreen(false);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(triggerCentre(), failureMessage);

    // Draw a 1x2 rectangle of red pixels.
    // Use {0, 0} so the test applies to the old paste-at-top-left behaviour
    // in case we need to revert the new behaviour.
    const QPoint rectTopLeft(0, 0);
    QVERIFY2(switchTool(ImageCanvas::PenTool), failureMessage);
    canvas->setPenForegroundColour(Qt::red);
    setCursorPosInScenePixels(rectTopLeft.x(), rectTopLeft.y());
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);
    setCursorPosInScenePixels(rectTopLeft.x(), rectTopLeft.y() + 1);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Select it.
    QVERIFY2(selectArea(QRect(rectTopLeft.x(), rectTopLeft.y(), 1, 2)), failureMessage);

    // Copy it.
    QVERIFY2(triggerCopy(), failureMessage);
    QCOMPARE(QGuiApplication::clipboard()->image(), canvas->currentProjectImage()->copy(rectTopLeft.x(), rectTopLeft.y(), 1, 2));

    // Delete it. This will leave a transparent area where the selection was.
    QTest::keyClick(window, Qt::Key_Delete);
    QVERIFY(!canvas->hasSelection());

    // Paste.
    QVERIFY2(triggerPaste(), failureMessage);

    // Move it down by half its height.
    QTest::keyClick(window, Qt::Key_Down);
    QCOMPARE(canvas->selectionArea(), QRect(rectTopLeft.x(), rectTopLeft.y() + 1, 1, 2));

    // Deselect.
    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(!canvas->hasSelection());

    // Undo. There should be nothing there but transparency.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 1), QColor(Qt::transparent));
}

void tst_App::undoPasteAcrossLayers_data()
{
    QTest::addColumn<QString>("originalProjectFileName");
    QTest::addColumn<QString>("pastedProjectFileName");
    QTest::addColumn<QRect>("copyArea");
    QTest::addColumn<int>("pasteX");
    QTest::addColumn<int>("pasteY");

    QTest::newRow("undoPasteAcrossLayers-1-original.slp")
        << QString::fromLatin1("undoPasteAcrossLayers-1-original.slp")
        << QString::fromLatin1("undoPasteAcrossLayers-1-pasted.slp")
        << QRect(0, 0, 32, 32) << 32 << 0;

    // Same as the above except with one layer hidden.
    QTest::newRow("undoPasteAcrossLayers-2-original.slp")
        << QString::fromLatin1("undoPasteAcrossLayers-2-original.slp")
        << QString::fromLatin1("undoPasteAcrossLayers-2-pasted.slp")
        << QRect(0, 0, 32, 32) << 32 << 0;
}

void tst_App::undoPasteAcrossLayers()
{
    QFETCH(QString, originalProjectFileName);
    QFETCH(QString, pastedProjectFileName);
    QFETCH(QRect, copyArea);
    QFETCH(int, pasteX);
    QFETCH(int, pasteY);

    // Copy the original project to our temp dir, load it, and store the original layer images to compare against later.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);
    QVERIFY2(copyFileFromResourcesToTempProjectDir(originalProjectFileName), failureMessage);
    const QUrl originalProjectUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1Char('/') + originalProjectFileName);
    QVERIFY2(loadProject(originalProjectUrl), failureMessage);
    const QVector<QImage> originalLayerImages = layeredImageProject->layerImages();

    // Select the area to copy.
    QVERIFY2(selectArea(copyArea), failureMessage);

    // Copy it.
    QVERIFY2(copyAcrossLayers(), failureMessage);

    // Paste.
    QVERIFY2(pasteAcrossLayers(pasteX, pasteY, true), failureMessage);
    QVERIFY(!canvas->hasSelection());
    const QVector<QImage> actualPastedLayerImages = layeredImageProject->layerImages();

    // Undo.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    const QVector<QImage> undoneLayerImages = layeredImageProject->layerImages();

    // Close the original project and load the one that contains the expected pasted result.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY2(copyFileFromResourcesToTempProjectDir(pastedProjectFileName), failureMessage);
    const QUrl pastedProjectUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1Char('/') + pastedProjectFileName);
    QVERIFY2(loadProject(pastedProjectUrl), failureMessage);
    const QVector<QImage> expectedPastedLayerImages = layeredImageProject->layerImages();
    QVERIFY2(triggerCloseProject(), failureMessage);

    // Now compare that the results are what we expected.
    QVERIFY2(compareImages(actualPastedLayerImages, expectedPastedLayerImages), failureMessage);
    QVERIFY2(compareImages(undoneLayerImages, originalLayerImages), failureMessage);
}

void tst_App::flipPastedImage()
{
    QVERIFY2(createNewImageProject(), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    QCOMPARE(flipHorizontallyToolButton->isEnabled(), false);
    QCOMPARE(flipVerticallyToolButton->isEnabled(), false);

    QImage image = ImageUtils::filledImage(32, 32);

    QPainter painter(&image);
    painter.fillRect(0, 0, image.width() / 2, 10, QColor(Qt::red));
    qGuiApp->clipboard()->setImage(image);

    QVERIFY2(triggerPaste(), failureMessage);
    QCOMPARE(canvas->tool(), ImageCanvas::SelectionTool);
    QCOMPARE(canvas->hasSelection(), true);
    QCOMPARE(flipHorizontallyToolButton->isEnabled(), true);
    QCOMPARE(flipVerticallyToolButton->isEnabled(), true);

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

void tst_App::flipOnTransparentBackground()
{
    QVERIFY2(createNewImageProject(256, 256, true), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::transparent));

    QVERIFY2(panTopLeftTo(10, 10), failureMessage);

    // Create the flipped image that we expect to see.
    QImage image = ImageUtils::filledImage(project->widthInPixels(), project->heightInPixels());
    image.setPixelColor(0, project->heightInPixels() - 1, Qt::red);

    // Draw a red dot.
    setCursorPosInScenePixels(0, 0);
    canvas->setPenForegroundColour(Qt::red);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Select the image.
//    QVERIFY2(selectArea(QRect(0, 0, canvas->width(), canvas->height())), failureMessage);
    QVERIFY2(triggerSelectAll(), failureMessage);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, project->widthInPixels(), project->heightInPixels()));

    // Flip the image.
    QVERIFY2(triggerFlipVertically(), failureMessage);
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, project->heightInPixels() - 1), QColor(Qt::red));
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

void tst_App::selectionCursorGuide()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QQuickItem *selectionCursorGuideItem = findChildItem(layeredImageCanvas, "selectionCursorGuide");
    QVERIFY(selectionCursorGuideItem);
    QVERIFY(!selectionCursorGuideItem->isVisible());

    // Move the mouse within the canvas so that we can test that
    // the guide is drawn as soon as the tool is switched.
    // we switch to the selection tool.
    setCursorPosInScenePixels(100, 100);
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY(layeredImageCanvas->containsMouse());

    QVERIFY2(switchTool(ImageCanvas::SelectionTool, KeyboardInputType), failureMessage);
    QVERIFY(selectionCursorGuideItem->isVisible());

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    setCursorPosInScenePixels(120, 120);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY(layeredImageCanvas->hasSelection());
    // The guide shouldn't be visible while there's a selection.
    QVERIFY(!selectionCursorGuideItem->isVisible());
}

void tst_App::rotateSelection_data()
{
    addImageProjectTypes();
}

void tst_App::rotateSelection()
{
    QFETCH(Project::Type, projectType);

    QVariantMap args;
    args.insert("imageWidth", QVariant(10));
    args.insert("imageHeight", QVariant(10));
    QVERIFY2(createNewProject(projectType, args), failureMessage);

    QCOMPARE(rotate90CcwToolButton->isEnabled(), false);
    QCOMPARE(rotate90CwToolButton->isEnabled(), false);

    // Paste an "L" onto the canvas.
    const QImage originalImage(":/resources/rotateSelection-original.png");
    qGuiApp->clipboard()->setImage(originalImage);
    QVERIFY2(triggerPaste(), failureMessage);
    QCOMPARE(canvas->hasSelection(), true);
    QCOMPARE(rotate90CcwToolButton->isEnabled(), true);
    QCOMPARE(rotate90CwToolButton->isEnabled(), true);

    // Deselect.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->hasSelection(), false);
    QCOMPARE(rotate90CcwToolButton->isEnabled(), false);
    QCOMPARE(rotate90CwToolButton->isEnabled(), false);

    if (projectType == Project::ImageType)
        QVERIFY2(setupTempProjectDir(), failureMessage);

    // Save and reload so we're on a clean slate.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path()
        + "/" + Project::typeToString(projectType)
        + "." + app.projectManager()->projectExtensionForType(projectType));
    QVERIFY(project->saveAs(saveUrl));
    QVERIFY2(loadProject(saveUrl), failureMessage);
    QCOMPARE(project->hasUnsavedChanges(), false);
    QCOMPARE(canvas->hasModifiedSelection(), false);
    QCOMPARE(project->undoStack()->canUndo(), false);

    // Select and rotate 90 degrees.
    QVERIFY2(selectArea(QRect(3, 2, 4, 5)), failureMessage);
    QCOMPARE(rotate90CcwToolButton->isEnabled(), true);
    QCOMPARE(rotate90CwToolButton->isEnabled(), true);
    // For debugging: zoom to see what's going on.
    // May cause failures if used before selectArea() though.
//    QVERIFY2(zoomTo(30), failureMessage);
    // TODO: go through ui
    canvas->rotateSelection(90);
    QCOMPARE(canvas->hasSelection(), true);
    QCOMPARE(canvas->hasModifiedSelection(), true);
    // See the undo shortcut in Shortcuts.qml for why this is the way it is.
    QCOMPARE(project->undoStack()->canUndo(), false);
    QCOMPARE(undoToolButton->isEnabled(), true);
    // For some reason, layered image project images are Format_ARGB32_Premultiplied.
    // We don't care about the format, so just convert them.
    // Also, we need to use contentImage() since modifications to selections
    // are now combined into one undo command when the selection is confirmed -
    // the actual project's images contents won't change, so we need to get the selection preview image.
    QImage actualImage = canvas->contentImage().convertToFormat(QImage::Format_ARGB32);
    const QImage expected90Image(":/resources/rotateSelection-90.png");
    QCOMPARE(actualImage, expected90Image);

    // Confirm the changes, then try drawing to ensure everything works as expected so far.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->hasSelection(), false);
    QCOMPARE(project->undoStack()->canUndo(), true);
    QCOMPARE(actualImage, expected90Image);
    setCursorPosInScenePixels(QPoint(0, 0));
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);
    actualImage = canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32);
    QImage expected90ImageModified = expected90Image;
    expected90ImageModified.setPixelColor(0, 0, Qt::black);
    QCOMPARE(actualImage, expected90ImageModified);

    // Undo the drawing.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    actualImage = canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32);
    QCOMPARE(actualImage, expected90Image);

    // Undo the first rotation.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    actualImage = canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32);
    QCOMPARE(actualImage, originalImage);

    // Start again by selecting and rotating 90 degrees.
    QVERIFY2(selectArea(QRect(3, 2, 4, 5)), failureMessage);
    QCOMPARE(canvas->hasSelection(), true);
    canvas->rotateSelection(90);
    actualImage = canvas->contentImage().convertToFormat(QImage::Format_ARGB32);
    QCOMPARE(actualImage, expected90Image);

    // Rotate 90 degrees again for a total of 180 degrees of rotation.
    canvas->rotateSelection(90);
    QCOMPARE(canvas->hasSelection(), true);
    actualImage = canvas->contentImage().convertToFormat(QImage::Format_ARGB32);
    const QImage expected180Image(":/resources/rotateSelection-180.png");
    QCOMPARE(actualImage, expected180Image);

    // Rotate 90 degrees again for a total of 270 degrees of rotation.
    canvas->rotateSelection(90);
    QCOMPARE(canvas->hasSelection(), true);
    actualImage = canvas->contentImage().convertToFormat(QImage::Format_ARGB32);
    const QImage expected270Image(":/resources/rotateSelection-270.png");
    QCOMPARE(actualImage, expected270Image);

    // Undoing selection modifications causes the original selection image contents
    // to be restored, regardless of how many modifications have been made since.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    // mspaint gets rid of the selection upon undoing.
    QCOMPARE(canvas->hasSelection(), false);
    actualImage = canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32);
    QCOMPARE(actualImage, originalImage);
}

void tst_App::rotateSelectionAtEdge_data()
{
    addImageProjectTypes();
}

void tst_App::rotateSelectionAtEdge()
{
    QFETCH(Project::Type, projectType);

    QVariantMap args;
    args.insert("imageWidth", QVariant(10));
    args.insert("imageHeight", QVariant(10));
    QVERIFY2(createNewProject(projectType, args), failureMessage);

    // Paste an "L" onto the canvas.
    const QImage originalImage(":/resources/rotateSelectionAtEdge-original.png");
    qGuiApp->clipboard()->setImage(originalImage);
    QVERIFY2(triggerPaste(), failureMessage);
    QCOMPARE(canvas->hasSelection(), true);
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->hasSelection(), false);

    // Select a portion of the image that's at the edge of the canvas and rotate 90 degrees.
    // The selection should be moved so that it's within the boundary of the image.
    QVERIFY2(selectArea(QRect(0, 2, 2, 5)), failureMessage);
    canvas->rotateSelection(90);
    QCOMPARE(canvas->selectionArea(), QRect(0, 4, 5, 2));
    QImage actualImage = canvas->contentImage().convertToFormat(QImage::Format_ARGB32);
    const QImage expected90Image(":/resources/rotateSelectionAtEdge-90.png");
    QCOMPARE(actualImage, expected90Image);

    // Undo the rotation.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    actualImage = canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32);
    QCOMPARE(actualImage, originalImage);
}

void tst_App::rotateSelectionTransparentBackground_data()
{
    addImageProjectTypes();
}

void tst_App::rotateSelectionTransparentBackground()
{
    QFETCH(Project::Type, projectType);

    QVariantMap args;
    args.insert("imageWidth", QVariant(10));
    args.insert("imageHeight", QVariant(10));
    args.insert("transparentImageBackground", QVariant(true));
    QVERIFY2(createNewProject(projectType, args), failureMessage);

    // Paste an "L" onto the canvas.
    const QImage originalImage(":/resources/rotateSelectionTransparentBackground-original.png");
    qGuiApp->clipboard()->setImage(originalImage);
    QVERIFY2(triggerPaste(), failureMessage);
    QCOMPARE(canvas->hasSelection(), true);

    // Confirm it.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->hasSelection(), false);

    // Select the centre of the image and rotate 90 degrees.
    // The selection shouldn't contents shouldn't pick up pixels from outside of it as it rotates.
    QVERIFY2(selectArea(QRect(3, 2, 4, 6)), failureMessage);
    canvas->rotateSelection(90);
    QCOMPARE(canvas->selectionArea(), QRect(2, 3, 6, 4));
    QImage actualImage = canvas->contentImage().convertToFormat(QImage::Format_ARGB32);
    const QImage expected90Image(":/resources/rotateSelectionTransparentBackground-90.png");
    QCOMPARE(actualImage, expected90Image);

    // Rotate 90 degrees again for a total of 180 degrees of rotation.
    canvas->rotateSelection(90);
    actualImage = canvas->contentImage().convertToFormat(QImage::Format_ARGB32);
    const QImage expected180Image(":/resources/rotateSelectionTransparentBackground-180.png");
    QCOMPARE(actualImage.pixelColor(0, 0), expected180Image.pixelColor(0, 0));
    QCOMPARE(actualImage, expected180Image);
}

void tst_App::hueSaturation_data()
{
    QTest::addColumn<Project::Type>("projectType");
    QTest::addColumn<QString>("expectedImagePath");
    QTest::addColumn<QString>("textFieldObjectName");
    QTest::addColumn<bool>("increase");

    QMap<QString, Project::Type> projectTypes;
    projectTypes.insert("ImageType", Project::ImageType);
    projectTypes.insert("LayeredImageType", Project::LayeredImageType);
    foreach (const auto projectTypeString, projectTypes.keys()) {
        const Project::Type projectType = projectTypes.value(projectTypeString);

        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",increasedHue")))
            << projectType << ":/resources/hueSaturation-hue-increased.png" << "hueSaturationDialogHueTextField" << true;
        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",decreasedHue")))
            << projectType << ":/resources/hueSaturation-hue-decreased.png" << "hueSaturationDialogHueTextField" << false;

        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",increasedSaturation")))
            << projectType << ":/resources/hueSaturation-saturation-increased.png" << "hueSaturationDialogSaturationTextField" << true;
        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",decreasedSaturation")))
            << projectType << ":/resources/hueSaturation-saturation-decreased.png" << "hueSaturationDialogSaturationTextField" << false;

        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",increasedLightness")))
            << projectType << ":/resources/hueSaturation-lightness-increased.png" << "hueSaturationDialogLightnessTextField" << true;
        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",decreasedLightness")))
            << projectType << ":/resources/hueSaturation-lightness-decreased.png" << "hueSaturationDialogLightnessTextField" << false;
    }
}

void tst_App::hueSaturation()
{
    QFETCH(Project::Type, projectType);
    QFETCH(QString, expectedImagePath);
    QFETCH(QString, textFieldObjectName);
    QFETCH(bool, increase);

    QVariantMap args;
    args.insert("imageWidth", QVariant(10));
    args.insert("imageHeight", QVariant(10));
    args.insert("transparentImageBackground", QVariant(true));
    QVERIFY2(createNewProject(projectType, args), failureMessage);

    // Zoom in to make visual debugging easier.
    canvas->setSplitScreen(false);
    canvas->currentPane()->setZoomLevel(48);

    // Paste in the original image.
    const QString originalImagePath = QLatin1String(":/resources/hueSaturation-original.png");
    const QImage originalImage(originalImagePath);
    QVERIFY(!originalImage.isNull());
    qGuiApp->clipboard()->setImage(originalImage);
    QVERIFY2(triggerPaste(), failureMessage);
    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(project->hasUnsavedChanges());

    // Select everything.
    QTest::keySequence(window, QKeySequence::SelectAll);

    // Open the dialog manually cause native menus.
    QObject *hueSaturationDialog;
    QVERIFY2(findAndOpenClosedPopupFromObjectName("hueSaturationDialog", &hueSaturationDialog), failureMessage);

    // Increase/decrease the value.
    QQuickItem *hueSaturationDialogHueTextField
        = window->findChild<QQuickItem*>(textFieldObjectName);
    QVERIFY(hueSaturationDialogHueTextField);
    hueSaturationDialogHueTextField->forceActiveFocus();
    QVERIFY(hueSaturationDialogHueTextField->hasActiveFocus());
    // Tried to do this with QTest::keyClick() but I couldn't get it to work:
    // hyphens and backspace (with text selected) did nothing with the default style.
    for (int i = 0; i < 10; ++i)
        QTest::keyClick(window, increase ? Qt::Key_Up : Qt::Key_Down);
    qreal hslValue = hueSaturationDialogHueTextField->property("text").toString().toDouble();
    const qreal expectedHslValue = increase ? 0.10 : -0.10;
    QVERIFY2(qAbs(hslValue - expectedHslValue) < 0.01,
        qPrintable(QString::fromLatin1("Expected HSL value of %1 but got %2").arg(expectedHslValue).arg(hslValue)));

    const QImage expectedImage(expectedImagePath);
    QVERIFY2(!expectedImage.isNull(), qPrintable(QString::fromLatin1(
        "Expected image at %1 could not be loaded").arg(expectedImagePath)));
    // The changes should be rendered...
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), expectedImage);
    // ... but not committed yet.
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage);

    // Cancel the dialog; the changes should not be applied.
    QQuickItem *hueSaturationDialogCancelButton
        = window->findChild<QQuickItem*>("hueSaturationDialogCancelButton");
    QVERIFY(hueSaturationDialogCancelButton);
    QVERIFY2(clickButton(hueSaturationDialogCancelButton), failureMessage);
    QTRY_COMPARE(hueSaturationDialog->property("visible").toBool(), false);
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), originalImage);
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage);

    // Re-open the dialog and increase/decrease the value again.
    QVERIFY(QMetaObject::invokeMethod(hueSaturationDialog, "open"));
    QTRY_COMPARE(hueSaturationDialog->property("opened").toBool(), true);
    // There was an issue where reopening the dialog after changing some values the last
    // time it was opened (even if it was cancelled) would cause the selection contents to disappear.
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), originalImage);
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage);
    hueSaturationDialogHueTextField->forceActiveFocus();
    QVERIFY(hueSaturationDialogHueTextField->hasActiveFocus());
    for (int i = 0; i < 10; ++i)
        QTest::keyClick(window, increase ? Qt::Key_Up : Qt::Key_Down);
    hslValue = hueSaturationDialogHueTextField->property("text").toString().toDouble();
    QVERIFY2(qAbs(hslValue - expectedHslValue) < 0.01,
        qPrintable(QString::fromLatin1("Expected HSL value of %1 but got %2").arg(expectedHslValue).arg(hslValue)));

    // The changes should be rendered...
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), expectedImage);
    // ... but not committed yet.
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage);

    // Accept the dialog; the changes should be applied.
    QQuickItem *hueSaturationDialogOkButton
        = window->findChild<QQuickItem*>("hueSaturationDialogOkButton");
    QVERIFY(hueSaturationDialogOkButton);
    QVERIFY2(clickButton(hueSaturationDialogOkButton), failureMessage);
    QTRY_COMPARE(hueSaturationDialog->property("visible").toBool(), false);
    // Confirm the selection to make the changes to the project's image.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), expectedImage);
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), expectedImage);
}

void tst_App::opacityDialog_data()
{
    QTest::addColumn<Project::Type>("projectType");
    QTest::addColumn<QString>("expectedImagePath");
    QTest::addColumn<QString>("textFieldObjectName");
    QTest::addColumn<bool>("increase");

    QMap<QString, Project::Type> projectTypes;
    projectTypes.insert("ImageType", Project::ImageType);
    // TODO: investigate why layered projects are off one by pixel
    // in the rendered image vs expected image comparison
//    projectTypes.insert("LayeredImageType", Project::LayeredImageType);
    foreach (const auto projectTypeString, projectTypes.keys()) {
        const Project::Type projectType = projectTypes.value(projectTypeString);

        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",increasedAlpha")))
            << projectType << ":/resources/opacityDialog-alpha-increased.png" << "opacityDialogOpacityTextField" << true;
        QTest::newRow(qPrintable(projectTypeString + QLatin1String(",decreasedAlpha")))
            << projectType << ":/resources/opacityDialog-alpha-decreased.png" << "opacityDialogOpacityTextField" << false;
    }
}

void tst_App::opacityDialog()
{
    QFETCH(Project::Type, projectType);
    QFETCH(QString, expectedImagePath);
    QFETCH(QString, textFieldObjectName);
    QFETCH(bool, increase);

    QVariantMap args;
    args.insert("imageWidth", QVariant(10));
    args.insert("imageHeight", QVariant(10));
    args.insert("transparentImageBackground", QVariant(true));
    QVERIFY2(createNewProject(projectType, args), failureMessage);

    // Zoom in to make visual debugging easier.
    canvas->setSplitScreen(false);
    canvas->currentPane()->setZoomLevel(48);

    // Paste in the original image.
    const QString originalImagePath = QLatin1String(":/resources/opacityDialog-original.png");
    const QImage originalImage(originalImagePath);
    QVERIFY(!originalImage.isNull());
    qGuiApp->clipboard()->setImage(originalImage);
    QVERIFY2(triggerPaste(), failureMessage);
    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(project->hasUnsavedChanges());

    // Select everything.
    QTest::keySequence(window, QKeySequence::SelectAll);

    // Open the dialog manually cause native menus.
    QObject *opacityDialog;
    QVERIFY2(findAndOpenClosedPopupFromObjectName("opacityDialog", &opacityDialog), failureMessage);

    // Increase/decrease the value.
    QQuickItem *opacityDialogOpacityTextField
        = window->findChild<QQuickItem*>(textFieldObjectName);
    QVERIFY(opacityDialogOpacityTextField);
    opacityDialogOpacityTextField->forceActiveFocus();
    QVERIFY(opacityDialogOpacityTextField->hasActiveFocus());
    // Tried to do this with QTest::keyClick() but I couldn't get it to work:
    // hyphens and backspace (with text selected) did nothing with the default style.
    // Also can't do it with up/down keys because of floating point precision issues.
    const qreal expectedAlphaValue = increase ? 0.10 : -0.10;
    QVERIFY(opacityDialogOpacityTextField->setProperty("text", QString::number(expectedAlphaValue)));
    qreal alphaValue = opacityDialogOpacityTextField->property("text").toString().toDouble();
    QVERIFY2(qAbs(alphaValue - expectedAlphaValue) < 0.01,
        qPrintable(QString::fromLatin1("Expected alhpa value of %1 but got %2").arg(expectedAlphaValue).arg(alphaValue)));
    // TODO: more hacks until we get input in the test working properly
    QVERIFY(opacityDialog->setProperty("hslAlpha", expectedAlphaValue));
    QVERIFY(QMetaObject::invokeMethod(opacityDialog, "modifySelectionHsl"));

    const QImage expectedImage(expectedImagePath);
    QVERIFY2(!expectedImage.isNull(), qPrintable(QString::fromLatin1(
        "Expected image at %1 could not be loaded").arg(expectedImagePath)));
    // The changes should be rendered...
    QVERIFY2(compareImages(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), expectedImage), failureMessage);
    // ... but not committed yet.
    QVERIFY2(compareImages(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage), failureMessage);

    // Cancel the dialog; the changes should not be applied.
    QQuickItem *opacityDialogCancelButton
        = window->findChild<QQuickItem*>("opacityDialogCancelButton");
    QVERIFY(opacityDialogCancelButton);
    QVERIFY2(clickButton(opacityDialogCancelButton), failureMessage);
    QTRY_COMPARE(opacityDialog->property("visible").toBool(), false);
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), originalImage);
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage);

    // Re-open the dialog and increase/decrease the value again.
    QVERIFY(QMetaObject::invokeMethod(opacityDialog, "open"));
    QTRY_COMPARE(opacityDialog->property("opened").toBool(), true);
    // There was an issue where reopening the dialog after changing some values the last
    // time it was opened (even if it was cancelled) would cause the selection contents to disappear.
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), originalImage);
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage);
    opacityDialogOpacityTextField->forceActiveFocus();
    QVERIFY(opacityDialogOpacityTextField->hasActiveFocus());
    QVERIFY(opacityDialogOpacityTextField->setProperty("text", QString::number(expectedAlphaValue)));
    alphaValue = opacityDialogOpacityTextField->property("text").toString().toDouble();
    QVERIFY2(qAbs(alphaValue - expectedAlphaValue) < 0.01,
        qPrintable(QString::fromLatin1("Expected HSL value of %1 but got %2").arg(expectedAlphaValue).arg(alphaValue)));
    // TODO: more hacks until we get input in the test working properly
    QVERIFY(opacityDialog->setProperty("hslAlpha", expectedAlphaValue));
    QVERIFY(QMetaObject::invokeMethod(opacityDialog, "modifySelectionHsl"));

    // The changes should be rendered...
    QVERIFY2(compareImages(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), expectedImage), failureMessage);
    // ... but not committed yet.
    QVERIFY2(compareImages(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), originalImage), failureMessage);

    // Accept the dialog; the changes should be applied.
    QQuickItem *opacityDialogOkButton
        = window->findChild<QQuickItem*>("opacityDialogOkButton");
    QVERIFY(opacityDialogOkButton);
    QVERIFY2(clickButton(opacityDialogOkButton), failureMessage);
    QTRY_COMPARE(opacityDialog->property("visible").toBool(), false);
    // Confirm the selection to make the changes to the project's image.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(canvas->contentImage().convertToFormat(QImage::Format_ARGB32), expectedImage);
    QCOMPARE(canvas->currentProjectImage()->convertToFormat(QImage::Format_ARGB32), expectedImage);
}

void tst_App::cropToSelection_data()
{
    addImageProjectTypes();
}

void tst_App::cropToSelection()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Nothing selected, so the tool button shouldn't be enabled.
    QVERIFY(!cropToSelectionToolButton->isEnabled());

    const QRect cropRect(40, 40, 10, 10);

    // Draw some pixels as markers to ensure that the crop worked.
    setCursorPosInScenePixels(cropRect.x(), cropRect.y());
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);
    setCursorPosInScenePixels(cropRect.x() + cropRect.width() - 1, cropRect.y() + cropRect.height() - 1);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Select an area.
    QVERIFY2(selectArea(cropRect), failureMessage);
    QVERIFY(cropToSelectionToolButton->isEnabled());

    // Do the cropping.
    QVERIFY2(clickButton(cropToSelectionToolButton), failureMessage);
    QVERIFY(!cropToSelectionToolButton->isEnabled());

    // Ensure the crop worked correctly.
    QVERIFY(imageGrabber.requestImage(canvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grab = imageGrabber.takeImage();
    QCOMPARE(grab.pixelColor(0, 0), QColor(Qt::black));
    QCOMPARE(grab.pixelColor(cropRect.width() - 1, cropRect.width() - 1), QColor(Qt::black));
}

void tst_App::animationPlayback_data()
{
    addImageProjectTypes();
}

void tst_App::animationPlayback()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);
    QCOMPARE(isUsingAnimation(), false);

    QVERIFY2(setAnimationPlayback(true), failureMessage);
    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);
    // Enabling animations automatically creates the first animation for us.
    QCOMPARE(animationSystem->animationCount(), 1);
    QVERIFY(animationSystem->currentAnimation());
    QCOMPARE(animationSystem->currentAnimationIndex(), 0);

    // Open the preview settings popup to modify the scale.
    QQuickItem *animationPanel = window->findChild<QQuickItem*>("animationPanel");
    QVERIFY(animationPanel);
    QQuickItem *animationPanelSettingsToolButton = window->findChild<QQuickItem*>("animationPanelSettingsToolButton");
    QVERIFY(animationPanelSettingsToolButton);
    QVERIFY2(clickButton(animationPanelSettingsToolButton), failureMessage);
    QObject *animationPreviewSettingsPopup = findOpenPopupFromTypeName("AnimationPreviewSettingsPopup");
    QVERIFY(animationPreviewSettingsPopup);
    QTRY_COMPARE(animationPreviewSettingsPopup->property("opened").toBool(), true);

    // Click in the middle of the slider to increase the scale.
    QQuickItem *animationPreviewScaleSlider = window->findChild<QQuickItem*>("animationPreviewScaleSlider");
    QVERIFY(animationPreviewScaleSlider);
    mouseEventOnCentre(animationPreviewScaleSlider, MouseClick);
    AnimationPlayback *currentAnimationPlayback = TestHelper::animationPlayback();
    Animation *currentAnimation = currentAnimationPlayback->animation();
    const qreal modifiedScaleValue = currentAnimationPlayback->scale();
    QVERIFY(modifiedScaleValue > 1.0);

    // Accept and close the preview settings popup.
    QQuickItem *previewSettingsSaveButton = findDialogButtonFromText(animationPreviewSettingsPopup, "OK");
    QVERIFY(previewSettingsSaveButton);
    QVERIFY2(clickButton(previewSettingsSaveButton), failureMessage);
    QTRY_COMPARE(animationPreviewSettingsPopup->property("visible").toBool(), false);

    // Open the animation settings popup for the current animation.
    QQuickItem *animation1Delegate = findListViewChild("animationListView", "Animation 1_Delegate");
    QVERIFY(animation1Delegate);
    QQuickItem *configureAnimationToolButton = animation1Delegate->findChild<QQuickItem*>("Animation 1_DelegateAnimationSettingsToolButton");
    QVERIFY(configureAnimationToolButton);
    QVERIFY2(clickButton(configureAnimationToolButton), failureMessage);
    QObject *animationSettingsPopup = findOpenPopupFromTypeName("AnimationSettingsPopup");
    QVERIFY(animationSettingsPopup);
    QTRY_COMPARE(animationSettingsPopup->property("opened").toBool(), true);

    const int oldFps = animationSystem->currentAnimation()->fps();
    const int expectedFps = oldFps + 1;
    const int oldFrameX = animationSystem->currentAnimation()->frameX();
    const int expectedFrameX = oldFrameX + 1;
    const int oldFrameY = animationSystem->currentAnimation()->frameY();
    const int expectedFrameY = oldFrameY + 1;
    const int oldFrameWidth = animationSystem->currentAnimation()->frameWidth();
    const int expectedFrameWidth = oldFrameWidth + 1;
    const int oldFrameHeight = animationSystem->currentAnimation()->frameHeight();
    const int expectedFrameHeight = oldFrameHeight + 1;
    const int oldFrameCount = animationSystem->currentAnimation()->frameCount();
    const int expectedFrameCount = oldFrameCount + 1;
    const bool expectedReverse = true;

    // Change (increase) the values.
    QVERIFY2(incrementSpinBox("animationFpsSpinBox", oldFps), failureMessage);
    // The changes shouldn't be made to the actual animation until the save button is clicked.
    QCOMPARE(animationSystem->currentAnimation()->fps(), oldFps);
    QCOMPARE(animationSystem->editAnimation()->fps(), expectedFps);
    QVERIFY2(incrementSpinBox("animationFrameXSpinBox", oldFrameX), failureMessage);
    QCOMPARE(animationSystem->currentAnimation()->frameX(), oldFrameX);
    QCOMPARE(animationSystem->editAnimation()->frameX(), expectedFrameX);
    QVERIFY2(incrementSpinBox("animationFrameYSpinBox", oldFrameY), failureMessage);
    QCOMPARE(animationSystem->currentAnimation()->frameY(), oldFrameY);
    QCOMPARE(animationSystem->editAnimation()->frameY(), expectedFrameY);
    QVERIFY2(incrementSpinBox("animationFrameWidthSpinBox", oldFrameWidth), failureMessage);
    QCOMPARE(animationSystem->currentAnimation()->frameWidth(), oldFrameWidth);
    QCOMPARE(animationSystem->editAnimation()->frameWidth(), expectedFrameWidth);
    QVERIFY2(incrementSpinBox("animationFrameHeightSpinBox", oldFrameHeight), failureMessage);
    QCOMPARE(animationSystem->currentAnimation()->frameHeight(), oldFrameHeight);
    QCOMPARE(animationSystem->editAnimation()->frameHeight(), expectedFrameHeight);
    QVERIFY2(incrementSpinBox("animationFrameCountSpinBox", oldFrameCount), failureMessage);
    QCOMPARE(animationSystem->currentAnimation()->frameCount(), oldFrameCount);
    QCOMPARE(animationSystem->editAnimation()->frameCount(), expectedFrameCount);
    QVERIFY2(setCheckBoxChecked("animationReverseCheckBox", true), failureMessage);

    // Accept and close the animation settings popup.
    QQuickItem *animationSettingsSaveButton = findDialogButtonFromText(animationSettingsPopup, "OK");
    QVERIFY(animationSettingsSaveButton);
    QVERIFY2(clickButton(animationSettingsSaveButton), failureMessage);
    QTRY_COMPARE(animationSettingsSaveButton->property("visible").toBool(), false);

    // Play the animation.
    QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
    QCOMPARE(currentAnimationPlayback->isPlaying(), true);
    // It's reversed, so it should start at the end.
    QCOMPARE(currentAnimationPlayback->currentFrameIndex(), 4);

    if (projectType == Project::ImageType)
        return;

    // Let it play a bit.
    QTRY_VERIFY(currentAnimationPlayback->currentFrameIndex() < 4);

    // Save.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/animationStuffSaved.slp"));
    QVERIFY(layeredImageProject->saveAs(saveUrl));
    QVERIFY(!layeredImageProject->hasUnsavedChanges());

    // Close.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!layeredImageProject->hasLoaded());
    QCOMPARE(isUsingAnimation(), false);

    // Load the saved file and check that our custom settings were remembered.
    QVERIFY2(loadProject(saveUrl), failureMessage);
    currentAnimationPlayback = TestHelper::animationPlayback();
    currentAnimation = currentAnimationPlayback->animation();
    QCOMPARE(isUsingAnimation(), true);
    QCOMPARE(currentAnimation->fps(), expectedFps);
    QCOMPARE(currentAnimation->frameX(), expectedFrameX);
    QCOMPARE(currentAnimation->frameY(), expectedFrameY);
    QCOMPARE(currentAnimation->frameWidth(), expectedFrameWidth);
    QCOMPARE(currentAnimation->frameHeight(), expectedFrameHeight);
    QCOMPARE(currentAnimation->frameCount(), expectedFrameCount);
    QCOMPARE(currentAnimation->isReverse(), expectedReverse);
    QCOMPARE(currentAnimationPlayback->scale(), modifiedScaleValue);
}

void tst_App::playNonLoopingAnimationTwice()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QVERIFY2(copyFileFromResourcesToTempProjectDir("simple-colour-animation.slp"), failureMessage);

    const QUrl projectUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/simple-colour-animation.slp"));
    QVERIFY2(loadProject(projectUrl), failureMessage);
    QVERIFY2(togglePanel("animationPanel", true), failureMessage);

    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);

    QCOMPARE(animationSystem->currentAnimationIndex(), 0);
    auto currentPlayback = TestHelper::animationPlayback();
    QVERIFY(!currentPlayback->shouldLoop());

    // Create the expected frames.
    QVector<QImage> expectedFrames;
    for (int i = 0; i < 3; ++i) {
        const QImage frame = layeredImageProject->layerAt(0)->image()->copy(i * 10, 0, 10, 10);
        expectedFrames.append(frame);
    }

    // Play the first animation and grab the frames.
    QVector<QImage> actualFrames;
    QVERIFY2(grabFramesOfCurrentAnimation(actualFrames), failureMessage);
    QCOMPARE(actualFrames.size(), expectedFrames.size());
    for (int i = 0; i < 3; ++i)
        QVERIFY2(everyPixelIs(actualFrames.at(i), expectedFrames.at(i).pixelColor(0, 0)), failureMessage);

    // Play it again. It should play the same each time.
    actualFrames.clear();
    QVERIFY2(grabFramesOfCurrentAnimation(actualFrames), failureMessage);
    QCOMPARE(actualFrames.size(), expectedFrames.size());
    for (int i = 0; i < 3; ++i)
        QVERIFY2(everyPixelIs(actualFrames.at(i), expectedFrames.at(i).pixelColor(0, 0)), failureMessage);
}

void tst_App::animationGifExport()
{
//#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
//    QSKIP("heap-buff-overflow with Qt 6: https://github.com/wernsey/bitmap/issues/8");
//#endif

    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QVERIFY2(copyFileFromResourcesToTempProjectDir("animation.slp"), failureMessage);

    const QUrl projectUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/animation.slp"));
    QVERIFY2(loadProject(projectUrl), failureMessage);
    QCOMPARE(isUsingAnimation(), true);

    // Export the GIF. Can't interact with native dialogs here, so we just do it directly.
    QSignalSpy errorSpy(layeredImageProject.data(), SIGNAL(errorOccurred(QString)));
    QVERIFY(errorSpy.isValid());
    const QUrl exportedGifUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/animation.gif"));
    layeredImageProject->exportGif(exportedGifUrl);
    QVERIFY(errorSpy.isEmpty());
    QVERIFY(QFile::exists(exportedGifUrl.toLocalFile()));

    /*
        Now read the GIF and verify that each frame is correct. I tried keeping
        the expected GIF in Git and comparing the hash of it against the
        generated GIF using QCryptographicHash, but the comparison failed for a
        GIF that should have been equal... so for now we use the old bitmap
        library (which also results in a more useful failure message).
    */
    GIF *gif = gif_load(exportedGifUrl.toLocalFile().toLatin1().constData());
    QVERIFY(gif);
    const int previewScale = 4;
    const int frameCount = 6;
    const int frameWidth = 36;
    const int frameHeight = 38;
    const int scaledFrameWidth = frameWidth * previewScale;
    const int scaledFrameHeight = frameHeight * previewScale;
    QCOMPARE(gif->w, frameWidth * previewScale);
    QCOMPARE(gif->h, frameHeight * previewScale);
    QCOMPARE(gif->n, frameCount);
    // Should be looping.
    QCOMPARE(gif->repetitions, 0);

    auto currentAnimationPlayback = TestHelper::animationPlayback();

    for (int frameIndex = 0; frameIndex < gif->n; ++frameIndex) {
        GIF_FRAME loadedGifFrame = gif->frames[frameIndex];
        QCOMPARE(loadedGifFrame.delay, qFloor(100.0 / currentAnimationPlayback->animation()->fps()));

        Bitmap *gifBitmap = loadedGifFrame.image;
        QCOMPARE(gifBitmap->w, frameWidth * previewScale);
        QCOMPARE(gifBitmap->h, frameHeight * previewScale);

        QImage frameSourceImage = layeredImageProject->exportedImage().copy(
            frameIndex * frameWidth, 0, frameWidth, frameHeight);
        frameSourceImage = frameSourceImage.convertToFormat(QImage::Format_RGBA8888);
        const QImage scaledFrameSourceImage = frameSourceImage.scaled(
            frameSourceImage.size() * currentAnimationPlayback->scale());
        const uchar *scaledFrameSourceImageBits =  scaledFrameSourceImage.bits();
        for (int y = 0; y < scaledFrameHeight; ++y) {
            for (int x = 0; x < scaledFrameWidth; ++x) {
                const int byteIndex = x * y;
                const int actualBlue = gifBitmap->data[byteIndex * 4];
                const int actualGreen = gifBitmap->data[byteIndex * 4 + 1];
                const int actualRed = gifBitmap->data[byteIndex * 4 + 2];
                const int actualAlpha = gifBitmap->data[byteIndex * 4 + 3];
                const int expectedBlue = scaledFrameSourceImageBits[byteIndex * 4 + 2];
                const int expectedGreen = scaledFrameSourceImageBits[byteIndex * 4 + 1];
                const int expectedRed = scaledFrameSourceImageBits[byteIndex * 4];
                const int expectedAlpha = scaledFrameSourceImageBits[byteIndex * 4 + 3];
                const QColor actualColour = QColor(actualRed, actualGreen, actualBlue, actualAlpha);
                const QColor expectedColour = QColor(expectedRed, expectedGreen, expectedBlue, expectedAlpha);
                QVERIFY2(actualColour == expectedColour,
                    qPrintable(QString::fromLatin1("Expected pixel at x=%1 y=%2 of frame %3 to be %4 but it's %5")
                        .arg(x).arg(y).arg(frameIndex).arg(actualColour.name(QColor::HexArgb)).arg(expectedColour.name(QColor::HexArgb))));
            }
        }
    }
}

void tst_App::newAnimations_data()
{
    addImageProjectTypes();
}

void tst_App::newAnimations()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);
    QCOMPARE(isUsingAnimation(), false);

    QVERIFY2(setAnimationPlayback(true), failureMessage);
    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);
    // Enabling animations automatically creates the first animation for us.
    QCOMPARE(animationSystem->animationCount(), 1);
    QVERIFY(animationSystem->currentAnimation());
    QCOMPARE(animationSystem->currentAnimationIndex(), 0);

    QVERIFY2(addNewAnimation("Animation 2", 1), failureMessage);
    QVERIFY2(addNewAnimation("Animation 3", 2), failureMessage);
    QVERIFY2(addNewAnimation("Animation 4", 3), failureMessage);
    QVERIFY2(addNewAnimation("Animation 5", 4), failureMessage);
}

void tst_App::duplicateAnimations_data()
{
    addImageProjectTypes();
}

void tst_App::duplicateAnimations()
{
    QFETCH(Project::Type, projectType);

    QVERIFY2(createNewProject(projectType), failureMessage);
    QCOMPARE(isUsingAnimation(), false);

    QVERIFY2(setAnimationPlayback(true), failureMessage);
    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);
    // Enabling animations automatically creates the first animation for us.
    QCOMPARE(animationSystem->animationCount(), 1);
    QVERIFY(animationSystem->currentAnimation());
    QCOMPARE(animationSystem->currentAnimationIndex(), 0);
    QVERIFY2(duplicateCurrentAnimation("Animation 1 Copy", 1), failureMessage);
    QVERIFY2(duplicateCurrentAnimation("Animation 1 Copy #2", 2), failureMessage);
    QVERIFY2(duplicateCurrentAnimation("Animation 1 Copy #3", 3), failureMessage);
    QVERIFY2(duplicateCurrentAnimation("Animation 1 Copy #4", 4), failureMessage);
}

// Tests that the list of animations is actually saved.
void tst_App::saveAnimations()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QCOMPARE(isUsingAnimation(), false);

    QVERIFY2(setAnimationPlayback(true), failureMessage);

    int expectedFps = 0;
    QUrl saveUrl;

    // Use a scope to emphasise that animationSystem is only valid until the project is closed.
    {
        auto *animationSystem = getAnimationSystem();
        QVERIFY(animationSystem);
        // Enabling animations automatically creates the first animation for us.
        QCOMPARE(animationSystem->animationCount(), 1);
        QVERIFY(animationSystem->currentAnimation());
        QCOMPARE(animationSystem->currentAnimationIndex(), 0);

        QVERIFY2(duplicateCurrentAnimation("Animation 1 Copy", 1), failureMessage);
        QVERIFY2(makeCurrentAnimation("Animation 1 Copy", 1), failureMessage);
        QObject *animationSettingsPopup = nullptr;
        QVERIFY2(openAnimationSettingsPopupForCurrentAnimation(&animationSettingsPopup), failureMessage);

        const int oldFps = animationSystem->currentAnimation()->fps();
        expectedFps = oldFps + 1;

        // Change the FPS.
        QVERIFY2(incrementSpinBox("animationFpsSpinBox", oldFps), failureMessage);
        // The changes shouldn't be made to the actual animation until the save button is clicked.
        QCOMPARE(animationSystem->currentAnimation()->fps(), oldFps);
        QCOMPARE(animationSystem->editAnimation()->fps(), expectedFps);

        // Accept and close the animation settings popup.
        QVERIFY2(clickDialogFooterButton(animationSettingsPopup, "OK"), failureMessage);

        // Save.
        saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + QLatin1String("/saveAnimations.slp"));
        QVERIFY(layeredImageProject->saveAs(saveUrl));
        QVERIFY(!layeredImageProject->hasUnsavedChanges());

        // Close.
        QVERIFY2(triggerCloseProject(), failureMessage);
        QVERIFY(!layeredImageProject->hasLoaded());
        QCOMPARE(isUsingAnimation(), false);
    }

    {
        // Load the saved file and check that our custom settings were remembered.
        QVERIFY2(loadProject(saveUrl), failureMessage);
        QVERIFY_NO_CREATION_ERRORS_OCCURRED();
        auto *animationSystem = getAnimationSystem();
        QVERIFY(animationSystem);
        QCOMPARE(animationSystem->animationCount(), 2);
        QCOMPARE(animationSystem->animationAt(1)->fps(), expectedFps);
    }
}

void tst_App::clickOnCurrentAnimation()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QCOMPARE(isUsingAnimation(), false);

    QVERIFY2(setAnimationPlayback(true), failureMessage);
    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);

    // Play the current animation.
    // TODO: find out why this pause is necessary on macOS when running with: saveAnimations clickOnCurrentAnimation
    QTest::qWait(100);
    QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
    AnimationPlayback *currentAnimationPlayback = TestHelper::animationPlayback();
    QCOMPARE(currentAnimationPlayback->isPlaying(), true);
    QCOMPARE(currentAnimationPlayback->currentFrameIndex(), 0);

    // Duplicate the current animation, and make the duplicate current.
    QVERIFY2(duplicateCurrentAnimation("Animation 1 Copy", 1), failureMessage);
    QVERIFY2(makeCurrentAnimation("Animation 1 Copy", 1), failureMessage);

    for (int i = 0; i < 2; ++i) {
        QTest::qWait(300);

        // Shouldn't crash when clicking on an already-current animation.
        QQuickItem *animationDelegate = nullptr;
        QVERIFY2(verifyAnimationName("Animation 1 Copy", &animationDelegate), failureMessage);
        QVERIFY2(clickButton(animationDelegate), failureMessage);
    }
}

void tst_App::renameAnimation()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QCOMPARE(isUsingAnimation(), false);

    QVERIFY2(setAnimationPlayback(true), failureMessage);
    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);

    auto animationPanelFlickable = window->findChild<QQuickItem*>("animationPanelFlickable");
    QVERIFY(animationPanelFlickable);

    QVERIFY(imageGrabber.requestImage(animationPanelFlickable));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabBeforeRename = imageGrabber.takeImage();

    QVERIFY2(makeCurrentAndRenameAnimation("Animation 1", "test"), failureMessage);

    // The preview should not be affected by a rename.
    QVERIFY(imageGrabber.requestImage(animationPanelFlickable));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabAfterRename = imageGrabber.takeImage();
    QCOMPARE(grabAfterRename, grabBeforeRename);
}

void tst_App::reverseAnimation()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    const QString projectFileName = QLatin1String("animation.slp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Load the project and play the animation.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    const QUrl projectUrl = QUrl::fromLocalFile(absolutePath);
    QVERIFY2(loadProject(projectUrl), failureMessage);
    // updateVariables() calls collapseAllPanels(), so we must manually show this, even if the
    // UI state for the project had it as open.
    QVERIFY2(togglePanel("animationPanel", true), failureMessage);

    QVERIFY2(setAnimationPlayback(true), failureMessage);
    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);

    // Play the current animation.
    QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
    AnimationPlayback *currentAnimationPlayback = TestHelper::animationPlayback();
    QCOMPARE(currentAnimationPlayback->isPlaying(), true);
    QCOMPARE(currentAnimationPlayback->currentFrameIndex(), 0);

    auto previewSpriteImage = window->findChild<QQuickItem*>("animationPreviewContainerSpriteImage");
    QVERIFY(previewSpriteImage);

    QVector<QImage> frames;

    auto currentPlayback = animationSystem->currentAnimationPlayback();
    auto currentAnimation = currentPlayback->animation();
    for (int frameIndex = 0; frameIndex < currentAnimation->frameCount(); ++frameIndex) {
        QTRY_COMPARE_WITH_TIMEOUT(currentPlayback->currentFrameIndex(), frameIndex, 1000);

        // Pause the animation so we can grab the SpriteImage without making the test flaky.
        QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
        QCOMPARE(currentAnimationPlayback->isPlaying(), false);

        QVERIFY(imageGrabber.requestImage(previewSpriteImage));

        if (frameIndex < currentAnimation->frameCount() - 1) {
            // This is not the last frame; resume.
            // We have this check for the same reason we pause; we want to make the tests reliable.
            // If we don't do this, the currentFrameIndex comparison below can fail since
            // the animations are time-based and so is the image grab.
            // Note that it's important that we do this before the QTRY_VERIFY(imageGrabber.isReady()) below,
            // as it is where the actual image grabbing happens.
            QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
            QCOMPARE(currentAnimationPlayback->isPlaying(), true);
        }

        QTRY_VERIFY(imageGrabber.isReady());
        frames.append(imageGrabber.takeImage());
    }

    // Reverse the animation.
    QObject *animationSettingsPopup = nullptr;
    QVERIFY2(openAnimationSettingsPopupForCurrentAnimation(&animationSettingsPopup), failureMessage);
    QVERIFY2(setCheckBoxChecked("animationReverseCheckBox", true), failureMessage);
    QVERIFY2(clickDialogFooterButton(animationSettingsPopup, "OK"), failureMessage);
    // The current index should be updated whenever the animation is reversed.
    QCOMPARE(currentPlayback->currentFrameIndex(), currentAnimation->frameCount() - 1);

    for (int frameIndex = currentAnimation->frameCount() - 1; frameIndex >= 0; --frameIndex) {
        QTRY_COMPARE_WITH_TIMEOUT(currentPlayback->currentFrameIndex(), frameIndex, 1000);

        // The animation is paused on the start of the first loop.
        if (currentAnimationPlayback->isPlaying()) {
            // Pause the animation so we can grab the SpriteImage without making the test flaky.
            QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
        }
        QCOMPARE(currentAnimationPlayback->isPlaying(), false);

        QVERIFY(imageGrabber.requestImage(previewSpriteImage));

        // Resume.
        QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
        QCOMPARE(currentAnimationPlayback->isPlaying(), true);

        QTRY_VERIFY(imageGrabber.isReady());
        QVERIFY2(imageGrabber.takeImage() == frames.at(frameIndex),
            qPrintable(QString::fromLatin1("Frame comparison for frame index %1 failed").arg(frameIndex)));
    }
}

void tst_App::animationFrameWidthTooLarge()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    const QString projectFileName = QLatin1String("animationFrameWidthTooLarge.slp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Load the project - it shouldn't crash.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    const QUrl projectUrl = QUrl::fromLocalFile(absolutePath);
    QVERIFY2(loadProject(projectUrl), failureMessage);
}

void tst_App::animationPreviewUpdated()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    const QString projectFileName = QLatin1String("animation.slp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Load the project.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    const QUrl projectUrl = QUrl::fromLocalFile(absolutePath);
    QVERIFY2(loadProject(projectUrl), failureMessage);

    // Open the animation panel.
    QVERIFY2(togglePanel("animationPanel", true), failureMessage);
    QVERIFY(isUsingAnimation());

    // Grab the preview image before we make changes to it.
    auto previewSpriteImage = window->findChild<QQuickItem*>("animationPreviewContainerSpriteImage");
    QVERIFY(previewSpriteImage);

    QVERIFY(imageGrabber.requestImage(previewSpriteImage));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage oldPreviewGrab = imageGrabber.takeImage();

    // Draw a red dot at {10, 10}. The preview should update immediately.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Grab the preview image after we've made changes to it.
    QVERIFY(imageGrabber.requestImage(previewSpriteImage));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage newPreviewGrab = imageGrabber.takeImage();
    QCOMPARE(newPreviewGrab.pixelColor(10, 10), QColor(Qt::red));
}

void tst_App::seekAnimation()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    const QString projectFileName = QLatin1String("animation.slp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Load the project.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    const QUrl projectUrl = QUrl::fromLocalFile(absolutePath);
    QVERIFY2(loadProject(projectUrl), failureMessage);

    // Open the animation panel.
    QVERIFY2(togglePanel("animationPanel", true), failureMessage);
    QVERIFY(isUsingAnimation());

    // Seek to the second frame.
    auto animationSeekSlider = window->findChild<QQuickItem*>("animationSeekSlider");
    QVERIFY(animationSeekSlider);
    QVERIFY2(moveSliderHandle(animationSeekSlider, 1), failureMessage);
    QCOMPARE(sliderValue(animationSeekSlider), 1);
    auto *animationSystem = getAnimationSystem();
    QVERIFY(animationSystem);
    QCOMPARE(animationSystem->currentAnimationPlayback()->currentFrameIndex(), 1);

    // Click play; the animation should start from the second frame.
    QVERIFY2(clickButton(animationPlayPauseButton), failureMessage);
    QCOMPARE(animationSystem->currentAnimationPlayback()->isPlaying(), true);
    QCOMPARE(animationSystem->currentAnimationPlayback()->currentFrameIndex(), 1);
}

void tst_App::animationFrameMarkers()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    const QString projectFileName = QLatin1String("animation.slp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Load the project.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    const QUrl projectUrl = QUrl::fromLocalFile(absolutePath);
    QVERIFY2(loadProject(projectUrl), failureMessage);

    // Open the animation panel.
    QVERIFY2(togglePanel("animationPanel", true), failureMessage);
    QVERIFY(isUsingAnimation());

    // The animation markers should be visible.
    auto markerRepeater = findChildItem(canvas, "layeredImageCanvasPaneItem0AnimationFrameMarkerRepeater");
    QVERIFY(markerRepeater);
    QVERIFY2(ensureRepeaterChildrenVisible(markerRepeater, 6), failureMessage);

    auto hoveredMarker = findChildItem(canvas, "layeredImageCanvasPaneItem0AnimationFrameMarker0");
    QVERIFY(hoveredMarker);

    // Clicking at the pixel under the hover marker shouldn't result in the
    // frame marker staying hidden after the mouse has moved away from it.
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);
    const QPoint originalMousePosInScene(5, 5);
    setCursorPosInScenePixels(originalMousePosInScene, false);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);
    QTRY_COMPARE(hoveredMarker->opacity(), 0);
    const QPoint targetMousePosInScene(project->size().width() - 1, project->size().height() - 1);
    lerpMouseMove(originalMousePosInScene, targetMousePosInScene);
    QTRY_VERIFY2_WITH_TIMEOUT(ensureRepeaterChildrenVisible(markerRepeater, 6), failureMessage, 500);

    // Undo changes so we don't get the save prompt when we close.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QVERIFY(!project->hasUnsavedChanges());

    // Close the project; the animation markers should no longer be visible.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(markerRepeater);
    QTRY_VERIFY2(ensureRepeaterChildrenVisible(markerRepeater, 0), failureMessage);
}

void tst_App::addAndRemoveLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    ImageLayer *expectedCurrentLayer = layeredImageProject->currentLayer();

    // Draw a blue square at {10, 10}.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Ensure that the blue square is visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBlueDot = imageGrabber.takeImage();
    QCOMPARE(grabWithBlueDot.pixelColor(10, 10), QColor(Qt::blue));

    // Add a new layer.
    QVERIFY2(addNewLayer("Layer 2", 0), failureMessage);

    // Select the new layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot on the new layer.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

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
    QVERIFY2(clickButton(deleteLayerButton), failureMessage);
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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QVERIFY2(verifyLayerName(QLatin1String("Layer 1")), failureMessage);

    // Both dots should be visible again.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), grabWithBothDots);
}

void tst_App::newLayerIndex()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Add a new layer. The order should then be:
    // - Layer 2
    // - Layer 1 (current)
    QVERIFY2(addNewLayer("Layer 2", 0), failureMessage);

    // Add another layer with Layer 1 still being current.
    // This checks that new layers are added above the current one,
    // which we can't verify when there was only one. The order should then be:
    // - Layer 2
    // - Layer 3
    // - Layer 1 (current)
    QVERIFY2(addNewLayer("Layer 3", 1), failureMessage);
}

void tst_App::layerVisibility()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Draw a blue square at {10, 10}.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Ensure that the blue square is visible.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithBlueDot = imageGrabber.takeImage();
    QCOMPARE(grabWithBlueDot.pixelColor(10, 10), QColor(Qt::blue));

    // Add a new layer.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot at the same position.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

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
    QVERIFY2(clickButton(layer2VisibilityCheckBox), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);

    // Ensure that the layer has been hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithRedDotHidden = imageGrabber.takeImage();
    QCOMPARE(grabWithRedDotHidden.pixelColor(10, 10), QColor(Qt::blue));

    // Undo the visibility change.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), true);
    // The canvas should look as it did before it was hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    QCOMPARE(imageGrabber.takeImage(), grabWithRedDot);
}

void tst_App::moveLayerUpAndDown()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QCOMPARE(moveLayerDownButton->isEnabled(), false);
    QCOMPARE(moveLayerUpButton->isEnabled(), false);

    // Add a new layer.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
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
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 3);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    // It should be possible to move the middle layer both up and down.
    QCOMPARE(moveLayerDownButton->isEnabled(), true);
    QCOMPARE(moveLayerUpButton->isEnabled(), true);

    // Move the current layer up.
    QVERIFY2(clickButton(moveLayerUpButton), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 2"));
    QCOMPARE(moveLayerDownButton->isEnabled(), true);
    QCOMPARE(moveLayerUpButton->isEnabled(), false);

    // Move the current layer down.
    QVERIFY2(clickButton(moveLayerDownButton), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));
    QCOMPARE(moveLayerDownButton->isEnabled(), true);
    QCOMPARE(moveLayerUpButton->isEnabled(), true);

    // TODO: draw a different-coloured pixel on each layer and do screen grab comparisons
}

void tst_App::mergeLayerUpAndDown()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QObject *mergeLayerDownMenuItem = window->findChild<QObject*>("mergeLayerDownMenuItem");
    QVERIFY(mergeLayerDownMenuItem);

    QObject *mergeLayerUpMenuItem = window->findChild<QObject*>("mergeLayerUpMenuItem");
    QVERIFY(mergeLayerUpMenuItem);

    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), false);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);

    // Draw something on Layer 1.
    setCursorPosInScenePixels(0, 0);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Add a new layer. The order should then be:
    // - Layer 2
    // - Layer 1 (current)
    QVERIFY2(addNewLayer("Layer 2", 0), failureMessage);
    // It should be possible to merge the lowest layer up but not down.
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), false);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);

    // Make the new layer the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw something on Layer 2.
    setCursorPosInScenePixels(1, 0);
    layeredImageCanvas->setPenForegroundColour(Qt::green);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // It should be possible to merge the highest layer down but not up.
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);

    // Add a new layer. The order should then be:
    // - Layer 3
    // - Layer 2
    // - Layer 1 (current)
    QVERIFY2(addNewLayer("Layer 3", 0), failureMessage);
    // It should be possible to merge the middle layer both up and down.
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);

    // Make the new layer the current layer.
    QVERIFY2(selectLayer("Layer 3", 0), failureMessage);

    // Draw something on Layer 3.
    setCursorPosInScenePixels(2, 0);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Make Layer 2 the current layer:
    // - Layer 3                  (blue)
    // - Layer 2 (current)        (green)
    // - Layer 1                  (red)
    QVERIFY2(selectLayer("Layer 2", 1), failureMessage);

    // Merge the current layer down. We don't have a shortcut for merging, and we don't really need one.
    // After merging, the layers should be:
    // - Layer 3
    // - Layer 1 (current)
    layeredImageProject->mergeCurrentLayerDown();
    QCOMPARE(layeredImageProject->layerCount(), 2);
    // Photoshop uses the lower layer's name, so we'll do that too.
    QVERIFY2(verifyCurrentLayer("Layer 1", 1), failureMessage);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), false);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(1, 0), QColor(Qt::green));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Undo the merge so that we have all three layers again:
    // - Layer 3
    // - Layer 2 (current)
    // - Layer 1
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 3);
    QVERIFY2(verifyCurrentLayer("Layer 2", 1), failureMessage);
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);
    QCOMPARE(layeredImageProject->layerAt(2)->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 3"));
    QCOMPARE(layeredImageProject->layerAt(2)->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(1, 0), QColor(Qt::green));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Merge the current layer up.
    // After merging, the layers should be:
    // - Layer 3 (current)
    // - Layer 1
    layeredImageProject->mergeCurrentLayerUp();
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QVERIFY2(verifyCurrentLayer("Layer 3", 0), failureMessage);
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 3"));
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), false);
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(1, 0), QColor(Qt::green));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(2, 0), QColor(Qt::blue));

    // Undo the merge so that we have all three layers again:
    // - Layer 3
    // - Layer 2 (current)
    // - Layer 1
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 3);
    QVERIFY2(verifyCurrentLayer("Layer 2", 1), failureMessage);
    QCOMPARE(mergeLayerDownMenuItem->property("enabled").toBool(), true);
    QCOMPARE(mergeLayerUpMenuItem->property("enabled").toBool(), true);
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
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

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
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));
}

void tst_App::duplicateLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QQuickItem *duplicateLayerButton = window->findChild<QQuickItem*>("duplicateLayerButton");
    QVERIFY(duplicateLayerButton);

    // Duplicate Layer 1 (all white). It should go above the current layer and be selected.
    QVERIFY2(clickButton(duplicateLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 1 copy"));
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 1"));
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(0, 0), QColor(Qt::white));

    // Undo it.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 1);
    QCOMPARE(layeredImageProject->currentLayer()->name(), QLatin1String("Layer 1"));
}

void tst_App::saveAndLoadLayeredImageProject()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Draw a blue square.
    setCursorPosInScenePixels(10, 10);
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Add a new layer.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabBeforeSaving = imageGrabber.takeImage();
    QCOMPARE(grabBeforeSaving.pixelColor(10, 10), QColor(Qt::blue));
    QCOMPARE(grabBeforeSaving.pixelColor(20, 20), QColor(Qt::red));

    // Add enough layers that the ListView becomes flickable.
    QQuickItem *layerListView = window->findChild<QQuickItem*>("layerListView");
    QVERIFY(layerListView);
    while (layerListView->property("contentHeight").toReal() < layerListView->height() * 3) {
        QVERIFY2(clickButton(newLayerButton), failureMessage);
    }
    const int finalLayerCount = layeredImageProject->layerCount();

    // Scroll to the end.
    const qreal contentHeight = layerListView->property("contentHeight").toReal();
    layerListView->setProperty("contentY", QVariant(contentHeight - layerListView->height()));

    // Select a layer with a non-zero index so that we can check that it's saved.
    QVERIFY2(selectLayer("Layer 1", finalLayerCount - 1), failureMessage);

    // Scroll halfway through the list so that we can check that the position is saved.
    const int contentY = contentHeight / 2;
    layerListView->setProperty("contentY", QVariant(contentY));

    // Save.
    const QUrl saveUrl = QUrl::fromLocalFile(tempProjectDir->path() + "/layeredimageproject.slp");
    QVERIFY(layeredImageProject->saveAs(saveUrl));
    QVERIFY(!layeredImageProject->hasUnsavedChanges());

    // Close.
    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY(!layeredImageProject->hasLoaded());
    QCOMPARE(layeredImageProject->layerCount(), 0);
    // The layer panel shouldn't show any layers.
    QCOMPARE(layerListView->property("count").toInt(), 0);
    QCOMPARE(layerListView->property("contentY").toInt(), 0);

    // Load the saved file using the proper approach, as simply calling
    // layeredImageProject->load(saveUrl) will not trigger the contentY to
    // be set upon loading the project.
    QVERIFY2(loadProject(saveUrl), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);
    // There may be a nicer way of knowing when the ListView is ready?
    do {
        layerListView = window->findChild<QQuickItem*>("layerListView");
        QTest::qWait(1);
    } while (!layerListView || qFuzzyIsNull(layerListView->property("contentHeight").toReal()));
    QCOMPARE(layeredImageProject->layerCount(), finalLayerCount);
    QCOMPARE(layeredImageProject->currentLayerIndex(), finalLayerCount - 1);
    QCOMPARE(layeredImageProject->currentLayer()->name(), "Layer 1");
    QCOMPARE(layerListView->property("count").toInt(), finalLayerCount);
    QCOMPARE(layerListView->property("contentY").toInt(), contentY);

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
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    // Make comparing grabbed image pixels easier.
    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Add a new layer.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);

    // Make it the current layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a red dot.
    setCursorPosInScenePixels(20, 20);
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Ensure that what the user sees is correct.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabBeforeSaving = imageGrabber.takeImage();
    QCOMPARE(grabBeforeSaving.pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(grabBeforeSaving.pixelColor(20, 20), QColor(Qt::red));

    // Move it below Layer 1.
    QVERIFY2(clickButton(moveLayerDownButton), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);
    QCOMPARE(layeredImageProject->layerAt(1)->name(), QLatin1String("Layer 2"));

    // Hide Layer 1. It should show the red square and a transparent background.
    QQuickItem *layer1Delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 1", &layer1Delegate), failureMessage);
    QQuickItem *layer1VisibilityCheckBox = layer1Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer1VisibilityCheckBox);
    QVERIFY2(clickButton(layer1VisibilityCheckBox), failureMessage);
    QCOMPARE(layeredImageProject->layerAt(0)->isVisible(), false);

    // Ensure that the layer has been hidden.
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Hidden = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Hidden.pixelColor(20, 20), QColor(Qt::red));

    // Show Layer 1. It should show only white.
    QVERIFY2(clickButton(layer1VisibilityCheckBox), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), true);
    QVERIFY(imageGrabber.requestImage(layeredImageCanvas));
    QTRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithLayer1Visible = imageGrabber.takeImage();
    QCOMPARE(grabWithLayer1Visible.pixelColor(0, 0), QColor(Qt::white));
    QCOMPARE(grabWithLayer1Visible.pixelColor(20, 20), QColor(Qt::white));

    // Make Layer 1 the current layer.
    QVERIFY2(selectLayer("Layer 1", 0), failureMessage);

    // Move Layer 1 back down. The red square should be visible on a white background.
    QVERIFY2(clickButton(moveLayerDownButton), failureMessage);
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
//    QVERIFY2(clickButton(newLayerButton), failureMessage);
//    QCOMPARE(layeredImageProject->layerCount(), 2);
//    QCOMPARE(layeredImageProject->hasUnsavedChanges(), true);
//    QCOMPARE(undoToolButton->isEnabled(), true);

//    // TODO
//}

// https://github.com/mitchcurtis/slate/issues/6
void tst_App::selectionConfirmedWhenSwitchingLayers()
{
    // Copy an image onto the clipboard.
    const QImage clipboardContents = ImageUtils::filledImage(100, 200, Qt::red);
    qGuiApp->clipboard()->setImage(clipboardContents);

    // Create a new layered image project with the dimensions of the clipboard contents.
    QVERIFY2(createNewLayeredImageProject(100, 200), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Create a new layer and make it the active layer
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QCOMPARE(layeredImageProject->hasUnsavedChanges(), true);
    QCOMPARE(undoToolButton->isEnabled(), true);
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

// Tests #69.
void tst_App::newLayerAfterMovingSelection()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    // Switch to the selection tool.
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    // Select an area.
    QVERIFY2(selectArea(QRect(0, 0, 5, 5)), failureMessage);

    // Drag the selection somewhere else.
    QVERIFY2(dragSelection(QPoint(18, 18)), failureMessage);

    // Add a new layer; it should confirm the move, delesect it, and, most importantly: not crash.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QVERIFY(!canvas->hasSelection());
    QCOMPARE(canvas->currentProjectImage()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(4, 4), QColor(Qt::transparent));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(18, 18), QColor(Qt::white));
    QCOMPARE(canvas->currentProjectImage()->pixelColor(22, 22), QColor(Qt::white));
}

void tst_App::undoAfterMovingTwoSelections()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    QVERIFY2(panTopLeftTo(0, 0), failureMessage);

    // Create two new layers. The order should then be:
    // - Layer 2
    // - Layer 3
    // - Layer 1 (current)
    QVERIFY2(addNewLayer("Layer 2", 0), failureMessage);
    QVERIFY2(addNewLayer("Layer 3", 1), failureMessage);

    // Select Layer 2, draw a semi-transparent grey dot on it.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);
    const QColor semiTransparentGrey(QColor::fromRgba(0xAA000000));
    layeredImageCanvas->setPenForegroundColour(semiTransparentGrey);
    setCursorPosInScenePixels(10, 10);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Select Layer 3 and draw a red dot on it.
    QVERIFY2(selectLayer("Layer 3", 1), failureMessage);
    const QColor red(Qt::red);
    layeredImageCanvas->setPenForegroundColour(red);
    setCursorPosInScenePixels(10, 20);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    const QImage expectedImage = layeredImageProject->exportedImage();

    // Select the grey dot and move it down.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);
    QVERIFY2(selectArea(QRect(10, 10, 1, 1)), failureMessage);
    QVERIFY2(dragSelection(QPoint(10, 14)), failureMessage);
    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(!layeredImageCanvas->hasSelection());
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 10), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 14), semiTransparentGrey);

    // Change layer, select the red dot and move it down.
    QVERIFY2(selectLayer("Layer 3", 1), failureMessage);
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);
    QVERIFY2(selectArea(QRect(10, 20, 1, 1)), failureMessage);
    QVERIFY2(dragSelection(QPoint(10, 24)), failureMessage);
    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY(!layeredImageCanvas->hasSelection());
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 20), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(10, 24), red);

    // Undo both moves; the grey shouldn't be duplicated.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(10, 10), semiTransparentGrey);
    QCOMPARE(layeredImageProject->layerAt(0)->image()->pixelColor(10, 14), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(10, 20), red);
    QCOMPARE(layeredImageProject->layerAt(1)->image()->pixelColor(10, 24), QColor(Qt::transparent));
    // The checks above should catch everything we're interested in, but just to be thorough...
    const QImage actualImage = layeredImageProject->exportedImage();
    QCOMPARE(actualImage, expectedImage);
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
    QVERIFY(layeredImageProject->saveAs(QUrl::fromLocalFile(savedProjectPath)));

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
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Save again.
    QVERIFY(layeredImageProject->saveAs(QUrl::fromLocalFile(savedProjectPath)));

    // No export should have happened and so the exported image shouldn't have changed.
    exportedImage = QImage(autoExportFilePath);
    QCOMPARE(exportedImage, expectedExportedImage);
}

void tst_App::exportFileNamedLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    layeredImageProject->setAutoExportEnabled(true);

    // Draw a red dot.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    setCursorPosInScenePixels(0, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Add some new layers.
    // Create two new layers. The order should then be:
    // - Layer 2
    // - Layer 3
    // - Layer 1 (current)
    QVERIFY2(addNewLayer("Layer 2", 0), failureMessage);
    QVERIFY2(addNewLayer("Layer 3", 1), failureMessage);
    ImageLayer *layer2 = layeredImageProject->layerAt(0);
    ImageLayer *layer3 = layeredImageProject->layerAt(1);
    ImageLayer *layer1 = layeredImageProject->layerAt(2);

    // Select Layer 3.
    QVERIFY2(selectLayer("Layer 3", 1), failureMessage);

    // Draw a green dot on layer 3.
    layeredImageCanvas->setPenForegroundColour(Qt::green);
    setCursorPosInScenePixels(1, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Select Layer 2.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a blue dot on layer 2.
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    setCursorPosInScenePixels(2, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Give layer 3 a name so that it's saved as a PNG.
    QVERIFY2(makeCurrentAndRenameLayer("Layer 3", "[test] Layer 3"), failureMessage);

    // Save the project so that auto export is triggered and the images saved.
    const QString savedProjectPath = tempProjectDir->path() + "/exportFileNameLayers-project.slp";
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));

    // Check that the "main" image (combined from regular layers) is saved.
    const QString exportedImagePath = tempProjectDir->path() + "/exportFileNameLayers-project.png";
    QVERIFY(QFile::exists(exportedImagePath));
    QImage exportedImage(exportedImagePath);
    // TODO: formats are wrong when comparing whole images, but I don't know why.
    QCOMPARE(exportedImage.pixelColor(0, 0), layer1->image()->pixelColor(0, 0));
    QCOMPARE(exportedImage.pixelColor(2, 0), layer2->image()->pixelColor(2, 0));

    // Check that the one file-named layer was saved as a separate image.
    const QString exportedTestLayerImagePath = tempProjectDir->path() + "/test.png";
    QVERIFY(QFile::exists(exportedTestLayerImagePath));
    QImage exportedLayerImage(exportedTestLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(1, 0), layer3->image()->pixelColor(1, 0));

    // Remove the image so that we can re-test it being exported.
    QVERIFY(QFile::remove(exportedTestLayerImagePath));
    QVERIFY(!QFile::exists(exportedTestLayerImagePath));

    // Hide that file-named layer; it should still be exported.
    QVERIFY2(changeLayerVisiblity("[test] Layer 3", false), failureMessage);

    // Save to export.
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));
    QVERIFY(QFile::exists(exportedTestLayerImagePath));
    exportedLayerImage = QImage(exportedTestLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(1, 0), layer3->image()->pixelColor(1, 0));

    // Add a new layer. The order should now be:
    // - Layer 2
    // - Layer 4
    // - [test] Layer 3 (current)
    // - Layer 1
    QVERIFY2(addNewLayer("Layer 4", 1), failureMessage);
    ImageLayer *layer4 = layeredImageProject->layerAt(1);

    // Rename it so it's in the same group as "test".
    QVERIFY2(makeCurrentAndRenameLayer("Layer 4", "[test] Layer 4"), failureMessage);

    // Draw a dot on it.
    layeredImageCanvas->setPenForegroundColour(Qt::darkMagenta);
    setCursorPosInScenePixels(3, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Export again. Both layers should have been exported to the same image.
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));
    QVERIFY(QFile::exists(exportedTestLayerImagePath));
    exportedLayerImage = QImage(exportedTestLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(1, 0), layer3->image()->pixelColor(1, 0));
    QCOMPARE(exportedLayerImage.pixelColor(3, 0), layer4->image()->pixelColor(3, 0));

    // Add another new layer. The order should now be:
    // - Layer 2
    // - Layer 5
    // - [test] Layer 4 (current)
    // - [test] Layer 3
    // - Layer 1
    QVERIFY2(addNewLayer("Layer 5", 1), failureMessage);
    ImageLayer *layer5 = layeredImageProject->layerAt(1);

    // Rename it so that it uses the project as a prefix.
    QVERIFY2(makeCurrentAndRenameLayer("Layer 5", "[%p-blah] Layer 5"), failureMessage);

    // Now we have the following layers (x = hidden):
    // x Layer 2
    // - [%p-blah] Layer 5 (current)
    // - [test] Layer 4
    // - [test] Layer 3
    // - Layer 1

    // Draw a dot on it as usual, so that we can verify it exports correctly.
    layeredImageCanvas->setPenForegroundColour(Qt::darkBlue);
    setCursorPosInScenePixels(4, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Export it.
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));
    const QString exportedBlahLayerImagePath = tempProjectDir->path() + "/exportFileNameLayers-project-blah.png";
    QVERIFY(QFile::exists(exportedBlahLayerImagePath));
    exportedLayerImage = QImage(exportedBlahLayerImagePath);
    QVERIFY(!exportedLayerImage.isNull());
    QCOMPARE(exportedLayerImage.pixelColor(4, 0), layer5->image()->pixelColor(4, 0));

    // Add [no-export] to each layer without a prefix so that they're not exported.
    QVERIFY2(makeCurrentAndRenameLayer("Layer 1", "[no-export] Layer 1"), failureMessage);
    QVERIFY2(makeCurrentAndRenameLayer("Layer 2", "[no-export] Layer 2"), failureMessage);

    // Now we have the following layers:
    // x [no-export] Layer 2
    // - [%p-blah] Layer 5
    // - [test] Layer 4
    // - [test] Layer 3
    // - [no-export] Layer 1

    // Remove the image so that we can re-test it being exported.
    QVERIFY(QFile::remove(exportedImagePath));
    QVERIFY(!QFile::exists(exportedImagePath));

    // Now it shouldn't be exported.
    QVERIFY(project->saveAs(QUrl::fromLocalFile(savedProjectPath)));
    QVERIFY(!QFile::exists(exportedImagePath));
}

void tst_App::disableToolsWhenLayerHidden()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    // The cursor should be normal.
    setCursorPosInScenePixels(0, 0);
    QTest::mouseMove(window, cursorWindowPos);
    QCOMPARE(window->cursor().shape(), Qt::BlankCursor);

    QQuickItem *layer1Delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 1", &layer1Delegate), failureMessage);
    QQuickItem *layer1VisibilityCheckBox = layer1Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer1VisibilityCheckBox);

    foreach (ImageCanvas::Tool tool, mTools) {
        // Hide the layer.
        QVERIFY2(clickButton(layer1VisibilityCheckBox), failureMessage);
        QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);

        // Switch tool.
        QVERIFY2(switchTool(tool), failureMessage);

        // Move onto the canvas. The cursor should be disabled for each tool.
        QTest::mouseMove(window, cursorWindowPos);

        QVERIFY2(window->cursor().shape() == Qt::ForbiddenCursor,
            qPrintable(QString::fromLatin1("Expected Qt::ForbiddenCursor for tool %1, but got %2")
                .arg(QtUtils::toString(tool)).arg(QtUtils::toString(window->cursor().shape()))));

        // Make the layer visible again.
        QVERIFY2(clickButton(layer1VisibilityCheckBox), failureMessage);
        QCOMPARE(layeredImageProject->currentLayer()->isVisible(), true);

        // The cursor should not be ForbiddenCursor now.
        QVERIFY(window->cursor().shape() != Qt::ForbiddenCursor);
    }

    // Hide the layer.
    QVERIFY2(clickButton(layer1VisibilityCheckBox), failureMessage);
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
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Move the contents down.
    QVERIFY2(moveContents(1, 3, false), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 2), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(1, 2), QColor(Qt::transparent));
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(1, 3), QColor(Qt::red));

    // Undo.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->image()->pixelColor(0, 0), QColor(Qt::red));
}

void tst_App::undoMoveContentsOfVisibleLayers()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    // Draw a red dot.
    layeredImageCanvas->setPenForegroundColour(Qt::red);
    setCursorPosInScenePixels(0, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Add a new layer.
    QVERIFY2(clickButton(newLayerButton), failureMessage);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    ImageLayer *layer1 = layeredImageProject->layerAt(1);
    ImageLayer *layer2 = layeredImageProject->layerAt(0);

    // Select the new layer.
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    // Draw a blue dot on layer 2.
    layeredImageCanvas->setPenForegroundColour(Qt::blue);
    setCursorPosInScenePixels(1, 0);
    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    // Hide layer 2.
    QQuickItem *layer2Delegate = nullptr;
    QVERIFY2(verifyLayerName("Layer 2", &layer2Delegate), failureMessage);
    QQuickItem *layer2VisibilityCheckBox = layer2Delegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY(layer2VisibilityCheckBox);
    QVERIFY2(clickButton(layer2VisibilityCheckBox), failureMessage);
    QCOMPARE(layeredImageProject->currentLayer()->isVisible(), false);

    // Move the contents down. Only layer 1 should have been moved.
    QVERIFY2(moveContents(0, 1, true), failureMessage);
    QCOMPARE(layer1->image()->pixelColor(0, 0), QColor(Qt::transparent));
    QCOMPARE(layer1->image()->pixelColor(0, 1), QColor(Qt::red));
    QCOMPARE(layer2->image()->pixelColor(1, 0), QColor(Qt::blue));

    // Undo.
    QVERIFY2(clickButton(undoToolButton), failureMessage);
    QCOMPARE(layer1->image()->pixelColor(0, 0), QColor(Qt::red));
    QCOMPARE(layer1->image()->pixelColor(0, 1), QColor(Qt::white));
    QCOMPARE(layer2->image()->pixelColor(1, 0), QColor(Qt::blue));
}

void tst_App::selectNextLayer()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);
    QVERIFY2(togglePanel("layerPanel", true), failureMessage);
    QVERIFY2(addNewLayer("Layer 2", 0), failureMessage);
    QVERIFY2(addNewLayer("Layer 3", 1), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 2);

    // Already at the bottom; should do nothing.
    QVERIFY2(!triggerSelectNextLayerDown(), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 2);

    QVERIFY2(triggerSelectNextLayerUp(), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 1);

    QVERIFY2(triggerSelectNextLayerUp(), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);

    // Already at the top; should do nothing.
    QVERIFY2(!triggerSelectNextLayerUp(), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);

    // Test that you can't use the shortcut while doing stuff with the mouse,
    // or while a selection is active.
    QVERIFY2(switchTool(ImageCanvas::SelectionTool), failureMessage);

    setCursorPosInScenePixels(QPoint(0, 0));
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY2(!triggerSelectNextLayerDown(), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);

    setCursorPosInScenePixels(QPoint(5, 5));
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY2(!triggerSelectNextLayerDown(), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->selectionArea(), QRect(0, 0, 5, 5));
    QVERIFY2(!triggerSelectNextLayerDown(), failureMessage);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
}

int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");
    tst_App test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "tst_app.moc"
