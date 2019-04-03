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

void TestHelper::clearAndEnterText(QQuickItem *textField, const QString &text)
{
    QVERIFY_THROW(textField->property("text").isValid());
    QVERIFY_THROW(textField->hasActiveFocus());

    if (!textField->property("text").toString().isEmpty()) {
        QTest::keySequence(window, QKeySequence::SelectAll);
        QVERIFY_THROW(!textField->property("selectedText").toString().isEmpty());

        QTest::keySequence(window, QKeySequence::Backspace);
        QVERIFY_THROW(textField->property("text").toString().isEmpty());
    }

    keyClicks(text);
    QCOMPARE_THROW(textField->property("text").toString(), text);
}

void TestHelper::changeCanvasSize(int width, int height, CloseDialogFlag closeDialog)
{
    // Open the canvas size popup.
    mouseEventOnCentre(canvasSizeButton, MouseClick);
    const QObject *canvasSizePopup = findPopupFromTypeName("CanvasSizePopup");
    QVERIFY_THROW(canvasSizePopup);
    QTRY_VERIFY2_THROW(canvasSizePopup->property("opened").toBool(), "Failed to open CanvasSizePopup");

    // Change the values and then cancel.
    // TODO: use actual input events...
    QQuickItem *widthSpinBox = canvasSizePopup->findChild<QQuickItem*>("changeCanvasWidthSpinBox");
    QVERIFY_THROW(widthSpinBox);
    // We want it to be easy to change the values with the keyboard..
    QVERIFY_THROW(widthSpinBox->hasActiveFocus());
    const int originalWidthSpinBoxValue = widthSpinBox->property("value").toInt();
    QVERIFY_THROW(widthSpinBox->setProperty("value", originalWidthSpinBoxValue + 1));
    QCOMPARE_THROW(widthSpinBox->property("value").toInt(), originalWidthSpinBoxValue + 1);

    QQuickItem *heightSpinBox = canvasSizePopup->findChild<QQuickItem*>("changeCanvasHeightSpinBox");
    const int originalHeightSpinBoxValue = heightSpinBox->property("value").toInt();
    QVERIFY_THROW(heightSpinBox);
    QVERIFY_THROW(heightSpinBox->setProperty("value", originalHeightSpinBoxValue - 1));
    QCOMPARE_THROW(heightSpinBox->property("value").toInt(), originalHeightSpinBoxValue - 1);

    QQuickItem *cancelButton = canvasSizePopup->findChild<QQuickItem*>("canvasSizePopupCancelButton");
    QVERIFY_THROW(cancelButton);
    mouseEventOnCentre(cancelButton, MouseClick);
    QTRY_VERIFY2_THROW(!canvasSizePopup->property("visible").toBool(), "Failed to cancel CanvasSizePopup");
    QCOMPARE_THROW(project->size().width(), originalWidthSpinBoxValue);
    QCOMPARE_THROW(project->size().height(), originalHeightSpinBoxValue);
    QVERIFY_THROW(canvas->hasActiveFocus());

    // Open the popup again.
    mouseEventOnCentre(canvasSizeButton, MouseClick);
    QVERIFY_THROW(canvasSizePopup);
    QTRY_VERIFY2_THROW(canvasSizePopup->property("opened").toBool(), "Failed to reopen CanvasSizePopup");
    // The old values should be restored.
    QCOMPARE_THROW(widthSpinBox->property("value").toInt(), originalWidthSpinBoxValue);
    QCOMPARE_THROW(heightSpinBox->property("value").toInt(), originalHeightSpinBoxValue);
    QVERIFY_THROW(widthSpinBox->hasActiveFocus());

    // Change the values.
    QVERIFY_THROW(widthSpinBox->setProperty("value", width));
    QCOMPARE_THROW(widthSpinBox->property("value").toInt(), width);
    QVERIFY_THROW(heightSpinBox->setProperty("value", height));
    QCOMPARE_THROW(heightSpinBox->property("value").toInt(), height);

    if (closeDialog == CloseDialog) {
        // Press OK to close the dialog.
        QQuickItem *okButton = canvasSizePopup->findChild<QQuickItem*>("canvasSizePopupOkButton");
        QVERIFY_THROW(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        QTRY_VERIFY2_THROW(!canvasSizePopup->property("visible").toBool(), "Failed to accept CanvasSizePopup");
        QCOMPARE_THROW(project->size().width(), width);
        QCOMPARE_THROW(project->size().height(), height);
        QCOMPARE_THROW(widthSpinBox->property("value").toInt(), width);
        QCOMPARE_THROW(heightSpinBox->property("value").toInt(), height);
        QVERIFY_THROW(canvas->hasActiveFocus());
    }
}

void TestHelper::changeImageSize(int width, int height)
{
    // Open the image size popup.
    mouseEventOnCentre(imageSizeButton, MouseClick);
    const QObject *imageSizePopup = findPopupFromTypeName("ImageSizePopup");
    QVERIFY_THROW(imageSizePopup);
    QVERIFY_THROW(imageSizePopup->property("visible").toBool());

    // Change the values and then cancel.
    // TODO: use actual input events...
    QQuickItem *widthSpinBox = imageSizePopup->findChild<QQuickItem*>("changeImageWidthSpinBox");
    QVERIFY_THROW(widthSpinBox);
    // We want it to be easy to change the values with the keyboard..
    QVERIFY_THROW(widthSpinBox->hasActiveFocus());
    const int originalWidthSpinBoxValue = widthSpinBox->property("value").toInt();
    QVERIFY_THROW(widthSpinBox->setProperty("value", originalWidthSpinBoxValue + 1));
    QCOMPARE_THROW(widthSpinBox->property("value").toInt(), originalWidthSpinBoxValue + 1);

    QQuickItem *heightSpinBox = imageSizePopup->findChild<QQuickItem*>("changeImageHeightSpinBox");
    const int originalHeightSpinBoxValue = heightSpinBox->property("value").toInt();
    QVERIFY_THROW(heightSpinBox);
    QVERIFY_THROW(heightSpinBox->setProperty("value", originalHeightSpinBoxValue - 1));
    QCOMPARE_THROW(heightSpinBox->property("value").toInt(), originalHeightSpinBoxValue - 1);

    QQuickItem *cancelButton = imageSizePopup->findChild<QQuickItem*>("imageSizePopupCancelButton");
    QVERIFY_THROW(cancelButton);
    mouseEventOnCentre(cancelButton, MouseClick);
    QVERIFY_THROW(!imageSizePopup->property("visible").toBool());
    QCOMPARE_THROW(project->size().width(), originalWidthSpinBoxValue);
    QCOMPARE_THROW(project->size().height(), originalHeightSpinBoxValue);

    // Open the popup again.
    mouseEventOnCentre(imageSizeButton, MouseClick);
    QVERIFY_THROW(imageSizePopup);
    QVERIFY_THROW(imageSizePopup->property("visible").toBool());
    // The old values should be restored.
    QCOMPARE_THROW(widthSpinBox->property("value").toInt(), originalWidthSpinBoxValue);
    QCOMPARE_THROW(heightSpinBox->property("value").toInt(), originalHeightSpinBoxValue);

    // Change the values and then press OK.
    QVERIFY_THROW(widthSpinBox->setProperty("value", width));
    QCOMPARE_THROW(widthSpinBox->property("value").toInt(), width);
    QVERIFY_THROW(heightSpinBox->setProperty("value", height));
    QCOMPARE_THROW(heightSpinBox->property("value").toInt(), height);

    QQuickItem *okButton = imageSizePopup->findChild<QQuickItem*>("imageSizePopupOkButton");
    QVERIFY_THROW(okButton);
    mouseEventOnCentre(okButton, MouseClick);
    QVERIFY_THROW(!imageSizePopup->property("visible").toBool());
    QCOMPARE_THROW(project->size().width(), width);
    QCOMPARE_THROW(project->size().height(), height);
    QCOMPARE_THROW(widthSpinBox->property("value").toInt(), width);
    QCOMPARE_THROW(heightSpinBox->property("value").toInt(), height);
}

void TestHelper::changeToolSize(int size)
{
    mouseEventOnCentre(toolSizeButton, MouseClick);
    const QObject *toolSizePopup = findPopupFromTypeName("ToolSizePopup");
    QVERIFY_THROW(toolSizePopup);
    QCOMPARE_THROW(toolSizePopup->property("visible").toBool(), true);

    QQuickItem *toolSizeSlider = toolSizePopup->findChild<QQuickItem*>("toolSizeSlider");
    QVERIFY_THROW(toolSizeSlider);

    QQuickItem *toolSizeSliderHandle = toolSizeSlider->property("handle").value<QQuickItem*>();
    QVERIFY_THROW(toolSizeSliderHandle);

    // Move the slider to the right to find the max pos.
    QVERIFY_THROW(toolSizeSlider->setProperty("value", toolSizeSlider->property("to").toReal()));
    QCOMPARE_THROW(toolSizeSlider->property("value"), toolSizeSlider->property("to"));
    const QPoint handleMaxPos = toolSizeSliderHandle->mapToScene(
        QPointF(toolSizeSliderHandle->width() / 2, toolSizeSliderHandle->height() / 2)).toPoint();

    // Move/reset the slider to the left since we move from left to right.
    QVERIFY_THROW(toolSizeSlider->setProperty("value", toolSizeSlider->property("from").toReal()));
    QCOMPARE_THROW(toolSizeSlider->property("value"), toolSizeSlider->property("from"));
    const QPoint handleMinPos = toolSizeSliderHandle->mapToScene(
        QPointF(toolSizeSliderHandle->width() / 2, toolSizeSlider->height() / 2)).toPoint();

    QPoint sliderHandlePos = handleMinPos;
    QTest::mousePress(toolSizeSlider->window(), Qt::LeftButton, Qt::NoModifier, sliderHandlePos);
    QCOMPARE_THROW(toolSizeSlider->property("pressed").toBool(), true);
    QCOMPARE_THROW(window->mouseGrabberItem(), toolSizeSlider);

    QTest::mouseMove(toolSizeSlider->window(), sliderHandlePos, 5);

    // Move the slider's handle until we find the value we want.
    for (;
         sliderValue(toolSizeSlider) != size && sliderHandlePos.x() < handleMaxPos.x();
         ++sliderHandlePos.rx()) {
        QTest::mouseMove(toolSizeSlider->window(), sliderHandlePos, 5);
    }
    --sliderHandlePos.rx();
    QTest::mouseRelease(toolSizeSlider->window(), Qt::LeftButton, Qt::NoModifier, sliderHandlePos);
    QCOMPARE_THROW(toolSizeSlider->property("pressed").toBool(), false);
    QCOMPARE_THROW(sliderValue(toolSizeSlider), size);

    // Close the popup.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE_THROW(toolSizePopup->property("visible").toBool(), false);
}

void TestHelper::changeToolShape(ImageCanvas::ToolShape toolShape)
{
    if (canvas->toolShape() == toolShape)
        return;

    mouseEventOnCentre(toolShapeButton, MouseClick);
    const QObject *toolShapeMenu = window->findChild<QObject*>("toolShapeMenu");
    QVERIFY_THROW(toolShapeMenu);
    QTRY_VERIFY_THROW(toolShapeMenu->property("opened").toBool() == true);

    if (toolShape == ImageCanvas::SquareToolShape) {
        QQuickItem *squareToolShapeMenuItem = toolShapeMenu->findChild<QQuickItem*>("squareToolShapeMenuItem");
        QVERIFY_THROW(squareToolShapeMenuItem);

        mouseEventOnCentre(squareToolShapeMenuItem, MouseClick);
        QCOMPARE_THROW(canvas->toolShape(), ImageCanvas::SquareToolShape);
    } else {
        QQuickItem *circleToolShapeMenuItem = toolShapeMenu->findChild<QQuickItem*>("circleToolShapeMenuItem");
        QVERIFY_THROW(circleToolShapeMenuItem);

        mouseEventOnCentre(circleToolShapeMenuItem, MouseClick);
        QCOMPARE_THROW(canvas->toolShape(), ImageCanvas::CircleToolShape);
    }
}

void TestHelper::moveContents(int x, int y, bool onlyVisibleLayers)
{
    const QImage originalContents = project->exportedImage();

    // Open the move contents dialog.
    triggerShortcut("moveContentsShortcut", app.settings()->moveContentsShortcut());
    const QObject *moveContentsDialog = findPopupFromTypeName("MoveContentsDialog");
    QVERIFY_THROW(moveContentsDialog);
    QVERIFY_THROW(moveContentsDialog->property("visible").toBool());

    // Change the values and then cancel.
    // TODO: use actual input events...
    QQuickItem *moveContentsXSpinBox = moveContentsDialog->findChild<QQuickItem*>("moveContentsXSpinBox");
    QVERIFY_THROW(moveContentsXSpinBox);
    // We want it to be easy to change the values with the keyboard..
    QVERIFY_THROW(moveContentsXSpinBox->hasActiveFocus());
    const int originalXSpinBoxValue = moveContentsXSpinBox->property("value").toInt();
    QVERIFY_THROW(moveContentsXSpinBox->setProperty("value", originalXSpinBoxValue + 1));
    QCOMPARE_THROW(moveContentsXSpinBox->property("value").toInt(), originalXSpinBoxValue + 1);

    QQuickItem *moveContentsYSpinBox = moveContentsDialog->findChild<QQuickItem*>("moveContentsYSpinBox");
    const int originalYSpinBoxValue = moveContentsYSpinBox->property("value").toInt();
    QVERIFY_THROW(moveContentsYSpinBox);
    QVERIFY_THROW(moveContentsYSpinBox->setProperty("value", originalYSpinBoxValue - 1));
    QCOMPARE_THROW(moveContentsYSpinBox->property("value").toInt(), originalYSpinBoxValue - 1);

    QQuickItem *cancelButton = moveContentsDialog->findChild<QQuickItem*>("moveContentsDialogCancelButton");
    QVERIFY_THROW(cancelButton);
    mouseEventOnCentre(cancelButton, MouseClick);
    QVERIFY_THROW(!moveContentsDialog->property("visible").toBool());
    QCOMPARE_THROW(project->exportedImage(), originalContents);

    // Open the dialog again.
    triggerShortcut("moveContentsShortcut", app.settings()->moveContentsShortcut());
    QVERIFY_THROW(moveContentsDialog->property("visible").toBool());
    // The old values should be restored.
    QCOMPARE_THROW(moveContentsXSpinBox->property("value").toInt(), originalXSpinBoxValue);
    QCOMPARE_THROW(moveContentsYSpinBox->property("value").toInt(), originalYSpinBoxValue);

    // Change the values and then press OK.
    QVERIFY_THROW(moveContentsXSpinBox->setProperty("value", x));
    QCOMPARE_THROW(moveContentsXSpinBox->property("value").toInt(), x);
    QVERIFY_THROW(moveContentsYSpinBox->setProperty("value", y));
    QCOMPARE_THROW(moveContentsYSpinBox->property("value").toInt(), y);

    if (onlyVisibleLayers) {
        QQuickItem *onlyMoveVisibleLayersCheckBox = moveContentsDialog->findChild<QQuickItem*>("onlyMoveVisibleLayersCheckBox");
        QVERIFY_THROW(onlyMoveVisibleLayersCheckBox);
        if (onlyMoveVisibleLayersCheckBox->property("checked").toBool() != onlyVisibleLayers) {
            mouseEventOnCentre(onlyMoveVisibleLayersCheckBox, MouseClick);
            QCOMPARE_THROW(onlyMoveVisibleLayersCheckBox->property("checked").toBool(), onlyVisibleLayers);
        }
    }

    QImage movedContents(originalContents.size(), QImage::Format_ARGB32_Premultiplied);
    movedContents.fill(Qt::transparent);

    QPainter painter(&movedContents);
    painter.drawImage(x, y, originalContents);
    painter.end();

    QQuickItem *okButton = moveContentsDialog->findChild<QQuickItem*>("moveContentsDialogOkButton");
    QVERIFY_THROW(okButton);
    mouseEventOnCentre(okButton, MouseClick);
    QVERIFY_THROW(!moveContentsDialog->property("visible").toBool());
    QCOMPARE_THROW(project->exportedImage(), movedContents);
    QCOMPARE_THROW(moveContentsXSpinBox->property("value").toInt(), x);
    QCOMPARE_THROW(moveContentsYSpinBox->property("value").toInt(), y);
}

int TestHelper::sliderValue(QQuickItem *slider) const
{
    const qreal position = slider->property("position").toReal();
    qreal value = 0;
    QMetaObject::invokeMethod(slider, "valueAt", Qt::DirectConnection, Q_RETURN_ARG(qreal, value), Q_ARG(qreal, position));
    return qFloor(value);
}

void TestHelper::selectColourAtCursorPos()
{
    if (tilesetProject) {
        QFAIL_THROW("Not implemented yet");
    } else {
        switchTool(TileCanvas::EyeDropperTool);

        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        const QColor actualColour = canvas->penForegroundColour();
        const QColor expectedColour = canvas->currentProjectImage()->pixelColor(cursorPos);
        QVERIFY2_THROW(actualColour == expectedColour,
            qPrintable(QString::fromLatin1("Expected canvas foreground colour to be %1, but it's %2")
                .arg(expectedColour.name(), actualColour.name())));

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    }
}

void TestHelper::drawPixelAtCursorPos()
{
    if (tilesetProject) {
        const Tile *targetTile = tilesetProject->tileAt(cursorPos);
        QVERIFY_THROW(targetTile);

        switchTool(TileCanvas::PenTool);
        switchMode(TileCanvas::PixelMode);

        // Draw on some pixels of the current tile.
        const QImage originalTileImage = targetTile->tileset()->image()->copy(targetTile->sourceRect());
        QImage expectedImage = originalTileImage;
        expectedImage.setPixelColor(tileCanvas->scenePosToTilePixelPos(cursorPos), tileCanvas->penForegroundColour());

        QTest::mouseMove(window, cursorPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY_THROW(targetTile->tileset()->image()->copy(targetTile->sourceRect()) != originalTileImage);
        QCOMPARE_THROW(targetTile->tileset()->image()->copy(targetTile->sourceRect()), expectedImage);
        QVERIFY_THROW(tilesetProject->hasUnsavedChanges());
        // Don't check that the title contains the change marker, as that won't happen
        // until release if this is the first change in the stack.

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QCOMPARE_THROW(targetTile->tileset()->image()->copy(targetTile->sourceRect()), expectedImage);
        QVERIFY_THROW(tilesetProject->hasUnsavedChanges());
        QVERIFY_THROW(window->title().contains("*"));
    } else {
        switchTool(TileCanvas::PenTool);

        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QColor actualColour = canvas->currentProjectImage()->pixelColor(cursorPos);
        QColor expectedColour = canvas->penForegroundColour();
        if (actualColour != expectedColour) {
            QString message;
            QDebug stream(&message);
            stream << "Expected current project image pixel at" << cursorPos << "to be" << expectedColour.name()
                << "after mouse press, but it's" << actualColour.name();
            QFAIL_THROW(qPrintable(message));
        }
        QVERIFY_THROW(project->hasUnsavedChanges());
        // Don't check that the title contains the change marker, as that won't happen
        // until release if this is the first change in the stack.

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        if (actualColour != expectedColour) {
            QString message;
            QDebug stream(&message);
            stream << "Expected current project image pixel at" << cursorPos << "to be" << expectedColour.name()
                << "after mouse release, but it's" << actualColour.name();
            QFAIL_THROW(qPrintable(message));
        }
        QVERIFY_THROW(project->hasUnsavedChanges());
        QVERIFY_THROW(window->title().contains("*"));
    }
}

void TestHelper::drawTileAtCursorPos()
{
    switchTool(TileCanvas::PenTool);
    switchMode(TileCanvas::TileMode);

    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    const int penId = tileCanvas->penTile()->id();
    const Tile *tile = tilesetProject->tileAt(cursorPos);
    QVERIFY_THROW(tile);
    QCOMPARE_THROW(tile->id(), penId);
}

void TestHelper::selectArea(const QRect &area)
{
    // Switch to the selection tool.
    switchTool(ImageCanvas::SelectionTool);

    QVERIFY2_THROW(!canvas->hasSelection(), "Can't select area when there's already an existing selection");

    // Select the area.
    setCursorPosInScenePixels(area.topLeft());
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    // If area is {0, 0, 5, 5}, we need to move to {x=5, y=5} to ensure
    // that we've selected all 5x5 pixels.
    setCursorPosInScenePixels(area.bottomRight() + QPoint(1, 1));
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY_THROW(canvas->hasSelection());
    QCOMPARE_THROW(canvas->selectionArea(), area);
}

void TestHelper::dragSelection(const QPoint &newTopLeft)
{
    QVERIFY2_THROW(canvas->hasSelection(), "No selection to drag");

    const QRect oldSelectionArea = canvas->selectionArea();
    QRect newSelectionArea = oldSelectionArea;
    newSelectionArea.moveTo(newTopLeft);

    setCursorPosInScenePixels(canvas->selectionArea().topLeft());
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    setCursorPosInScenePixels(newTopLeft);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE_THROW(canvas->selectionArea(), newSelectionArea);
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
    QVERIFY2_THROW(rDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('r'), rDiff, fuzz)));

    const int gDiff = qAbs(colour2.green() - colour1.green());
    QVERIFY2_THROW(gDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('g'), gDiff, fuzz)));

    const int bDiff = qAbs(colour2.blue() - colour1.blue());
    QVERIFY2_THROW(bDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('b'), bDiff, fuzz)));

    const int aDiff = qAbs(colour2.alpha() - colour1.alpha());
    QVERIFY2_THROW(aDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('a'), aDiff, fuzz)));

    return true;
}

