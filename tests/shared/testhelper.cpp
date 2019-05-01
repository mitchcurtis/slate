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

#include "testhelper.h"

#include <QPainter>

#include "imagelayer.h"
#include "projectmanager.h"
#include "utils.h"

TestHelper::TestHelper(int &argc, char **argv) :
    app(argc, argv, QStringLiteral("Slate Test Suite")),
    window(qobject_cast<QQuickWindow*>(app.qmlEngine()->rootObjects().first())),
    overlay(nullptr),
    projectManager(nullptr),
    imageProject(nullptr),
    tilesetProject(nullptr),
    layeredImageProject(nullptr),
    imageCanvas(nullptr),
    tileCanvas(nullptr),
    canvasSizeButton(nullptr),
    imageSizeButton(nullptr),
    modeToolButton(nullptr),
    penToolButton(nullptr),
    eyeDropperToolButton(nullptr),
    fillToolButton(nullptr),
    eraserToolButton(nullptr),
    selectionToolButton(nullptr),
    toolSizeButton(nullptr),
    rotate90CcwToolButton(nullptr),
    rotate90CwToolButton(nullptr),
    flipHorizontallyToolButton(nullptr),
    flipVerticallyToolButton(nullptr),
    undoButton(nullptr),
    redoButton(nullptr),
    penForegroundColourButton(nullptr),
    penBackgroundColourButton(nullptr),
    tilesetSwatchPanel(nullptr),
    tilesetSwatchFlickable(nullptr),
    newLayerButton(nullptr),
    moveLayerDownButton(nullptr),
    moveLayerUpButton(nullptr),
    animationPlayPauseButton(nullptr),
    tilesetBasename("test-tileset.png")
{
    mTools.append(ImageCanvas::PenTool);
    mTools.append(ImageCanvas::EyeDropperTool);
    mTools.append(ImageCanvas::EraserTool);
    mTools.append(ImageCanvas::FillTool);
    mTools.append(ImageCanvas::SelectionTool);/*,
    mTools.append(ImageCanvas::CropTool); TODO: not implemented yet*/

    allProjectTypes << Project::TilesetType << Project::ImageType << Project::LayeredImageType;
    allRightClickBehaviours << ImageCanvas::PenToolRightClickAppliesEraser << ImageCanvas::PenToolRightClickAppliesEyeDropper
        << ImageCanvas::PenToolRightClickAppliesBackgroundColour;
}

TestHelper::~TestHelper()
{
}

void TestHelper::initTestCase()
{
    // Tests could have failed on the last run, so just enforce the default settings.
    app.settings()->setGridVisible(app.settings()->defaultGridVisible());
    QVERIFY(app.settings()->isGridVisible());
    app.settings()->setRulersVisible(false);
    QVERIFY(!app.settings()->areRulersVisible());
    app.settings()->setGuidesLocked(false);
    QVERIFY(!app.settings()->areGuidesLocked());

    // However, this should never change.
    QVERIFY(!app.settings()->loadLastOnStartup());

    QVERIFY(window->property("overlay").isValid());
    overlay = window->property("overlay").value<QQuickItem*>();
    QVERIFY(overlay);

    projectManager = app.projectManager();

    // Whenever the project manager's project changes, it means we should
    // reset our errorOccurred spy, as createNewProject() is about to be called on it.
    connect(projectManager.data(), &ProjectManager::temporaryProjectChanged, this, &TestHelper::resetCreationErrorSpy);
    // The project would have already been created by now, as a layered image project is always
    // loaded by default, so we have to manually create the spy now.
    resetCreationErrorSpy();

    layeredImageCanvas = window->findChild<LayeredImageCanvas*>("layeredImageCanvas");
    QVERIFY(layeredImageCanvas);

    project = projectManager->project();
    QVERIFY(project);

    layeredImageProject = qobject_cast<LayeredImageProject*>(project.data());
    QVERIFY(layeredImageProject);

    canvasSizeButton = window->findChild<QQuickItem*>("canvasSizeButton");
    QVERIFY(canvasSizeButton);

    imageSizeButton = window->findChild<QQuickItem*>("imageSizeButton");
    QVERIFY(imageSizeButton);

    modeToolButton = window->findChild<QQuickItem*>("modeToolButton");
    QVERIFY(modeToolButton);

    penToolButton = window->findChild<QQuickItem*>("penToolButton");
    QVERIFY(penToolButton);

    eyeDropperToolButton = window->findChild<QQuickItem*>("eyeDropperToolButton");
    QVERIFY(eyeDropperToolButton);

    fillToolButton = window->findChild<QQuickItem*>("fillToolButton");
    QVERIFY(fillToolButton);

    eraserToolButton = window->findChild<QQuickItem*>("eraserToolButton");
    QVERIFY(eraserToolButton);

    selectionToolButton = window->findChild<QQuickItem*>("selectionToolButton");
    QVERIFY(selectionToolButton);

    toolSizeButton = window->findChild<QQuickItem*>("toolSizeButton");
    QVERIFY(toolSizeButton);

    toolShapeButton = window->findChild<QQuickItem*>("toolShapeButton");
    QVERIFY(toolShapeButton);

    rotate90CcwToolButton = window->findChild<QQuickItem*>("rotate90CcwToolButton");
    QVERIFY(rotate90CcwToolButton);

    rotate90CwToolButton = window->findChild<QQuickItem*>("rotate90CwToolButton");
    QVERIFY(rotate90CwToolButton);

    flipHorizontallyToolButton = window->findChild<QQuickItem*>("flipHorizontallyToolButton");
    QVERIFY(flipHorizontallyToolButton);

    flipVerticallyToolButton = window->findChild<QQuickItem*>("flipVerticallyToolButton");
    QVERIFY(flipVerticallyToolButton);

    undoButton = window->findChild<QQuickItem*>("undoButton");
    QVERIFY(undoButton);

    redoButton = window->findChild<QQuickItem*>("redoButton");
    QVERIFY(redoButton);

    splitScreenToolButton = window->findChild<QQuickItem*>("splitScreenToolButton");
    QVERIFY(splitScreenToolButton);

    lockSplitterToolButton = window->findChild<QQuickItem*>("lockSplitterToolButton");
    QVERIFY(lockSplitterToolButton);

    penForegroundColourButton = window->findChild<QQuickItem*>("penForegroundColourButton");
    QVERIFY(penForegroundColourButton);

    penBackgroundColourButton = window->findChild<QQuickItem*>("penBackgroundColourButton");
    QVERIFY(penBackgroundColourButton);

    lighterButton = window->findChild<QQuickItem*>("lighterButton");
    QVERIFY(lighterButton);

    darkerButton = window->findChild<QQuickItem*>("darkerButton");
    QVERIFY(darkerButton);

    saturateButton = window->findChild<QQuickItem*>("saturateButton");
    QVERIFY(saturateButton);

    desaturateButton = window->findChild<QQuickItem*>("desaturateButton");
    QVERIFY(desaturateButton);

    // This is not in a Loader, and hence it is always available.
    swatchesPanel = window->findChild<QQuickItem*>("swatchesPanel");
    QVERIFY(swatchesPanel);

    QPixmap checkerPixmap = QPixmap(":/images/checker.png");
    QCOMPARE(checkerPixmap.isNull(), false);
    mCheckerImage = checkerPixmap.toImage();
}

void TestHelper::cleanup()
{
    failureMessage.clear();

    app.settings()->clearRecentFiles();

    if (layeredImageProject)
        layeredImageProject->setAutoExportEnabled(false);
}

void TestHelper::resetCreationErrorSpy()
{
    if (projectManager->temporaryProject()) {
        projectCreationFailedSpy.reset(new QSignalSpy(projectManager, SIGNAL(creationFailed(QString))));
    }
}

void TestHelper::mouseEventOnCentre(QQuickItem *item, TestMouseEventType eventType, Qt::MouseButton button)
{
    QQuickWindow *itemWindow = item->window();
    Q_ASSERT(itemWindow);

    const QPoint centre = item->mapToScene(QPointF(item->width() / 2, item->height() / 2)).toPoint();
    switch (eventType) {
    case MousePress:
        QTest::mousePress(itemWindow, button, Qt::NoModifier, centre);
        break;
    case MouseRelease:
        QTest::mouseRelease(itemWindow, button, Qt::NoModifier, centre);
        break;
    case MouseClick:
        QTest::mouseClick(itemWindow, button, Qt::NoModifier, centre);
        break;
    case MouseDoubleClick:
        QTest::mouseDClick(itemWindow, button, Qt::NoModifier, centre);
        break;
    }
}

void TestHelper::mouseEvent(QQuickItem *item, const QPointF &localPos,
    TestMouseEventType eventType, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, int delay)
{
    const QPoint centre = item->mapToScene(localPos).toPoint();
    switch (eventType) {
    case MousePress:
        QTest::mousePress(item->window(), button, modifiers, centre, delay);
        break;
    case MouseRelease:
        QTest::mouseRelease(item->window(), button, modifiers, centre, delay);
        break;
    case MouseClick:
        QTest::mouseClick(item->window(), button, modifiers, centre, delay);
        break;
    case MouseDoubleClick:
        QTest::mouseDClick(item->window(), button, modifiers, centre, delay);
        break;
    }
}

void TestHelper::wheelEvent(QQuickItem *item, const QPoint &localPos, const int degrees)
{
    QWheelEvent wheelEvent(localPos, item->window()->mapToGlobal(localPos), QPoint(0, 0), QPoint(0, degrees * 8), 0, Qt::Vertical, Qt::NoButton, 0);
    QSpontaneKeyEvent::setSpontaneous(&wheelEvent);
    if (!qApp->notify(item->window(), &wheelEvent))
        QTest::qWarn("Wheel event not accepted by receiving window");
}

void TestHelper::keyClicks(const QString &text)
{
    for (const auto ch : qAsConst(text))
        QTest::keySequence(window, QKeySequence(ch));
}

bool TestHelper::clearAndEnterText(QQuickItem *textField, const QString &text)
{
    VERIFY(textField->property("text").isValid());
    VERIFY(textField->hasActiveFocus());

    if (!textField->property("text").toString().isEmpty()) {
        QTest::keySequence(window, QKeySequence::SelectAll);
        VERIFY(!textField->property("selectedText").toString().isEmpty());

        QTest::keySequence(window, QKeySequence::Backspace);
        VERIFY(textField->property("text").toString().isEmpty());
    }

    keyClicks(text);
    VERIFY(textField->property("text").toString() == text);
    return true;
}

