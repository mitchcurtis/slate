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

#include "projectmanager.h"
#include "imagelayer.h"

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
#ifdef NON_NATIVE_MENUS
    fileToolButton(nullptr),
    optionsToolButton(nullptr),
    viewToolButton(nullptr),
    newMenuButton(nullptr),
    closeMenuButton(nullptr),
    openMenuButton(nullptr),
    saveMenuButton(nullptr),
    saveAsMenuButton(nullptr),
    settingsMenuButton(nullptr),
    revertMenuButton(nullptr),
    centreMenuButton(nullptr),
    showGridMenuButton(nullptr),
    showRulersMenuButton(nullptr),
    splitScreenMenuButton(nullptr),
    duplicateTileMenuButton(nullptr),
    rotateTileLeftMenuButton(nullptr),
    rotateTileRightMenuButton(nullptr),
#endif
    canvasSizeButton(nullptr),
    modeToolButton(nullptr),
    penToolButton(nullptr),
    eyeDropperToolButton(nullptr),
    fillToolButton(nullptr),
    eraserToolButton(nullptr),
    selectionToolButton(nullptr),
    toolSizeButton(nullptr),
    undoButton(nullptr),
    redoButton(nullptr),
    penForegroundColourButton(nullptr),
    penBackgroundColourButton(nullptr),
    tilesetSwatch(nullptr),
    newLayerButton(nullptr),
    tilesetBasename("test-tileset.png")
{
}

TestHelper::~TestHelper()
{
}

void TestHelper::initTestCase()
{
    // Tests could have failed on the last run, so just enforce the default settings.
    app.settings()->setGridVisible(app.settings()->defaultGridVisible());
    QVERIFY(app.settings()->isGridVisible());
    app.settings()->setSplitScreen(app.settings()->defaultSplitScreen());
    QVERIFY(app.settings()->isSplitScreen());
    app.settings()->setRulersVisible(false);
    QVERIFY(!app.settings()->areRulersVisible());

    // However, this should never change.
    QVERIFY(!app.settings()->loadLastOnStartup());

    QVERIFY(window->property("overlay").isValid());
    overlay = window->property("overlay").value<QQuickItem*>();
    QVERIFY(overlay);

    projectManager = window->contentItem()->findChild<ProjectManager*>();
    QVERIFY(projectManager);

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

#ifdef NON_NATIVE_MENUS
    fileToolButton = window->findChild<QQuickItem*>("fileToolButton");
    QVERIFY(fileToolButton);

    optionsToolButton = window->findChild<QQuickItem*>("optionsToolButton");
    QVERIFY(optionsToolButton);

    viewToolButton = window->findChild<QQuickItem*>("viewToolButton");
    QVERIFY(viewToolButton);

    newMenuButton = window->findChild<QQuickItem*>("newMenuButton");
    QVERIFY(newMenuButton);

    closeMenuButton = window->findChild<QQuickItem*>("closeMenuButton");
    QVERIFY(closeMenuButton);

    openMenuButton = window->findChild<QQuickItem*>("openMenuButton");
    QVERIFY(openMenuButton);

    saveMenuButton = window->findChild<QQuickItem*>("saveMenuButton");
    QVERIFY(saveMenuButton);

    saveAsMenuButton = window->findChild<QQuickItem*>("saveAsMenuButton");
    QVERIFY(saveAsMenuButton);

    settingsMenuButton = window->findChild<QQuickItem*>("settingsMenuButton");
    QVERIFY(settingsMenuButton);

    revertMenuButton = window->findChild<QQuickItem*>("revertMenuButton");
    QVERIFY(revertMenuButton);

    centreMenuButton = window->findChild<QQuickItem*>("centreMenuButton");
    QVERIFY(centreMenuButton);

    showGridMenuButton = window->findChild<QQuickItem*>("showGridMenuButton");
    QVERIFY(showGridMenuButton);

    showRulersMenuButton = window->findChild<QQuickItem*>("showRulersMenuButton");
    QVERIFY(showRulersMenuButton);

    splitScreenMenuButton = window->findChild<QQuickItem*>("splitScreenMenuButton");
    QVERIFY(splitScreenMenuButton);
#endif

    canvasSizeButton = window->findChild<QQuickItem*>("canvasSizeButton");
    QVERIFY(canvasSizeButton);

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

    undoButton = window->findChild<QQuickItem*>("undoButton");
    QVERIFY(undoButton);

    redoButton = window->findChild<QQuickItem*>("redoButton");
    QVERIFY(redoButton);

    penForegroundColourButton = window->findChild<QQuickItem*>("penForegroundColourButton");
    QVERIFY(penForegroundColourButton);

    penBackgroundColourButton = window->findChild<QQuickItem*>("penBackgroundColourButton");
    QVERIFY(penBackgroundColourButton);
}

void TestHelper::resetCreationErrorSpy()
{
    if (projectManager->temporaryProject())
        creationErrorOccurredSpy.reset(new QSignalSpy(projectManager->temporaryProject(), SIGNAL(errorOccurred(QString))));
}

void TestHelper::mouseEventOnCentre(QQuickItem *item, TestMouseEventType eventType)
{
    QQuickWindow *itemWindow = item->window();
    if (!itemWindow) {
        QString str;
        QDebug debug(&str);
        debug << "Item" << item << "has no window; can't send mouse events to it";
        QFAIL(qPrintable(str));
    }

    const QPoint centre = item->mapToScene(QPointF(item->width() / 2, item->height() / 2)).toPoint();
    switch (eventType) {
    case MousePress:
        QTest::mousePress(itemWindow, Qt::LeftButton, Qt::NoModifier, centre);
        break;
    case MouseRelease:
        QTest::mouseRelease(itemWindow, Qt::LeftButton, Qt::NoModifier, centre);
        break;
    case MouseClick:
        QTest::mouseClick(itemWindow, Qt::LeftButton, Qt::NoModifier, centre);
        break;
    case MouseDoubleClick:
        QTest::mouseDClick(itemWindow, Qt::LeftButton, Qt::NoModifier, centre);
        break;
    }
}