void TestHelper::fuzzyImageCompare(const QImage &image1, const QImage &image2)
{
    QCOMPARE_THROW(image1.size(), image2.size());

    for (int y = 0; y < image1.height(); ++y) {
        for (int x = 0; x < image1.width(); ++x) {
            QVERIFY_THROW(!fuzzyColourCompare(image1.pixelColor(x, y), image2.pixelColor(x, y)));
        }
    }
}

void TestHelper::everyPixelIs(const QImage &image, const QColor &colour)
{
    for (int y = 0; y < image.size().height(); ++y) {
        for (int x = 0; x < image.size().width(); ++x) {
            if (image.pixelColor(x, y) != colour) {
                QString message;
                QDebug debug(&message);
                debug.nospace() << "Expected every pixel of " << image << " to be " << colour
                    << ", but pixel at " << x << ", " << y << " is " << image.pixelColor(x, y);
                QFAIL_THROW(qPrintable(message));
            }
        }
    }
}

void TestHelper::compareSwatches(const Swatch &actualSwatch, const Swatch &expectedSwatch)
{
    if (actualSwatch.colours() == expectedSwatch.colours())
        return;

    QString message;
    message = "Swatches are not equal:";
    message += "\n  actual ";
    QDebug(&message) << actualSwatch.colours();
    message += "\nexpected ";
    QDebug(&message) << expectedSwatch.colours();
    QFAIL_THROW(qPrintable(message));
}