bool TestHelper::changeCanvasSize(int width, int height, CloseDialogFlag closeDialog)
{
    // Open the canvas size popup.
    mouseEventOnCentre(canvasSizeButton, MouseClick);
    const QObject *canvasSizePopup = findPopupFromTypeName("CanvasSizePopup");
    VERIFY(canvasSizePopup);
    TRY_VERIFY2(canvasSizePopup->property("opened").toBool(), "Failed to open CanvasSizePopup");

    // Change the values and then cancel.
    // TODO: use actual input events...
    QQuickItem *widthSpinBox = canvasSizePopup->findChild<QQuickItem*>("changeCanvasWidthSpinBox");
    VERIFY(widthSpinBox);
    // We want it to be easy to change the values with the keyboard..
    VERIFY(widthSpinBox->hasActiveFocus());
    const int originalWidthSpinBoxValue = widthSpinBox->property("value").toInt();
    VERIFY(widthSpinBox->setProperty("value", originalWidthSpinBoxValue + 1));
    VERIFY(widthSpinBox->property("value").toInt() == originalWidthSpinBoxValue + 1);

    QQuickItem *heightSpinBox = canvasSizePopup->findChild<QQuickItem*>("changeCanvasHeightSpinBox");
    const int originalHeightSpinBoxValue = heightSpinBox->property("value").toInt();
    VERIFY(heightSpinBox);
    VERIFY(heightSpinBox->setProperty("value", originalHeightSpinBoxValue - 1));
    VERIFY(heightSpinBox->property("value").toInt() == originalHeightSpinBoxValue - 1);

    QQuickItem *cancelButton = canvasSizePopup->findChild<QQuickItem*>("canvasSizePopupCancelButton");
    VERIFY(cancelButton);
    mouseEventOnCentre(cancelButton, MouseClick);
    TRY_VERIFY2(!canvasSizePopup->property("visible").toBool(), "Failed to cancel CanvasSizePopup");
    VERIFY(project->size().width() == originalWidthSpinBoxValue);
    VERIFY(project->size().height() == originalHeightSpinBoxValue);
    VERIFY(canvas->hasActiveFocus());

    // Open the popup again.
    mouseEventOnCentre(canvasSizeButton, MouseClick);
    VERIFY(canvasSizePopup);
    TRY_VERIFY2(canvasSizePopup->property("opened").toBool(), "Failed to reopen CanvasSizePopup");
    // The old values should be restored.
    VERIFY(widthSpinBox->property("value").toInt() == originalWidthSpinBoxValue);
    VERIFY(heightSpinBox->property("value").toInt() == originalHeightSpinBoxValue);
    VERIFY(widthSpinBox->hasActiveFocus());

    // Change the values.
    VERIFY(widthSpinBox->setProperty("value", width));
    VERIFY(widthSpinBox->property("value").toInt() == width);
    VERIFY(heightSpinBox->setProperty("value", height));
    VERIFY(heightSpinBox->property("value").toInt() == height);

    if (closeDialog == CloseDialog) {
        // Press OK to close the dialog.
        QQuickItem *okButton = canvasSizePopup->findChild<QQuickItem*>("canvasSizePopupOkButton");
        VERIFY(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        TRY_VERIFY2(!canvasSizePopup->property("visible").toBool(), "Failed to accept CanvasSizePopup");
        VERIFY(project->size().width() == width);
        VERIFY(project->size().height() == height);
        VERIFY(widthSpinBox->property("value").toInt() == width);
        VERIFY(heightSpinBox->property("value").toInt() == height);
        VERIFY(canvas->hasActiveFocus());
    }

    return true;
}

bool TestHelper::changeImageSize(int width, int height)
{
    // Open the image size popup.
    mouseEventOnCentre(imageSizeButton, MouseClick);
    const QObject *imageSizePopup = findPopupFromTypeName("ImageSizePopup");
    VERIFY(imageSizePopup);
    VERIFY(imageSizePopup->property("visible").toBool());

    // Change the values and then cancel.
    // TODO: use actual input events...
    QQuickItem *widthSpinBox = imageSizePopup->findChild<QQuickItem*>("changeImageWidthSpinBox");
    VERIFY(widthSpinBox);
    // We want it to be easy to change the values with the keyboard..
    VERIFY(widthSpinBox->hasActiveFocus());
    const int originalWidthSpinBoxValue = widthSpinBox->property("value").toInt();
    VERIFY(widthSpinBox->setProperty("value", originalWidthSpinBoxValue + 1));
    VERIFY(widthSpinBox->property("value").toInt() == originalWidthSpinBoxValue + 1);

    QQuickItem *heightSpinBox = imageSizePopup->findChild<QQuickItem*>("changeImageHeightSpinBox");
    const int originalHeightSpinBoxValue = heightSpinBox->property("value").toInt();
    VERIFY(heightSpinBox);
    VERIFY(heightSpinBox->setProperty("value", originalHeightSpinBoxValue - 1));
    VERIFY(heightSpinBox->property("value").toInt() == originalHeightSpinBoxValue - 1);

    QQuickItem *cancelButton = imageSizePopup->findChild<QQuickItem*>("imageSizePopupCancelButton");
    VERIFY(cancelButton);
    mouseEventOnCentre(cancelButton, MouseClick);
    VERIFY(!imageSizePopup->property("visible").toBool());
    VERIFY(project->size().width() == originalWidthSpinBoxValue);
    VERIFY(project->size().height() == originalHeightSpinBoxValue);

    // Open the popup again.
    mouseEventOnCentre(imageSizeButton, MouseClick);
    VERIFY(imageSizePopup);
    VERIFY(imageSizePopup->property("visible").toBool());
    // The old values should be restored.
    VERIFY(widthSpinBox->property("value").toInt() == originalWidthSpinBoxValue);
    VERIFY(heightSpinBox->property("value").toInt() == originalHeightSpinBoxValue);

    // Change the values and then press OK.
    VERIFY(widthSpinBox->setProperty("value", width));
    VERIFY(widthSpinBox->property("value").toInt() == width);
    VERIFY(heightSpinBox->setProperty("value", height));
    VERIFY(heightSpinBox->property("value").toInt() == height);

    QQuickItem *okButton = imageSizePopup->findChild<QQuickItem*>("imageSizePopupOkButton");
    VERIFY(okButton);
    mouseEventOnCentre(okButton, MouseClick);
    VERIFY(!imageSizePopup->property("visible").toBool());
    VERIFY(project->size().width() == width);
    VERIFY(project->size().height() == height);
    VERIFY(widthSpinBox->property("value").toInt() == width);
    VERIFY(heightSpinBox->property("value").toInt() == height);

    return true;
}

bool TestHelper::changeToolSize(int size)
{
    mouseEventOnCentre(toolSizeButton, MouseClick);
    const QObject *toolSizePopup = findPopupFromTypeName("ToolSizePopup");
    VERIFY(toolSizePopup);
    VERIFY(toolSizePopup->property("visible").toBool() == true);

    QQuickItem *toolSizeSlider = toolSizePopup->findChild<QQuickItem*>("toolSizeSlider");
    VERIFY(toolSizeSlider);

    QQuickItem *toolSizeSliderHandle = toolSizeSlider->property("handle").value<QQuickItem*>();
    VERIFY(toolSizeSliderHandle);

    // Move the slider to the right to find the max pos.
    VERIFY(toolSizeSlider->setProperty("value", toolSizeSlider->property("to").toReal()));
    VERIFY(toolSizeSlider->property("value") == toolSizeSlider->property("to"));
    const QPoint handleMaxPos = toolSizeSliderHandle->mapToScene(
        QPointF(toolSizeSliderHandle->width() / 2, toolSizeSliderHandle->height() / 2)).toPoint();

    // Move/reset the slider to the left since we move from left to right.
    VERIFY(toolSizeSlider->setProperty("value", toolSizeSlider->property("from").toReal()));
    VERIFY(toolSizeSlider->property("value") == toolSizeSlider->property("from"));
    const QPoint handleMinPos = toolSizeSliderHandle->mapToScene(
        QPointF(toolSizeSliderHandle->width() / 2, toolSizeSlider->height() / 2)).toPoint();

    QPoint sliderHandlePos = handleMinPos;
    QTest::mousePress(toolSizeSlider->window(), Qt::LeftButton, Qt::NoModifier, sliderHandlePos);
    VERIFY(toolSizeSlider->property("pressed").toBool() == true);
    VERIFY(window->mouseGrabberItem() == toolSizeSlider);

    QTest::mouseMove(toolSizeSlider->window(), sliderHandlePos, 5);

    // Move the slider's handle until we find the value we want.
    for (;
         sliderValue(toolSizeSlider) != size && sliderHandlePos.x() < handleMaxPos.x();
         ++sliderHandlePos.rx()) {
        QTest::mouseMove(toolSizeSlider->window(), sliderHandlePos, 5);
    }
    --sliderHandlePos.rx();
    QTest::mouseRelease(toolSizeSlider->window(), Qt::LeftButton, Qt::NoModifier, sliderHandlePos);
    VERIFY(toolSizeSlider->property("pressed").toBool() == false);
    VERIFY(sliderValue(toolSizeSlider) == size);

    // Close the popup.
    QTest::keyClick(window, Qt::Key_Escape);
    VERIFY(toolSizePopup->property("visible").toBool() == false);

    return true;
}

bool TestHelper::changeToolShape(ImageCanvas::ToolShape toolShape)
{
    if (canvas->toolShape() == toolShape)
        return true;

    mouseEventOnCentre(toolShapeButton, MouseClick);
    const QObject *toolShapeMenu = window->findChild<QObject*>("toolShapeMenu");
    VERIFY(toolShapeMenu);
    TRY_VERIFY(toolShapeMenu->property("opened").toBool() == true);

    if (toolShape == ImageCanvas::SquareToolShape) {
        QQuickItem *squareToolShapeMenuItem = toolShapeMenu->findChild<QQuickItem*>("squareToolShapeMenuItem");
        VERIFY(squareToolShapeMenuItem);

        mouseEventOnCentre(squareToolShapeMenuItem, MouseClick);
        VERIFY(canvas->toolShape() == ImageCanvas::SquareToolShape);
    } else {
        QQuickItem *circleToolShapeMenuItem = toolShapeMenu->findChild<QQuickItem*>("circleToolShapeMenuItem");
        VERIFY(circleToolShapeMenuItem);

        mouseEventOnCentre(circleToolShapeMenuItem, MouseClick);
        VERIFY(canvas->toolShape() == ImageCanvas::CircleToolShape);
    }

    return true;
}

bool TestHelper::moveContents(int x, int y, bool onlyVisibleLayers)
{
    const QImage originalContents = project->exportedImage();

    // Open the move contents dialog.
    VERIFY2(triggerShortcut("moveContentsShortcut", app.settings()->moveContentsShortcut()), failureMessage);
    const QObject *moveContentsDialog = findPopupFromTypeName("MoveContentsDialog");
    VERIFY(moveContentsDialog);
    VERIFY(moveContentsDialog->property("visible").toBool());

    // Change the values and then cancel.
    // TODO: use actual input events...
    QQuickItem *moveContentsXSpinBox = moveContentsDialog->findChild<QQuickItem*>("moveContentsXSpinBox");
    VERIFY(moveContentsXSpinBox);
    // We want it to be easy to change the values with the keyboard..
    VERIFY(moveContentsXSpinBox->hasActiveFocus());
    const int originalXSpinBoxValue = moveContentsXSpinBox->property("value").toInt();
    VERIFY(moveContentsXSpinBox->setProperty("value", originalXSpinBoxValue + 1));
    VERIFY(moveContentsXSpinBox->property("value").toInt() == originalXSpinBoxValue + 1);

    QQuickItem *moveContentsYSpinBox = moveContentsDialog->findChild<QQuickItem*>("moveContentsYSpinBox");
    const int originalYSpinBoxValue = moveContentsYSpinBox->property("value").toInt();
    VERIFY(moveContentsYSpinBox);
    VERIFY(moveContentsYSpinBox->setProperty("value", originalYSpinBoxValue - 1));
    VERIFY(moveContentsYSpinBox->property("value").toInt() == originalYSpinBoxValue - 1);

    QQuickItem *cancelButton = moveContentsDialog->findChild<QQuickItem*>("moveContentsDialogCancelButton");
    VERIFY(cancelButton);
    mouseEventOnCentre(cancelButton, MouseClick);
    VERIFY(!moveContentsDialog->property("visible").toBool());
    VERIFY(project->exportedImage() == originalContents);

    // Open the dialog again.
    VERIFY(triggerShortcut("moveContentsShortcut", app.settings()->moveContentsShortcut()));
    VERIFY(moveContentsDialog->property("visible").toBool());
    // The old values should be restored.
    VERIFY(moveContentsXSpinBox->property("value").toInt() == originalXSpinBoxValue);
    VERIFY(moveContentsYSpinBox->property("value").toInt() == originalYSpinBoxValue);

    // Change the values and then press OK.
    VERIFY(moveContentsXSpinBox->setProperty("value", x));
    VERIFY(moveContentsXSpinBox->property("value").toInt() == x);
    VERIFY(moveContentsYSpinBox->setProperty("value", y));
    VERIFY(moveContentsYSpinBox->property("value").toInt() == y);

    if (onlyVisibleLayers) {
        QQuickItem *onlyMoveVisibleLayersCheckBox = moveContentsDialog->findChild<QQuickItem*>("onlyMoveVisibleLayersCheckBox");
        VERIFY(onlyMoveVisibleLayersCheckBox);
        if (onlyMoveVisibleLayersCheckBox->property("checked").toBool() != onlyVisibleLayers) {
            mouseEventOnCentre(onlyMoveVisibleLayersCheckBox, MouseClick);
            VERIFY(onlyMoveVisibleLayersCheckBox->property("checked").toBool() == onlyVisibleLayers);
        }
    }

    QImage movedContents(originalContents.size(), QImage::Format_ARGB32_Premultiplied);
    movedContents.fill(Qt::transparent);

    QPainter painter(&movedContents);
    painter.drawImage(x, y, originalContents);
    painter.end();

    QQuickItem *okButton = moveContentsDialog->findChild<QQuickItem*>("moveContentsDialogOkButton");
    VERIFY(okButton);
    mouseEventOnCentre(okButton, MouseClick);
    VERIFY(!moveContentsDialog->property("visible").toBool());
    VERIFY(project->exportedImage() == movedContents);
    VERIFY(moveContentsXSpinBox->property("value").toInt() == x);
    VERIFY(moveContentsYSpinBox->property("value").toInt() == y);

    return true;
}

int TestHelper::sliderValue(QQuickItem *slider) const
{
    const qreal position = slider->property("position").toReal();
    qreal value = 0;
    QMetaObject::invokeMethod(slider, "valueAt", Qt::DirectConnection, Q_RETURN_ARG(qreal, value), Q_ARG(qreal, position));
    return qFloor(value);
}

bool TestHelper::selectColourAtCursorPos()
{
    if (tilesetProject) {
        FAIL("Not implemented yet");
    } else {
        if (!switchTool(TileCanvas::EyeDropperTool))
            return false;

        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        const QColor actualColour = canvas->penForegroundColour();
        const QColor expectedColour = canvas->currentProjectImage()->pixelColor(cursorPos);
        VERIFY2(actualColour == expectedColour,
            qPrintable(QString::fromLatin1("Expected canvas foreground colour to be %1, but it's %2")
                .arg(expectedColour.name(), actualColour.name())));

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    }
    return true;
}

bool TestHelper::drawPixelAtCursorPos()
{
    if (tilesetProject) {
        const Tile *targetTile = tilesetProject->tileAt(cursorPos);
        VERIFY(targetTile);

        if (!switchTool(TileCanvas::PenTool))
            return false;
        if (!switchMode(TileCanvas::PixelMode))
            return false;

        // Draw on some pixels of the current tile.
        const QImage originalTileImage = targetTile->tileset()->image()->copy(targetTile->sourceRect());
        QImage expectedImage = originalTileImage;
        expectedImage.setPixelColor(tileCanvas->scenePosToTilePixelPos(cursorPos), tileCanvas->penForegroundColour());

        QTest::mouseMove(window, cursorPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        VERIFY(targetTile->tileset()->image()->copy(targetTile->sourceRect()) != originalTileImage);
        VERIFY(targetTile->tileset()->image()->copy(targetTile->sourceRect()) == expectedImage);
        VERIFY(tilesetProject->hasUnsavedChanges());
        // Don't check that the title contains the change marker, as that won't happen
        // until release if this is the first change in the stack.

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        VERIFY(targetTile->tileset()->image()->copy(targetTile->sourceRect()) == expectedImage);
        VERIFY(tilesetProject->hasUnsavedChanges());
        VERIFY(window->title().contains("*"));
    } else {
        if (!switchTool(TileCanvas::PenTool))
            return false;

        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QColor actualColour = canvas->currentProjectImage()->pixelColor(cursorPos);
        QColor expectedColour = canvas->penForegroundColour();
        if (actualColour != expectedColour) {
            QString message;
            QDebug stream(&message);
            stream << "Expected current project image pixel at" << cursorPos << "to be" << expectedColour.name()
                << "after mouse press, but it's" << actualColour.name();
            FAIL(qPrintable(message));
        }
        VERIFY(project->hasUnsavedChanges());
        // Don't check that the title contains the change marker, as that won't happen
        // until release if this is the first change in the stack.

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        if (actualColour != expectedColour) {
            QString message;
            QDebug stream(&message);
            stream << "Expected current project image pixel at" << cursorPos << "to be" << expectedColour.name()
                << "after mouse release, but it's" << actualColour.name();
            FAIL(qPrintable(message));
        }
        VERIFY(project->hasUnsavedChanges());
        VERIFY(window->title().contains("*"));
    }

    return true;
}

bool TestHelper::drawTileAtCursorPos()
{
    if (!switchTool(TileCanvas::PenTool))
        return false;
    if (!switchMode(TileCanvas::TileMode))
        return false;

    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    const int penId = tileCanvas->penTile()->id();
    const Tile *tile = tilesetProject->tileAt(cursorPos);
    VERIFY(tile);
    VERIFY(tile->id() == penId);
    return true;
}

bool TestHelper::selectArea(const QRect &area)
{
    // Switch to the selection tool.
    if (!switchTool(ImageCanvas::SelectionTool))
        return false;

    if (canvas->hasSelection()) {
        failureMessage = "Can't select area when there's already an existing selection";
        return false;
    }

    // Select the area.
    setCursorPosInScenePixels(area.topLeft());
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    // If area is {0, 0, 5, 5}, we need to move to {x=5, y=5} to ensure
    // that we've selected all 5x5 pixels.
    setCursorPosInScenePixels(area.bottomRight() + QPoint(1, 1));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    VERIFY(canvas->hasSelection());
    VERIFY(canvas->selectionArea() == area);
    return true;
}

bool TestHelper::dragSelection(const QPoint &newTopLeft)
{
    if (!canvas->hasSelection()) {
        failureMessage = "No selection to drag";
        return false;
    }

    const QRect oldSelectionArea = canvas->selectionArea();
    QRect newSelectionArea = oldSelectionArea;
    newSelectionArea.moveTo(newTopLeft);

    setCursorPosInScenePixels(canvas->selectionArea().topLeft());
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(newTopLeft);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    VERIFY(canvas->selectionArea() == newSelectionArea);
    return true;
}

static QString fuzzyColourCompareFailMsg(const QColor &colour1, const QColor &colour2,
    const QChar &componentName, int difference, int fuzz)
{
    return QString::fromLatin1("colour1 %1 is not equal to colour2 %2; %3 difference of %4 is larger than fuzz of %5")
            .arg(colour1.name()).arg(colour2.name()).arg(componentName).arg(difference).arg(fuzz);
}

bool TestHelper::fuzzyColourCompare(const QColor &colour1, const QColor &colour2, int fuzz)
{
    const int rDiff = qAbs(colour2.red() - colour1.red());
    VERIFY2(rDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('r'), rDiff, fuzz)));

    const int gDiff = qAbs(colour2.green() - colour1.green());
    VERIFY2(gDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('g'), gDiff, fuzz)));

    const int bDiff = qAbs(colour2.blue() - colour1.blue());
    VERIFY2(bDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('b'), bDiff, fuzz)));

    const int aDiff = qAbs(colour2.alpha() - colour1.alpha());
    VERIFY2(aDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('a'), aDiff, fuzz)));

    return true;
}