void TestHelper::mouseEvent(QQuickItem *item, const QPointF &localPos, TestMouseEventType eventType, Qt::KeyboardModifiers modifiers, int delay)
{
    const QPoint centre = item->mapToScene(localPos).toPoint();
    switch (eventType) {
    case MousePress:
        QTest::mousePress(item->window(), Qt::LeftButton, modifiers, centre, delay);
        break;
    case MouseRelease:
        QTest::mouseRelease(item->window(), Qt::LeftButton, modifiers, centre, delay);
        break;
    case MouseClick:
        QTest::mouseClick(item->window(), Qt::LeftButton, modifiers, centre, delay);
        break;
    case MouseDoubleClick:
        QTest::mouseDClick(item->window(), Qt::LeftButton, modifiers, centre, delay);
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

void TestHelper::changeCanvasSize(int width, int height)
{
    // Open the canvas size popup.
    mouseEventOnCentre(canvasSizeButton, MouseClick);
    const QObject *canvasSizePopup = findPopupFromTypeName("CanvasSizePopup");
    QVERIFY(canvasSizePopup);
    QVERIFY(canvasSizePopup->property("visible").toBool());

    // Change the values and then cancel.
    // TODO: use actual input events...
    QQuickItem *widthSpinBox = canvasSizePopup->findChild<QQuickItem*>("changeCanvasWidthSpinBox");
    QVERIFY(widthSpinBox);
    // We want it to be easy to change the values with the keyboard..
    QVERIFY(widthSpinBox->hasActiveFocus());
    const int originalWidthSpinBoxValue = widthSpinBox->property("value").toInt();
    QVERIFY(widthSpinBox->setProperty("value", originalWidthSpinBoxValue + 1));
    QCOMPARE(widthSpinBox->property("value").toInt(), originalWidthSpinBoxValue + 1);

    QQuickItem *heightSpinBox = canvasSizePopup->findChild<QQuickItem*>("changeCanvasHeightSpinBox");
    const int originalHeightSpinBoxValue = heightSpinBox->property("value").toInt();
    QVERIFY(heightSpinBox);
    QVERIFY(heightSpinBox->setProperty("value", originalHeightSpinBoxValue - 1));
    QCOMPARE(heightSpinBox->property("value").toInt(), originalHeightSpinBoxValue - 1);

    QQuickItem *cancelButton = canvasSizePopup->findChild<QQuickItem*>("canvasSizePopupCancelButton");
    QVERIFY(cancelButton);
    mouseEventOnCentre(cancelButton, MouseClick);
    QVERIFY(!canvasSizePopup->property("visible").toBool());
    QCOMPARE(project->size().width(), originalWidthSpinBoxValue);
    QCOMPARE(project->size().height(), originalHeightSpinBoxValue);

    // Open the popup again.
    mouseEventOnCentre(canvasSizeButton, MouseClick);
    QVERIFY(canvasSizePopup);
    QVERIFY(canvasSizePopup->property("visible").toBool());
    // The old values should be restored.
    QCOMPARE(widthSpinBox->property("value").toInt(), originalWidthSpinBoxValue);
    QCOMPARE(heightSpinBox->property("value").toInt(), originalHeightSpinBoxValue);

    // Change the values and then press OK.
    QVERIFY(widthSpinBox->setProperty("value", width));
    QCOMPARE(widthSpinBox->property("value").toInt(), width);
    QVERIFY(heightSpinBox->setProperty("value", height));
    QCOMPARE(heightSpinBox->property("value").toInt(), height);

    QQuickItem *okButton = canvasSizePopup->findChild<QQuickItem*>("canvasSizePopupOkButton");
    QVERIFY(okButton);
    mouseEventOnCentre(okButton, MouseClick);
    QVERIFY(!canvasSizePopup->property("visible").toBool());
    QCOMPARE(project->size().width(), width);
    QCOMPARE(project->size().height(), height);
    QCOMPARE(widthSpinBox->property("value").toInt(), width);
    QCOMPARE(heightSpinBox->property("value").toInt(), height);
}

void TestHelper::changeToolSize(int size)
{
    mouseEventOnCentre(toolSizeButton, MouseClick);
    const QObject *toolSizePopup = findPopupFromTypeName("ToolSizePopup");
    QVERIFY(toolSizePopup);
    QCOMPARE(toolSizePopup->property("visible").toBool(), true);

    QQuickItem *toolSizeSlider = toolSizePopup->findChild<QQuickItem*>("toolSizeSlider");
    QVERIFY(toolSizeSlider);

    QQuickItem *toolSizeSliderHandle = toolSizeSlider->property("handle").value<QQuickItem*>();
    QVERIFY(toolSizeSliderHandle);

    // Move the slider to the right to find the max pos.
    QVERIFY(toolSizeSlider->setProperty("value", toolSizeSlider->property("to").toReal()));
    QCOMPARE(toolSizeSlider->property("value"), toolSizeSlider->property("to"));
    const QPoint handleMaxPos = toolSizeSliderHandle->mapToScene(
        QPointF(toolSizeSliderHandle->width() / 2, toolSizeSliderHandle->height() / 2)).toPoint();

    // Move/reset the slider to the left since we move from left to right.
    QVERIFY(toolSizeSlider->setProperty("value", toolSizeSlider->property("from").toReal()));
    QCOMPARE(toolSizeSlider->property("value"), toolSizeSlider->property("from"));
    const QPoint handleMinPos = toolSizeSliderHandle->mapToScene(
        QPointF(toolSizeSliderHandle->width() / 2, toolSizeSlider->height() / 2)).toPoint();

    QPoint sliderHandlePos = handleMinPos;
    QTest::mousePress(toolSizeSlider->window(), Qt::LeftButton, Qt::NoModifier, sliderHandlePos);
    QCOMPARE(toolSizeSlider->property("pressed").toBool(), true);
    QCOMPARE(window->mouseGrabberItem(), toolSizeSlider);

    QTest::mouseMove(toolSizeSlider->window(), sliderHandlePos, 5);

    // Move the slider's handle until we find the value we want.
    for (;
         sliderValue(toolSizeSlider) != size && sliderHandlePos.x() < handleMaxPos.x();
         ++sliderHandlePos.rx()) {
        QTest::mouseMove(toolSizeSlider->window(), sliderHandlePos, 5);
    }
    QTest::mouseRelease(toolSizeSlider->window(), Qt::LeftButton, Qt::NoModifier, sliderHandlePos);
    QCOMPARE(toolSizeSlider->property("pressed").toBool(), false);
    QCOMPARE(sliderValue(toolSizeSlider), size);

    // Close the popup.
    QTest::keyClick(window, Qt::Key_Escape);
    QCOMPARE(toolSizePopup->property("visible").toBool(), false);
}

int TestHelper::sliderValue(QQuickItem *slider) const
{
    const qreal position = slider->property("position").toReal();
    qreal value = 0;
    QMetaObject::invokeMethod(slider, "valueAt", Qt::DirectConnection, Q_RETURN_ARG(qreal, value), Q_ARG(qreal, position));
    return qFloor(value);
}

void TestHelper::drawPixelAtCursorPos()
{
    if (tilesetProject) {
        const Tile *targetTile = tilesetProject->tileAt(cursorPos);
        QVERIFY(targetTile);

        switchTool(TileCanvas::PenTool);
        switchMode(TileCanvas::PixelMode);

        // Draw on some pixels of the current tile.
        const QImage originalTileImage = targetTile->tileset()->image()->copy(targetTile->sourceRect());
        QImage expectedImage = originalTileImage;
        expectedImage.setPixelColor(tileCanvas->scenePosToTilePixelPos(cursorPos), tileCanvas->penForegroundColour());

        QTest::mouseMove(window, cursorPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QVERIFY(targetTile->tileset()->image()->copy(targetTile->sourceRect()) != originalTileImage);
        QCOMPARE(targetTile->tileset()->image()->copy(targetTile->sourceRect()), expectedImage);
        QVERIFY(tilesetProject->hasUnsavedChanges());
        QVERIFY(window->title().contains("*"));

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QCOMPARE(targetTile->tileset()->image()->copy(targetTile->sourceRect()), expectedImage);
        QVERIFY(tilesetProject->hasUnsavedChanges());
        QVERIFY(window->title().contains("*"));
    } else {
        switchTool(TileCanvas::PenTool);

        QTest::mouseMove(window, cursorWindowPos);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QCOMPARE(canvas->currentProjectImage()->pixelColor(cursorPos), canvas->penForegroundColour());
        QVERIFY(project->hasUnsavedChanges());
        QVERIFY(window->title().contains("*"));

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
        QCOMPARE(canvas->currentProjectImage()->pixelColor(cursorPos), canvas->penForegroundColour());
        QVERIFY(project->hasUnsavedChanges());
        QVERIFY(window->title().contains("*"));
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
    QCOMPARE(tilesetProject->tileAt(cursorPos)->id(), penId);
}

static QString fuzzyColourCompareFailMsg(const QColor &colour1, const QColor &colour2,
    const QChar &componentName, int difference, int fuzz)
{
    return QString::fromLatin1("colour1 %1 is not equal to colour2 %2; %3 difference of %4 is larger than fuzz of %5")
            .arg(colour1.name()).arg(colour2.name()).arg(componentName).arg(difference).arg(fuzz);
}

void TestHelper::fuzzyColourCompare(const QColor &colour1, const QColor &colour2, int fuzz)
{
    const int rDiff = qAbs(colour2.red() - colour1.red());
    QVERIFY2(rDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('r'), rDiff, fuzz)));

    const int gDiff = qAbs(colour2.green() - colour1.green());
    QVERIFY2(gDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('g'), gDiff, fuzz)));

    const int bDiff = qAbs(colour2.blue() - colour1.blue());
    QVERIFY2(bDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('b'), bDiff, fuzz)));

    const int aDiff = qAbs(colour2.alpha() - colour1.alpha());
    QVERIFY2(aDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(colour1, colour2, QLatin1Char('a'), aDiff, fuzz)));
}