void TestHelper::enableAutoSwatch()
{
    // The swatches panel is hidden by default when testing; see updateVariables().
    togglePanel("swatchesPanel", true);

    // Wait until the previous view is destroyed (if any).
    QTRY_VERIFY_THROW(!window->findChild<QQuickItem*>("autoSwatchGridView"));

    // Sanity check.
    QVERIFY_THROW(!app.settings()->isAutoSwatchEnabled());
    // Enable auto swatch.
    app.settings()->setAutoSwatchEnabled(true);
}

void TestHelper::verifySwatchViewDelegateExists(const QQuickItem *viewContentItem, const QColor &colour)
{
    const auto childItems = viewContentItem->childItems();
    for (const QQuickItem *delegateItem : childItems) {
        if (delegateItem->property("color").value<QColor>() == colour)
            return;
    }

    QString message;
    QDebug debug(&message);
    debug.nospace() << "Couldn't find a delegate in swatch view content item " << viewContentItem
        << " whose contentItem colour is " << colour;
    QFAIL_THROW(qPrintable(message));
}

QQuickItem *TestHelper::findSwatchViewDelegateAtIndex(int index)
{
    QQuickItem *swatchGridView = window->findChild<QQuickItem*>("swatchGridView");
    if (!swatchGridView)
        return nullptr;

    return findViewDelegateAtIndex(swatchGridView, index);
}

void TestHelper::addSwatchWithForegroundColour()
{
    // Roll back to the previous value in case of test failure.
    Utils::ScopeGuard swatchPanelExpandedGuard([=](){
        const bool oldExpandedValue = swatchesPanel->property("expanded").toBool();
        swatchesPanel->setProperty("expanded", oldExpandedValue);
    });

    swatchesPanel->setProperty("expanded", true);

    QQuickItem *swatchGridView = window->findChild<QQuickItem*>("swatchGridView");
    QVERIFY_THROW(swatchGridView);
    QVERIFY_THROW(QMetaObject::invokeMethod(swatchGridView, "forceLayout"));

    QQuickItem *viewContentItem = swatchGridView->property("contentItem").value<QQuickItem*>();
    QVERIFY_THROW(viewContentItem);

    const int previousDelegateCount = swatchGridView->property("count").toInt();
    const QString expectedDelegateObjectName = QString::fromLatin1("swatchGridView_Delegate_%1_%2")
        .arg(previousDelegateCount).arg(canvas->penForegroundColour().name(QColor::HexArgb));

    // Add the swatch.
    QQuickItem *newSwatchColourButton = window->findChild<QQuickItem*>("newSwatchColourButton");
    QVERIFY_THROW(newSwatchColourButton);
    mouseEventOnCentre(newSwatchColourButton, MouseClick);
    QVERIFY_THROW(QMetaObject::invokeMethod(swatchGridView, "forceLayout"));
    QVERIFY2_THROW(swatchGridView->property("count").toInt() == previousDelegateCount + 1,
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
        QFAIL_THROW(qPrintable(message));
    }
}

void TestHelper::renameSwatchColour(int index, const QString &name)
{
    // Open the context menu by right clicking on the delegate.
    QQuickItem *delegate = findSwatchViewDelegateAtIndex(index);
    QVERIFY_THROW(delegate);
    mouseEventOnCentre(delegate, MouseClick, Qt::RightButton);
    QObject *swatchContextMenu = findPopupFromTypeName("SwatchContextMenu");
    QVERIFY_THROW(swatchContextMenu);
    QTRY_VERIFY_THROW(swatchContextMenu->property("opened").toBool());

    // Select the rename menu item.
    QQuickItem *renameSwatchColourMenuItem = window->findChild<QQuickItem*>("renameSwatchColourMenuItem");
    QVERIFY_THROW(renameSwatchColourMenuItem);
    // TODO: mouse event isn't getting through to the menu item without this.
    // This is likely caused by https://codereview.qt-project.org/#/c/225747/
    // and is probably the same issue encountered in the tests there.
    // Replace with Qt API if https://bugreports.qt.io/browse/QTBUG-71224 ever gets done.
    QTest::qWait(50);
    mouseEventOnCentre(renameSwatchColourMenuItem, MouseClick);
    QTRY_VERIFY_THROW(!swatchContextMenu->property("opened").toBool());

    QObject *renameSwatchColourDialog = findPopupFromTypeName("RenameSwatchColourDialog");
    QVERIFY_THROW(renameSwatchColourDialog);
    QTRY_VERIFY_THROW(renameSwatchColourDialog->property("opened").toBool());

    // Do the renaming.
    QQuickItem *swatchNameTextField = window->findChild<QQuickItem*>("swatchNameTextField");
    QVERIFY_THROW(swatchNameTextField);
    clearAndEnterText(swatchNameTextField, name);
    QTest::keyClick(window, Qt::Key_Return);
    QTRY_VERIFY_THROW(!renameSwatchColourDialog->property("opened").toBool());
}

void TestHelper::deleteSwatchColour(int index)
{
    // Open the context menu by right clicking on the delegate.
    QQuickItem *delegate = findSwatchViewDelegateAtIndex(index);
    QVERIFY_THROW(delegate);
    mouseEventOnCentre(delegate, MouseClick, Qt::RightButton);

    QObject *swatchContextMenu = findPopupFromTypeName("SwatchContextMenu");
    QVERIFY_THROW(swatchContextMenu);
    QTRY_VERIFY_THROW(swatchContextMenu->property("opened").toBool());

    QQuickItem *deleteSwatchColourMenuItem = window->findChild<QQuickItem*>("deleteSwatchColourMenuItem");
    QVERIFY_THROW(deleteSwatchColourMenuItem);
    mouseEventOnCentre(deleteSwatchColourMenuItem, MouseClick);
    QTRY_VERIFY_THROW(!swatchContextMenu->property("opened").toBool());
}