bool TestHelper::fuzzyImageCompare(const QImage &image1, const QImage &image2)
{
    VERIFY(image1.size() == image2.size());

    for (int y = 0; y < image1.height(); ++y) {
        for (int x = 0; x < image1.width(); ++x) {
            if (!fuzzyColourCompare(image1.pixelColor(x, y), image2.pixelColor(x, y)))
                return false;
        }
    }

    return true;
}

bool TestHelper::everyPixelIs(const QImage &image, const QColor &colour)
{
    for (int y = 0; y < image.size().height(); ++y) {
        for (int x = 0; x < image.size().width(); ++x) {
            if (image.pixelColor(x, y) != colour) {
                QString message;
                QDebug debug(&message);
                debug.nospace() << "Expected every pixel of " << image << " to be " << colour
                    << ", but pixel at " << x << ", " << y << " is " << image.pixelColor(x, y);
                failureMessage = message.toLatin1();
                return false;
            }
        }
    }

    return true;
}

bool TestHelper::compareSwatches(const Swatch &actualSwatch, const Swatch &expectedSwatch)
{
    if (actualSwatch.colours() == expectedSwatch.colours())
        return true;

    QString message;
    message = "Swatches are not equal:";
    message += "\n  actual ";
    QDebug(&message) << actualSwatch.colours();
    message += "\nexpected ";
    QDebug(&message) << expectedSwatch.colours();
    failureMessage = qPrintable(message);
    return false;
}

bool TestHelper::enableAutoSwatch()
{
    // The swatches panel is hidden by default when testing; see updateVariables().
    if (!togglePanel("swatchesPanel", true))
        return false;

    // Wait until the previous view is destroyed (if any).
    TRY_VERIFY(!window->findChild<QQuickItem*>("autoSwatchGridView"));

    // Sanity check.
    VERIFY(!app.settings()->isAutoSwatchEnabled());
    // Enable auto swatch.
    app.settings()->setAutoSwatchEnabled(true);
    return true;
}

bool TestHelper::swatchViewDelegateExists(const QQuickItem *viewContentItem, const QColor &colour)
{
    const auto childItems = viewContentItem->childItems();
    for (const QQuickItem *delegateItem : childItems) {
        if (delegateItem->property("color").value<QColor>() == colour)
            return true;
    }

    QString message;
    QDebug debug(&message);
    debug.nospace() << "Couldn't find a delegate in swatch view content item " << viewContentItem
        << " whose contentItem colour is " << colour;
    failureMessage = message.toLatin1();
    return false;
}

QQuickItem *TestHelper::findSwatchViewDelegateAtIndex(int index)
{
    QQuickItem *swatchGridView = window->findChild<QQuickItem*>("swatchGridView");
    if (!swatchGridView)
        return nullptr;

    return findViewDelegateAtIndex(swatchGridView, index);
}

bool TestHelper::addSwatchWithForegroundColour()
{
    // Roll back to the previous value in case of test failure.
    Utils::ScopeGuard swatchPanelExpandedGuard([=](){
        const bool oldExpandedValue = swatchesPanel->property("expanded").toBool();
        swatchesPanel->setProperty("expanded", oldExpandedValue);
    });

    swatchesPanel->setProperty("expanded", true);

    QQuickItem *swatchGridView = window->findChild<QQuickItem*>("swatchGridView");
    VERIFY(swatchGridView);
    VERIFY(QMetaObject::invokeMethod(swatchGridView, "forceLayout"));

    QQuickItem *viewContentItem = swatchGridView->property("contentItem").value<QQuickItem*>();
    VERIFY(viewContentItem);

    const int previousDelegateCount = swatchGridView->property("count").toInt();
    const QString expectedDelegateObjectName = QString::fromLatin1("swatchGridView_Delegate_%1_%2")
        .arg(previousDelegateCount).arg(canvas->penForegroundColour().name(QColor::HexArgb));

    // Add the swatch.
    QQuickItem *newSwatchColourButton = window->findChild<QQuickItem*>("newSwatchColourButton");
    VERIFY(newSwatchColourButton);
    mouseEventOnCentre(newSwatchColourButton, MouseClick);
    VERIFY(QMetaObject::invokeMethod(swatchGridView, "forceLayout"));
    VERIFY2(swatchGridView->property("count").toInt() == previousDelegateCount + 1,
        qPrintable(QString::fromLatin1("Expected %1 swatch delegates after adding one, but there are %2")
            .arg(previousDelegateCount + 1).arg(swatchGridView->property("count").toInt())));
    // findChild() doesn't work here for some reason.
    const auto childItems = viewContentItem->childItems();
    QQuickItem *swatchDelegate = nullptr;
    for (const auto childItem : qAsConst(childItems)) {
        if (childItem->objectName() == expectedDelegateObjectName) {
            swatchDelegate = childItem;
            break;
        }
    }
    if (!swatchDelegate) {
        QString message;
        QDebug stream(&message);
        stream.nospace() << "Expected a swatch delegate to exist named " << expectedDelegateObjectName
            << " but there wasn't. Swatch delegates: " << viewContentItem->childItems();
        FAIL(qPrintable(message));
    }
    return true;
}

bool TestHelper::renameSwatchColour(int index, const QString &name)
{
    // Open the context menu by right clicking on the delegate.
    QQuickItem *delegate = findSwatchViewDelegateAtIndex(index);
    VERIFY(delegate);
    mouseEventOnCentre(delegate, MouseClick, Qt::RightButton);
    QObject *swatchContextMenu = findPopupFromTypeName("SwatchContextMenu");
    VERIFY(swatchContextMenu);
    TRY_VERIFY(swatchContextMenu->property("opened").toBool());

    // Select the rename menu item.
    QQuickItem *renameSwatchColourMenuItem = window->findChild<QQuickItem*>("renameSwatchColourMenuItem");
    VERIFY(renameSwatchColourMenuItem);
    // TODO: mouse event isn't getting through to the menu item without this.
    // This is likely caused by https://codereview.qt-project.org/#/c/225747/
    // and is probably the same issue encountered in the tests there.
    // Replace with Qt API if https://bugreports.qt.io/browse/QTBUG-71224 ever gets done.
    QTest::qWait(50);
    mouseEventOnCentre(renameSwatchColourMenuItem, MouseClick);
    TRY_VERIFY(!swatchContextMenu->property("opened").toBool());

    QObject *renameSwatchColourDialog = findPopupFromTypeName("RenameSwatchColourDialog");
    VERIFY(renameSwatchColourDialog);
    TRY_VERIFY(renameSwatchColourDialog->property("opened").toBool());

    // Do the renaming.
    QQuickItem *swatchNameTextField = window->findChild<QQuickItem*>("swatchNameTextField");
    VERIFY(swatchNameTextField);
    VERIFY2(clearAndEnterText(swatchNameTextField, name), failureMessage);
    QTest::keyClick(window, Qt::Key_Return);
    TRY_VERIFY(!renameSwatchColourDialog->property("opened").toBool());
    return true;
}