void TestHelper::fuzzyImageCompare(const QImage &image1, const QImage &image2)
{
    QCOMPARE(image1.size(), image2.size());

    for (int y = 0; y < image1.height(); ++y) {
        for (int x = 0; x < image1.width(); ++x) {
            fuzzyColourCompare(image1.pixelColor(x, y), image2.pixelColor(x, y));
        }
    }
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

static QQuickItem *findChildWithText(QQuickItem *item, const QString &text)
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

QQuickItem *TestHelper::findDialogButton(const QObject *dialog, const QString &text)
{
    QQuickItem *footer = dialog->property("footer").value<QQuickItem*>();
    if (!footer)
        return nullptr;

    return findChildWithText(footer, text);
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

QPoint TestHelper::mapToTile(const QPoint &cursorPos) const
{
    return cursorPos - tileCanvas->mapToScene(QPointF(0, 0)).toPoint();
}

QPoint TestHelper::tileSceneCentre(int xPosInTiles, int yPosInTiles) const
{
    return tileCanvas->mapToScene(QPointF(
        xPosInTiles * tilesetProject->tileWidth() + tilesetProject->tileWidth() / 2,
        yPosInTiles * tilesetProject->tileHeight() + tilesetProject->tileHeight() / 2)).toPoint()
            + tileCanvas->firstPane()->offset();
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
    cursorWindowPos = canvas->mapToScene(cursorPos).toPoint() + canvas->firstPane()->offset();
    if (verifyWithinWindow) {
        QVERIFY2(cursorWindowPos.x() >= 0 && cursorWindowPos.y() >= 0,
                 qPrintable(QString::fromLatin1("x %1 y %2").arg(cursorWindowPos.x()).arg(cursorWindowPos.y())));
    }
}

void TestHelper::setCursorPosInScenePixels(const QPoint &posInScenePixels)
{
    cursorPos = posInScenePixels;
    cursorWindowPos = canvas->mapToScene(QPointF(posInScenePixels.x(), posInScenePixels.y())).toPoint()
            + canvas->firstPane()->offset();
}

QPoint TestHelper::tilesetTileCentre(int xPosInTiles, int yPosInTiles) const
{
    return QPoint(
        xPosInTiles * tilesetProject->tileWidth() + tilesetProject->tileWidth() / 2,
        yPosInTiles * tilesetProject->tileHeight() + tilesetProject->tileHeight() / 2);
}

QPoint TestHelper::tilesetTileSceneCentre(int xPosInTiles, int yPosInTiles) const
{
    return tilesetSwatch->mapToScene(QPointF(
         xPosInTiles * tilesetProject->tileWidth() + tilesetProject->tileWidth() / 2,
         yPosInTiles * tilesetProject->tileHeight() + tilesetProject->tileHeight() / 2)).toPoint();
}

void TestHelper::keySequence(QWindow *window, QKeySequence sequence)
{
    for (int i = 0; i < sequence.count(); ++i) {
        Qt::Key key = Qt::Key(sequence[i] & ~Qt::KeyboardModifierMask);
        Qt::KeyboardModifiers modifiers = Qt::KeyboardModifiers(sequence[i] & Qt::KeyboardModifierMask);
        QTest::keyClick(window, key, modifiers);
    }
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

void TestHelper::triggerShortcut(const QString &objectName, const QString &sequenceAsString)
{
    QObject *shortcut = window->findChild<QObject*>(objectName);
    QVERIFY(shortcut);
    QVERIFY2(shortcut->property("enabled").toBool(), qPrintable(QString::fromLatin1(
        "Shortcut \"%1\" is not enabled (%2 has active focus; does this shortcut require the canvas to have it?)")
            .arg(objectName, window->activeFocusItem() ? window->activeFocusItem()->objectName() : QString())));

    QSignalSpy activatedSpy(shortcut, SIGNAL(activated()));
    QVERIFY(activatedSpy.isValid());

    QTest::qWaitForWindowActive(window);
    const int value = QKeySequence(sequenceAsString)[0];
    Qt::KeyboardModifiers mods = (Qt::KeyboardModifiers)(value & Qt::KeyboardModifierMask);
    QTest::keyClick(window, value & ~mods, mods);
    QCOMPARE(activatedSpy.count(), 1);
}

void TestHelper::triggerNewProject()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(newMenuButton, MouseClick);
#else
    triggerShortcut("newShortcut", app.settings()->newShortcut());
#endif
}

void TestHelper::triggerCloseProject()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(closeMenuButton, MouseClick);
#else
    triggerShortcut("closeShortcut", app.settings()->closeShortcut());
#endif
}

void TestHelper::triggerSaveProject()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(saveMenuButton, MouseClick);
#else
    triggerShortcut("saveShortcut", app.settings()->saveShortcut());
#endif
}