void TestHelper::addNewGuide(Qt::Orientation orientation, int position)
{
    if (!app.settings()->areRulersVisible()) {
        triggerRulersVisible();
        QVERIFY_THROW(app.settings()->areRulersVisible());
    }

    const bool horizontal = orientation == Qt::Horizontal;
    const int originalGuideCount = project->guides().size();
    const int newGuideIndex = originalGuideCount;
    const QPoint originalOffset = canvas->currentPane()->integerOffset();
    const qreal originalZoomLevel = canvas->currentPane()->zoomLevel();

    QQuickItem *ruler = canvas->findChild<QQuickItem*>(horizontal
        ? "firstHorizontalRuler" : "firstVerticalRuler");
    QVERIFY_THROW(ruler);
    const qreal rulerThickness = horizontal ? ruler->height() : ruler->width();

    // Pan so that the top left of the canvas is at the rulers' corners.
    panTopLeftTo(rulerThickness, rulerThickness);

    canvas->currentPane()->setZoomLevel(1.0);

    // Drop a horizontal guide onto the canvas.
    const QPoint pressPos(
        horizontal ? 50 : rulerThickness / 2,
        horizontal ? rulerThickness / 2 : 50);
    setCursorPosInPixels(pressPos);
    QTest::mouseMove(window, cursorWindowPos);
    QVERIFY_THROW(!canvas->pressedRuler());

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QVERIFY_THROW(canvas->pressedRuler());

    // Do the actual moving onto the canvas.
    const QPoint releasePos(
        horizontal ? 50 : rulerThickness + position,
        horizontal ? rulerThickness + position : 50);
    setCursorPosInPixels(releasePos);
    QTest::mouseMove(window, cursorWindowPos);

    // Now it should be visible on the canvas.
    QVERIFY_THROW(imageGrabber.requestImage(canvas));
    QTRY_VERIFY_THROW(imageGrabber.isReady());
    const QImage grabWithGuide = imageGrabber.takeImage();
    QCOMPARE_THROW(grabWithGuide.pixelColor(releasePos.x(), releasePos.y()), QColor(Qt::cyan));

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    QVERIFY_THROW(!canvas->pressedRuler());
    QVERIFY2_THROW(project->guides().size() == originalGuideCount + 1, qPrintable(QString::fromLatin1(
        "Expected %1 guide(s), but got %2").arg(originalGuideCount + 1).arg(project->guides().size())));
    QCOMPARE_THROW(project->guides().at(newGuideIndex).position(), position);
    QVERIFY_THROW(project->undoStack()->canUndo());

    canvas->currentPane()->setOffset(originalOffset);
    canvas->currentPane()->setZoomLevel(originalZoomLevel);
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
        // As with mouseEventOnCentre(), we don't want this to be a e.g. QVERIFY2_THROW, because then we'd have to
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

void TestHelper::triggerShortcut(const QString &objectName, const QString &sequenceAsString)
{
    QObject *shortcut = window->findChild<QObject*>(objectName);
    QVERIFY2_THROW(shortcut, qPrintable(QString::fromLatin1("Shortcut \"%1\" could not be found").arg(objectName)));
    QVERIFY2_THROW(shortcut->property("enabled").toBool(), qPrintable(QString::fromLatin1(
        "Shortcut \"%1\" is not enabled (%2 has active focus; does this shortcut require the canvas to have it?)")
            .arg(objectName, window->activeFocusItem() ? window->activeFocusItem()->objectName() : QString())));

    QSignalSpy activatedSpy(shortcut, SIGNAL(activated()));
    QVERIFY_THROW(activatedSpy.isValid());

    QVERIFY_THROW(QTest::qWaitForWindowActive(window));
    const int value = QKeySequence(sequenceAsString)[0];
    Qt::KeyboardModifiers mods = (Qt::KeyboardModifiers)(value & Qt::KeyboardModifierMask);
    QTest::keyClick(window, value & ~mods, mods);
    QVERIFY2_THROW(activatedSpy.count() == 1, qPrintable(QString::fromLatin1(
        "The activated() signal was not emitted for %1 with sequence %2").arg(objectName).arg(sequenceAsString)));
}

void TestHelper::triggerNewProject()
{
    triggerShortcut("newShortcut", app.settings()->newShortcut());
}

void TestHelper::triggerCloseProject()
{
    triggerShortcut("closeShortcut", app.settings()->closeShortcut());
}

void TestHelper::triggerSaveProject()
{
    triggerShortcut("saveShortcut", app.settings()->saveShortcut());
}

void TestHelper::triggerSaveProjectAs()
{
    QFAIL_THROW("TODO: no saveas shortcut");
//    triggerShortcut(app.settings()->saveShortcut());
}

void TestHelper::triggerOpenProject()
{
    triggerShortcut("openShortcut", app.settings()->openShortcut());
}

void TestHelper::triggerRevert()
{
    triggerShortcut("revertShortcut", app.settings()->revertShortcut());
}

void TestHelper::triggerPaste()
{
    triggerShortcut("pasteShortcut", QKeySequence(QKeySequence::Paste).toString());
}

void TestHelper::triggerFlipHorizontally()
{
    triggerShortcut("flipHorizontallyShortcut", app.settings()->flipHorizontallyShortcut());
}

void TestHelper::triggerFlipVertically()
{
    triggerShortcut("flipVerticallyShortcut", app.settings()->flipVerticallyShortcut());
}

void TestHelper::triggerSelectAll()
{
    triggerShortcut("selectAllShortcut", QKeySequence(QKeySequence::SelectAll).toString());
}

void TestHelper::triggerCentre()
{
    triggerShortcut("centreShortcut", app.settings()->centreShortcut());
}

void TestHelper::triggerGridVisible()
{
    triggerShortcut("gridVisibleShortcut", app.settings()->gridVisibleShortcut());
}

void TestHelper::triggerRulersVisible()
{
    triggerShortcut("rulersVisibleShortcut", app.settings()->rulersVisibleShortcut());
}

void TestHelper::triggerGuidesVisible()
{
    triggerShortcut("guidesVisibleShortcut", app.settings()->guidesVisibleShortcut());
}

void TestHelper::triggerSplitScreen()
{
    triggerShortcut("splitScreenShortcut", app.settings()->splitScreenShortcut());
}

void TestHelper::triggerSplitterLocked()
{
    triggerShortcut("splitterLockedShortcut", app.settings()->splitterLockedShortcut());
}

void TestHelper::setSplitterLocked(bool splitterLocked)
{
    if (canvas->splitter()->isEnabled() != !splitterLocked) {
        triggerSplitterLocked();
        QCOMPARE_THROW(canvas->splitter()->isEnabled(), !splitterLocked);
    }
}

void TestHelper::triggerAnimationPlayback()
{
    return triggerShortcut("animationPlaybackShortcut", app.settings()->animationPlaybackShortcut());
}

void TestHelper::setAnimationPlayback(bool usingAnimation)
{
    if (isUsingAnimation() != usingAnimation) {
        triggerAnimationPlayback();
        QCOMPARE_THROW(isUsingAnimation(), usingAnimation);
    }
}

void TestHelper::triggerOptions()
{
    triggerShortcut("optionsShortcut", app.settings()->optionsShortcut());
}

void TestHelper::selectLayer(const QString &layerName, int layerIndex)
{
    QTRY_VERIFY_THROW(findListViewChild("layerListView", layerName));
    QQuickItem *layerDelegate = findListViewChild("layerListView", layerName);
    QVERIFY_THROW(layerDelegate);
    mouseEventOnCentre(layerDelegate, MouseClick);
    QVERIFY_THROW(layerDelegate->property("checked").toBool());
    QVERIFY2_THROW(layeredImageProject->currentLayerIndex() == layerIndex,
        qPrintable(QString::fromLatin1("Expected currentLayerIndex to be %1 after selecting it, but it's %2")
            .arg(layerIndex).arg(layeredImageProject->currentLayerIndex())));
}

void TestHelper::verifyLayerName(const QString &layerName, QQuickItem **layerDelegate)
{
    // It seems that the ListView sometimes need some extra time to create the second item (e.g. when debugging).
    QTRY_VERIFY_THROW(findListViewChild("layerListView", layerName));
    QQuickItem *delegate = findListViewChild("layerListView", layerName);
    QVERIFY_THROW(delegate);
    QQuickItem *layerDelegateNameTextField = delegate->findChild<QQuickItem*>("layerNameTextField");
    QVERIFY_THROW(layerDelegateNameTextField);
    QCOMPARE_THROW(layerDelegateNameTextField->property("text").toString(), layerName);
    if (layerDelegate)
        *layerDelegate = delegate;
}

void TestHelper::makeCurrentAndRenameLayer(const QString &from, const QString &to)
{
    QQuickItem *layerDelegate = nullptr;
    verifyLayerName(from, &layerDelegate);

    mouseEventOnCentre(layerDelegate, MouseClick);
    QCOMPARE_THROW(layerDelegate->property("checked").toBool(), true);
    QCOMPARE_THROW(layeredImageProject->currentLayer()->name(), from);

    QQuickItem *nameTextField = layerDelegate->findChild<QQuickItem*>("layerNameTextField");
    QVERIFY_THROW(nameTextField);

    // A single click should not give the text field focus.
    mouseEventOnCentre(nameTextField, MouseClick);
    QVERIFY_THROW(!nameTextField->hasActiveFocus());

    // A double click should.
    mouseEventOnCentre(nameTextField, MouseDoubleClick);
    QCOMPARE_THROW(nameTextField->hasActiveFocus(), true);

    // Enter the text.
    QTest::keySequence(window, QKeySequence(QKeySequence::SelectAll));
    foreach (const auto character, to)
        QTest::keyClick(window, character.toLatin1());
    QVERIFY2_THROW(nameTextField->property("text").toString() == to, qPrintable(QString::fromLatin1(
        "Expected layerNameTextField to contain \"%1\" after inputting new layer name, but it contains \"%2\"")
            .arg(to, nameTextField->property("text").toString())));
    QCOMPARE_THROW(layeredImageProject->currentLayer()->name(), from);

    // Confirm the changes.
    QTest::keyClick(window, Qt::Key_Enter);
    QVERIFY2_THROW(nameTextField->property("text").toString() == to, qPrintable(QString::fromLatin1(
        "Expected layerNameTextField to contain \"%1\" after confirming changes, but it contains \"%2\"")
            .arg(to, nameTextField->property("text").toString())));
    QCOMPARE_THROW(layeredImageProject->currentLayer()->name(), to);
}

void TestHelper::changeLayerVisiblity(const QString &layerName, bool visible)
{
    QQuickItem *layerDelegate = nullptr;
    verifyLayerName(layerName, &layerDelegate);

    const ImageLayer *layer = layeredImageProject->layerAt(layerName);
    QVERIFY_THROW(layer);

    QQuickItem *layerVisibilityCheckBox = layerDelegate->findChild<QQuickItem*>("layerVisibilityCheckBox");
    QVERIFY_THROW(layerVisibilityCheckBox);

    // Sanity check that the check box's state matches the layer's state.
    const bool oldLayerVisibilityCheckBoxValue = layerVisibilityCheckBox->property("checked").toBool();
    QCOMPARE_THROW(oldLayerVisibilityCheckBoxValue, !layer->isVisible());

    // If the layer's visibility already matches the target visibility, we have nothing to do.
    if (oldLayerVisibilityCheckBoxValue == !visible)
        return;

    // Hide the layer.
    mouseEventOnCentre(layerVisibilityCheckBox, MouseClick);
    QCOMPARE_THROW(layer->isVisible(), visible);
    QCOMPARE_THROW(layerVisibilityCheckBox->property("checked").toBool(), !visible);
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

void TestHelper::createNewProject(Project::Type projectType, const QVariantMap &args)
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
        QVERIFY_THROW(window->cursor().shape() == Qt::ArrowCursor);
    }

    if (projectCreationFailedSpy)
        projectCreationFailedSpy->clear();

    // Click the new project button.
    triggerNewProject();

    // Check that we get prompted to discard any changes.
    if (project && project->hasUnsavedChanges())
        discardChanges();

    // Ensure that the new project popup is visible.
    const QObject *newProjectPopup = findPopupFromTypeName("NewProjectPopup");
    QVERIFY_THROW(newProjectPopup);
    QVERIFY_THROW(newProjectPopup->property("visible").toBool());
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
    QVERIFY_THROW(tilesetProjectButton);

    mouseEventOnCentre(tilesetProjectButton, MouseClick);
    QVERIFY_THROW(tilesetProjectButton->property("checked").toBool());

    QTRY_VERIFY_THROW(!newProjectPopup->property("visible").toBool());

    if (projectType == Project::TilesetType) {
        // Create a temporary directory containing a tileset image for us to use.
        setupTempTilesetProjectDir();

        // Now the New Tileset Project popup should be visible.
        QTRY_VERIFY_THROW(findPopupFromTypeName("NewTilesetProjectPopup"));
        const QObject *newTilesetProjectPopup = findPopupFromTypeName("NewTilesetProjectPopup");
        QVERIFY_THROW(newTilesetProjectPopup->property("visible").toBool());

        // Ensure that the popup gets reset each time it's opened.
        QQuickItem *useExistingTilesetCheckBox = newTilesetProjectPopup->findChild<QQuickItem*>("useExistingTilesetCheckBox");
        QVERIFY_THROW(useExistingTilesetCheckBox);
        QVERIFY_THROW(useExistingTilesetCheckBox->property("checked").isValid());
        QVERIFY_THROW(!useExistingTilesetCheckBox->property("checked").toBool());

        QQuickItem *tilesetField = newTilesetProjectPopup->findChild<QQuickItem*>("tilesetPathTextField");
        QVERIFY_THROW(tilesetField);
        QVERIFY_THROW(tilesetField->property("text").isValid());
        QVERIFY_THROW(tilesetField->property("text").toString().isEmpty());

        QQuickItem *tileWidthSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tileWidthSpinBox");
        QVERIFY_THROW(tileWidthSpinBox);
        QCOMPARE_THROW(tileWidthSpinBox->property("value").toInt(), 32);

        QQuickItem *tileHeightSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tileHeightSpinBox");
        QVERIFY_THROW(tileHeightSpinBox);
        QCOMPARE_THROW(tileHeightSpinBox->property("value").toInt(), 32);

        QQuickItem *tilesWideSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tilesWideSpinBox");
        QVERIFY_THROW(tilesWideSpinBox);
        QCOMPARE_THROW(tilesWideSpinBox->property("value").toInt(), 10);

        QQuickItem *tilesHighSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tilesHighSpinBox");
        QVERIFY_THROW(tilesHighSpinBox);
        QCOMPARE_THROW(tilesHighSpinBox->property("value").toInt(), 10);

        QVERIFY_THROW(tileWidthSpinBox->setProperty("value", tileWidth));
        QCOMPARE_THROW(tileWidthSpinBox->property("value").toInt(), tileWidth);

        QVERIFY_THROW(tileHeightSpinBox->setProperty("value", tileHeight));
        QCOMPARE_THROW(tileHeightSpinBox->property("value").toInt(), tileHeight);

        if (tilesetTilesWide == -1 || tilesetTilesHigh == -1) {
            // Using an existing tileset image.

            // Check the "use existing tileset" checkbox.
            // Hack: it has focus in the app, but not in the test => meh
            useExistingTilesetCheckBox->forceActiveFocus();
            QVERIFY_THROW(useExistingTilesetCheckBox->hasActiveFocus());
            QTest::keyClick(window, Qt::Key_Space);
            QVERIFY_THROW(useExistingTilesetCheckBox->property("checked").toBool());

            // Input the filename.
            QTest::keyClick(window, Qt::Key_Tab);
            QVERIFY_THROW(tilesetField->hasActiveFocus());
            // TODO: input path using events
            QVERIFY_THROW(tilesetField->property("text").isValid());
            QVERIFY_THROW(tilesetField->setProperty("text", tempTilesetUrl.toString()));

            QQuickItem *invalidFileIcon = newTilesetProjectPopup->findChild<QQuickItem*>("invalidFileIcon");
            QVERIFY_THROW(invalidFileIcon);
            QObject *fileValidator = newTilesetProjectPopup->findChild<QObject*>("validator");
            QVERIFY2_THROW(qFuzzyCompare(invalidFileIcon->property("opacity").toReal(), 0),
                qPrintable(QString::fromLatin1("File should be valid, but got error message \"%1\"; file path: %2")
                    .arg(fileValidator->property("errorMessage").toString())
                    .arg(tilesetField->property("text").toString())));
        } else {
            // Using a new tileset image.
            QVERIFY_THROW(tilesetTilesWide > 0 && tilesetTilesHigh > 0);

            QVERIFY_THROW(tilesWideSpinBox->setProperty("value", tilesetTilesWide));
            QCOMPARE_THROW(tilesWideSpinBox->property("value").toInt(), tilesetTilesWide);

            QVERIFY_THROW(tilesHighSpinBox->setProperty("value", tilesetTilesHigh));
            QCOMPARE_THROW(tilesHighSpinBox->property("value").toInt(), tilesetTilesHigh);

            QQuickItem *transparentBackgroundCheckBox = newTilesetProjectPopup->findChild<QQuickItem*>("transparentBackgroundCheckBox");
            QVERIFY_THROW(transparentBackgroundCheckBox);
            QVERIFY_THROW(transparentBackgroundCheckBox->property("checked").toBool());

            if (!transparentTilesetBackground) {
                mouseEventOnCentre(transparentBackgroundCheckBox, MouseClick);
                QVERIFY_THROW(!transparentBackgroundCheckBox->property("checked").toBool());
            }
        }

        // Confirm creation of the project.
        QQuickItem *okButton = newTilesetProjectPopup->findChild<QQuickItem*>("newTilesetProjectOkButton");
        QVERIFY_THROW(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        QVERIFY_THROW(!newTilesetProjectPopup->property("visible").toBool());
    } else {
        // Create a temporary directory that we can save into, etc.
        if (projectType == Project::LayeredImageType)
            setupTempLayeredImageProjectDir();

        // Now the New Image Project popup should be visible.
        QTRY_VERIFY_THROW(findPopupFromTypeName("NewImageProjectPopup"));
        const QObject *newImageProjectPopup = findPopupFromTypeName("NewImageProjectPopup");
        QVERIFY_THROW(newImageProjectPopup->property("visible").toBool());

        // Ensure that the popup gets reset each time it's opened.
        QQuickItem *imageWidthSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageWidthSpinBox");
        QVERIFY_THROW(imageWidthSpinBox);
        QCOMPARE_THROW(imageWidthSpinBox->property("value").toInt(), 256);

        QQuickItem *imageHeightSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageHeightSpinBox");
        QVERIFY_THROW(imageHeightSpinBox);
        QCOMPARE_THROW(imageHeightSpinBox->property("value").toInt(), 256);

        QVERIFY_THROW(imageWidthSpinBox->setProperty("value", imageWidth));
        QCOMPARE_THROW(imageWidthSpinBox->property("value").toInt(), imageWidth);

        QVERIFY_THROW(imageHeightSpinBox->setProperty("value", imageHeight));
        QCOMPARE_THROW(imageHeightSpinBox->property("value").toInt(), imageHeight);

        QQuickItem *transparentImageBackgroundCheckBox = newImageProjectPopup->findChild<QQuickItem*>("transparentImageBackgroundCheckBox");
        QVERIFY_THROW(transparentImageBackgroundCheckBox);
        QVERIFY_THROW(transparentImageBackgroundCheckBox->property("checked").isValid());
        QVERIFY_THROW(!transparentImageBackgroundCheckBox->property("checked").toBool());

        if (transparentImageBackground) {
            mouseEventOnCentre(transparentImageBackgroundCheckBox, MouseClick);
            QCOMPARE_THROW(transparentImageBackgroundCheckBox->property("checked").toBool(), transparentImageBackground);
        }

        // Confirm creation of the project.
        QQuickItem *okButton = newImageProjectPopup->findChild<QQuickItem*>("newImageProjectOkButton");
        QVERIFY_THROW(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        QTRY_VERIFY_THROW(!newImageProjectPopup->property("visible").toBool());
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

void TestHelper::createNewTilesetProject(int tileWidth, int tileHeight, int tilesetTilesWide, int tilesetTilesHigh,
    bool transparentTilesetBackground)
{
    QVariantMap args;
    args.insert("tileWidth", tileWidth);
    args.insert("tileHeight", tileHeight);
    args.insert("tilesetTilesWide", tilesetTilesWide);
    args.insert("tilesetTilesHigh", tilesetTilesHigh);
    args.insert("transparentTilesetBackground", transparentTilesetBackground);
    createNewProject(Project::TilesetType, args);
}

void TestHelper::createNewImageProject(int imageWidth, int imageHeight, bool transparentImageBackground)
{
    QVariantMap args;
    args.insert("imageWidth", imageWidth);
    args.insert("imageHeight", imageHeight);
    args.insert("transparentImageBackground", transparentImageBackground);

    createNewProject(Project::ImageType, args);
}

void TestHelper::createNewLayeredImageProject(int imageWidth, int imageHeight, bool transparentImageBackground)
{
    QVariantMap args;
    args.insert("imageWidth", imageWidth);
    args.insert("imageHeight", imageHeight);
    args.insert("transparentImageBackground", transparentImageBackground);
    createNewProject(Project::LayeredImageType, args);

    QCOMPARE_THROW(layeredImageProject->layerCount(), 1);
    QCOMPARE_THROW(layeredImageProject->currentLayerIndex(), 0);
    QVERIFY_THROW(layeredImageProject->currentLayer());
    QCOMPARE_THROW(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 1"));

    {
        // Ensure that what the user sees (the delegate) is correct.
        QQuickItem *layer1Delegate = findListViewChild("layerListView", QLatin1String("Layer 1"));
        QVERIFY_THROW(layer1Delegate);

        QQuickItem *nameTextField = layer1Delegate->findChild<QQuickItem*>("layerNameTextField");
        QVERIFY_THROW(nameTextField);
        QCOMPARE_THROW(nameTextField->property("text").toString(), QLatin1String("Layer 1"));
    }

    newLayerButton = window->findChild<QQuickItem*>("newLayerButton");
    QVERIFY_THROW(newLayerButton);

    duplicateLayerButton = window->findChild<QQuickItem*>("duplicateLayerButton");
    QVERIFY_THROW(duplicateLayerButton);

    moveLayerUpButton = window->findChild<QQuickItem*>("moveLayerUpButton");
    QVERIFY_THROW(moveLayerUpButton);

    moveLayerDownButton = window->findChild<QQuickItem*>("moveLayerDownButton");
    QVERIFY_THROW(moveLayerDownButton);
}

void TestHelper::loadProject(const QUrl &url, const QString &expectedFailureMessage)
{
    if (projectCreationFailedSpy)
        projectCreationFailedSpy->clear();

    QWindow *window = qobject_cast<QWindow*>(app.qmlEngine()->rootObjects().first());
    QVERIFY_THROW(window);

    // Load it.
    QVERIFY_THROW(QMetaObject::invokeMethod(window, "loadProject", Qt::DirectConnection, Q_ARG(QVariant, url)));

    if (expectedFailureMessage.isEmpty()) {
        // Expect success.
        VERIFY_NO_CREATION_ERRORS_OCCURRED();
        updateVariables(false, projectManager->projectTypeForUrl(url));
        return;
    }

    // Expect failure.
    QVERIFY2_THROW(!projectCreationFailedSpy->isEmpty() && projectCreationFailedSpy->first().first() == expectedFailureMessage,
        qPrintable(QString::fromLatin1("Expected failure to load project %1 with the following error message:\n%2\nBut got:\n%3")
            .arg(url.path()).arg(expectedFailureMessage).arg(projectCreationFailedSpy->first().first().toString())));

    const QObject *errorPopup = findPopupFromTypeName("ErrorPopup");
    QVERIFY_THROW(errorPopup);
    QVERIFY_THROW(errorPopup->property("visible").toBool());
    QCOMPARE_THROW(errorPopup->property("text").toString(), expectedFailureMessage);
    QVERIFY_THROW(errorPopup->property("focus").isValid());
    QVERIFY_THROW(errorPopup->property("focus").toBool());

    // Check that the cursor goes back to an arrow when there's a modal popup visible,
    // even if the mouse is over the canvas and not the popup.
    QTest::mouseMove(window, canvas->mapToScene(QPointF(10, 10)).toPoint());
    QVERIFY_THROW(!canvas->hasActiveFocus());
    QCOMPARE_THROW(window->cursor().shape(), Qt::ArrowCursor);

    // Close the error message popup.
    // QTest::mouseClick(window, Qt::LeftButton) didn't work on mac after a couple of data row runs,
    // so we use the keyboard to close it instead.
    QTest::keyClick(window, Qt::Key_Escape);
    QVERIFY_THROW(!errorPopup->property("visible").toBool());

    if (projectCreationFailedSpy)
        projectCreationFailedSpy->clear();
}

void TestHelper::updateVariables(bool isNewProject, Project::Type projectType)
{
    // The projects and canvases that we had references to should have
    // been destroyed by now.
    QTRY_VERIFY_THROW(!project);
    QVERIFY_THROW(!imageProject);
    QVERIFY_THROW(!tilesetProject);

    QTRY_VERIFY_THROW(!canvas);
    QVERIFY_THROW(!imageCanvas);
    QVERIFY_THROW(!tileCanvas);

    project = projectManager->project();
    QVERIFY_THROW(project);

    // The old canvas might still exist, but if it doesn't have a project,
    // then it's about to be destroyed, so wait for that to happen first
    // before proceeding.
    canvas = window->findChild<ImageCanvas*>();
    if (!canvas->project())
        QTRY_VERIFY_THROW(!canvas);

    canvas = window->findChild<ImageCanvas*>();
    QVERIFY_THROW(canvas);
    QTRY_VERIFY_THROW(canvas->window());

    animationPlayPauseButton = window->findChild<QQuickItem*>("animationPlayPauseButton");
    if (projectType == Project::ImageType || projectType == Project::LayeredImageType)
        QVERIFY_THROW(animationPlayPauseButton);

    if (isNewProject) {
        // The old default was to split the screen,
        // and so the tests might be depending on it to be split.
        // Also, it's good to ensure that it's tested.
        canvas->setSplitScreen(true);
        QVERIFY_THROW(canvas->isSplitScreen());
    }
    QCOMPARE_THROW(splitScreenToolButton->isEnabled(), true);
    QCOMPARE_THROW(splitScreenToolButton->property("checked").toBool(), canvas->isSplitScreen());
    QCOMPARE_THROW(lockSplitterToolButton->isEnabled(), canvas->isSplitScreen());
    QCOMPARE_THROW(lockSplitterToolButton->property("checked").toBool(), !canvas->splitter()->isEnabled());

    // This is the old default. Some tests seem to choke with it set to true
    // (because of wheel events) and I haven't looked into it yet because it's not really important.
    canvas->setGesturesEnabled(false);

    canvas->setPenForegroundColour(Qt::black);
    canvas->setPenBackgroundColour(Qt::white);
    // This determines which colour the ColourSelector considers "current",
    // and hence which value is shown in the hex field.
    QVERIFY_THROW(penForegroundColourButton->setProperty("checked", QVariant(true)));
    canvas->setToolShape(ImageCanvas::SquareToolShape);

    app.settings()->setAutoSwatchEnabled(false);
    app.settings()->setPenToolRightClickBehaviour(app.settings()->defaultPenToolRightClickBehaviour());

    if (projectType == Project::TilesetType) {
        tilesetProject = qobject_cast<TilesetProject*>(project);
        QVERIFY_THROW(tilesetProject);

        tileCanvas = qobject_cast<TileCanvas*>(canvas);
        QVERIFY_THROW(tileCanvas);
    } else if (projectType == Project::ImageType) {
        imageProject = qobject_cast<ImageProject*>(project);
        QVERIFY_THROW(imageProject);

        imageCanvas = canvas;
    } else if (projectType == Project::LayeredImageType) {
        layeredImageProject = qobject_cast<LayeredImageProject*>(project);
        QVERIFY_THROW(layeredImageProject);

        layeredImageCanvas = qobject_cast<LayeredImageCanvas*>(canvas);;
    }

    canvas->forceActiveFocus();
//    QTRY_VERIFY_THROW(canvas->hasActiveFocus());

    QVERIFY_THROW(project->hasLoaded());

    if (isNewProject) {
        QCOMPARE_THROW(project->url(), QUrl());
        QVERIFY_THROW(project->isNewProject());

        // Reset any settings that have changed back to their defaults.
        QVariant settingsAsVariant = qmlEngine(canvas)->rootContext()->contextProperty("settings");
        QVERIFY_THROW(settingsAsVariant.isValid());
        ApplicationSettings *settings = settingsAsVariant.value<ApplicationSettings*>();
        QVERIFY_THROW(settings);
        settings->resetShortcutsToDefaults();

        if (settings->areRulersVisible()) {
            triggerRulersVisible();
            QCOMPARE_THROW(settings->areRulersVisible(), false);
        }

        cursorPos = QPoint();
        cursorWindowPos = QPoint();
    }

    // Sanity check.
    QTRY_VERIFY_THROW(canvas->window() == canvasSizeButton->window());
    QVERIFY_THROW(!canvas->splitter()->isPressed());
    QVERIFY_THROW(!canvas->splitter()->isHovered());

    if (isNewProject) {
        QCOMPARE_THROW(canvas->splitter()->position(), 0.5);
    }

    if (projectType == Project::TilesetType) {
        // Establish references to TilesetProject-specific properties.
        tilesetSwatchPanel = window->findChild<QQuickItem*>("tilesetSwatchPanel");
        QVERIFY_THROW(tilesetSwatchPanel);
        QCOMPARE_THROW(tilesetSwatchPanel->isVisible(), true);
        QVERIFY_THROW(!qFuzzyIsNull(tilesetSwatchPanel->width()));
        // This started failing recently for some reason, but giving it some time seems to help.
        QTRY_VERIFY_THROW(!qFuzzyIsNull(tilesetSwatchPanel->height()));

        // Ensure that the tileset swatch flickable has the correct contentY.
        tilesetSwatchFlickable = tilesetSwatchPanel->findChild<QQuickItem*>("tilesetSwatchFlickable");
        QVERIFY_THROW(tilesetSwatchFlickable);
        QVERIFY_THROW(tilesetSwatchFlickable->property("contentY").isValid());
        QCOMPARE_THROW(tilesetSwatchFlickable->property("contentY").toReal(), 0.0);

        QVERIFY_THROW(imageGrabber.requestImage(tileCanvas));
        QTRY_VERIFY_THROW(imageGrabber.isReady());
        QCOMPARE_THROW(imageGrabber.takeImage().pixelColor(16, 16), tileCanvas->mapBackgroundColour());
    } else {
        QVERIFY_THROW(window->findChild<QQuickItem*>("selectionToolButton"));
    }

    // Make sure we call any functions that use the variables we set last.

    // Having too many panels can result in the panel that a particular test is interested in being
    // too small, meaning certain tool buttons cannot be clicked because they're hidden.
    // It's more reliable to just show the relevant panels where necessary.
    collapseAllPanels();

    if (isNewProject) {
        // If we're not loading a saved project (where we'd want to use the saved split sizes),
        // return the SplitView back to roughly its original size.
        // It doesn't matter if it's not perfect, it's just important that it's not already
        // at its max width so that the tests can resize it.
        QPointer<QQuickItem> panelSplitView = window->findChild<QQuickItem*>("panelSplitView");
        QVERIFY_THROW(panelSplitView);
        dragSplitViewHandle("mainSplitView", 0, QPoint(window->width() - 240, window->height() / 2));
    }
}

void TestHelper::discardChanges()
{
    const QObject *discardChangesDialog = window->contentItem()->findChild<QObject*>("discardChangesDialog");
    QVERIFY_THROW(discardChangesDialog);
    QTRY_VERIFY_THROW(discardChangesDialog->property("opened").toBool());

    QQuickItem *discardChangesButton = findDialogButtonFromObjectName(discardChangesDialog, "discardChangesDialogButton");
    QVERIFY_THROW(discardChangesButton);
    mouseEventOnCentre(discardChangesButton, MouseClick);
    QTRY_VERIFY_THROW(!discardChangesDialog->property("visible").toBool());
}

void TestHelper::verifyErrorAndDismiss(const QString &expectedErrorMessage)
{
    QObject *errorDialog = findPopupFromTypeName("ErrorPopup");
    QVERIFY_THROW(errorDialog);
    QTRY_VERIFY_THROW(errorDialog->property("opened").toBool());

    // Save the error message so that we can dismiss the dialog beforehand.
    // This way, if the message comparison fails, the dialog won't interfere
    // with the next test.
    const QString errorMessage = errorDialog->property("text").toString();

    QTest::keyClick(window, Qt::Key_Escape);
    QTRY_VERIFY_THROW(!errorDialog->property("visible").toBool());

    QVERIFY2_THROW(errorMessage.contains(expectedErrorMessage), qPrintable(QString::fromLatin1(
        "Error message does not contain expected error message: %1").arg(errorMessage)));
}

void TestHelper::verifyNoErrorOrDismiss()
{
    QObject *errorDialog = findPopupFromTypeName("ErrorPopup");
    QVERIFY_THROW(errorDialog);

    const bool wasVisible = errorDialog->property("visible").toBool();
    QString errorMessage;
    if (wasVisible) {
        errorMessage = errorDialog->property("text").toString();
        // Dismissing ensures that the dialog doesn't interfere with the next test.
        QTest::keyClick(window, Qt::Key_Escape);
        QTRY_VERIFY_THROW(!errorDialog->property("visible").toBool());
    }

    QVERIFY2_THROW(!wasVisible, qPrintable(QLatin1String("Expected no error, but got: ") + errorMessage));
}

void TestHelper::copyFileFromResourcesToTempProjectDir(const QString &baseName)
{
    QFile sourceFile(":/resources/" + baseName);
    QVERIFY2_THROW(sourceFile.open(QIODevice::ReadOnly), qPrintable(QString::fromLatin1(
        "Failed to open %1: %2").arg(sourceFile.fileName()).arg(sourceFile.errorString())));
    sourceFile.close();

    const QString saveFilePath = tempProjectDir->path() + "/" + baseName;
    QVERIFY2_THROW(QFile::copy(sourceFile.fileName(), saveFilePath), qPrintable(QString::fromLatin1(
        "Failed to copy %1 to %2: %3").arg(sourceFile.fileName()).arg(saveFilePath).arg(sourceFile.errorString())));

    // A file copied from a file that is part of resources will always be read-only...
    QFile copiedFile(saveFilePath);
    QVERIFY_THROW(copiedFile.size() > 0);

    QVERIFY_THROW(copiedFile.setPermissions(QFile::ReadUser | QFile::WriteUser));
    QVERIFY_THROW(copiedFile.size() > 0);
    QVERIFY2_THROW(copiedFile.open(QIODevice::ReadWrite), qPrintable(QString::fromLatin1(
        "Error opening file at %1: %2").arg(saveFilePath).arg(copiedFile.errorString())));
}

void TestHelper::setupTempTilesetProjectDir()
{
    QStringList toCopy;
    toCopy << tilesetBasename;
    // More stuff here.

    QStringList copiedPaths;
    setupTempProjectDir(toCopy, &copiedPaths);

    tempTilesetUrl = QUrl::fromLocalFile(copiedPaths.at(0));
    // More stuff here.
}

void TestHelper::setupTempLayeredImageProjectDir()
{
    setupTempProjectDir();
}

void TestHelper::setupTempProjectDir(const QStringList &resourceFilesToCopy, QStringList *filesCopied)
{
    tempProjectDir.reset(new QTemporaryDir);
    QVERIFY2_THROW(tempProjectDir->isValid(), qPrintable(tempProjectDir->errorString()));

    foreach (const QString &baseName, resourceFilesToCopy) {
        copyFileFromResourcesToTempProjectDir(baseName);

        if (filesCopied) {
            const QString saveFilePath = tempProjectDir->path() + "/" + baseName;
            *filesCopied << saveFilePath;
        }
    }
}

bool TestHelper::isPanelExpanded(const QString &panelObjectName)
{
    QQuickItem *panel = window->findChild<QQuickItem*>(panelObjectName);
    QVERIFY_THROW(panel);
    return panel->property("expanded").toBool();
}

void TestHelper::collapseAllPanels()
{
    if (project->type() == Project::TilesetType)
        togglePanel("tilesetSwatchPanel", false);
    else if (project->type() == Project::LayeredImageType)
        togglePanel("layerPanel", false);

    togglePanel("colourPanel", false);

    togglePanel("swatchesPanel", false);

    if (project->type() == Project::ImageType || project->type() == Project::LayeredImageType) {
        // Don't change the expanded state if the panel is not even visible,
        // as we want it to be the default (true) for tests involving animation playback
        // to ensure that it shows when enabled.
        QQuickItem *animationPanel = window->findChild<QQuickItem*>("animationPanel");
        QVERIFY_THROW(animationPanel);
        QVERIFY_THROW(animationPanel->isVisible());
        togglePanel("animationPanel", false);
    }
}

void TestHelper::togglePanel(const QString &panelObjectName, bool expanded)
{
    QQuickItem *panel = window->findChild<QQuickItem*>(panelObjectName);
    QVERIFY_THROW(panel);

    if (panel->property("expanded").toBool() == expanded)
        return;

    const qreal originalHeight = panel->height();
    QVERIFY_THROW(panel->setProperty("expanded", QVariant(expanded)));
    QCOMPARE_THROW(panel->property("expanded").toBool(), expanded);
    if (expanded) {
        // Ensure that it has time to grow, otherwise stuff like input events will not work.
        QTRY_VERIFY2_THROW(panel->height() > originalHeight, qPrintable(QString::fromLatin1(
            "Expected expanded height of %1 to be larger than collapsed height of %2, but it wasn't")
                .arg(panelObjectName).arg(originalHeight)));
    } else {
        // If it's not visible, it's height might not update until it's visible again, apparently.
        // That's fine with us.
        if (panel->isVisible()) {
            // Ensure that the panel isn't visually expanded. We don't want to hard-code what the collapsed height
            // is, so we just make sure it's less than some large height.
            QTRY_VERIFY2_THROW(panel->height() < 100, qPrintable(QString::fromLatin1(
                "Expected collapsed height of %1 to be small, but it's %2")
                    .arg(panelObjectName).arg(panel->height())));
        }
    }
}

void TestHelper::dragSplitViewHandle(const QString &splitViewObjectName, int index,
    const QPoint &newHandleCentreRelativeToSplitView, QPoint *oldHandleCentreRelativeToSplitView)
{
    QPointer<QQuickItem> splitView = window->findChild<QQuickItem*>(splitViewObjectName);
    QVERIFY_THROW(splitView);

    QPointer<QQuickItem> handleItem = findSplitViewHandle(splitViewObjectName, index);
    QVERIFY_THROW(handleItem);

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
    QVERIFY_THROW(handleHoveredProperty.isValid());
    QCOMPARE_THROW(handleHoveredProperty.read().toBool(), true);
    QQmlProperty handlePressedProperty(handleItem.data(), "SplitHandle.pressed", qmlContext(handleItem.data()));
    QVERIFY_THROW(handlePressedProperty.isValid());
    QCOMPARE_THROW(handlePressedProperty.read().toBool(), false);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, handleCentre);
    QCOMPARE_THROW(handlePressedProperty.read().toBool(), true);

    QTest::mouseMove(window, newCentre);
    QCOMPARE_THROW(handlePressedProperty.read().toBool(), true);

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, newCentre);
    QCOMPARE_THROW(handleHoveredProperty.read().toBool(), true);
    QCOMPARE_THROW(handlePressedProperty.read().toBool(), false);
}

void TestHelper::togglePanels(const QStringList &panelObjectNames, bool expanded)
{
    for (const QString &panelObjectName : qAsConst(panelObjectNames))
        togglePanel(panelObjectName, expanded);
}

void TestHelper::switchMode(TileCanvas::Mode mode)
{
    if (tileCanvas->mode() == mode)
        return;

    mouseEventOnCentre(modeToolButton, MouseClick);
    const bool expectChecked = mode == TileCanvas::TileMode;
    QVERIFY2_THROW(modeToolButton->property("checked").toBool() == expectChecked, qPrintable(QString::fromLatin1(
        "modeToolButton.checked should be %1, but it's %2 (trying to set mode %3)")
            .arg(expectChecked).arg(modeToolButton->property("checked").toBool()).arg(mode)));
    QCOMPARE_THROW(tileCanvas->mode(), mode);
}

void TestHelper::switchTool(ImageCanvas::Tool tool, InputType inputType)
{
    if (canvas->tool() == tool)
        return;

    if (inputType == KeyboardInputType)
        ENSURE_ACTIVE_FOCUS(canvas);

    switch (tool) {
    case ImageCanvas::PenTool:
        if (inputType == MouseInputType) {
            QVERIFY_THROW(penToolButton->isEnabled());
            mouseEventOnCentre(penToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->penToolShortcut());
        }
        break;
    case ImageCanvas::EyeDropperTool:
        if (inputType == MouseInputType) {
            QVERIFY_THROW(eyeDropperToolButton->isEnabled());
            mouseEventOnCentre(eyeDropperToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->eyeDropperToolShortcut());
        }
        break;
    case ImageCanvas::FillTool:
        if (inputType == MouseInputType) {
            QVERIFY_THROW(fillToolButton->isEnabled());
            mouseEventOnCentre(fillToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->fillToolShortcut());
        }
        break;
    case ImageCanvas::EraserTool:
        if (inputType == MouseInputType) {
            QVERIFY_THROW(eraserToolButton->isEnabled());
            mouseEventOnCentre(eraserToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->eraserToolShortcut());
        }
        break;
    case ImageCanvas::SelectionTool:
        if (inputType == MouseInputType) {
            QVERIFY_THROW(selectionToolButton->isEnabled());
            mouseEventOnCentre(selectionToolButton, MouseClick);
        } else {
            QTest::keySequence(window, app.settings()->selectionToolShortcut());
        }
        break;
    default:
        QFAIL_THROW("tool not handled!");
    }

    QVERIFY2_THROW(canvas->tool() == tool, qPrintable(QString::fromLatin1(
        "Expected tool %1 but current tool is %2").arg(tool).arg(canvas->tool())));
}

void TestHelper::setPenForegroundColour(QString argbString)
{
    QQuickItem *hexTextField = window->findChild<QQuickItem*>("hexTextField");
    QVERIFY_THROW(hexTextField);

    argbString.replace("#", QString());

    hexTextField->forceActiveFocus();
    QMetaObject::invokeMethod(hexTextField, "selectAll", Qt::DirectConnection);

    for (int i = 0; i < argbString.length(); ++i) {
        QTest::keyClick(window, argbString.at(i).toLatin1());
    }
    QTest::keyClick(window, Qt::Key_Return);
    QCOMPARE_THROW(hexTextField->property("text").toString(), argbString);
}

void TestHelper::panTopLeftTo(int x, int y)
{
    const QPoint panDistance = QPoint(x, y) - canvas->firstPane()->integerOffset();
    panBy(panDistance.x(), panDistance.y());
}

void TestHelper::panBy(int xDistance, int yDistance)
{
    QPoint pressPos = firstPaneSceneCentre();
    QTest::mouseMove(window, pressPos);
    QCOMPARE_THROW(canvas->currentPane(), canvas->firstPane());

    // TODO: get image checks working
    //        QVERIFY_THROW(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        const QImage originalImage = imageGrabber.takeImage();

    const QPoint originalOffset = canvas->currentPane()->integerOffset();
    const QPoint expectedOffset = originalOffset + QPoint(xDistance, yDistance);

    QTest::keyPress(window, Qt::Key_Space);
    QVERIFY2_THROW(window->cursor().shape() == Qt::OpenHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::OpenHandCursor after Qt::Key_Space press, but got %1").arg(window->cursor().shape())));
    QCOMPARE_THROW(canvas->currentPane()->integerOffset(), originalOffset);
    //        QVERIFY_THROW(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        // Cursor changed to OpenHandCursor.
    //        QImage currentImage = imageGrabber.takeImage();
    //        QVERIFY_THROW(currentImage != originalImage);
    //        QImage lastImage = currentImage;

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, pressPos);
    QVERIFY2_THROW(window->cursor().shape() == Qt::ClosedHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::ClosedHandCursor after mouse press, but got %1").arg(window->cursor().shape())));
    QCOMPARE_THROW(canvas->currentPane()->integerOffset(), originalOffset);
    //        QVERIFY_THROW(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        currentImage = imageGrabber.takeImage();
    //        // Cursor changed to ClosedHandCursor.
    //        QVERIFY_THROW(currentImage != lastImage);
    //        lastImage = currentImage;

    QTest::mouseMove(window, pressPos + QPoint(xDistance, yDistance));
    QVERIFY2_THROW(window->cursor().shape() == Qt::ClosedHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::ClosedHandCursor after mouse move, but got %1").arg(window->cursor().shape())));
    QCOMPARE_THROW(canvas->currentPane()->integerOffset(), expectedOffset);
    //        QVERIFY_THROW(imageGrabber.requestImage(canvas));
    //        // Pane offset changed.
    //        currentImage = imageGrabber.takeImage();
    //        QVERIFY_THROW(currentImage != lastImage);
    //        lastImage = currentImage;

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, pressPos + QPoint(xDistance, yDistance));
    QVERIFY2_THROW(window->cursor().shape() == Qt::OpenHandCursor, qPrintable(QString::fromLatin1(
        "Expected Qt::ClosedHandCursor after mouse release, but got %1").arg(window->cursor().shape())));
    QCOMPARE_THROW(canvas->currentPane()->integerOffset(), expectedOffset);

    QTest::keyRelease(window, Qt::Key_Space);
    // If we have a selection, the cursor might not be Qt::BlankCursor, and that's OK.
    if (!canvas->hasSelection()) {
        // Move the mouse away from any guides, etc.
        QTest::mouseMove(window, QPoint(0, 0));
        QVERIFY2_THROW(window->cursor().shape() == Qt::BlankCursor, qPrintable(QString::fromLatin1(
            "Expected Qt::BlankCursor after Qt::Key_Space release, but got %1").arg(window->cursor().shape())));
        QTest::mouseMove(window, pressPos + QPoint(xDistance, yDistance));
    }
    QCOMPARE_THROW(canvas->currentPane()->integerOffset(), expectedOffset);
}

void TestHelper::zoomTo(int zoomLevel)
{
    zoomTo(zoomLevel, cursorWindowPos);
}

void TestHelper::zoomTo(int zoomLevel, const QPoint &pos)
{
    CanvasPane *currentPane = canvas->currentPane();
    for (int i = 0; currentPane->zoomLevel() < zoomLevel; ++i)
        wheelEvent(canvas, pos, 1);
    QCOMPARE_THROW(currentPane->integerZoomLevel(), zoomLevel);
}