bool TestHelper::deleteSwatchColour(int index)
{
    // Open the context menu by right clicking on the delegate.
    QQuickItem *delegate = findSwatchViewDelegateAtIndex(index);
    VERIFY(delegate);
    mouseEventOnCentre(delegate, MouseClick, Qt::RightButton);

    QObject *swatchContextMenu = findPopupFromTypeName("SwatchContextMenu");
    VERIFY(swatchContextMenu);
    TRY_VERIFY(swatchContextMenu->property("opened").toBool());

    QQuickItem *deleteSwatchColourMenuItem = window->findChild<QQuickItem*>("deleteSwatchColourMenuItem");
    VERIFY(deleteSwatchColourMenuItem);
    mouseEventOnCentre(deleteSwatchColourMenuItem, MouseClick);
    TRY_VERIFY(!swatchContextMenu->property("opened").toBool());
    return true;
}

bool TestHelper::addNewGuide(Qt::Orientation orientation, int position)
{
    if (!app.settings()->areRulersVisible()) {
        if (!triggerRulersVisible())
            return false;
        VERIFY(app.settings()->areRulersVisible());
    }

    const bool horizontal = orientation == Qt::Horizontal;
    const int originalGuideCount = project->guides().size();
    const int newGuideIndex = originalGuideCount;
    const QPoint originalOffset = canvas->currentPane()->integerOffset();
    const qreal originalZoomLevel = canvas->currentPane()->zoomLevel();

    QQuickItem *ruler = canvas->findChild<QQuickItem*>(horizontal
        ? "firstHorizontalRuler" : "firstVerticalRuler");
    VERIFY(ruler);
    const qreal rulerThickness = horizontal ? ruler->height() : ruler->width();

    // Pan so that the top left of the canvas is at the rulers' corners.
    if (!panTopLeftTo(rulerThickness, rulerThickness))
        return false;

    canvas->currentPane()->setZoomLevel(1.0);

    // Drop a horizontal guide onto the canvas.
    const QPoint pressPos(
        horizontal ? 50 : rulerThickness / 2,
        horizontal ? rulerThickness / 2 : 50);
    setCursorPosInPixels(pressPos);
    QTest::mouseMove(window, cursorWindowPos);
    VERIFY(!canvas->pressedRuler());

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    VERIFY(canvas->pressedRuler());

    // Do the actual moving onto the canvas.
    const QPoint releasePos(
        horizontal ? 50 : rulerThickness + position,
        horizontal ? rulerThickness + position : 50);
    setCursorPosInPixels(releasePos);
    QTest::mouseMove(window, cursorWindowPos);

    // Now it should be visible on the canvas.
    VERIFY(imageGrabber.requestImage(canvas));
    TRY_VERIFY(imageGrabber.isReady());
    const QImage grabWithGuide = imageGrabber.takeImage();
    VERIFY(grabWithGuide.pixelColor(releasePos.x(), releasePos.y()) == QColor(Qt::cyan));

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    VERIFY(!canvas->pressedRuler());
    VERIFY2(project->guides().size() == originalGuideCount + 1, qPrintable(QString::fromLatin1(
        "Expected %1 guide(s), but got %2").arg(originalGuideCount + 1).arg(project->guides().size())));
    VERIFY(project->guides().at(newGuideIndex).position() == position);
    VERIFY(project->undoStack()->canUndo());

    canvas->currentPane()->setOffset(originalOffset);
    canvas->currentPane()->setZoomLevel(originalZoomLevel);
    return true;
}

QObject *TestHelper::findPopupFromTypeName(const QString &typeName) const
{
    QObject *popup = nullptr;
    foreach (QQuickItem *child, overlay->childItems()) {
        if (QString::fromLatin1(child->metaObject()->className()) == "QQuickPopupItem") {
            if (QString::fromLatin1(child->parent()->metaObject()->className()).contains(typeName)) {
                popup = child->parent();
                break;
            }
        }
    }
    return popup;
}

QQuickItem *TestHelper::findDialogButtonFromText(const QObject *dialog, const QString &text)
{
    QQuickItem *footer = dialog->property("footer").value<QQuickItem*>();
    if (!footer)
        return nullptr;

    return findChildWithText(footer, text);
}

QQuickItem *TestHelper::findDialogButtonFromObjectName(const QObject *dialog, const QString &objectName)
{
    QQuickItem *footer = dialog->property("footer").value<QQuickItem*>();
    if (!footer)
        return nullptr;

    return footer->findChild<QQuickItem*>(objectName);
}

QQuickItem *TestHelper::findListViewChild(QQuickItem *listView, const QString &childObjectName) const
{
    QQuickItem *listViewContentItem = listView->property("contentItem").value<QQuickItem*>();
    if (!listViewContentItem)
        return nullptr;

    QQuickItem *listViewChild = nullptr;
    foreach (QQuickItem *child, listViewContentItem->childItems()) {
        if (child->objectName() == childObjectName) {
            listViewChild = child;
            break;
        }
    }
    return listViewChild;
}

QQuickItem *TestHelper::findListViewChild(const QString &listViewObjectName, const QString &childObjectName) const
{
    QQuickItem *listView = window->findChild<QQuickItem*>(listViewObjectName);
    if (!listView)
        return nullptr;

    return findListViewChild(listView, childObjectName);
}

QQuickItem *TestHelper::findChildWithText(QQuickItem *item, const QString &text)
{
    foreach (QQuickItem *child, item->childItems()) {
        if (child->property("text").toString() == text)
            return child;
        else {
            QQuickItem *match = findChildWithText(child, text);
            if (match)
                return match;
        }
    }
    return nullptr;
}

QQuickItem *TestHelper::findViewDelegateAtIndex(QQuickItem *view, int index)
{
    QQuickItem *viewContentItem = view->property("contentItem").value<QQuickItem*>();
    if (!viewContentItem)
        return nullptr;

    const auto childItems = viewContentItem->childItems();
    for (QQuickItem *child : childItems) {
        QQmlContext *context = qmlContext(child);
        if (!context)
            continue;

        QVariant indexProperty = context->contextProperty("index");
        if (indexProperty.toInt() == index)
            return child;
    }
    return nullptr;
}

QQuickItem *TestHelper::findSplitViewHandle(const QString &splitViewObjectName, int handleIndex) const
{
    QQuickItem *splitView = window->findChild<QQuickItem*>(splitViewObjectName);
    if (!splitView)
        return nullptr;

    const QVariant orientationProperty = splitView->property("orientation");
    if (!orientationProperty.isValid())
        return nullptr;

    const Qt::Orientation orientation = orientationProperty.value<Qt::Orientation>();
    const bool horizontal = orientation == Qt::Horizontal;

    // Just a guess since we have no other identifying information.
    const int maxSplitterSize = 10;

    const QList<QQuickItem*> childItems = splitView->childItems();
    QList<QQuickItem*> handleItems;
    for (QQuickItem *child : childItems) {
        if (horizontal && child->width() >= 1 && child->width() <= maxSplitterSize)
            handleItems.append(child);
        else if (!horizontal && child->height() >= 1 && child->height() <= maxSplitterSize)
            handleItems.append(child);
    }

    if (handleIndex < 0 || handleIndex >= handleItems.size())
        return nullptr;

    return handleItems.at(handleIndex);
}

QPoint TestHelper::mapToTile(const QPoint &cursorPos) const
{
    return cursorPos - tileCanvas->mapToScene(QPointF(0, 0)).toPoint();
}

QPoint TestHelper::tileSceneCentre(int xPosInTiles, int yPosInTiles) const
{
    return tileCanvas->mapToScene(QPointF(
        xPosInTiles * tilesetProject->tileWidth() + tilesetProject->tileWidth() / 2,
        yPosInTiles * tilesetProject->tileHeight() + tilesetProject->tileHeight() / 2)).toPoint()
            + tileCanvas->firstPane()->integerOffset();
}

QPoint TestHelper::tileCanvasCentre(int xPosInTiles, int yPosInTiles) const
{
    return QPoint(
        xPosInTiles * tilesetProject->tileWidth() + tilesetProject->tileWidth() / 2,
                yPosInTiles * tilesetProject->tileHeight() + tilesetProject->tileHeight() / 2);
}

QPointF TestHelper::canvasCentre() const
{
    return QPointF(canvas->width() / 2, canvas->height() / 2);
}

QPoint TestHelper::canvasSceneCentre() const
{
    return canvas->mapToScene(canvasCentre()).toPoint();
}

QPoint TestHelper::firstPaneSceneCentre() const
{
    return canvas->mapToScene(QPointF((canvas->firstPane()->size() * canvas->width()) / 2, canvas->height() / 2)).toPoint();
}

void TestHelper::setCursorPosInPixels(const QPoint &posInPixels)
{
    cursorPos = posInPixels;
    cursorWindowPos = canvas->mapToScene(QPointF(posInPixels.x(), posInPixels.y())).toPoint();
}

void TestHelper::setCursorPosInTiles(int xPosInTiles, int yPosInTiles)
{
    cursorPos = tileCanvasCentre(xPosInTiles, yPosInTiles);
    cursorWindowPos = tileSceneCentre(xPosInTiles, yPosInTiles);
}

void TestHelper::setCursorPosInScenePixels(int xPosInScenePixels, int yPosInScenePixels, bool verifyWithinWindow)
{
    cursorPos = QPoint(xPosInScenePixels, yPosInScenePixels);
    cursorWindowPos = canvas->mapToScene(cursorPos).toPoint() + canvas->firstPane()->integerOffset();
    if (verifyWithinWindow) {
        // As with mouseEventOnCentre(), we don't want this to be a e.g. VERIFY2, because then we'd have to
        // verify its return value everywhere we use it, and we use it a lot, so just assert instead.
        Q_ASSERT_X(cursorWindowPos.x() >= 0 && cursorWindowPos.y() >= 0, Q_FUNC_INFO,
            qPrintable(QString::fromLatin1("x %1 y %2").arg(cursorWindowPos.x()).arg(cursorWindowPos.y())));
    }
}

void TestHelper::setCursorPosInScenePixels(const QPoint &posInScenePixels)
{
    cursorPos = posInScenePixels;
    const int integerZoomLevel = canvas->currentPane()->integerZoomLevel();
    const QPointF localZoomedPixelPos = QPointF(
        posInScenePixels.x() * integerZoomLevel,
        posInScenePixels.y() * integerZoomLevel);
    cursorWindowPos = canvas->mapToScene(localZoomedPixelPos).toPoint() + canvas->firstPane()->integerOffset();
}

QPoint TestHelper::tilesetTileCentre(int xPosInTiles, int yPosInTiles) const
{
    return QPoint(
        xPosInTiles * tilesetProject->tileWidth() + tilesetProject->tileWidth() / 2,
        yPosInTiles * tilesetProject->tileHeight() + tilesetProject->tileHeight() / 2);
}

QPoint TestHelper::tilesetTileSceneCentre(int xPosInTiles, int yPosInTiles) const
{
    return tilesetSwatchFlickable->mapToScene(QPointF(
         xPosInTiles * tilesetProject->tileWidth() + tilesetProject->tileWidth() / 2,
         yPosInTiles * tilesetProject->tileHeight() + tilesetProject->tileHeight() / 2)).toPoint();
}

int TestHelper::digits(int number)
{
    QVector<int> digits;
    number = qAbs(number);
    do {
        digits.push_front(number % 10);
        number /= 10;
    } while (number > 0);

    return digits.size();
}