void TestHelper::triggerSaveProjectAs()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(saveAsMenuButton, MouseClick);
#else
    QFAIL("TODO: no saveas shortcut");
//    triggerShortcut(app.settings()->saveShortcut());
#endif
}

void TestHelper::triggerOpenProject()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(openMenuButton, MouseClick);
#else
    triggerShortcut("openShortcut", app.settings()->openShortcut());
#endif
}

void TestHelper::triggerRevert()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(fileToolButton, MouseClick);
    mouseEventOnCentre(revertMenuButton, MouseClick);
#else
    triggerShortcut("revertShortcut", app.settings()->revertShortcut());
#endif
}

void TestHelper::triggerCentre()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(centreMenuButton, MouseClick);
#else
    triggerShortcut("centreShortcut", app.settings()->centreShortcut());
#endif
}

void TestHelper::triggerGridVisible()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(showGridMenuButton, MouseClick);
#else
    triggerShortcut("gridVisibleShortcut", app.settings()->gridVisibleShortcut());
#endif
}

void TestHelper::triggerRulersVisible()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(showRulersMenuButton, MouseClick);
#else
    triggerShortcut("rulersVisibleShortcut", app.settings()->rulersVisibleShortcut());
#endif
}

void TestHelper::triggerSplitScreen()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(viewToolButton, MouseClick);
    mouseEventOnCentre(splitScreenMenuButton, MouseClick);