int TestHelper::digitAt(int number, int index)
{
    QVector<int> digits;
    number = qAbs(number);
    do {
        digits.push_front(number % 10);
        number /= 10;
    } while (number > 0);

    return index < digits.size() ? digits.at(index) : 0;
}

bool TestHelper::isUsingAnimation() const
{
    return imageProject ? imageProject->isUsingAnimation() : layeredImageProject->isUsingAnimation();
}

AnimationPlayback *TestHelper::animationPlayback()
{
    return imageProject ? imageProject->animationPlayback() : layeredImageProject->animationPlayback();
}

bool TestHelper::triggerShortcut(const QString &objectName, const QString &sequenceAsString)
{
    QObject *shortcut = window->findChild<QObject*>(objectName);
    VERIFY2(shortcut, qPrintable(QString::fromLatin1("Shortcut \"%1\" could not be found").arg(objectName)));
    VERIFY2(shortcut->property("enabled").toBool(), qPrintable(QString::fromLatin1(
        "Shortcut \"%1\" is not enabled (%2 has active focus; does this shortcut require the canvas to have it?)")
            .arg(objectName, window->activeFocusItem() ? window->activeFocusItem()->objectName() : QString())));

    QSignalSpy activatedSpy(shortcut, SIGNAL(activated()));
    VERIFY(activatedSpy.isValid());

    VERIFY(QTest::qWaitForWindowActive(window));
    const int value = QKeySequence(sequenceAsString)[0];
    Qt::KeyboardModifiers mods = (Qt::KeyboardModifiers)(value & Qt::KeyboardModifierMask);
    QTest::keyClick(window, value & ~mods, mods);
    VERIFY2(activatedSpy.count() == 1, qPrintable(QString::fromLatin1(
        "The activated() signal was not emitted for %1 with sequence %2").arg(objectName).arg(sequenceAsString)));

    return true;
}

bool TestHelper::triggerNewProject()
{
    return triggerShortcut("newShortcut", app.settings()->newShortcut());
}

bool TestHelper::triggerCloseProject()
{
    return triggerShortcut("closeShortcut", app.settings()->closeShortcut());
}

bool TestHelper::triggerSaveProject()
{
    return triggerShortcut("saveShortcut", app.settings()->saveShortcut());
}

bool TestHelper::triggerSaveProjectAs()
{
    FAIL("TODO: no saveas shortcut");
//    triggerShortcut(app.settings()->saveShortcut());
}

bool TestHelper::triggerOpenProject()
{
    return triggerShortcut("openShortcut", app.settings()->openShortcut());
}

bool TestHelper::triggerRevert()
{
    return triggerShortcut("revertShortcut", app.settings()->revertShortcut());
}

bool TestHelper::triggerPaste()
{
    return triggerShortcut("pasteShortcut", QKeySequence(QKeySequence::Paste).toString());
}

bool TestHelper::triggerFlipHorizontally()
{
    return triggerShortcut("flipHorizontallyShortcut", app.settings()->flipHorizontallyShortcut());
}

bool TestHelper::triggerFlipVertically()
{
    return triggerShortcut("flipVerticallyShortcut", app.settings()->flipVerticallyShortcut());
}

bool TestHelper::triggerSelectAll()
{
    return triggerShortcut("selectAllShortcut", QKeySequence(QKeySequence::SelectAll).toString());
}

bool TestHelper::triggerCentre()
{
    return triggerShortcut("centreShortcut", app.settings()->centreShortcut());
}

bool TestHelper::triggerGridVisible()
{
    return triggerShortcut("gridVisibleShortcut", app.settings()->gridVisibleShortcut());
}

bool TestHelper::triggerRulersVisible()
{
    return triggerShortcut("rulersVisibleShortcut", app.settings()->rulersVisibleShortcut());
}

bool TestHelper::triggerGuidesVisible()
{
    return triggerShortcut("guidesVisibleShortcut", app.settings()->guidesVisibleShortcut());
}

bool TestHelper::triggerSplitScreen()
{
    return triggerShortcut("splitScreenShortcut", app.settings()->splitScreenShortcut());
}

bool TestHelper::triggerSplitterLocked()
{
    return triggerShortcut("splitterLockedShortcut", app.settings()->splitterLockedShortcut());
}

bool TestHelper::setSplitterLocked(bool splitterLocked)
{
    if (canvas->splitter()->isEnabled() != !splitterLocked) {
        if (!triggerSplitterLocked())
            return false;
        VERIFY(canvas->splitter()->isEnabled() == !splitterLocked);
    }
    return true;
}

bool TestHelper::triggerAnimationPlayback()
{
    return triggerShortcut("animationPlaybackShortcut", app.settings()->animationPlaybackShortcut());
}

bool TestHelper::setAnimationPlayback(bool usingAnimation)
{
    if (isUsingAnimation() != usingAnimation) {
        if (!triggerAnimationPlayback())
            return false;
        VERIFY(isUsingAnimation() == usingAnimation);
    }
    return true;
}

bool TestHelper::triggerOptions()
{
    return triggerShortcut("optionsShortcut", app.settings()->optionsShortcut());
}

bool TestHelper::selectLayer(const QString &layerName, int layerIndex)
{
    TRY_VERIFY(findListViewChild("layerListView", layerName));
    QQuickItem *layerDelegate = findListViewChild("layerListView", layerName);
    VERIFY(layerDelegate);
    mouseEventOnCentre(layerDelegate, MouseClick);
    VERIFY(layerDelegate->property("checked").toBool());
    VERIFY2(layeredImageProject->currentLayerIndex() == layerIndex,
        qPrintable(QString::fromLatin1("Expected currentLayerIndex to be %1 after selecting it, but it's %2")
            .arg(layerIndex).arg(layeredImageProject->currentLayerIndex())));
    return true;
}

bool TestHelper::verifyLayerName(const QString &layerName, QQuickItem **layerDelegate)
{
    // It seems that the ListView sometimes need some extra time to create the second item (e.g. when debugging).
    TRY_VERIFY(findListViewChild("layerListView", layerName));
    QQuickItem *delegate = findListViewChild("layerListView", layerName);
    VERIFY(delegate);
    QQuickItem *layerDelegateNameTextField = delegate->findChild<QQuickItem*>("layerNameTextField");
    VERIFY(layerDelegateNameTextField);
    VERIFY(layerDelegateNameTextField->property("text").toString() == layerName);
    if (layerDelegate)
        *layerDelegate = delegate;
    return true;
}

bool TestHelper::makeCurrentAndRenameLayer(const QString &from, const QString &to)
{
    QQuickItem *layerDelegate = nullptr;
    VERIFY2(verifyLayerName(from, &layerDelegate), failureMessage);

    mouseEventOnCentre(layerDelegate, MouseClick);
    VERIFY(layerDelegate->property("checked").toBool() == true);
    VERIFY(layeredImageProject->currentLayer()->name() == from);

    QQuickItem *nameTextField = layerDelegate->findChild<QQuickItem*>("layerNameTextField");
    VERIFY(nameTextField);

    // A single click should not give the text field focus.
    mouseEventOnCentre(nameTextField, MouseClick);
    VERIFY(!nameTextField->hasActiveFocus());

    // A double click should.
    mouseEventOnCentre(nameTextField, MouseDoubleClick);
    VERIFY(nameTextField->hasActiveFocus() == true);

    // Enter the text.
    QTest::keySequence(window, QKeySequence(QKeySequence::SelectAll));
    foreach (const auto character, to)
        QTest::keyClick(window, character.toLatin1());
    VERIFY2(nameTextField->property("text").toString() == to, qPrintable(QString::fromLatin1(
        "Expected layerNameTextField to contain \"%1\" after inputting new layer name, but it contains \"%2\"")
            .arg(to, nameTextField->property("text").toString())));
    VERIFY(layeredImageProject->currentLayer()->name() == from);

    // Confirm the changes.
    QTest::keyClick(window, Qt::Key_Enter);
    VERIFY2(nameTextField->property("text").toString() == to, qPrintable(QString::fromLatin1(
        "Expected layerNameTextField to contain \"%1\" after confirming changes, but it contains \"%2\"")
            .arg(to, nameTextField->property("text").toString())));
    VERIFY(layeredImageProject->currentLayer()->name() == to);

    return true;
}

bool TestHelper::changeLayerVisiblity(const QString &layerName, bool visible)
{
    QQuickItem *layerDelegate = nullptr;
    VERIFY2(verifyLayerName(layerName, &layerDelegate), failureMessage);

    const ImageLayer *layer = layeredImageProject->layerAt(layerName);
    VERIFY(layer);

    QQuickItem *layerVisibilityCheckBox = layerDelegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    VERIFY(layerVisibilityCheckBox);

    // Sanity check that the check box's state matches the layer's state.
    const bool oldLayerVisibilityCheckBoxValue = layerVisibilityCheckBox->property("checked").toBool();
    VERIFY(oldLayerVisibilityCheckBoxValue == !layer->isVisible());

    // If the layer's visibility already matches the target visibility, we have nothing to do.
    if (oldLayerVisibilityCheckBoxValue == !visible)
        return true;

    // Hide the layer.
    mouseEventOnCentre(layerVisibilityCheckBox, MouseClick);
    VERIFY(layer->isVisible() == visible);
    VERIFY(layerVisibilityCheckBox->property("checked").toBool() == !visible);

    return true;
}

void TestHelper::addAllProjectTypes()
{
    QTest::addColumn<Project::Type>("projectType");

    QTest::newRow("TilesetType") << Project::TilesetType;
    QTest::newRow("ImageType") << Project::ImageType;
    QTest::newRow("LayeredImageType") << Project::LayeredImageType;
}

void TestHelper::addImageProjectTypes()
{
    QTest::addColumn<Project::Type>("projectType");

    QTest::newRow("ImageType") << Project::ImageType;
    QTest::newRow("LayeredImageType") << Project::LayeredImageType;
}

void TestHelper::addActualProjectTypes()
{
    QTest::addColumn<Project::Type>("projectType");
    QTest::addColumn<QString>("projectExtension");

    QTest::newRow("TilesetType") << Project::TilesetType << "stp";
    QTest::newRow("LayeredImageType") << Project::LayeredImageType << "slp";
}

bool TestHelper::createNewProject(Project::Type projectType, const QVariantMap &args)
{
    const bool isTilesetProject = projectType == Project::TilesetType;

    // tileset args
    const int tileWidth = isTilesetProject ? args.value("tileWidth", 25).toInt() : 0;
    const int tileHeight = isTilesetProject ? args.value("tileHeight", 25).toInt() : 0;
    const int tilesetTilesWide = isTilesetProject ? args.value("tilesetTilesWide", -1).toInt() : 0;
    const int tilesetTilesHigh = isTilesetProject ? args.value("tilesetTilesHigh", -1).toInt() : 0;
    const bool transparentTilesetBackground = isTilesetProject ? args.value("transparentTilesetBackground", false).toBool() : false;

    // image args
    const int imageWidth = !isTilesetProject ? args.value("imageWidth", 256).toInt() : 0;
    const int imageHeight = !isTilesetProject ? args.value("imageHeight", 256).toInt() : 0;
    const bool transparentImageBackground = !isTilesetProject ? args.value("transparentImageBackground", false).toBool() : false;

    if (!project) {
        // Hovering over the canvas with no project open should result in the default cursor being displayed.
        QTest::mouseMove(window, QPointF(window->width() / 2, window->height() / 2).toPoint());
        VERIFY(window->cursor().shape() == Qt::ArrowCursor);
    }

    if (projectCreationFailedSpy)
        projectCreationFailedSpy->clear();

    // Click the new project button.
    if (!triggerNewProject())
        return false;

    // Check that we get prompted to discard any changes.
    if (project && project->hasUnsavedChanges()) {
        if (!discardChanges())
            return false;
    }

    // Ensure that the new project popup is visible.
    const QObject *newProjectPopup = findPopupFromTypeName("NewProjectPopup");
    VERIFY(newProjectPopup);
    VERIFY(newProjectPopup->property("visible").toBool());
    // TODO: remove this when https://bugreports.qt.io/browse/QTBUG-53420 is fixed
    newProjectPopup->property("contentItem").value<QQuickItem*>()->forceActiveFocus();
    ENSURE_ACTIVE_FOCUS(newProjectPopup);

    QString newProjectButtonObjectName;
    if (projectType == Project::TilesetType) {
        newProjectButtonObjectName = QLatin1String("tilesetProjectButton");
    } else if (projectType == Project::ImageType) {
        newProjectButtonObjectName = QLatin1String("imageProjectButton");
    } else {
        newProjectButtonObjectName = QLatin1String("layeredImageProjectButton");
    }

    // Click on the appropriate project type button.
    QQuickItem *tilesetProjectButton = newProjectPopup->findChild<QQuickItem*>(newProjectButtonObjectName);
    VERIFY(tilesetProjectButton);

    mouseEventOnCentre(tilesetProjectButton, MouseClick);
    VERIFY(tilesetProjectButton->property("checked").toBool());

    TRY_VERIFY(!newProjectPopup->property("visible").toBool());

    if (projectType == Project::TilesetType) {
        // Create a temporary directory containing a tileset image for us to use.
        if (!setupTempTilesetProjectDir())
            return false;

        // Now the New Tileset Project popup should be visible.
        TRY_VERIFY(findPopupFromTypeName("NewTilesetProjectPopup"));
        const QObject *newTilesetProjectPopup = findPopupFromTypeName("NewTilesetProjectPopup");
        VERIFY(newTilesetProjectPopup->property("visible").toBool());

        // Ensure that the popup gets reset each time it's opened.
        QQuickItem *useExistingTilesetCheckBox = newTilesetProjectPopup->findChild<QQuickItem*>("useExistingTilesetCheckBox");
        VERIFY(useExistingTilesetCheckBox);
        VERIFY(useExistingTilesetCheckBox->property("checked").isValid());
        VERIFY(!useExistingTilesetCheckBox->property("checked").toBool());

        QQuickItem *tilesetField = newTilesetProjectPopup->findChild<QQuickItem*>("tilesetPathTextField");
        VERIFY(tilesetField);
        VERIFY(tilesetField->property("text").isValid());
        VERIFY(tilesetField->property("text").toString().isEmpty());

        QQuickItem *tileWidthSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tileWidthSpinBox");
        VERIFY(tileWidthSpinBox);
        VERIFY(tileWidthSpinBox->property("value").toInt() == 32);

        QQuickItem *tileHeightSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tileHeightSpinBox");
        VERIFY(tileHeightSpinBox);
        VERIFY(tileHeightSpinBox->property("value").toInt() == 32);

        QQuickItem *tilesWideSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tilesWideSpinBox");
        VERIFY(tilesWideSpinBox);
        VERIFY(tilesWideSpinBox->property("value").toInt() == 10);

        QQuickItem *tilesHighSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tilesHighSpinBox");
        VERIFY(tilesHighSpinBox);
        VERIFY(tilesHighSpinBox->property("value").toInt() == 10);

        VERIFY(tileWidthSpinBox->setProperty("value", tileWidth));
        VERIFY(tileWidthSpinBox->property("value").toInt() == tileWidth);

        VERIFY(tileHeightSpinBox->setProperty("value", tileHeight));
        VERIFY(tileHeightSpinBox->property("value").toInt() == tileHeight);

        if (tilesetTilesWide == -1 || tilesetTilesHigh == -1) {
            // Using an existing tileset image.

            // Check the "use existing tileset" checkbox.
            // Hack: it has focus in the app, but not in the test => meh
            useExistingTilesetCheckBox->forceActiveFocus();
            VERIFY(useExistingTilesetCheckBox->hasActiveFocus());
            QTest::keyClick(window, Qt::Key_Space);
            VERIFY(useExistingTilesetCheckBox->property("checked").toBool());

            // Input the filename.
            QTest::keyClick(window, Qt::Key_Tab);
            VERIFY(tilesetField->hasActiveFocus());
            // TODO: input path using events
            VERIFY(tilesetField->property("text").isValid());
            VERIFY(tilesetField->setProperty("text", tempTilesetUrl.toString()));

            QQuickItem *invalidFileIcon = newTilesetProjectPopup->findChild<QQuickItem*>("invalidFileIcon");
            VERIFY(invalidFileIcon);
            QObject *fileValidator = newTilesetProjectPopup->findChild<QObject*>("validator");
            VERIFY2(qFuzzyCompare(invalidFileIcon->property("opacity").toReal(), 0),
                qPrintable(QString::fromLatin1("File should be valid, but got error message \"%1\"; file path: %2")
                    .arg(fileValidator->property("errorMessage").toString())
                    .arg(tilesetField->property("text").toString())));
        } else {
            // Using a new tileset image.
            VERIFY(tilesetTilesWide > 0 && tilesetTilesHigh > 0);

            VERIFY(tilesWideSpinBox->setProperty("value", tilesetTilesWide));
            VERIFY(tilesWideSpinBox->property("value").toInt() == tilesetTilesWide);

            VERIFY(tilesHighSpinBox->setProperty("value", tilesetTilesHigh));
            VERIFY(tilesHighSpinBox->property("value").toInt() == tilesetTilesHigh);

            QQuickItem *transparentBackgroundCheckBox = newTilesetProjectPopup->findChild<QQuickItem*>("transparentBackgroundCheckBox");
            VERIFY(transparentBackgroundCheckBox);
            VERIFY(transparentBackgroundCheckBox->property("checked").toBool());

            if (!transparentTilesetBackground) {
                mouseEventOnCentre(transparentBackgroundCheckBox, MouseClick);
                VERIFY(!transparentBackgroundCheckBox->property("checked").toBool());
            }
        }

        // Confirm creation of the project.
        QQuickItem *okButton = newTilesetProjectPopup->findChild<QQuickItem*>("newTilesetProjectOkButton");
        VERIFY(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        VERIFY(!newTilesetProjectPopup->property("visible").toBool());
    } else {
        // Create a temporary directory that we can save into, etc.
        if (projectType == Project::LayeredImageType) {
            if (!setupTempLayeredImageProjectDir())
                return false;
        }

        // Now the New Image Project popup should be visible.
        TRY_VERIFY(findPopupFromTypeName("NewImageProjectPopup"));
        const QObject *newImageProjectPopup = findPopupFromTypeName("NewImageProjectPopup");
        VERIFY(newImageProjectPopup->property("visible").toBool());

        // Ensure that the popup gets reset each time it's opened.
        QQuickItem *imageWidthSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageWidthSpinBox");
        VERIFY(imageWidthSpinBox);
        VERIFY(imageWidthSpinBox->property("value").toInt() == 256);

        QQuickItem *imageHeightSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageHeightSpinBox");
        VERIFY(imageHeightSpinBox);
        VERIFY(imageHeightSpinBox->property("value").toInt() == 256);

        VERIFY(imageWidthSpinBox->setProperty("value", imageWidth));
        VERIFY(imageWidthSpinBox->property("value").toInt() == imageWidth);

        VERIFY(imageHeightSpinBox->setProperty("value", imageHeight));
        VERIFY(imageHeightSpinBox->property("value").toInt() == imageHeight);

        QQuickItem *transparentImageBackgroundCheckBox = newImageProjectPopup->findChild<QQuickItem*>("transparentImageBackgroundCheckBox");
        VERIFY(transparentImageBackgroundCheckBox);
        VERIFY(transparentImageBackgroundCheckBox->property("checked").isValid());
        VERIFY(!transparentImageBackgroundCheckBox->property("checked").toBool());

        if (transparentImageBackground) {
            mouseEventOnCentre(transparentImageBackgroundCheckBox, MouseClick);
            VERIFY(transparentImageBackgroundCheckBox->property("checked").toBool() == transparentImageBackground);
        }

        // Confirm creation of the project.
        QQuickItem *okButton = newImageProjectPopup->findChild<QQuickItem*>("newImageProjectOkButton");
        VERIFY(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        TRY_VERIFY(!newImageProjectPopup->property("visible").toBool());
    }

    // When run after e.g. selectionToolTileCanvas(), cancelSelectionToolImageCanvas()
    // would randomly fail when it first tries to switch tools. selectionToolButton is visible,
    // enabled, and its x position doesn't change during the test, but using qt.quick.mouse
    // and qt.quick.mouse.target logging categories to check, it seems that toolSizeButton was getting
    // the mouse event. I'm still not sure what causes the issue, but giving everything
    // a chance to settle down helps.
    QTest::qWait(0);

    return updateVariables(true, projectType);
}

bool TestHelper::createNewTilesetProject(int tileWidth, int tileHeight, int tilesetTilesWide, int tilesetTilesHigh,
    bool transparentTilesetBackground)
{
    QVariantMap args;
    args.insert("tileWidth", tileWidth);
    args.insert("tileHeight", tileHeight);
    args.insert("tilesetTilesWide", tilesetTilesWide);
    args.insert("tilesetTilesHigh", tilesetTilesHigh);
    args.insert("transparentTilesetBackground", transparentTilesetBackground);
    return createNewProject(Project::TilesetType, args);
}

bool TestHelper::createNewImageProject(int imageWidth, int imageHeight, bool transparentImageBackground)
{
    QVariantMap args;
    args.insert("imageWidth", imageWidth);
    args.insert("imageHeight", imageHeight);
    args.insert("transparentImageBackground", transparentImageBackground);

    if (!createNewProject(Project::ImageType, args))
        return false;

    return true;
}

bool TestHelper::createNewLayeredImageProject(int imageWidth, int imageHeight, bool transparentImageBackground)
{
    QVariantMap args;
    args.insert("imageWidth", imageWidth);
    args.insert("imageHeight", imageHeight);
    args.insert("transparentImageBackground", transparentImageBackground);
    if (!createNewProject(Project::LayeredImageType, args))
        return false;

    VERIFY(layeredImageProject->layerCount() == 1);
    VERIFY(layeredImageProject->currentLayerIndex() == 0);
    VERIFY(layeredImageProject->currentLayer());
    VERIFY(layeredImageProject->layerAt(0)->name() == QLatin1String("Layer 1"));

    {
        // Ensure that what the user sees (the delegate) is correct.
        QQuickItem *layer1Delegate = findListViewChild("layerListView", QLatin1String("Layer 1"));
        VERIFY(layer1Delegate);

        QQuickItem *nameTextField = layer1Delegate->findChild<QQuickItem*>("layerNameTextField");
        VERIFY(nameTextField);
        VERIFY(nameTextField->property("text").toString() == QLatin1String("Layer 1"));
    }

    newLayerButton = window->findChild<QQuickItem*>("newLayerButton");
    VERIFY(newLayerButton);

    duplicateLayerButton = window->findChild<QQuickItem*>("duplicateLayerButton");
    VERIFY(duplicateLayerButton);

    moveLayerUpButton = window->findChild<QQuickItem*>("moveLayerUpButton");
    VERIFY(moveLayerUpButton);

    moveLayerDownButton = window->findChild<QQuickItem*>("moveLayerDownButton");
    VERIFY(moveLayerDownButton);

    return true;
}

bool TestHelper::loadProject(const QUrl &url, const QRegularExpression &expectedFailureMessage)
{
    if (projectCreationFailedSpy)
        projectCreationFailedSpy->clear();

    QQuickWindow *window = qobject_cast<QQuickWindow*>(app.qmlEngine()->rootObjects().first());
    VERIFY(window);

    // Load it.
    VERIFY(QMetaObject::invokeMethod(window, "loadProject", Qt::DirectConnection, Q_ARG(QVariant, url)));

    if (!expectedFailureMessage.isValid() || expectedFailureMessage.pattern().isEmpty()) {
        // Expect success.
        VERIFY_NO_CREATION_ERRORS_OCCURRED();
        return updateVariables(false, projectManager->projectTypeForUrl(url));
    }

    // Expect failure.
    VERIFY2(!projectCreationFailedSpy->isEmpty() && expectedFailureMessage.match(projectCreationFailedSpy->first().first().toString()).hasMatch(),
        qPrintable(QString::fromLatin1("Expected failure to load project %1 with the following error message:\n%2\nBut got:\n%3")
            .arg(url.path()).arg(expectedFailureMessage.pattern()).arg(projectCreationFailedSpy->first().first().toString())));

    const QObject *errorPopup = findPopupFromTypeName("ErrorPopup");
    VERIFY(errorPopup);
    VERIFY(errorPopup->property("visible").toBool());
    VERIFY(expectedFailureMessage.match(errorPopup->property("text").toString()).hasMatch());
    VERIFY(errorPopup->property("focus").isValid());
    VERIFY(errorPopup->property("focus").toBool());

    QPoint mousePos;
    if (canvas) {
        canvas->mapToScene(QPointF(10, 10)).toPoint();
    } else {
        mousePos = QPoint(100, 100);
    }

    // Check that the cursor goes back to an arrow when there's a modal popup visible,
    // even if the mouse is over the canvas and not the popup.
    QTest::mouseMove(window, mousePos);
    if (canvas)
        VERIFY(!canvas->hasActiveFocus());
    VERIFY(window->cursor().shape() == Qt::ArrowCursor);

    // Close the error message popup.
    // QTest::mouseClick(window, Qt::LeftButton) didn't work on mac after a couple of data row runs,
    // so we use the keyboard to close it instead.
    QTest::keyClick(window, Qt::Key_Escape);
    VERIFY(!errorPopup->property("visible").toBool());

    if (projectCreationFailedSpy)
        projectCreationFailedSpy->clear();

    return true;
}

bool TestHelper::updateVariables(bool isNewProject, Project::Type projectType)
{
    // The projects and canvases that we had references to should have
    // been destroyed by now.
    TRY_VERIFY(!project);
    VERIFY(!imageProject);
    VERIFY(!tilesetProject);

    TRY_VERIFY(!canvas);
    VERIFY(!imageCanvas);
    VERIFY(!tileCanvas);

    project = projectManager->project();
    VERIFY(project);

    // The old canvas might still exist, but if it doesn't have a project,
    // then it's about to be destroyed, so wait for that to happen first
    // before proceeding.
    canvas = window->findChild<ImageCanvas*>();
    if (!canvas->project())
        TRY_VERIFY(!canvas);

    canvas = window->findChild<ImageCanvas*>();
    VERIFY(canvas);
    TRY_VERIFY(canvas->window());

    animationPlayPauseButton = window->findChild<QQuickItem*>("animationPlayPauseButton");
    if (projectType == Project::ImageType || projectType == Project::LayeredImageType)
        VERIFY(animationPlayPauseButton);

    if (isNewProject) {
        // The old default was to split the screen,
        // and so the tests might be depending on it to be split.
        // Also, it's good to ensure that it's tested.
        canvas->setSplitScreen(true);
        VERIFY(canvas->isSplitScreen());
    }
    VERIFY(splitScreenToolButton->isEnabled() == true);
    VERIFY(splitScreenToolButton->property("checked").toBool() == canvas->isSplitScreen());
    VERIFY(lockSplitterToolButton->isEnabled() == canvas->isSplitScreen());
    VERIFY(lockSplitterToolButton->property("checked").toBool() == !canvas->splitter()->isEnabled());

    // This is the old default. Some tests seem to choke with it set to true
    // (because of wheel events) and I haven't looked into it yet because it's not really important.
    canvas->setGesturesEnabled(false);

    canvas->setPenForegroundColour(Qt::black);
    canvas->setPenBackgroundColour(Qt::white);
    // This determines which colour the ColourSelector considers "current",
    // and hence which value is shown in the hex field.
    VERIFY(penForegroundColourButton->setProperty("checked", QVariant(true)));
    canvas->setToolShape(ImageCanvas::SquareToolShape);

    app.settings()->setAutoSwatchEnabled(false);
    app.settings()->setPenToolRightClickBehaviour(app.settings()->defaultPenToolRightClickBehaviour());

    if (projectType == Project::TilesetType) {
        tilesetProject = qobject_cast<TilesetProject*>(project);
        VERIFY(tilesetProject);

        tileCanvas = qobject_cast<TileCanvas*>(canvas);
        VERIFY(tileCanvas);
    } else if (projectType == Project::ImageType) {
        imageProject = qobject_cast<ImageProject*>(project);
        VERIFY(imageProject);

        imageCanvas = canvas;
    } else if (projectType == Project::LayeredImageType) {
        layeredImageProject = qobject_cast<LayeredImageProject*>(project);
        VERIFY(layeredImageProject);

        layeredImageCanvas = qobject_cast<LayeredImageCanvas*>(canvas);;
    }

    canvas->forceActiveFocus();
//    TRY_VERIFY(canvas->hasActiveFocus());

    VERIFY(project->hasLoaded());

    if (isNewProject) {
        VERIFY(project->url() == QUrl());
        VERIFY(project->isNewProject());

        // Reset any settings that have changed back to their defaults.
        QVariant settingsAsVariant = qmlEngine(canvas)->rootContext()->contextProperty("settings");
        VERIFY(settingsAsVariant.isValid());
        ApplicationSettings *settings = settingsAsVariant.value<ApplicationSettings*>();
        VERIFY(settings);
        settings->resetShortcutsToDefaults();

        if (settings->areRulersVisible()) {
            if (!triggerRulersVisible())
                return false;
            VERIFY(settings->areRulersVisible() == false);
        }

        cursorPos = QPoint();
        cursorWindowPos = QPoint();
    }

    // Sanity check.
    TRY_VERIFY(canvas->window() == canvasSizeButton->window());
    VERIFY(!canvas->splitter()->isPressed());
    VERIFY(!canvas->splitter()->isHovered());

    if (isNewProject) {
        VERIFY(canvas->splitter()->position() == 0.5);
    }

    if (projectType == Project::TilesetType) {
        // Establish references to TilesetProject-specific properties.
        tilesetSwatchPanel = window->findChild<QQuickItem*>("tilesetSwatchPanel");
        VERIFY(tilesetSwatchPanel);
        VERIFY(tilesetSwatchPanel->isVisible() == true);
        VERIFY(!qFuzzyIsNull(tilesetSwatchPanel->width()));
        // This started failing recently for some reason, but giving it some time seems to help.
        TRY_VERIFY(!qFuzzyIsNull(tilesetSwatchPanel->height()));

        // Ensure that the tileset swatch flickable has the correct contentY.
        tilesetSwatchFlickable = tilesetSwatchPanel->findChild<QQuickItem*>("tilesetSwatchFlickable");
        VERIFY(tilesetSwatchFlickable);
        VERIFY(tilesetSwatchFlickable->property("contentY").isValid());
        VERIFY(tilesetSwatchFlickable->property("contentY").toReal() == 0.0);

        VERIFY(imageGrabber.requestImage(tileCanvas));
        TRY_VERIFY(imageGrabber.isReady());
        VERIFY(imageGrabber.takeImage().pixelColor(16, 16) == tileCanvas->mapBackgroundColour());
    } else {
        VERIFY(window->findChild<QQuickItem*>("selectionToolButton"));
    }

    // Make sure we call any functions that use the variables we set last.

    // Having too many panels can result in the panel that a particular test is interested in being
    // too small, meaning certain tool buttons cannot be clicked because they're hidden.
    // It's more reliable to just show the relevant panels where necessary.
    if (!collapseAllPanels())
        return false;

    if (isNewProject) {
        // If we're not loading a saved project (where we'd want to use the saved split sizes),
        // return the SplitView back to roughly its original size.
        // It doesn't matter if it's not perfect, it's just important that it's not already
        // at its max width so that the tests can resize it.
        if (!dragSplitViewHandle("mainSplitView", 0, QPoint(window->width() - 240, window->height() / 2)))
            return false;

        // Restore the colour panel's contentY to the default.
        QQuickItem *colourPanelFlickable = window->findChild<QQuickItem*>("colourPanelFlickable");
        VERIFY(colourPanelFlickable);
        VERIFY(colourPanelFlickable->setProperty("contentY", 0));
    }

    return true;
}

bool TestHelper::discardChanges()
{
    const QObject *discardChangesDialog = window->contentItem()->findChild<QObject*>("discardChangesDialog");
    VERIFY(discardChangesDialog);
    TRY_VERIFY(discardChangesDialog->property("opened").toBool());

    QQuickItem *discardChangesButton = findDialogButtonFromObjectName(discardChangesDialog, "discardChangesDialogButton");
    VERIFY(discardChangesButton);
    mouseEventOnCentre(discardChangesButton, MouseClick);
    TRY_VERIFY(!discardChangesDialog->property("visible").toBool());
    return true;
}

bool TestHelper::verifyErrorAndDismiss(const QString &expectedErrorMessage)
{
    QObject *errorDialog = findPopupFromTypeName("ErrorPopup");
    VERIFY(errorDialog);
    TRY_VERIFY(errorDialog->property("opened").toBool());

    // Save the error message so that we can dismiss the dialog beforehand.
    // This way, if the message comparison fails, the dialog won't interfere
    // with the next test.
    const QString errorMessage = errorDialog->property("text").toString();

    QTest::keyClick(window, Qt::Key_Escape);
    TRY_VERIFY(!errorDialog->property("visible").toBool());

    VERIFY2(errorMessage.contains(expectedErrorMessage), qPrintable(QString::fromLatin1(
        "Error message does not contain expected error message: %1").arg(errorMessage)));

    return true;
}

bool TestHelper::verifyNoErrorOrDismiss()
{
    QObject *errorDialog = findPopupFromTypeName("ErrorPopup");
    if (!errorDialog)
        return true;

    const bool wasVisible = errorDialog->property("visible").toBool();
    QString errorMessage;
    if (wasVisible) {
        errorMessage = errorDialog->property("text").toString();
        // Dismissing ensures that the dialog doesn't interfere with the next test.
        QTest::keyClick(window, Qt::Key_Escape);
        TRY_VERIFY(!errorDialog->property("visible").toBool());
    }

    VERIFY2(!wasVisible, qPrintable(QLatin1String("Expected no error, but got: ") + errorMessage));
    return true;
}

bool TestHelper::copyFileFromResourcesToTempProjectDir(const QString &baseName)
{
    QFile sourceFile(":/resources/" + baseName);
    VERIFY2(sourceFile.open(QIODevice::ReadOnly), qPrintable(QString::fromLatin1(
        "Failed to open %1: %2").arg(sourceFile.fileName()).arg(sourceFile.errorString())));
    const bool isEmpty = sourceFile.readAll().isEmpty();
    sourceFile.close();

    const QString saveFilePath = tempProjectDir->path() + "/" + baseName;
    VERIFY2(QFile::copy(sourceFile.fileName(), saveFilePath), qPrintable(QString::fromLatin1(
        "Failed to copy %1 to %2: %3").arg(sourceFile.fileName()).arg(saveFilePath).arg(sourceFile.errorString())));

    // A file copied from a file that is part of resources will always be read-only...
    QFile copiedFile(saveFilePath);
    if (!isEmpty)
        VERIFY(copiedFile.size() > 0);
    VERIFY(copiedFile.setPermissions(QFile::ReadUser | QFile::WriteUser));
    VERIFY2(copiedFile.open(QIODevice::ReadWrite), qPrintable(QString::fromLatin1(
        "Error opening file at %1: %2").arg(saveFilePath).arg(copiedFile.errorString())));
    return true;
}

bool TestHelper::setupTempTilesetProjectDir()
{
    QStringList toCopy;
    toCopy << tilesetBasename;
    // More stuff here.

    QStringList copiedPaths;
    if (!setupTempProjectDir(toCopy, &copiedPaths))
        return false;

    tempTilesetUrl = QUrl::fromLocalFile(copiedPaths.at(0));
    // More stuff here.

    return true;
}

bool TestHelper::setupTempLayeredImageProjectDir()
{
    return setupTempProjectDir();
}

bool TestHelper::setupTempProjectDir(const QStringList &resourceFilesToCopy, QStringList *filesCopied)
{
    tempProjectDir.reset(new QTemporaryDir);
    VERIFY2(tempProjectDir->isValid(), qPrintable(tempProjectDir->errorString()));

    foreach (const QString &baseName, resourceFilesToCopy) {
        if (!copyFileFromResourcesToTempProjectDir(baseName))
            return false;

        if (filesCopied) {
            const QString saveFilePath = tempProjectDir->path() + "/" + baseName;
            *filesCopied << saveFilePath;
        }
    }

    return true;
}

bool TestHelper::isPanelExpanded(const QString &panelObjectName)
{
    QQuickItem *panel = window->findChild<QQuickItem*>(panelObjectName);
    VERIFY(panel);
    return panel->property("expanded").toBool();
}

bool TestHelper::collapseAllPanels()
{
    if (project->type() == Project::TilesetType) {
        if (!togglePanel("tilesetSwatchPanel", false))
            return false;
    } else if (project->type() == Project::LayeredImageType) {
        if (!togglePanel("layerPanel", false))
            return false;
    }

    if (!togglePanel("colourPanel", false))
        return false;

    if (!togglePanel("swatchesPanel", false))
        return false;

    if (project->type() == Project::ImageType || project->type() == Project::LayeredImageType) {
        // Don't change the expanded state if the panel is not even visible,
        // as we want it to be the default (true) for tests involving animation playback
        // to ensure that it shows when enabled.
        QQuickItem *animationPanel = window->findChild<QQuickItem*>("animationPanel");
        VERIFY(animationPanel);
        if (animationPanel->isVisible() && !togglePanel("animationPanel", false))
            return false;
    }

    return true;
}

bool TestHelper::togglePanel(const QString &panelObjectName, bool expanded)
{
    QQuickItem *panel = window->findChild<QQuickItem*>(panelObjectName);
    VERIFY(panel);

    if (panel->property("expanded").toBool() == expanded)
        return true;

    const qreal originalHeight = panel->height();
    VERIFY(panel->setProperty("expanded", QVariant(expanded)));
    VERIFY(panel->property("expanded").toBool() == expanded);
    if (expanded) {
        // Ensure that it has time to grow, otherwise stuff like input events will not work.
        TRY_VERIFY2(panel->height() > originalHeight, qPrintable(QString::fromLatin1(
            "Expected expanded height of %1 to be larger than collapsed height of %2, but it wasn't")
                .arg(panelObjectName).arg(originalHeight)));
    } else {
        // If it's not visible, it's height might not update until it's visible again, apparently.
        // That's fine with us.
        if (panel->isVisible()) {
            // Ensure that the panel isn't visually expanded. We don't want to hard-code what the collapsed height
            // is, so we just make sure it's less than some large height.
            TRY_VERIFY2(panel->height() < 100, qPrintable(QString::fromLatin1(
                "Expected collapsed height of %1 to be small, but it's %2")
                    .arg(panelObjectName).arg(panel->height())));
        }
    }
    return true;
}

bool TestHelper::dragSplitViewHandle(const QString &splitViewObjectName, int index,
    const QPoint &newHandleCentreRelativeToSplitView, QPoint *oldHandleCentreRelativeToSplitView)
{
    QPointer<QQuickItem> splitView = window->findChild<QQuickItem*>(splitViewObjectName);
    VERIFY(splitView);

    QPointer<QQuickItem> handleItem = findSplitViewHandle(splitViewObjectName, index);
    VERIFY(handleItem);

    const QPoint handleRelativeCentre = handleItem->mapToItem(splitView, QPointF(
        handleItem->width() / 2, handleItem->height() / 2)).toPoint();

    if (oldHandleCentreRelativeToSplitView)
        *oldHandleCentreRelativeToSplitView = handleRelativeCentre;

    const QPoint handleCentre = splitView->mapToScene(QPointF(
        handleRelativeCentre.x(), handleRelativeCentre.y())).toPoint();
    const QPoint newCentre = splitView->mapToScene(QPointF(
        newHandleCentreRelativeToSplitView.x(), newHandleCentreRelativeToSplitView.y())).toPoint();

    QTest::mouseMove(window, handleCentre);
    QQmlProperty handleHoveredProperty(handleItem.data(), "SplitHandle.hovered", qmlContext(handleItem.data()));
    VERIFY(handleHoveredProperty.isValid());
    VERIFY(handleHoveredProperty.read().toBool() == true);
    QQmlProperty handlePressedProperty(handleItem.data(), "SplitHandle.pressed", qmlContext(handleItem.data()));
    VERIFY(handlePressedProperty.isValid());
    VERIFY(handlePressedProperty.read().toBool() == false);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, handleCentre);
    VERIFY(handlePressedProperty.read().toBool() == true);

    QTest::mouseMove(window, newCentre);
    VERIFY(handlePressedProperty.read().toBool() == true);

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, newCentre);
    VERIFY(handleHoveredProperty.read().toBool() == true);
    VERIFY(handlePressedProperty.read().toBool() == false);
    return true;
}