#else
    triggerShortcut("splitScreenShortcut", app.settings()->splitScreenShortcut());
#endif
}

void TestHelper::triggerOptions()
{
#ifdef NON_NATIVE_MENUS
    mouseEventOnCentre(optionsToolButton, MouseClick);
    mouseEventOnCentre(settingsMenuButton, MouseClick);
#else
    triggerShortcut("optionsShortcut", app.settings()->optionsShortcut());
#endif
}

void TestHelper::selectLayer(const QString &layerName, int layerIndex)
{
    QTRY_VERIFY(findListViewChild("layerListView", layerName));
    QQuickItem *layerDelegate = findListViewChild("layerListView", layerName);
    QVERIFY(layerDelegate);
    mouseEventOnCentre(layerDelegate, MouseClick);
    QCOMPARE(layerDelegate->property("checked").toBool(), true);
    QCOMPARE(layeredImageProject->currentLayerIndex(), layerIndex);
}

void TestHelper::verifyLayerName(const QString &layerName, QQuickItem **layerDelegate)
{
    // It seems that the ListView sometimes need some extra time to create the second item (e.g. when debugging).
    QTRY_VERIFY(findListViewChild("layerListView", layerName));
    QQuickItem *delegate = findListViewChild("layerListView", layerName);
    QVERIFY(delegate);
    QQuickItem *layerDelegateNameTextField = delegate->findChild<QQuickItem*>("layerNameTextField");
    QVERIFY(layerDelegateNameTextField);
    QCOMPARE(layerDelegateNameTextField->property("text").toString(), layerName);
    if (layerDelegate)
        *layerDelegate = delegate;
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

void TestHelper::createNewProject(Project::Type projectType, const QVariantMap &args)
{
    const Project::Type previousProjectType = tilesetProject ? Project::TilesetType : (imageProject ? Project::ImageType : Project::LayeredImageType);
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
        QCOMPARE(window->cursor().shape(), Qt::ArrowCursor);
    }

    if (creationErrorOccurredSpy)
        creationErrorOccurredSpy->clear();

    // Click the new project button.
    triggerNewProject();

    // Check that we get prompted to discard any changes.
    if (project && project->hasUnsavedChanges()) {
        const QObject *discardChangesDialog = window->contentItem()->findChild<QObject*>("discardChangesDialog");
        QVERIFY(discardChangesDialog);
        QVERIFY(discardChangesDialog->property("visible").toBool());

        QQuickItem *yesButton = findDialogButton(discardChangesDialog, "Yes");
        QVERIFY(yesButton);
        mouseEventOnCentre(yesButton, MouseClick);
        QVERIFY(!discardChangesDialog->property("visible").toBool());
    }

    // Ensure that the new project popup is visible.
    const QObject *newProjectPopup = findPopupFromTypeName("NewProjectPopup");
    QVERIFY(newProjectPopup);
    QVERIFY(newProjectPopup->property("visible").toBool());
    // TODO: remove this when https://bugreports.qt.io/browse/QTBUG-53420 is fixed
    newProjectPopup->property("contentItem").value<QQuickItem*>()->forceActiveFocus();
    QVERIFY2(newProjectPopup->property("activeFocus").toBool(),
        qPrintable(QString::fromLatin1("NewProjectPopup doesn't have active focus (%1 does)").arg(window->activeFocusItem()->objectName())));

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
    QVERIFY(tilesetProjectButton);

    mouseEventOnCentre(tilesetProjectButton, MouseClick);
    QCOMPARE(tilesetProjectButton->property("checked").toBool(), true);

    QTRY_COMPARE(newProjectPopup->property("visible").toBool(), false);

    if (projectType == Project::TilesetType) {
        // Create a temporary directory containing a tileset image for us to use.
        setupTempTilesetProjectDir();

        // Now the New Tileset Project popup should be visible.
        QTRY_VERIFY(findPopupFromTypeName("NewTilesetProjectPopup"));
        const QObject *newTilesetProjectPopup = findPopupFromTypeName("NewTilesetProjectPopup");
        QVERIFY(newTilesetProjectPopup->property("visible").toBool());

        // Ensure that the popup gets reset each time it's opened.
        QQuickItem *useExistingTilesetCheckBox = newTilesetProjectPopup->findChild<QQuickItem*>("useExistingTilesetCheckBox");
        QVERIFY(useExistingTilesetCheckBox);
        QVERIFY(useExistingTilesetCheckBox->property("checked").isValid());
        QVERIFY(!useExistingTilesetCheckBox->property("checked").toBool());

        QQuickItem *tilesetField = newTilesetProjectPopup->findChild<QQuickItem*>("tilesetPathTextField");
        QVERIFY(tilesetField);
        QVERIFY(tilesetField->property("text").isValid());
        QVERIFY(tilesetField->property("text").toString().isEmpty());

        QQuickItem *tileWidthSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tileWidthSpinBox");
        QVERIFY(tileWidthSpinBox);
        QCOMPARE(tileWidthSpinBox->property("value").toInt(), 32);

        QQuickItem *tileHeightSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tileHeightSpinBox");
        QVERIFY(tileHeightSpinBox);
        QCOMPARE(tileHeightSpinBox->property("value").toInt(), 32);

        QQuickItem *tilesWideSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tilesWideSpinBox");
        QVERIFY(tilesWideSpinBox);
        QCOMPARE(tilesWideSpinBox->property("value").toInt(), 10);

        QQuickItem *tilesHighSpinBox = newTilesetProjectPopup->findChild<QQuickItem*>("tilesHighSpinBox");
        QVERIFY(tilesHighSpinBox);
        QCOMPARE(tilesHighSpinBox->property("value").toInt(), 10);

        QVERIFY(tileWidthSpinBox->setProperty("value", tileWidth));
        QCOMPARE(tileWidthSpinBox->property("value").toInt(), tileWidth);

        QVERIFY(tileHeightSpinBox->setProperty("value", tileHeight));
        QCOMPARE(tileHeightSpinBox->property("value").toInt(), tileHeight);

        if (tilesetTilesWide == -1 || tilesetTilesHigh == -1) {
            // Using an existing tileset image.

            // Check the "use existing tileset" checkbox.
            // Hack: it has focus in the app, but not in the test => meh
            useExistingTilesetCheckBox->forceActiveFocus();
            QVERIFY(useExistingTilesetCheckBox->hasActiveFocus());
            QTest::keyClick(window, Qt::Key_Space);
            QVERIFY(useExistingTilesetCheckBox->property("checked").toBool());

            // Input the filename.
            QTest::keyClick(window, Qt::Key_Tab);
            QVERIFY(tilesetField->hasActiveFocus());
            // TODO: input path using events
            QVERIFY(tilesetField->property("text").isValid());
            QVERIFY(tilesetField->setProperty("text", tempTilesetUrl.toString()));

            QQuickItem *invalidFileIcon = newTilesetProjectPopup->findChild<QQuickItem*>("invalidFileIcon");
            QVERIFY(invalidFileIcon);
            QObject *fileValidator = newTilesetProjectPopup->findChild<QObject*>("validator");
            QVERIFY2(qFuzzyCompare(invalidFileIcon->property("opacity").toReal(), 0),
                qPrintable(QString::fromLatin1("File should be valid, but got error message \"%1\"; file path: %2")
                    .arg(fileValidator->property("errorMessage").toString())
                    .arg(tilesetField->property("text").toString())));
        } else {
            // Using a new tileset image.
            QVERIFY(tilesetTilesWide > 0 && tilesetTilesHigh > 0);

            QVERIFY(tilesWideSpinBox->setProperty("value", tilesetTilesWide));
            QCOMPARE(tilesWideSpinBox->property("value").toInt(), tilesetTilesWide);

            QVERIFY(tilesHighSpinBox->setProperty("value", tilesetTilesHigh));
            QCOMPARE(tilesHighSpinBox->property("value").toInt(), tilesetTilesHigh);

            QQuickItem *transparentBackgroundCheckBox = newTilesetProjectPopup->findChild<QQuickItem*>("transparentBackgroundCheckBox");
            QVERIFY(transparentBackgroundCheckBox);
            QCOMPARE(transparentBackgroundCheckBox->property("checked").toBool(), true);

            if (!transparentTilesetBackground) {
                mouseEventOnCentre(transparentBackgroundCheckBox, MouseClick);
                QCOMPARE(transparentBackgroundCheckBox->property("checked").toBool(), false);
            }
        }

        // Confirm creation of the project.
        QQuickItem *okButton = newTilesetProjectPopup->findChild<QQuickItem*>("newTilesetProjectOkButton");
        QVERIFY(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        QVERIFY(!newTilesetProjectPopup->property("visible").toBool());
    } else {
        // Create a temporary directory that we can save into, etc.
        if (projectType == Project::LayeredImageType) {
            setupTempLayeredImageProjectDir();
        }

        // Now the New Image Project popup should be visible.
        QTRY_VERIFY(findPopupFromTypeName("NewImageProjectPopup"));
        const QObject *newImageProjectPopup = findPopupFromTypeName("NewImageProjectPopup");
        QVERIFY(newImageProjectPopup->property("visible").toBool());

        // Ensure that the popup gets reset each time it's opened.
        QQuickItem *imageWidthSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageWidthSpinBox");
        QVERIFY(imageWidthSpinBox);
        QCOMPARE(imageWidthSpinBox->property("value").toInt(), 256);

        QQuickItem *imageHeightSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageHeightSpinBox");
        QVERIFY(imageHeightSpinBox);
        QCOMPARE(imageHeightSpinBox->property("value").toInt(), 256);

        QVERIFY(imageWidthSpinBox->setProperty("value", imageWidth));
        QCOMPARE(imageWidthSpinBox->property("value").toInt(), imageWidth);

        QVERIFY(imageHeightSpinBox->setProperty("value", imageHeight));
        QCOMPARE(imageHeightSpinBox->property("value").toInt(), imageHeight);

        QQuickItem *transparentImageBackgroundCheckBox = newImageProjectPopup->findChild<QQuickItem*>("transparentImageBackgroundCheckBox");
        QVERIFY(transparentImageBackgroundCheckBox);
        QVERIFY(transparentImageBackgroundCheckBox->property("checked").isValid());
        QCOMPARE(transparentImageBackgroundCheckBox->property("checked").toBool(), false);

        if (transparentImageBackground) {
            mouseEventOnCentre(transparentImageBackgroundCheckBox, MouseClick);
            QCOMPARE(transparentImageBackgroundCheckBox->property("checked").toBool(), transparentImageBackground);
        }

        // Confirm creation of the project.
        QQuickItem *okButton = newImageProjectPopup->findChild<QQuickItem*>("newImageProjectOkButton");
        QVERIFY(okButton);
        mouseEventOnCentre(okButton, MouseClick);
        QVERIFY(!newImageProjectPopup->property("visible").toBool());
    }

    if (previousProjectType != projectType) {
        // The projects and canvases that we had references to should have
        // been destroyed by now.
        QTRY_VERIFY(!project);
        QVERIFY(!imageProject);
        QVERIFY(!tilesetProject);

        QVERIFY(!canvas);
        QVERIFY(!imageCanvas);
        QVERIFY(!tileCanvas);
    }

    project = window->contentItem()->findChild<Project*>();
    QVERIFY(project);

    canvas = window->findChild<ImageCanvas*>();
    QVERIFY(canvas);

    if (projectType == Project::TilesetType) {
        tilesetProject = qobject_cast<TilesetProject*>(project);
        QVERIFY(tilesetProject);

        tileCanvas = qobject_cast<TileCanvas*>(canvas);
        QVERIFY(tileCanvas);
    } else if (projectType == Project::ImageType) {
        imageProject = qobject_cast<ImageProject*>(project);
        QVERIFY(imageProject);

        imageCanvas = canvas;
    } else if (projectType == Project::LayeredImageType) {
        layeredImageProject = qobject_cast<LayeredImageProject*>(project);
        QVERIFY(layeredImageProject);

        layeredImageCanvas = qobject_cast<LayeredImageCanvas*>(canvas);;
    }

    canvas->forceActiveFocus();
//    QTRY_VERIFY(canvas->hasActiveFocus());

    QCOMPARE(project->url(), QUrl());
    QVERIFY(project->hasLoaded());
    QVERIFY(project->isNewProject());

    // Reset any settings that have changed back to their defaults.
    QVariant settingsAsVariant = qmlEngine(canvas)->rootContext()->contextProperty("settings");
    QVERIFY(settingsAsVariant.isValid());
    ApplicationSettings *settings = settingsAsVariant.value<ApplicationSettings*>();
    QVERIFY(settings);
    settings->resetShortcutsToDefaults();

    if (settings->areRulersVisible()) {
        triggerRulersVisible();
        QCOMPARE(settings->areRulersVisible(), false);
    }

    cursorPos = QPoint();
    cursorWindowPos = QPoint();

    // Sanity check.
    QCOMPARE(canvas->window(), canvasSizeButton->window());
    QCOMPARE(canvas->splitter()->position(), 0.5);
    QVERIFY(!canvas->splitter()->isPressed());
    QVERIFY(!canvas->splitter()->isHovered());

#ifdef NON_NATIVE_MENUS
    QVERIFY(newMenuButton->isEnabled());
    QVERIFY(saveMenuButton->isEnabled());
    QVERIFY(saveAsMenuButton->isEnabled());
    QVERIFY(openMenuButton->isEnabled());
    QVERIFY(closeMenuButton->isEnabled());
    QVERIFY(settingsMenuButton->isEnabled());
#endif

    if (isTilesetProject) {
        // Establish references to TilesetProject-specific properties.
        tilesetSwatch = window->findChild<QQuickItem*>("tilesetSwatch");
        QVERIFY(tilesetSwatch);
        QCOMPARE(tilesetSwatch->isVisible(), true);
        QVERIFY(!qFuzzyIsNull(tilesetSwatch->width()));
        QVERIFY(!qFuzzyIsNull(tilesetSwatch->height()));

        // Ensure that the tileset swatch flickable has the correct contentY.
        QQuickItem *tilesetSwatchFlickable = tilesetSwatch->findChild<QQuickItem*>("tilesetSwatchFlickable");
        QVERIFY(tilesetSwatchFlickable);
        QVERIFY(tilesetSwatchFlickable->property("contentY").isValid());
        QCOMPARE(tilesetSwatchFlickable->property("contentY").toReal(), 0.0);

#ifdef NON_NATIVE_MENUS
        duplicateTileMenuButton = window->findChild<QQuickItem*>("duplicateTileMenuButton");
        QVERIFY(duplicateTileMenuButton);

        rotateTileLeftMenuButton = window->findChild<QQuickItem*>("rotateTileLeftMenuButton");
        QVERIFY(rotateTileLeftMenuButton);

        rotateTileRightMenuButton = window->findChild<QQuickItem*>("rotateTileRightMenuButton");
        QVERIFY(rotateTileRightMenuButton);
#endif

        QVERIFY(imageGrabber.requestImage(tileCanvas));
        QTRY_VERIFY(imageGrabber.isReady());
        QCOMPARE(imageGrabber.takeImage().pixelColor(16, 16), tileCanvas->mapBackgroundColour());
    } else {
        QVERIFY(window->findChild<QQuickItem*>("selectionToolButton"));
    }
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

    QCOMPARE(layeredImageProject->layerCount(), 1);
    QCOMPARE(layeredImageProject->currentLayerIndex(), 0);
    QVERIFY(layeredImageProject->currentLayer());
    QCOMPARE(layeredImageProject->layerAt(0)->name(), QLatin1String("Layer 1"));

    {
        // Ensure that what the user sees (the delegate) is correct.
        QQuickItem *layer1Delegate = findListViewChild("layerListView", QLatin1String("Layer 1"));
        QVERIFY(layer1Delegate);

        QQuickItem *nameTextField = layer1Delegate->findChild<QQuickItem*>("layerNameTextField");
        QVERIFY(nameTextField);
        QCOMPARE(nameTextField->property("text").toString(), QLatin1String("Layer 1"));
    }

    newLayerButton = window->findChild<QQuickItem*>("newLayerButton");
    QVERIFY(newLayerButton);

    moveLayerUpButton = window->findChild<QQuickItem*>("moveLayerUpButton");
    QVERIFY(moveLayerUpButton);

    moveLayerDownButton = window->findChild<QQuickItem*>("moveLayerDownButton");
    QVERIFY(moveLayerDownButton);
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
    QVERIFY2(tempProjectDir->isValid(), qPrintable(tempProjectDir->errorString()));

    foreach (const QString &basename, resourceFilesToCopy) {
        QFile sourceFile(":/resources/" + basename);
        QVERIFY2(sourceFile.open(QIODevice::ReadOnly), qPrintable(QString::fromLatin1(
            "Failed to open %1: %2").arg(sourceFile.fileName()).arg(sourceFile.errorString())));
        sourceFile.close();

        const QString saveFilePath = tempProjectDir->path() + "/" + basename;
        QVERIFY2(QFile::copy(sourceFile.fileName(), saveFilePath), qPrintable(QString::fromLatin1(
            "Failed to copy %1 to %2: %3").arg(sourceFile.fileName()).arg(saveFilePath).arg(sourceFile.errorString())));

        // A file copied from a file that is part of resources will always be read-only...
        QFile copiedFile(saveFilePath);
        QVERIFY(copiedFile.size() > 0);

        QVERIFY(copiedFile.setPermissions(QFile::ReadUser | QFile::WriteUser));
        QVERIFY(copiedFile.size() > 0);
        QVERIFY2(copiedFile.open(QIODevice::ReadWrite), qPrintable(QString::fromLatin1(
            "Error opening file at %1: %2").arg(saveFilePath).arg(copiedFile.errorString())));

        if (filesCopied)
            *filesCopied << saveFilePath;
    }
}

void TestHelper::switchMode(TileCanvas::Mode mode)
{
    if (tileCanvas->mode() == mode)
        return;

    mouseEventOnCentre(modeToolButton, MouseClick);
    const bool expectChecked = mode == TileCanvas::TileMode;
    QVERIFY2(modeToolButton->property("checked").toBool() == expectChecked, qPrintable(QString::fromLatin1(
        "modeToolButton.checked should be %1, but it's %2 (trying to set mode %3)")
            .arg(expectChecked).arg(modeToolButton->property("checked").toBool()).arg(mode)));
    QCOMPARE(tileCanvas->mode(), mode);
}

void TestHelper::switchTool(ImageCanvas::Tool tool)
{
    if (canvas->tool() == tool)
        return;

    if (tool == ImageCanvas::PenTool) {
        mouseEventOnCentre(penToolButton, MouseClick);
    } else if (tool == ImageCanvas::EyeDropperTool) {
        mouseEventOnCentre(eyeDropperToolButton, MouseClick);
    } else if (tool == ImageCanvas::FillTool) {
        mouseEventOnCentre(fillToolButton, MouseClick);
    } else if (tool == ImageCanvas::EraserTool) {
        mouseEventOnCentre(eraserToolButton, MouseClick);
    } else if (tool == ImageCanvas::SelectionTool) {
        mouseEventOnCentre(selectionToolButton, MouseClick);
    }
    QCOMPARE(canvas->tool(), tool);
}

void TestHelper::panTopLeftTo(int x, int y)
{
    const QPoint panDistance = QPoint(x, y) - canvas->firstPane()->offset();
    panBy(panDistance.x(), panDistance.y());
}

void TestHelper::panBy(int xDistance, int yDistance)
{
    QPoint pressPos = firstPaneSceneCentre();
    QTest::mouseMove(window, pressPos);
    QCOMPARE(canvas->currentPane(), canvas->firstPane());

    // TODO: get image checks working
    //        QVERIFY(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        const QImage originalImage = imageGrabber.takeImage();

    const QPoint originalOffset = canvas->currentPane()->offset();
    const QPoint expectedOffset = originalOffset + QPoint(xDistance, yDistance);

    QTest::keyPress(window, Qt::Key_Space);
    QCOMPARE(window->cursor().shape(), Qt::OpenHandCursor);
    QCOMPARE(canvas->currentPane()->offset(), originalOffset);
    //        QVERIFY(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        // Cursor changed to OpenHandCursor.
    //        QImage currentImage = imageGrabber.takeImage();
    //        QVERIFY(currentImage != originalImage);
    //        QImage lastImage = currentImage;

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, pressPos);
    QCOMPARE(window->cursor().shape(), Qt::ClosedHandCursor);
    QCOMPARE(canvas->currentPane()->offset(), originalOffset);
    //        QVERIFY(imageGrabber.requestImage(canvas));
    //        QTRY_VERIFY(imageGrabber.isReady());
    //        currentImage = imageGrabber.takeImage();
    //        // Cursor changed to ClosedHandCursor.
    //        QVERIFY(currentImage != lastImage);
    //        lastImage = currentImage;

    QTest::mouseMove(window, pressPos + QPoint(xDistance, yDistance));
    QCOMPARE(window->cursor().shape(), Qt::ClosedHandCursor);
    QCOMPARE(canvas->currentPane()->offset(), expectedOffset);
    //        QVERIFY(imageGrabber.requestImage(canvas));
    //        // Pane offset changed.
    //        currentImage = imageGrabber.takeImage();
    //        QVERIFY(currentImage != lastImage);
    //        lastImage = currentImage;

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, pressPos + QPoint(xDistance, yDistance));
    QCOMPARE(window->cursor().shape(), Qt::OpenHandCursor);
    QCOMPARE(canvas->currentPane()->offset(), expectedOffset);

    QTest::keyRelease(window, Qt::Key_Space);
    QCOMPARE(window->cursor().shape(), Qt::BlankCursor);
    QCOMPARE(canvas->currentPane()->offset(), expectedOffset);
}