bool TestHelper::togglePanels(const QStringList &panelObjectNames, bool expanded)
{
    for (const QString &panelObjectName : qAsConst(panelObjectNames)) {
        if (!togglePanel(panelObjectName, expanded))
            return false;
    }
    return true;
}

bool TestHelper::expandAndResizePanel(const QString &panelObjectName)
{
    static QVector<QPair<QString, int>> reasonablePanelSizes;
    if (reasonablePanelSizes.isEmpty()) {
        QQuickItem *mainSplitView = window->findChild<QQuickItem*>("mainSplitView");
        VERIFY(mainSplitView);

        // Make them kinda big since there's usually only one visible at a time.
        reasonablePanelSizes.append(qMakePair(QLatin1String("colourPanel"), mainSplitView->height() * 0.5));
        reasonablePanelSizes.append(qMakePair(QLatin1String("swatchesPanel"), mainSplitView->height() * 0.2));
        reasonablePanelSizes.append(qMakePair(QLatin1String("tilesetSwatchPanel"), mainSplitView->height() * 0.5));
        reasonablePanelSizes.append(qMakePair(QLatin1String("layerPanel"), mainSplitView->height() * 0.5));
        reasonablePanelSizes.append(qMakePair(QLatin1String("animationPanel"), mainSplitView->height() * 0.5));
    }

    const auto panelIt = std::find_if(reasonablePanelSizes.begin(), reasonablePanelSizes.end(), [=](const QPair<QString, int> &pair) {
        return pair.first == panelObjectName;
    });
    VERIFY(panelIt != reasonablePanelSizes.end());

    if (!togglePanel(panelObjectName, true))
        return false;

    QQuickItem *panelSplitView = window->findChild<QQuickItem*>(panelObjectName);
    VERIFY(panelSplitView);
    return dragSplitViewHandle("panelSplitView", std::distance(reasonablePanelSizes.begin(), panelIt),
        QPoint(window->width() - (panelSplitView->width() / 2), panelIt->second));
}

bool TestHelper::switchMode(TileCanvas::Mode mode)
{
    if (tileCanvas->mode() == mode)
        return true;

    mouseEventOnCentre(modeToolButton, MouseClick);
    const bool expectChecked = mode == TileCanvas::TileMode;
    VERIFY2(modeToolButton->property("checked").toBool() == expectChecked, qPrintable(QString::fromLatin1(
        "modeToolButton.checked should be %1, but it's %2 (trying to set mode %3)")
            .arg(expectChecked).arg(modeToolButton->property("checked").toBool()).arg(mode)));
    VERIFY(tileCanvas->mode() == mode);
    return true;
}

bool TestHelper::switchTool(ImageCanvas::Tool tool, InputType inputType)
{
    if (canvas->tool() == tool)
        return true;

    if (inputType == KeyboardInputType)
        ENSURE_ACTIVE_FOCUS(canvas);

    switch (tool) {
    case ImageCanvas::PenTool:
        if (inputType == MouseInputType) {
            VERIFY(penToolButton->isEnabled());
            mouseEventOnCentre(penToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->penToolShortcut());
        }
        break;
    case ImageCanvas::EyeDropperTool:
        if (inputType == MouseInputType) {
            VERIFY(eyeDropperToolButton->isEnabled());
            mouseEventOnCentre(eyeDropperToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->eyeDropperToolShortcut());
        }
        break;
    case ImageCanvas::FillTool:
        if (inputType == MouseInputType) {
            VERIFY(fillToolButton->isEnabled());
            mouseEventOnCentre(fillToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->fillToolShortcut());
        }
        break;
    case ImageCanvas::EraserTool:
        if (inputType == MouseInputType) {
            VERIFY(eraserToolButton->isEnabled());
            mouseEventOnCentre(eraserToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->eraserToolShortcut());
        }
        break;
    case ImageCanvas::SelectionTool:
        if (inputType == MouseInputType) {
            VERIFY(selectionToolButton->isEnabled());
            mouseEventOnCentre(selectionToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->selectionToolShortcut());
        }
        break;
    default:
        qWarning() << "tool not handled!";
        return false;
    }

    VERIFY2(canvas->tool() == tool, qPrintable(QString::fromLatin1(
        "Expected tool %1 but current tool is %2").arg(tool).arg(canvas->tool())));
    return true;
}

bool TestHelper::setPenForegroundColour(QString argbString)
{
    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    VERIFY(hexTextField);

    argbString.replace("#", QString());

    hexTextField->forceActiveFocus();
    QMetaObject::invokeMethod(hexTextField, "selectAll", Qt::DirectConnection);

    for (int i = 0; i < argbString.length(); ++i) {
        QTest::keyClick(window, argbString.at(i).toLatin1());
    }
    QTest::keyClick(window, Qt::Key_Return);
    VERIFY(hexTextField->property("text").toString() == argbString);
    return true;
}

bool TestHelper::panTopLeftTo(int x, int y)
{
    const QPoint panDistance = QPoint(x, y) - canvas->firstPane()->integerOffset();
    return panBy(panDistance.x(), panDistance.y());
}

bool TestHelper::panBy(int xDistance, int yDistance)
{
    QPoint pressPos = firstPaneSceneCentre();
    QTest::mouseMove(window, pressPos);
    VERIFY(canvas->currentPane() == canvas->firstPane());

    // TODO: get image checks working
    //        VERIFY(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        const QImage originalImage = imageGrabber.takeImage();

    const QPoint originalOffset = canvas->currentPane()->integerOffset();
    const QPoint expectedOffset = originalOffset + QPoint(xDistance, yDistance);

    QTest::keyPress(window, Qt::Key_Space);
    VERIFY2(window->cursor().shape() == Qt::OpenHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::OpenHandCursor after Qt::Key_Space press, but got %1").arg(window->cursor().shape())));
    VERIFY(canvas->currentPane()->integerOffset() == originalOffset);
    //        VERIFY(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        // Cursor changed to OpenHandCursor.
    //        QImage currentImage = imageGrabber.takeImage();
    //        VERIFY(currentImage != originalImage);
    //        QImage lastImage = currentImage;

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, pressPos);
    VERIFY2(window->cursor().shape() == Qt::ClosedHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::ClosedHandCursor after mouse press, but got %1").arg(window->cursor().shape())));
    VERIFY(canvas->currentPane()->integerOffset() == originalOffset);
    //        VERIFY(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        currentImage = imageGrabber.takeImage();
    //        // Cursor changed to ClosedHandCursor.
    //        VERIFY(currentImage != lastImage);
    //        lastImage = currentImage;

    QTest::mouseMove(window, pressPos + QPoint(xDistance, yDistance));
    VERIFY2(window->cursor().shape() == Qt::ClosedHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::ClosedHandCursor after mouse move, but got %1").arg(window->cursor().shape())));
    VERIFY(canvas->currentPane()->integerOffset() == expectedOffset);
    //        VERIFY(imageGrabber.requestImage(canvas));
    //        // Pane offset changed.
    //        currentImage = imageGrabber.takeImage();
    //        VERIFY(currentImage != lastImage);
    //        lastImage = currentImage;

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, pressPos + QPoint(xDistance, yDistance));
    VERIFY2(window->cursor().shape() == Qt::OpenHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::ClosedHandCursor after mouse release, but got %1").arg(window->cursor().shape())));
    VERIFY(canvas->currentPane()->integerOffset() == expectedOffset);

    QTest::keyRelease(window, Qt::Key_Space);
    // If we have a selection, the cursor might not be Qt::BlankCursor, and that's OK.
    if (!canvas->hasSelection()) {
        // Move the mouse away from any guides, etc.
        QTest::mouseMove(window, QPoint(0, 0));
        VERIFY2(window->cursor().shape() == Qt::BlankCursor, qPrintable(QString::fromLatin1(
            "Expected Qt::BlankCursor after Qt::Key_Space release, but got %1").arg(window->cursor().shape())));
        QTest::mouseMove(window, pressPos + QPoint(xDistance, yDistance));
    }
    VERIFY(canvas->currentPane()->integerOffset() == expectedOffset);

    return true;
}

bool TestHelper::zoomTo(int zoomLevel)
{
    return zoomTo(zoomLevel, cursorWindowPos);
}

bool TestHelper::zoomTo(int zoomLevel, const QPoint &pos)
{
    CanvasPane *currentPane = canvas->currentPane();
    for (int i = 0; currentPane->zoomLevel() < zoomLevel; ++i)
        wheelEvent(canvas, pos, 1);
    VERIFY(currentPane->integerZoomLevel() == zoomLevel);
    return true;
}
