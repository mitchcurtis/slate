/*
    Copyright 2020, Mitch Curtis

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

#include <QClipboard>
#include <QPainter>
#include <QPair>
#include <QLoggingCategory>
#include <QQuickStyle>
#include <QtQuickTest>

#include "clipboard.h"
#include "imagelayer.h"
#include "imageutils.h"
#include "projectmanager.h"
#include "projectutils.h"
#include "qtutils.h"

Q_LOGGING_CATEGORY(lcTestHelper, "tests.testHelper")
Q_LOGGING_CATEGORY(lcFindListViewChild, "tests.testHelper.findListViewChild")
Q_LOGGING_CATEGORY(lcFindPopupFromTypeName, "tests.testHelper.findPopupFromTypeName")

static const char *macOSStyleName = "macOS";

TestHelper::TestHelper(int &argc, char **argv) :
    app(argc, argv, QStringLiteral("Slate Test Suite")),
    window(qobject_cast<QQuickWindow*>(app.qmlEngine()->rootObjects().first())),
    offscreenPlatform(QGuiApplication::platformName() == QStringLiteral("offscreen")),
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
    QVERIFY(window);
    QVERIFY(QTest::qWaitForWindowExposed(window));
    const QPoint screenCentre = window->screen()->availableGeometry().center();
    window->setPosition(screenCentre.x() - window->size().width() / 2, screenCentre.y() - window->size().height() / 2);

    // This should not be enabled for tests.
    QVERIFY(!app.settings()->loadLastOnStartup());

    QQmlExpression overlayExpression(qmlContext(window), window, "Overlay.overlay");
    overlay = overlayExpression.evaluate().value<QQuickItem*>();
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

    toolBar = window->findChild<QQuickItem*>("toolBar");
    QVERIFY(toolBar);

    canvasSizeToolButton = window->findChild<QQuickItem*>("canvasSizeToolButton");
    QVERIFY(canvasSizeToolButton);

    imageSizeToolButton = window->findChild<QQuickItem*>("imageSizeToolButton");
    QVERIFY(imageSizeToolButton);

    cropToSelectionToolButton = window->findChild<QQuickItem*>("cropToSelectionToolButton");
    QVERIFY(cropToSelectionToolButton);

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

    noteToolButton = window->findChild<QQuickItem*>("noteToolButton");
    QVERIFY(noteToolButton);

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

    undoToolButton = window->findChild<QQuickItem*>("undoToolButton");
    QVERIFY(undoToolButton);

    redoToolButton = window->findChild<QQuickItem*>("redoToolButton");
    QVERIFY(redoToolButton);

    lockGuidesToolButton = window->findChild<QQuickItem*>("lockGuidesToolButton");
    QVERIFY(lockGuidesToolButton);

    showNotesToolButton = window->findChild<QQuickItem*>("showNotesToolButton");
    QVERIFY(showNotesToolButton);

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

    fullScreenToolButton = window->findChild<QQuickItem*>("fullScreenToolButton");
    QVERIFY(fullScreenToolButton);

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

    qGuiApp->clipboard()->setImage(QImage());

    if (canvas) {
        qCDebug(lcTestHelper) << "trying to close any open popups";
        // If a test fails, a popup could still be open. Ensure that they're
        // all closed to prevent failures in future tests.
        for (int i = 0; i < 100 && !canvas->hasActiveFocus(); ++i) {
            QTest::qWait(20);
            QTest::keyClick(window, Qt::Key_Escape);
        }
        QVERIFY(canvas->hasActiveFocus());
    }
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
    QWheelEvent wheelEvent(localPos, item->window()->mapToGlobal(localPos), QPoint(0, 0), QPoint(0, degrees * 8),
        Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QSpontaneKeyEvent::setSpontaneous(&wheelEvent);
    if (!qApp->notify(item->window(), &wheelEvent))
        qWarning("Wheel event not accepted by receiving window");
}

void TestHelper::keyClicks(const QString &text)
{
    for (const auto ch : qAsConst(text))
        QTest::keySequence(window, QKeySequence(ch));
}

//#define LERP_MOUSE_MOVE_DEBUG

void TestHelper::lerpMouseMove(const QPoint &fromScenePos, const QPoint &toScenePos, int delayInMs, int steps)
{
    lerpMouseMoveUntil(fromScenePos, toScenePos, delayInMs, steps, nullptr);
}

void TestHelper::lerpMouseMoveUntil(const QPoint &fromScenePos, const QPoint &toScenePos, int delayInMs,
    int steps, const std::function<bool ()> &untilFunc)
{
    const int xDistance = toScenePos.x() - fromScenePos.x();
    const int yDistance = toScenePos.y() - fromScenePos.y();
#ifdef LERP_MOUSE_MOVE_DEBUG
    qDebug() << "fromScenePos" << fromScenePos << "toScenePos" << toScenePos << "xDistance" << xDistance << "yDistance" << yDistance;
#endif

    if (steps == -1) {
        static int const defaultSteps = 10;
        steps = QLineF(fromScenePos, toScenePos).length() / defaultSteps;
    }

    QEasingCurve mouseLerp;
    for (int i = 0; i < steps; ++i) {
        if (untilFunc && untilFunc())
            return;

        const qreal progress = i / qreal(steps);
        const qreal lerpValue = mouseLerp.valueForProgress(progress);

        const int mouseX = fromScenePos.x() + (xDistance * lerpValue);
        const int mouseY = fromScenePos.y() + (yDistance * lerpValue);
        setCursorPosInScenePixels(mouseX, mouseY, false);
#ifdef LERP_MOUSE_MOVE_DEBUG
        qDebug() << i << progress << "lerpValue" << lerpValue << "mouseX" << mouseX << "mouseY" << mouseY << cursorWindowPos;
#endif
        QTest::mouseMove(window, cursorWindowPos, delayInMs);
    }
}

// Could make this hasActiveFocus(), but the line number in the failure message is
// better if the calling code does the verification.
QByteArray TestHelper::activeFocusFailureMessage(QQuickItem *item, const QString &when)
{
    const QString whenText = !when.isEmpty() ? ' ' + when : QString();
    return QString::fromLatin1("Expected text input %1 to have focus%2, but %3 has it instead")
        .arg(detailedObjectName(item)).arg(whenText).arg(detailedObjectName(window->activeFocusItem())).toLatin1();
}

bool TestHelper::enterText(QQuickItem *textField, const QString &text, EnterTextFlags flags)
{
    VERIFY(textField->property("text").isValid());
    VERIFY2(textField->hasActiveFocus(), activeFocusFailureMessage(textField));

    if (flags.testFlag(EnterTextFlag::ClearTextFirst) && !textField->property("text").toString().isEmpty()) {
        QTest::keySequence(window, QKeySequence::SelectAll);
        VERIFY(!textField->property("selectedText").toString().isEmpty());

        QTest::keySequence(window, Qt::Key_Backspace);
        VERIFY2(textField->property("text").toString().isEmpty(),
            QString::fromLatin1("Expected the text of %1 to be empty, but it's %2")
                .arg(detailedObjectName(textField)).arg(textField->property("text").toString()));
    }

    keyClicks(text);
    const QString textFieldText = textField->property("text").toString();
    if (flags.testFlag(EnterTextFlag::CompareAsIntegers)) {
        bool convertedTextToIntSuccessfully = false;
        const int actualTextAsInt = textFieldText.toInt(&convertedTextToIntSuccessfully);
        VERIFY(convertedTextToIntSuccessfully);
        const int expectedTextAsInt = text.toInt(&convertedTextToIntSuccessfully);
        VERIFY(convertedTextToIntSuccessfully);
        COMPARE_NON_FLOAT(actualTextAsInt, expectedTextAsInt);
    } else {
        COMPARE_NON_FLOAT(textFieldText, text);
    }
    return true;
}

bool TestHelper::selectComboBoxItem(const QString &comboBoxObjectName, int index)
{
    QQuickItem *comboBox = window->findChild<QQuickItem*>(comboBoxObjectName);
    VERIFY(comboBox);
    mouseEventOnCentre(comboBox, MouseClick);
    QObject *comboBoxPopup = comboBox->property("popup").value<QObject*>();
    VERIFY(comboBoxPopup);
    TRY_VERIFY(comboBoxPopup->property("opened").toBool() == true);

    QQuickItem *listView = comboBoxPopup->property("contentItem").value<QQuickItem*>();
    VERIFY(listView);
    QQuickItem *delegate = nullptr;
    VERIFY(QMetaObject::invokeMethod(listView, "itemAtIndex", Qt::DirectConnection,
        Q_RETURN_ARG(QQuickItem*, delegate), Q_ARG(int, index)));
    if (!clickButton(delegate))
        return false;
    VERIFY2(comboBox->property("currentIndex").toInt() == index,
        qPrintable(QString::fromLatin1("Expected currentIndex of %1 to be %2, but it's %3")
            .arg(comboBoxObjectName).arg(index).arg(comboBox->property("currentIndex").toInt())));
    return true;
}

bool TestHelper::enterTextIntoEditableSpinBox(QQuickItem *spinBox, const QString &text)
{
    VERIFY(spinBox);
    VERIFY2(spinBox->hasActiveFocus(), activeFocusFailureMessage(spinBox));
    QQuickItem *spinBoxTextInput = spinBox->property("contentItem").value<QQuickItem*>();
    VERIFY(spinBoxTextInput);
    const EnterTextFlags enterTextFlags = EnterTextFlag::ClearTextFirst | EnterTextFlag::CompareAsIntegers;
    if (!enterText(spinBoxTextInput, text, enterTextFlags))
        return false;
    return true;
}

bool TestHelper::incrementSpinBox(const QString &spinBoxObjectName, int expectedInitialValue)
{
    QQuickItem *spinBox = window->findChild<QQuickItem*>(spinBoxObjectName);
    VERIFY2(spinBox, qPrintable(QString::fromLatin1(
        "Failed to find SpinBox named \"%1\" amongst window children").arg(spinBoxObjectName)));
    QVariant valueVariant = spinBox->property("value");
    VERIFY(valueVariant.isValid());
    const int oldValue = valueVariant.toInt();
    VERIFY2(oldValue == expectedInitialValue, qPrintable(QString::fromLatin1(
        "Expected initial value to be %1, but it's %2").arg(expectedInitialValue).arg(oldValue)));

    if (QQuickStyle::name() == macOSStyleName)
        mouseEvent(spinBox, QPoint(spinBox->width() - 10, 4), MouseClick);
    else
        mouseEvent(spinBox, QPoint(spinBox->width() - 10, spinBox->height() / 2), MouseClick);
    COMPARE_NON_FLOAT(spinBox->property("value").toInt(), expectedInitialValue + 1);
    return true;
}

bool TestHelper::decrementSpinBox(const QString &spinBoxObjectName, int expectedInitialValue)
{
    QQuickItem *spinBox = window->findChild<QQuickItem*>(spinBoxObjectName);
    VERIFY(spinBox);
    QVariant valueVariant = spinBox->property("value");
    VERIFY(valueVariant.isValid());
    const int oldValue = valueVariant.toInt();
    VERIFY2(oldValue == expectedInitialValue, qPrintable(QString::fromLatin1(
        "Expected initial value to be %1, but it's %2").arg(expectedInitialValue).arg(oldValue)));

    if (QQuickStyle::name() == macOSStyleName)
        mouseEvent(spinBox, QPoint(spinBox->width() - 10, spinBox->height() - 4), MouseClick);
    else
        mouseEvent(spinBox, QPoint(10, spinBox->height() / 2), MouseClick);
    VERIFY(spinBox->property("value").toInt() == expectedInitialValue - 1);
    return true;
}

bool TestHelper::setCheckBoxChecked(const QString &checkBoxObjectName, bool checked)
{
    auto checkBox = window->findChild<QQuickItem*>(checkBoxObjectName);
    VERIFY(checkBox);
    VERIFY(checkBox->property("enabled").toBool() == true);
    VERIFY(checkBox->property("visible").toBool() == true);

    if (checkBox->property("checked").toBool() == checked)
        return true;

    if (!clickButton(checkBox))
        return false;
    VERIFY(checkBox->property("checked").toBool() == checked);
    return true;
}

bool TestHelper::clickDialogFooterButton(const QObject *dialog, const QString &buttonText)
{
    QQuickItem *button = findDialogButtonFromText(dialog, buttonText);
    VERIFY(button);
    if (!clickButton(button))
        return false;
    TRY_VERIFY(dialog->property("visible").toBool() == false);
    return true;
}

QString TestHelper::detailedObjectName(QObject *object)
{
    if (!object)
        return QLatin1String("null");

    static int detailedObjectNameDepth = -1;
    if (detailedObjectNameDepth == -1) {
        const QByteArray envVar = qgetenv("DETAILED_OBJECT_NAME_DEPTH");
        if (!envVar.isEmpty()) {
            bool converted = false;
            const int depth = envVar.toInt(&converted);
            if (converted)
                detailedObjectNameDepth = depth;
        }
        if (detailedObjectNameDepth == -1)
            detailedObjectNameDepth = 4;
    }

    QString name = object->objectName();
    // If the object doesn't have a name, use its QDebug operator<<.
    if (name.isEmpty())
        name = QtUtils::toString(object);

    // If the parent is null, try the QQuickItem parent instead.
    QObject *parent = object->parent();
    if (!parent) {
        auto asItem = qobject_cast<QQuickItem*>(object);
        if (asItem)
            parent = asItem->parentItem();
    }

    int depth = 0;
    while (parent && depth++ <= detailedObjectNameDepth) {
        QString parentObjectName = parent->objectName();
        if (parentObjectName.isEmpty())
            parentObjectName = QtUtils::toString(parent);
        name.prepend(parentObjectName + " => ");

        parent = parent->parent();
        if (!parent) {
            auto parentAsItem = qobject_cast<QQuickItem*>(parent);
            if (parentAsItem)
                parent = parentAsItem->parentItem();
        }
    }
    return name;
}

QVector<QImage> TestHelper::getLayerImages() const
{
    QVector<QImage> images;
    if (project->type() == Project::ImageType)
        images.append(*imageProject->image());
    else
        images = layeredImageProject->layerImages();
    return images;
}

Q_REQUIRED_RESULT bool TestHelper::copyAcrossLayers()
{
    VERIFY2(layeredImageProject, "Need LayeredImageProject in order to copy across layers");
    VERIFY2(canvas->hasSelection(), "Need a selection in order to copy across layers");
    layeredImageProject->copyAcrossLayers(canvas->selectionArea());
    return Clipboard::instance()->copiedLayerCount() == layeredImageProject->layerCount();
}

Q_REQUIRED_RESULT bool TestHelper::pasteAcrossLayers(int pasteX, int pasteY, bool onlyPasteIntoVisibleLayers)
{
    const QVector<QImage> originalImages = getLayerImages();

    // Open the paste across layers dialog.
    QObject *pasteAcrossLayersDialog;
    if (!findAndOpenClosedPopupFromObjectName("pasteAcrossLayersDialog", &pasteAcrossLayersDialog))
        return false;

    // Change the values and then cancel.
    QQuickItem *pasteXSpinBox = pasteAcrossLayersDialog->findChild<QQuickItem*>("pasteXSpinBox");
    VERIFY(pasteXSpinBox);
    const int originalXSpinBoxValue = pasteXSpinBox->property("value").toInt();
    if (!enterTextIntoEditableSpinBox(pasteXSpinBox, QString::number(originalXSpinBoxValue + 1)))
        return false;

    QQuickItem *pasteYSpinBox = pasteAcrossLayersDialog->findChild<QQuickItem*>("pasteYSpinBox");
    VERIFY(pasteYSpinBox);
    const int originalYSpinBoxValue = pasteYSpinBox->property("value").toInt();
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(pasteYSpinBox, QString::number(originalYSpinBoxValue - 1)))
        return false;
    // Tab again because we want the focus to leave the text input so that valueModified is emitted.
    QTest::keyClick(window, Qt::Key_Tab);

    // Paste into hidden layers just to try out a value different than the default.
    if (!setCheckBoxChecked("onlyPasteIntoVisibleLayersCheckBox", false))
        return false;

    // Check that the live preview has changed.
    QVector<QImage> pastedImages = ImageUtils::pasteAcrossLayers(layeredImageProject->layers(),
        layeredImageProject->layerImagesBeforeLivePreview(), 1, -1, false);
    if (!compareImages(getLayerImages(), pastedImages , "live preview should show pasted contents (before cancelling)"))
        return false;

    QQuickItem *cancelButton = pasteAcrossLayersDialog->findChild<QQuickItem*>("pasteAcrossLayersDialogCancelButton");
    VERIFY(cancelButton);
    if (!clickButton(cancelButton))
        return false;
    TRY_VERIFY(!pasteAcrossLayersDialog->property("visible").toBool());
    if (!compareImages(getLayerImages(), originalImages, "live preview should show unpasted contents (after cancelling)"))
        return false;

    // Open the dialog again.
    if (!findAndOpenClosedPopupFromObjectName("pasteAcrossLayersDialog"))
        return false;
    // The old values should be restored.
    VERIFY(pasteXSpinBox->property("value").toInt() == originalXSpinBoxValue);
    VERIFY(pasteYSpinBox->property("value").toInt() == originalYSpinBoxValue);

    // Change the values and then press OK.
    if (!enterTextIntoEditableSpinBox(pasteXSpinBox, QString::number(pasteX)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(pasteYSpinBox, QString::number(pasteY)))
        return false;
    // Tab again because we want the focus to leave the text input so that valueModified is emitted.
    QTest::keyClick(window, Qt::Key_Tab);

    // Check that the preview has changed.
    pastedImages = ImageUtils::pasteAcrossLayers(layeredImageProject->layers(),
        layeredImageProject->layerImagesBeforeLivePreview(), pasteX, pasteY, onlyPasteIntoVisibleLayers);
    if (!compareImages(getLayerImages(), pastedImages , "live preview should show pasted contents (before accepting)"))
        return false;

    if (!setCheckBoxChecked("onlyPasteIntoVisibleLayersCheckBox", onlyPasteIntoVisibleLayers))
        return false;

    QQuickItem *okButton = pasteAcrossLayersDialog->findChild<QQuickItem*>("pasteAcrossLayersDialogOkButton");
    VERIFY(okButton);
    if (!clickButton(okButton))
        return false;
    TRY_VERIFY(!pasteAcrossLayersDialog->property("visible").toBool());
    if (!compareImages(getLayerImages(), pastedImages , "image contents should be pasted (after accepting)"))
        return false;
    VERIFY(pasteXSpinBox->property("value").toInt() == pasteX);
    VERIFY(pasteYSpinBox->property("value").toInt() == pasteY);

    return true;
}

bool TestHelper::changeCanvasSize(int width, int height, CloseDialogFlag closeDialog)
{
    // Open the canvas size popup.
    if (!clickButton(canvasSizeToolButton))
        return false;
    const QObject *canvasSizePopup = findOpenPopupFromTypeName("CanvasSizePopup");
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
    if (!clickButton(cancelButton))
        return false;
    TRY_VERIFY2(!canvasSizePopup->property("visible").toBool(), "Failed to cancel CanvasSizePopup");
    VERIFY(project->size().width() == originalWidthSpinBoxValue);
    VERIFY(project->size().height() == originalHeightSpinBoxValue);
    VERIFY(canvas->hasActiveFocus());

    // Open the popup again.
    if (!clickButton(canvasSizeToolButton))
        return false;
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
        if (!clickButton(okButton))
        return false;
        TRY_VERIFY2(!canvasSizePopup->property("visible").toBool(), "Failed to accept CanvasSizePopup");
        VERIFY(project->size().width() == width);
        VERIFY(project->size().height() == height);
        VERIFY(widthSpinBox->property("value").toInt() == width);
        VERIFY(heightSpinBox->property("value").toInt() == height);
        VERIFY(canvas->hasActiveFocus());
    }

    return true;
}

bool TestHelper::changeImageSize(int width, int height, bool preserveAspectRatio)
{
    const QImage originalContents = project->exportedImage();

    // Open the image size popup.
    if (!clickButton(imageSizeToolButton))
        return false;
    const QObject *imageSizePopup = findOpenPopupFromTypeName("ImageSizePopup");
    VERIFY(imageSizePopup);
    VERIFY(imageSizePopup->property("visible").toBool());

    // Check/uncheck the preserve aspect ratio button if necessary.
    QQuickItem *preserveAspectRatioButton = imageSizePopup->findChild<QQuickItem*>("preserveAspectRatioButton");
    VERIFY(preserveAspectRatioButton);
    if (preserveAspectRatioButton->property("checked").toBool() != preserveAspectRatio) {
        QTest::qWait(1000);
        if (!clickButton(preserveAspectRatioButton))
            return false;
        QTest::qWait(1000);
        COMPARE_NON_FLOAT(preserveAspectRatioButton->property("checked").toBool(), preserveAspectRatio);
    }

    // Change the values and then cancel.
    QQuickItem *widthSpinBox = imageSizePopup->findChild<QQuickItem*>("changeImageWidthSpinBox");
    VERIFY(widthSpinBox);
    const int originalWidthSpinBoxValue = widthSpinBox->property("value").toInt();
    if (!enterTextIntoEditableSpinBox(widthSpinBox, QString::number(originalWidthSpinBoxValue + 1)))
        return false;

    QQuickItem *heightSpinBox = imageSizePopup->findChild<QQuickItem*>("changeImageHeightSpinBox");
    const int originalHeightSpinBoxValue = heightSpinBox->property("value").toInt();
    VERIFY(heightSpinBox);
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(heightSpinBox, QString::number(originalHeightSpinBoxValue - 1)))
        return false;
    // Tab again because we want the focus to leave the text input so that valueModified is emitted.
    QTest::keyClick(window, Qt::Key_Tab);

    // Check that the live preview has changed.
    QImage resizedContents = ImageUtils::resizeContents(originalContents, originalWidthSpinBoxValue + 1, originalWidthSpinBoxValue - 1);
    if (!compareImages(project->exportedImage(), resizedContents, "live preview should show resized contents (before cancelling)"))
        return false;

    QQuickItem *cancelButton = imageSizePopup->findChild<QQuickItem*>("imageSizePopupCancelButton");
    VERIFY(cancelButton);
    if (!clickButton(cancelButton))
        return false;
    TRY_VERIFY(!imageSizePopup->property("visible").toBool());
    VERIFY(project->size().width() == originalWidthSpinBoxValue);
    VERIFY(project->size().height() == originalHeightSpinBoxValue);
    if (!compareImages(project->exportedImage(), originalContents, "live preview should show original contents (after cancelling)"))
        return false;

    // Open the popup again.
    if (!clickButton(imageSizeToolButton))
        return false;
    VERIFY(imageSizePopup);
    TRY_VERIFY(imageSizePopup->property("opened").toBool());
    // The old values should be restored.
    VERIFY(widthSpinBox->property("value").toInt() == originalWidthSpinBoxValue);
    VERIFY(heightSpinBox->property("value").toInt() == originalHeightSpinBoxValue);

    // Check/uncheck the preserve aspect ratio button if necessary.
    if (preserveAspectRatioButton->property("checked").toBool() != preserveAspectRatio) {
        if (!clickButton(preserveAspectRatioButton))
            return false;
        VERIFY(preserveAspectRatioButton->property("checked").toBool() == preserveAspectRatio);
    }

    // Change the values and then press OK.
    if (!enterTextIntoEditableSpinBox(widthSpinBox, QString::number(width)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(heightSpinBox, QString::number(height)))
        return false;
    // Tab again because we want the focus to leave the text input so that valueModified is emitted.
    QTest::keyClick(window, Qt::Key_Tab);

    // Check that the preview has changed.
    resizedContents = ImageUtils::resizeContents(originalContents, width, height);
    if (!compareImages(project->exportedImage(), resizedContents, "live preview should show resized contents (before accepting)"))
        return false;

    QQuickItem *okButton = imageSizePopup->findChild<QQuickItem*>("imageSizePopupOkButton");
    VERIFY(okButton);
    if (!clickButton(okButton))
        return false;
    TRY_VERIFY(!imageSizePopup->property("visible").toBool());
    VERIFY(project->size().width() == width);
    VERIFY(project->size().height() == height);
    VERIFY(widthSpinBox->property("value").toInt() == width);
    VERIFY(heightSpinBox->property("value").toInt() == height);
    if (!compareImages(project->exportedImage(), resizedContents, "image contents should be resized (after accepting)"))
        return false;

    return true;
}

bool TestHelper::changeToolSize(int size)
{
    if (!clickButton(toolSizeButton))
        return false;
    const QObject *toolSizePopup = findOpenPopupFromTypeName("ToolSizePopup");
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
    COMPARE_NON_FLOAT(sliderValue(toolSizeSlider), size);

    // Close the popup.
    QTest::keyClick(window, Qt::Key_Escape);
    VERIFY(toolSizePopup->property("visible").toBool() == false);

    return true;
}

bool TestHelper::changeToolShape(ImageCanvas::ToolShape toolShape)
{
    if (canvas->toolShape() == toolShape)
        return true;

    if (!clickButton(toolShapeButton))
        return false;
    const QObject *toolShapeMenu = window->findChild<QObject*>("toolShapeMenu");
    VERIFY(toolShapeMenu);
    TRY_VERIFY(toolShapeMenu->property("opened").toBool() == true);

    if (toolShape == ImageCanvas::SquareToolShape) {
        QQuickItem *squareToolShapeMenuItem = toolShapeMenu->findChild<QQuickItem*>("squareToolShapeMenuItem");
        VERIFY(squareToolShapeMenuItem);

        if (!clickButton(squareToolShapeMenuItem))
        return false;
        VERIFY(canvas->toolShape() == ImageCanvas::SquareToolShape);
    } else {
        QQuickItem *circleToolShapeMenuItem = toolShapeMenu->findChild<QQuickItem*>("circleToolShapeMenuItem");
        VERIFY(circleToolShapeMenuItem);

        if (!clickButton(circleToolShapeMenuItem))
        return false;
        VERIFY(canvas->toolShape() == ImageCanvas::CircleToolShape);
    }

    return true;
}

bool TestHelper::moveContents(int x, int y, bool onlyVisibleLayers)
{
    const QImage originalContents = project->exportedImage();

    // Open the move contents dialog.
    VERIFY2(triggerShortcut("moveContentsShortcut", app.settings()->moveContentsShortcut()), failureMessage);
    const QObject *moveContentsDialog = findOpenPopupFromTypeName("MoveContentsDialog");
    VERIFY(moveContentsDialog);
    TRY_VERIFY(moveContentsDialog->property("opened").toBool());

    // Change the values and then cancel.
    QQuickItem *moveContentsXSpinBox = moveContentsDialog->findChild<QQuickItem*>("moveContentsXSpinBox");
    VERIFY(moveContentsXSpinBox);
    const int originalXSpinBoxValue = moveContentsXSpinBox->property("value").toInt();
    if (!enterTextIntoEditableSpinBox(moveContentsXSpinBox, QString::number(originalXSpinBoxValue + 1)))
        return false;

    QQuickItem *moveContentsYSpinBox = moveContentsDialog->findChild<QQuickItem*>("moveContentsYSpinBox");
    VERIFY(moveContentsYSpinBox);
    const int originalYSpinBoxValue = moveContentsYSpinBox->property("value").toInt();
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(moveContentsYSpinBox, QString::number(originalYSpinBoxValue - 1)))
        return false;
    // Tab again because we want the focus to leave the text input so that valueModified is emitted.
    QTest::keyClick(window, Qt::Key_Tab);

    // Check that the live preview has changed.
    QImage movedContents = ImageUtils::moveContents(originalContents, 1, -1);
    if (!compareImages(project->exportedImage(), movedContents, "live preview should show moved contents (before cancelling)"))
        return false;

    QQuickItem *cancelButton = moveContentsDialog->findChild<QQuickItem*>("moveContentsDialogCancelButton");
    VERIFY(cancelButton);
    if (!clickButton(cancelButton))
        return false;
    TRY_VERIFY(!moveContentsDialog->property("visible").toBool());
    if (!compareImages(project->exportedImage(), originalContents, "live preview should show unmoved contents (after cancelling)"))
        return false;

    // Open the dialog again.
    VERIFY(triggerShortcut("moveContentsShortcut", app.settings()->moveContentsShortcut()));
    TRY_VERIFY(moveContentsDialog->property("opened").toBool());
    // The old values should be restored.
    VERIFY(moveContentsXSpinBox->property("value").toInt() == originalXSpinBoxValue);
    VERIFY(moveContentsYSpinBox->property("value").toInt() == originalYSpinBoxValue);

    // Change the values and then press OK.
    if (!enterTextIntoEditableSpinBox(moveContentsXSpinBox, QString::number(x)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(moveContentsYSpinBox, QString::number(y)))
        return false;
    // Tab again because we want the focus to leave the text input so that valueModified is emitted.
    QTest::keyClick(window, Qt::Key_Tab);

    // Check that the preview has changed.
    movedContents = ImageUtils::moveContents(originalContents, x, y);
    if (!compareImages(project->exportedImage(), movedContents, "live preview should show moved contents (before accepting)"))
        return false;

    if (onlyVisibleLayers) {
        QQuickItem *onlyMoveVisibleLayersCheckBox = moveContentsDialog->findChild<QQuickItem*>("onlyMoveVisibleLayersCheckBox");
        VERIFY(onlyMoveVisibleLayersCheckBox);
        if (onlyMoveVisibleLayersCheckBox->property("checked").toBool() != onlyVisibleLayers) {
            if (!clickButton(onlyMoveVisibleLayersCheckBox))
        return false;
            VERIFY(onlyMoveVisibleLayersCheckBox->property("checked").toBool() == onlyVisibleLayers);
        }
    }

    QQuickItem *okButton = moveContentsDialog->findChild<QQuickItem*>("moveContentsDialogOkButton");
    VERIFY(okButton);
    if (!clickButton(okButton))
        return false;
    TRY_VERIFY(!moveContentsDialog->property("visible").toBool());
    if (!compareImages(project->exportedImage(), movedContents, "image contents should be moved (after accepting)"))
        return false;
    VERIFY(moveContentsXSpinBox->property("value").toInt() == x);
    VERIFY(moveContentsYSpinBox->property("value").toInt() == y);

    return true;
}

bool TestHelper::rearrangeContentsIntoGrid(int cellWidth, int cellHeight, int columns, int rows)
{
    const QVector<QImage> originalImages = getLayerImages();

    QVector<QImage> expectedImages = ImageUtils::rearrangeContentsIntoGrid(originalImages, cellWidth, cellHeight, columns, rows);

    // Open the rearrange contents dialog.
    QObject *rearrangeContentsDialog;
    if (!findAndOpenClosedPopupFromObjectName("rearrangeContentsIntoGridDialog", &rearrangeContentsDialog))
        return false;

    // Change the values and then cancel.
    QQuickItem *cellWidthSpinBox = rearrangeContentsDialog->findChild<QQuickItem*>("cellWidthSpinBox");
    VERIFY(cellWidthSpinBox);
    const int originalCellWidthSpinBoxValue = cellWidthSpinBox->property("value").toInt();
    if (!enterTextIntoEditableSpinBox(cellWidthSpinBox, QString::number(originalCellWidthSpinBoxValue + 1)))
        return false;

    QQuickItem *cellHeightSpinBox = rearrangeContentsDialog->findChild<QQuickItem*>("cellHeightSpinBox");
    VERIFY(cellHeightSpinBox);
    const int originalCellHeightSpinBoxValue = cellHeightSpinBox->property("value").toInt();
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(cellHeightSpinBox, QString::number(originalCellHeightSpinBoxValue + 2)))
        return false;

    QQuickItem *columnsSpinBox = rearrangeContentsDialog->findChild<QQuickItem*>("columnsSpinBox");
    VERIFY(columnsSpinBox);
    const int originalColumnsSpinBoxValue = columnsSpinBox->property("value").toInt();
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(columnsSpinBox, QString::number(originalColumnsSpinBoxValue + 3)))
        return false;

    QQuickItem *rowsSpinBox = rearrangeContentsDialog->findChild<QQuickItem*>("rowsSpinBox");
    VERIFY(rowsSpinBox);
    const int originalRowsSpinBoxValue = rowsSpinBox->property("value").toInt();
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(rowsSpinBox, QString::number(originalRowsSpinBoxValue + 4)))
        return false;
    // Tab again because we want the focus to leave the text input so that valueModified is emitted.
    QTest::keyClick(window, Qt::Key_Tab);

    // Check that the live preview has changed.
    QVector<QImage> rearrangedImages = ImageUtils::rearrangeContentsIntoGrid(originalImages,
        originalCellWidthSpinBoxValue + 1, originalCellHeightSpinBoxValue + 2,
        originalColumnsSpinBoxValue + 3, originalRowsSpinBoxValue + 4);
    if (!compareImages(getLayerImages(), rearrangedImages, "live preview should show rearranged contents (before cancelling)"))
        return false;

    QQuickItem *cancelButton = rearrangeContentsDialog->findChild<QQuickItem*>("rearrangeContentsIntoGridDialogCancelButton");
    VERIFY(cancelButton);
    if (!clickButton(cancelButton))
        return false;
    TRY_VERIFY(!rearrangeContentsDialog->property("visible").toBool());
    if (!compareImages(getLayerImages(), originalImages, "live preview should show non-rearranged contents (after cancelling)"))
        return false;

    // Open the dialog again.
    if (!findAndOpenClosedPopupFromObjectName("rearrangeContentsIntoGridDialog"))
        return false;
    // The old values should be restored.
    VERIFY(cellWidthSpinBox->property("value").toInt() == originalCellWidthSpinBoxValue);
    VERIFY(cellHeightSpinBox->property("value").toInt() == originalCellHeightSpinBoxValue);
    VERIFY(columnsSpinBox->property("value").toInt() == originalColumnsSpinBoxValue);
    VERIFY(rowsSpinBox->property("value").toInt() == originalRowsSpinBoxValue);

    // Change the values and then press OK.
    if (!enterTextIntoEditableSpinBox(cellWidthSpinBox, QString::number(cellWidth)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(cellHeightSpinBox, QString::number(cellHeight)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(columnsSpinBox, QString::number(columns)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(rowsSpinBox, QString::number(rows)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);

    // Check that the preview has changed.
    rearrangedImages = ImageUtils::rearrangeContentsIntoGrid(originalImages, cellWidth, cellHeight, columns, rows);
    if (!compareImages(getLayerImages(), rearrangedImages, "live preview should show rearranged contents (before accepting)"))
        return false;

    QQuickItem *okButton = rearrangeContentsDialog->findChild<QQuickItem*>("rearrangeContentsIntoGridDialogOkButton");
    VERIFY(okButton);
    if (!clickButton(okButton))
        return false;
    TRY_VERIFY(!rearrangeContentsDialog->property("visible").toBool());
    if (!compareImages(getLayerImages(), rearrangedImages, "image contents should be rearranged (after accepting)"))
        return false;

    // As we accepted the dialog, the next time we open it (in this project) it should have the
    // most recently accepted values. This makes it easier to e.g. add some rows or columns later on,
    // since the values (especially cell width and height) are remembered.
    if (!findAndOpenClosedPopupFromObjectName("rearrangeContentsIntoGridDialog"))
        return false;

    VERIFY(cellWidthSpinBox->property("value").toInt() == cellWidth);
    VERIFY(cellHeightSpinBox->property("value").toInt() == cellHeight);
    VERIFY(columnsSpinBox->property("value").toInt() == columns);
    VERIFY(rowsSpinBox->property("value").toInt() == rows);

    // Close it again.
    if (!clickButton(cancelButton))
        return false;
    TRY_VERIFY(!rearrangeContentsDialog->property("visible").toBool());

    return true;
}

int TestHelper::sliderValue(QQuickItem *slider) const
{
    const qreal position = slider->property("position").toReal();
    qreal value = 0;
    QMetaObject::invokeMethod(slider, "valueAt", Qt::DirectConnection, Q_RETURN_ARG(qreal, value), Q_ARG(qreal, position));
    return qFloor(value);
}

bool TestHelper::moveSliderHandle(QQuickItem *slider, qreal expectedValue)
{
    VERIFY(slider);

    QQuickItem *sliderHandle = slider->property("handle").value<QQuickItem*>();
    VERIFY(sliderHandle);

    const QPoint pressPos = sliderHandle->mapToScene(
        QPointF(sliderHandle->width() / 2, slider->height() / 2)).toPoint();
    QTest::mousePress(slider->window(), Qt::LeftButton, Qt::NoModifier, pressPos);
    VERIFY(slider->property("pressed").toBool() == true);
    VERIFY(window->mouseGrabberItem() == slider);

    const qreal startingValue = sliderValue(slider);
    const qreal fromValue = slider->property("from").toReal();
    const qreal toValue = slider->property("to").toReal();
    if (qFuzzyCompare(startingValue, expectedValue))
        return true;

    const int dragDirection = startingValue < expectedValue ? 1 : -1;

    // Move the slider's handle until we find the value we want.
    QPoint movePos = pressPos;
    for (; ; movePos.rx() += dragDirection) {
        QTest::mouseMove(slider->window(), movePos, 5);

        const qreal value = sliderValue(slider);
        if (qFuzzyCompare(value, expectedValue)) {
            QTest::mouseRelease(slider->window(), Qt::LeftButton, Qt::NoModifier, movePos);
            VERIFY(slider->property("pressed").toBool() == false);
            return true;
        }

        if (dragDirection > 0) {
            if (qFuzzyCompare(value, toValue))
                break;
        } else {
            // going backwards
            if (qFuzzyCompare(value, fromValue))
                break;
        }
    }
    QTest::mouseRelease(slider->window(), Qt::LeftButton, Qt::NoModifier, movePos);
    VERIFY(slider->property("pressed").toBool() == false);
    return false;
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
    const QPoint pressPos = area.topLeft();
    setCursorPosInScenePixels(pressPos);
    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    // If area is {0, 0, 5, 5}, we need to move to {x=5, y=5} to ensure
    // that we've selected all 5x5 pixels.
    const QPoint releasePos = area.bottomRight() + QPoint(1, 1);
    setCursorPosInScenePixels(releasePos);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    VERIFY2(canvas->hasSelection(), qPrintable(QString::fromLatin1("Mouse drag from {%1, %2} to {%3, %4} did not result in a selection")
        .arg(pressPos.x()).arg(pressPos.y()).arg(releasePos.x()).arg(releasePos.y())));
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

static QString fuzzyColourCompareFailMsg(const QColor &actualColour, const QColor &expectedColour,
    const QString &componentName, int difference, int fuzz)
{
    QString message = QString::fromLatin1("Actual colour %1 is not equal to expected colour %2")
        .arg(actualColour.name(QColor::HexArgb)).arg(expectedColour.name(QColor::HexArgb));

    message += QString::fromLatin1("; difference in %3 of %4 is larger than fuzz of %5").arg(componentName).arg(difference).arg(fuzz);

    return message;
}

bool TestHelper::fuzzyColourCompare(const QColor &actualColour, const QColor &expectedColour, int fuzz)
{
    const int rDiff = qAbs(expectedColour.red() - actualColour.red());
    VERIFY2(rDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(actualColour, expectedColour, QLatin1String("red"), rDiff, fuzz)));

    const int gDiff = qAbs(expectedColour.green() - actualColour.green());
    VERIFY2(gDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(actualColour, expectedColour, QLatin1String("green"), gDiff, fuzz)));

    const int bDiff = qAbs(expectedColour.blue() - actualColour.blue());
    VERIFY2(bDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(actualColour, expectedColour, QLatin1String("blue"), bDiff, fuzz)));

    const int aDiff = qAbs(expectedColour.alpha() - actualColour.alpha());
    VERIFY2(aDiff <= fuzz, qPrintable(fuzzyColourCompareFailMsg(actualColour, expectedColour, QLatin1String("alpha"), aDiff, fuzz)));

    return true;
}

bool TestHelper::fuzzyImageCompare(const QImage &actualImage, const QImage &expectedImage, int fuzz, const QString &context)
{
    auto saveImagesToPwd = [=](){
        const QString actualImageFilePath = QDir().absolutePath() + '/' + QTest::currentTestFunction() + QLatin1String("-actual.png");
        const QString expectedImageFilePath = QDir().absolutePath() + '/' + QTest::currentTestFunction() + QLatin1String("-expected.png");
        qInfo() << "Saving actual and expected images to\n" << actualImageFilePath << "\nand\n" << expectedImageFilePath;
        actualImage.save(actualImageFilePath);
        expectedImage.save(expectedImageFilePath);
    };

    if (actualImage.size() != expectedImage.size()) {
        const QString contextStr = context.isEmpty() ? context : " (" + context + ')';
        failureMessage = QString::fromLatin1("Failure comparing images %1: actual size %2 is not the same as expected size %3)")
            .arg(contextStr).arg(QtUtils::toString(actualImage.size())).arg(QtUtils::toString(expectedImage.size())).toLatin1();

        saveImagesToPwd();

        return false;
    }

    for (int y = 0; y < actualImage.height(); ++y) {
        for (int x = 0; x < actualImage.width(); ++x) {
            if (!fuzzyColourCompare(actualImage.pixelColor(x, y), expectedImage.pixelColor(x, y), fuzz)) {
                // TODO: remove fromLatin1 call for third arg in Qt 6; only added it to workaround ambiguous argument warning
                const QString contextStr = context.isEmpty() ? context : " (" + context + ')';
                failureMessage = QString::fromLatin1("Failure comparing pixels%1 at (%2, %3):%4")
                    .arg(contextStr).arg(x).arg(y).arg(QString::fromLatin1(failureMessage)).toLatin1();

                saveImagesToPwd();

                return false;
            }
        }
    }

    return true;
}

bool TestHelper::compareImages(const QImage &actualImage, const QImage &expectedImage, const QString &context)
{
    return fuzzyImageCompare(actualImage, expectedImage, 0, context);
}

bool TestHelper::compareImages(const QVector<QImage> &actualImages, const QVector<QImage> &expectedImages, const QString &context)
{
    if (actualImages.size() != expectedImages.size()) {
        const QString contextStr = context.isEmpty() ? context : " (" + context + ')';
        failureMessage = QString::fromLatin1("Failure comparing list of %1 images %2: actual image count %3 is not the same as expected image count %4)")
            .arg(expectedImages.size()).arg(contextStr).arg(QtUtils::toString(actualImages.size()))
                .arg(QtUtils::toString(expectedImages.size())).toLatin1();
        return false;
    }

    for (int i = 0; i < expectedImages.size(); ++i) {
        if (!compareImages(actualImages.at(i), expectedImages.at(i), context)) {
            const QString contextStr = context.isEmpty() ? context : " (" + context + ')';
            failureMessage = QString::fromLatin1("Failure comparing list of %1 images - images at index %2:\n")
                .arg(expectedImages.size()).arg(i).toLatin1() + failureMessage;
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
    const bool oldExpandedValue = swatchesPanel->property("expanded").toBool();
    QtUtils::ScopeGuard swatchPanelExpandedGuard([=](){
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
    if (!clickButton(newSwatchColourButton))
        return false;
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
    QObject *swatchContextMenu = findOpenPopupFromTypeName("SwatchContextMenu");
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
    if (!clickButton(renameSwatchColourMenuItem))
        return false;
    TRY_VERIFY(!swatchContextMenu->property("visible").toBool());

    QObject *renameSwatchColourDialog = findOpenPopupFromTypeName("RenameSwatchColourDialog");
    VERIFY(renameSwatchColourDialog);
    TRY_VERIFY(renameSwatchColourDialog->property("opened").toBool());

    // Do the renaming.
    QQuickItem *swatchNameTextField = window->findChild<QQuickItem*>("swatchNameTextField");
    VERIFY(swatchNameTextField);
    VERIFY2(enterText(swatchNameTextField, name), failureMessage);
    QTest::keyClick(window, Qt::Key_Return);
    TRY_VERIFY(!renameSwatchColourDialog->property("visible").toBool());
    return true;
}

bool TestHelper::deleteSwatchColour(int index)
{
    // Open the context menu by right clicking on the delegate.
    QQuickItem *delegate = findSwatchViewDelegateAtIndex(index);
    VERIFY(delegate);
    mouseEventOnCentre(delegate, MouseClick, Qt::RightButton);

    QObject *swatchContextMenu = findOpenPopupFromTypeName("SwatchContextMenu");
    VERIFY(swatchContextMenu);
    TRY_VERIFY(swatchContextMenu->property("opened").toBool());

    QQuickItem *deleteSwatchColourMenuItem = window->findChild<QQuickItem*>("deleteSwatchColourMenuItem");
    VERIFY(deleteSwatchColourMenuItem);
    if (!clickButton(deleteSwatchColourMenuItem))
        return false;
    TRY_VERIFY(!swatchContextMenu->property("visible").toBool());
    return true;
}

bool TestHelper::addNewGuide(int position, Qt::Orientation orientation)
{
    if (!canvas->areRulersVisible()) {
        if (!triggerRulersVisible())
            return false;
        VERIFY(canvas->areRulersVisible());
    }
    if (canvas->areGuidesLocked()) {
        if (!clickButton(lockGuidesToolButton))
            return false;
        VERIFY(!canvas->areGuidesLocked());
    }

    // Use something that doesn't show the selection guide, as it will affect our grab comparisons.
    const ImageCanvas::Tool originalTool = canvas->tool();
    if (originalTool == ImageCanvas::SelectionTool && !switchTool(ImageCanvas::PenTool))
        return false;

    const bool horizontal = orientation == Qt::Horizontal;
    const int originalGuideCount = project->guides().size();
    const int newGuideIndex = originalGuideCount;
    const QPoint originalOffset = canvas->currentPane()->integerOffset();
    const qreal originalZoomLevel = canvas->currentPane()->zoomLevel();

    QQuickItem *ruler = findChildItem(canvas, horizontal ? "firstHorizontalRuler" : "firstVerticalRuler");
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
    COMPARE_NON_FLOAT(grabWithGuide.pixelColor(releasePos.x(), releasePos.y()).name(QColor::HexArgb),
        QColor(Qt::cyan).name(QColor::HexArgb));

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    VERIFY(!canvas->pressedRuler());
    VERIFY2(project->guides().size() == originalGuideCount + 1, qPrintable(QString::fromLatin1(
        "Expected %1 guide(s), but got %2").arg(originalGuideCount + 1).arg(project->guides().size())));
    VERIFY(project->guides().at(newGuideIndex).position() == position);
    VERIFY(project->undoStack()->canUndo());

    if (canvas->tool() != originalTool && !switchTool(originalTool))
        return false;

    canvas->currentPane()->setOffset(originalOffset);
    canvas->currentPane()->setZoomLevel(originalZoomLevel);
    return true;
}

bool TestHelper::addNewGuides(int horizontalSpacing, int verticalSpacing, AddNewGuidesFlag flag)
{
    // First thing we do is sanity check that all existing guides are unique.
    const QVector<Guide> originalGuides = project->guides();
    const QVector<Guide> uniqueOriginalGuides = QtUtils::uniqueValues(originalGuides);
    VERIFY2(uniqueOriginalGuides == originalGuides, "Expected existing guides to be unique");

    // Then, gather up the expected (and duplicate) guides.
    QVector<Guide> expectedAddedGuides;
    ProjectUtils::addGuidesForSpacing(project, expectedAddedGuides, horizontalSpacing, verticalSpacing);
    if (flag == AddNewGuidesFlag::ExpectAllUnique)
        VERIFY2(!expectedAddedGuides.isEmpty(), "Failed to generate unique expected guides");
    else
        VERIFY2(expectedAddedGuides.isEmpty(), "Expected to generate no unique expected guides");

    // Grab the canvas before we make any changes.
    VERIFY(imageGrabber.requestImage(canvas));
    TRY_VERIFY(imageGrabber.isReady());
    const QImage originalCanvasGrab = imageGrabber.takeImage();

    // Open the dialog manually cause native menus.
    QObject *addGuidesDialog;
    if (!findAndOpenClosedPopupFromObjectName("addGuidesDialog", &addGuidesDialog))
        return false;

    // Enter the spacing values into the spin boxes and accept the dialog.
    auto horizontalSpinBox = addGuidesDialog->findChild<QQuickItem*>("addGuidesHorizontalSpacingSpinBox");
    VERIFY(horizontalSpinBox);
    VERIFY2(horizontalSpinBox->hasActiveFocus(), activeFocusFailureMessage(horizontalSpinBox, "when opening Add Guides dialog"));
    auto verticalSpinBox = addGuidesDialog->findChild<QQuickItem*>("addGuidesVerticalSpacingSpinBox");
    VERIFY(verticalSpinBox);
    if (!enterTextIntoEditableSpinBox(horizontalSpinBox, QString::number(horizontalSpacing)))
        return false;
    QTest::keyClick(window, Qt::Key_Tab);
    if (!enterTextIntoEditableSpinBox(verticalSpinBox, QString::number(verticalSpacing)))
        return false;
    if (!acceptDialog(addGuidesDialog, "addGuidesDialogOkButton"))
        return false;
    // Get a decent failure message instead of just "Compared values are not the same".
    COMPARE_NON_FLOAT_WITH_MSG(QtUtils::toString(project->guides()), QtUtils::toString(originalGuides + expectedAddedGuides),
        "Mismatch in actual vs expected guides after accepting Add Guides dialog");

    // The canvas should be redrawn after adding guides.
    VERIFY(imageGrabber.requestImage(canvas));
    TRY_VERIFY(imageGrabber.isReady());
    if (!expectedAddedGuides.isEmpty()) {
        // Not all guides we added were duplicates, so expect the canvas to be redrawn.
        VERIFY(imageGrabber.takeImage() != originalCanvasGrab);
    } else {
        // All of the guides we added were duplicates, so the canvas shouldn't be redrawn.
        if (!compareImages(imageGrabber.takeImage(), originalCanvasGrab))
            return false;
    }
    return true;
}

bool TestHelper::addSelectedColoursToTexturedFillSwatch()
{
    QSignalSpy errorSpy(project.data(), SIGNAL(errorOccurred(QString)));
    VERIFY(errorSpy.isValid());
    canvas->addSelectedColoursToTexturedFillSwatch();
    VERIFY(errorSpy.isEmpty());

    // Open the settings dialog.
    QObject *settingsDialog = window->findChild<QObject*>("texturedFillSettingsDialog");
    VERIFY(settingsDialog);
    VERIFY(QMetaObject::invokeMethod(settingsDialog, "open"));
    TRY_VERIFY(settingsDialog->property("opened").toBool());

    // Switch to swatch-based textured fill.
    VERIFY2(selectComboBoxItem("fillTypeComboBox", int(TexturedFillParameters::SwatchFillType)), failureMessage);
    return true;
}

bool TestHelper::addNewNoteAtCursorPos(const QString &text)
{
    const int previousNoteCount = project->notes().size();

    // Move the mouse to verify that the position is correct upon opening a creation dialog.
    QTest::mouseMove(window, cursorWindowPos);

    // Open the note dialog. It shouldn't have the text that was previously entered.
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    const QObject *noteDialog = findOpenPopupFromTypeName("NoteDialog");
    VERIFY(noteDialog);
    TRY_VERIFY(noteDialog->property("opened").toBool());

    QQuickItem *noteDialogTextField = noteDialog->findChild<QQuickItem*>("noteDialogTextField");
    VERIFY(noteDialogTextField);
    VERIFY(noteDialogTextField->property("text").toString().isEmpty());

    QQuickItem *noteDialogXTextField = noteDialog->findChild<QQuickItem*>("noteDialogXTextField");
    VERIFY(noteDialogXTextField);
    VERIFY(noteDialogXTextField->property("text").toString() == QString::number(cursorPos.x()));

    QQuickItem *noteDialogYTextField = noteDialog->findChild<QQuickItem*>("noteDialogYTextField");
    VERIFY(noteDialogYTextField);
    VERIFY(noteDialogYTextField->property("text").toString() == QString::number(cursorPos.y()));

    VERIFY2(enterText(noteDialogTextField, text), failureMessage);

    // Accept the dialog.
    QTest::keyClick(window, Qt::Key_Return);
    TRY_VERIFY(!noteDialog->property("visible").toBool());
    VERIFY(project->notes().size() == previousNoteCount + 1);
    VERIFY(project->notes().at(previousNoteCount).position() == cursorPos);
    VERIFY(project->notes().at(previousNoteCount).text() == text);
    VERIFY(!project->notes().at(previousNoteCount).size().isEmpty());
    return true;
}

bool TestHelper::dragNoteAtIndex(int noteIndex, const QPoint &newPosition)
{
    VERIFY(project->isValidNoteIndex(noteIndex));

    const Note oldNote = project->notes().at(noteIndex);
    const QPoint noteSize(oldNote.size().width(), oldNote.size().height());
    QPoint noteCentrePos = oldNote.position() + noteSize / 2;
    setCursorPosInScenePixels(noteCentrePos);
    QTest::mouseMove(window, cursorWindowPos);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    VERIFY2(canvas->pressedNoteIndex() == noteIndex, qPrintable(QString::fromLatin1(
        "Expected note at index %1 to be pressed, but %2 is pressed instead")
            .arg(noteIndex).arg(canvas->pressedNoteIndex())));

    setCursorPosInScenePixels(newPosition + noteSize / 2);
    QTest::mouseMove(window, cursorWindowPos);

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    VERIFY(canvas->pressedNoteIndex() == -1);
    VERIFY2(project->notes().at(noteIndex).position() == newPosition, qPrintable(QString::fromLatin1(
        "Expected note at index %1 to be at position %2, but it's at %3 instead")
            .arg(noteIndex).arg(QTest::toString(newPosition)).arg(QTest::toString(project->notes().at(noteIndex).position()))));
    return true;
}

AnimationSystem *TestHelper::getAnimationSystem() const
{
    return imageProject ? imageProject->animationSystem() : layeredImageProject->animationSystem();
}

bool TestHelper::verifyAnimationName(const QString &animationName, QQuickItem **animationDelegate)
{
    QQuickItem *delegate = findListViewChild("animationListView", animationName + QLatin1String("_Delegate"));
    VERIFY(delegate);
    QQuickItem *animationDelegateNameTextField = delegate->findChild<QQuickItem*>("animationNameTextField");
    VERIFY(animationDelegateNameTextField);
    VERIFY(animationDelegateNameTextField->property("text").toString() == animationName);
    if (animationDelegate)
        *animationDelegate = delegate;
    return true;
}

bool TestHelper::addNewAnimation(const QString &expectedGeneratedAnimationName, int expectedIndex)
{
    VERIFY(imageProject || layeredImageProject);

    auto *animationSystem = getAnimationSystem();

    const int oldAnimationCount = animationSystem->animationCount();

    // Add the animation.
    if (!clickButton(newAnimationButton))
        return false;

    const int animationCount = animationSystem->animationCount();
    VERIFY(animationCount == oldAnimationCount + 1);
    Animation *newAnimation = animationSystem->animationAt(oldAnimationCount);
    VERIFY(newAnimation);
    // New animations are appended to the end of the list and made current.
    VERIFY(animationSystem->currentAnimation() == newAnimation);
    VERIFY(animationSystem->currentAnimationIndex() == expectedIndex);
    const QString actualAnimationName = animationSystem->animationAt(expectedIndex)->name();
    if (actualAnimationName != expectedGeneratedAnimationName) {
        QString message;
        QDebug stream(&message);
        stream.nospace() << "Expected new animation name to be " << expectedGeneratedAnimationName
            << " but it's " << actualAnimationName << ". Animation:\n";
        for (int i = 0; i < animationCount; ++i) {
            stream << "    index " << i << ": " << animationSystem->animationAt(i)->name() << "\n";
        }
        FAIL(qPrintable(message));
    }
    VERIFY(undoToolButton->isEnabled());
    return true;
}

bool TestHelper::makeCurrentAnimation(const QString &animationName, int index)
{
    VERIFY(imageProject || layeredImageProject);

    auto *animationSystem = getAnimationSystem();

    const int oldCurrentIndex = animationSystem->currentAnimationIndex();
    VERIFY2(index != oldCurrentIndex, "Can't make animation current because it already is");

    QQuickItem *animationDelegate = nullptr;
    VERIFY2(verifyAnimationName(animationName, &animationDelegate), failureMessage);

    if (!clickButton(animationDelegate))
        return false;
    VERIFY(animationDelegate->property("checked").toBool() == true);
    VERIFY(animationSystem->currentAnimationIndex() == index);

    return true;
}

bool TestHelper::duplicateCurrentAnimation(const QString &expectedGeneratedAnimationName, int expectedDuplicateIndex)
{
    VERIFY(imageProject || layeredImageProject);

    auto *animationSystem = getAnimationSystem();

    const int currentIndex = animationSystem->currentAnimationIndex();
    const Animation *currentAnimation = animationSystem->currentAnimation();
    VERIFY(currentAnimation);
    const int oldAnimationCount = animationSystem->animationCount();

    // Add the animation.
    if (!clickButton(duplicateAnimationButton))
        return false;

    const int animationCount = animationSystem->animationCount();
    VERIFY(animationCount == oldAnimationCount + 1);
    Animation *duplicateAnimation = animationSystem->animationAt(currentIndex + 1);
    VERIFY(duplicateAnimation);
    // New animations are appended to the end of the list, so the currentIndex should not change.
    VERIFY(animationSystem->currentAnimation() == currentAnimation);
    VERIFY(animationSystem->currentAnimationIndex() == currentIndex);
    const QString actualAnimationName = animationSystem->animationAt(expectedDuplicateIndex)->name();
    if (actualAnimationName != expectedGeneratedAnimationName) {
        QString message;
        QDebug stream(&message);
        stream.nospace() << "Expected duplicate animation name to be " << expectedGeneratedAnimationName
            << " but it's " << actualAnimationName << ". Animation:\n";
        for (int i = 0; i < animationCount; ++i) {
            stream << "    index " << i << ": " << animationSystem->animationAt(i)->name() << "\n";
        }
        FAIL(qPrintable(message));
    }
    VERIFY(undoToolButton->isEnabled());
    return true;
}

bool TestHelper::makeCurrentAndRenameAnimation(const QString &from, const QString &to)
{
    VERIFY(imageProject || layeredImageProject);

    auto *animationSystem = getAnimationSystem();
    const int index = animationSystem->indexOfAnimation(from);
    VERIFY(index != -1);

    QQuickItem *animationDelegate = nullptr;
    VERIFY2(verifyAnimationName(from, &animationDelegate), failureMessage);

    // If it's not current, make it.
    if (animationSystem->currentAnimationIndex() != index) {
        if (!clickButton(animationDelegate))
        return false;
        VERIFY(animationDelegate->property("checked").toBool() == true);
        VERIFY(animationSystem->currentAnimationIndex() == index);
    }

    QQuickItem *nameTextField = animationDelegate->findChild<QQuickItem*>("animationNameTextField");
    VERIFY(nameTextField);

    // Give the text field focus.
    mouseEventOnCentre(nameTextField, MouseDoubleClick);
    VERIFY(nameTextField->hasActiveFocus() == true);

    // Enter the text.
    QTest::keySequence(window, QKeySequence(QKeySequence::SelectAll));
    foreach (const auto character, to)
        QTest::keyClick(window, character.toLatin1());
    VERIFY2(nameTextField->property("text").toString() == to, qPrintable(QString::fromLatin1(
        "Expected animationNameTextField to contain \"%1\" after entering new layer name, but it contains \"%2\"")
            .arg(to, nameTextField->property("text").toString())));
    VERIFY(animationSystem->currentAnimation()->name() == from);

    // Confirm the changes.
    QTest::keyClick(window, Qt::Key_Enter);
    VERIFY2(nameTextField->property("text").toString() == to, qPrintable(QString::fromLatin1(
        "Expected animationNameTextField to contain \"%1\" after confirming changes, but it contains \"%2\"")
            .arg(to, nameTextField->property("text").toString())));
    VERIFY(animationSystem->currentAnimation()->name() == to);
    VERIFY(project->hasUnsavedChanges());
    return true;
}

bool TestHelper::openAnimationSettingsPopupForCurrentAnimation(QObject **popup)
{
    auto currentAnimation = getAnimationSystem()->currentAnimation();
    VERIFY(currentAnimation);

    const auto delegateObjectName = currentAnimation->name() + "_Delegate";
    QQuickItem *animationDelegate = findListViewChild("animationListView", delegateObjectName);
    VERIFY(animationDelegate);

    QQuickItem *configureAnimationToolButton = animationDelegate->findChild<QQuickItem*>(delegateObjectName + "AnimationSettingsToolButton");
    VERIFY(configureAnimationToolButton);

    if (!clickButton(configureAnimationToolButton))
        return false;
    QObject *animationSettingsPopup = findOpenPopupFromTypeName("AnimationSettingsPopup");
    VERIFY(animationSettingsPopup);
    if (popup)
        *popup = animationSettingsPopup;
    TRY_VERIFY(animationSettingsPopup->property("opened").toBool());
    return true;
}

bool TestHelper::grabFramesOfCurrentAnimation(QVector<QImage> &frames)
{
    auto currentPlayback = TestHelper::animationPlayback();
    auto currentAnimation = currentPlayback->animation();
    VERIFY2(!currentPlayback->shouldLoop(), "This function hasn't been tested with looping animations");

    VERIFY2(currentPlayback->currentFrameIndex() == 0, qPrintable(QString::fromLatin1(
        "Expected currentFrameIndex to be 0 when this function is called, but it's %1").arg(currentPlayback->currentFrameIndex())));

    if (!clickButton(animationPlayPauseButton))
        return false;
    VERIFY(currentPlayback->isPlaying() == true);

    VERIFY2(currentPlayback->currentFrameIndex() == 0, qPrintable(QString::fromLatin1(
        "Expected currentFrameIndex to be 0 after hitting play, but it's %1").arg(currentPlayback->currentFrameIndex())));

    auto previewSpriteImage = window->findChild<QQuickItem*>("animationPreviewContainerSpriteImage");
    VERIFY(previewSpriteImage);

    for (int frameIndex = 0; frameIndex < currentAnimation->frameCount(); ++frameIndex) {
        TRY_VERIFY2(currentPlayback->currentFrameIndex() == frameIndex, qPrintable(QString::fromLatin1(
            "Expected currentFrameIndex to be %1 before pausing, but it's %2").arg(frameIndex).arg(currentPlayback->currentFrameIndex())));

        // Pause the animation so we can grab the SpriteImage without making the test flaky.
        currentPlayback->setPlaying(false);
        VERIFY(currentPlayback->isPlaying() == false);
        VERIFY(currentPlayback->pauseIndex() == frameIndex);

        VERIFY(imageGrabber.requestImage(previewSpriteImage));
        TRY_VERIFY(imageGrabber.isReady());
        frames.append(imageGrabber.takeImage());

        // Resume.
        const int frameIndexAfterPausing = currentPlayback->currentFrameIndex();
        VERIFY2(frameIndexAfterPausing == frameIndex, qPrintable(QString::fromLatin1(
            "Expected currentFrameIndex to be %1 after resuming, but it's %2").arg(frameIndex).arg(frameIndexAfterPausing)));
        currentPlayback->setPlaying(true);
        VERIFY(currentPlayback->isPlaying() == true);
    }
    TRY_VERIFY_WITH_TIMEOUT(currentPlayback->isPlaying() == false, 2000);
    VERIFY(currentPlayback->pauseIndex() == -1);
    return true;
}

// Necessary for dialogs that can only be opened by (native) menus.
bool TestHelper::findAndOpenClosedPopupFromObjectName(const QString &objectName, QObject **dialog)
{
    QObject *theDialog = window->findChild<QObject*>(objectName);
    VERIFY(theDialog);
    VERIFY(QMetaObject::invokeMethod(theDialog, "open"));
    TRY_VERIFY(theDialog->property("opened").toBool());
    if (dialog)
        *dialog = theDialog;
    return true;
}

QObject *TestHelper::findOpenPopupFromTypeName(const QString &typeName) const
{
    QObject *popup = nullptr;
    qCDebug(lcFindPopupFromTypeName) << "looking through" << overlay->childItems().size()
        << "items for popup for typeName" << typeName;

    foreach (QQuickItem *child, overlay->childItems()) {
        qCDebug(lcFindPopupFromTypeName) << "- looking at overlay child" << child;

        if (QString::fromLatin1(child->metaObject()->className()) == "QQuickPopupItem") {
            qCDebug(lcFindPopupFromTypeName) << "  - className() is what we expect (\"QQuickPopupItem\")";

            if (QString::fromLatin1(child->parent()->metaObject()->className()).contains(typeName)) {
                qCDebug(lcFindPopupFromTypeName) << "    - typeName() of the child's parent contains typeName:"
                    << child->parent()->metaObject()->className();

                popup = child->parent();
                break;
            } else {
                qCDebug(lcFindPopupFromTypeName) << "    - typeName() of the child's parent does not contain typeName:"
                    << child->parent()->metaObject()->className();
            }
        } else {
            qCDebug(lcFindPopupFromTypeName) << "  - className() is not what we expect (\"QQuickPopupItem\"):"
                << child->metaObject()->className();
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

bool TestHelper::acceptDialog(QObject *dialog, const QString &acceptButtonObjectName)
{
    QSignalSpy acceptedSpy(dialog, SIGNAL(accepted()));
    VERIFY(acceptedSpy.isValid());

    QQuickItem *acceptButton = findDialogButtonFromObjectName(dialog, acceptButtonObjectName);
    VERIFY(acceptButton);
    if (!clickButton(acceptButton))
        return false;
    TRY_VERIFY(!dialog->property("visible").toBool());
    COMPARE_NON_FLOAT(acceptedSpy.count(), 1);
    return true;
}

bool TestHelper::rejectDialog(QObject *dialog, const QString &rejectButtonObjectName)
{
    QSignalSpy rejectedSpy(dialog, SIGNAL(rejected()));
    VERIFY(rejectedSpy.isValid());

    QQuickItem *rejectButton = findDialogButtonFromObjectName(dialog, rejectButtonObjectName);
    VERIFY(rejectButton);
    if (!clickButton(rejectButton))
        return false;
    TRY_VERIFY(!dialog->property("visible").toBool());
    COMPARE_NON_FLOAT(rejectedSpy.count(), 1);
    return true;
}

QQuickItem *TestHelper::findListViewChild(QQuickItem *listView, const QString &childObjectName) const
{
    if (QQuickTest::qIsPolishScheduled(listView))
        if (!QQuickTest::qWaitForItemPolished(listView))
            return nullptr;

    QQuickItem *listViewContentItem = listView->property("contentItem").value<QQuickItem*>();
    if (!listViewContentItem)
        return nullptr;

    qCDebug(lcFindListViewChild) << "looking for ListView child" << childObjectName << "in" << listView->objectName();
    QQuickItem *listViewChild = nullptr;
    foreach (QQuickItem *child, listViewContentItem->childItems()) {
        qCDebug(lcFindListViewChild).nospace() << "- " << child->objectName();
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
    if (QQuickTest::qIsPolishScheduled(item))
        if (!QQuickTest::qWaitForItemPolished(item))
            return nullptr;

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

QQuickItem *TestHelper::findChildItem(QQuickItem *parentItem, const QString &objectName)
{
    const auto childItems = parentItem->childItems();
    for (QQuickItem *child : childItems) {
        if (child->objectName() == objectName)
            return child;
        else {
            QQuickItem *match = findChildItem(child, objectName);
            if (match)
                return match;
        }
    }
    return nullptr;
}

bool TestHelper::clickButton(QQuickItem *button, Qt::MouseButton mouseButton)
{
    if (!ensureItemVisible(button))
        return false;

    QSignalSpy buttonClickedSpy(button, SIGNAL(clicked()));
    VERIFY(buttonClickedSpy.isValid());

    mouseEventOnCentre(button, MouseClick, mouseButton);
    if (buttonClickedSpy.count() < 1) {
        // In case there is e.g. a popup transitioning out and blocking it.
        const auto imageGrabPath = QDir().absolutePath() + "/window-grab.png";
        qDebug() << "Saving window's image grab to " << imageGrabPath;
        VERIFY(window->grabWindow().save(imageGrabPath));
    }
    VERIFY2(buttonClickedSpy.count() == 1, qPrintable(QString::fromLatin1(
        "Expected clicked signal of \"%1\" to be emitted once, but it was emitted %2 time(s)")
            .arg(button->objectName()).arg(buttonClickedSpy.count())));
    return true;
}

bool TestHelper::ensureItemVisible(QQuickItem *item, EnsureVisibleFlags flags)
{
    VERIFY(item);
    if (flags.testFlag(CheckVisible)) {
        VERIFY2(item->property("visible").toBool(),
            QString::fromLatin1("Expected visible property of \"%1\" to be true").arg(item->objectName()));
    }
    if (flags.testFlag(CheckEnabled)) {
        VERIFY2(item->property("enabled").toBool(),
            QString::fromLatin1("Expected enabled property of \"%1\" to be true").arg(item->objectName()));
    }
    if (flags.testFlag(CheckWidth)) {
        VERIFY2(item->width() > 0.0,
            QString::fromLatin1("Expected width property of \"%1\" to be greater than zero").arg(item->objectName()));
    }
    if (flags.testFlag(CheckHeight)) {
        VERIFY2(item->height() > 0.0,
            QString::fromLatin1("Expected height property of \"%1\" to be greater than zero").arg(item->objectName()));
    }
    if (flags.testFlag(CheckOpacity)) {
        VERIFY2(item->opacity() > 0.0,
            QString::fromLatin1("Expected opacity property of \"%1\" to be greater than zero").arg(item->objectName()));
    }
    return true;
}

bool TestHelper::ensureScrollViewChildVisible(const QString &scrollViewObjectName, const QString &childObjectName)
{
    auto scrollView = window->findChild<QQuickItem*>(scrollViewObjectName);
    VERIFY(scrollView);
    auto flickable = scrollView->property("contentItem").value<QQuickItem*>();
    VERIFY(flickable);
    auto child = window->findChild<QQuickItem*>(childObjectName);
    VERIFY(child);
    return ensureFlickableChildVisible(flickable, child);
}

bool TestHelper::ensureFlickableChildVisible(QQuickItem *flickable, QQuickItem *child)
{
    VERIFY(flickable);
    VERIFY(child);
    VERIFY(flickable->isAncestorOf(child));

    auto flickableContentItem = flickable->property("contentItem").value<QQuickItem*>();
    VERIFY(flickableContentItem);

    const int newContentY = child->mapToItem(flickableContentItem, QPoint(0, 0)).y();
    VERIFY(flickable->setProperty("contentY", QVariant(newContentY)));
    // Setting the contentY may have put us outside of the bounds of the flickable, so return within them.
    VERIFY(QMetaObject::invokeMethod(flickable, "returnToBounds"));
    // Wait for the contentY to "animate".
    TRY_VERIFY(qFuzzyIsNull(flickable->property("verticalOvershoot").toReal()));
    return true;
}

bool TestHelper::ensureRepeaterChildrenVisible(QQuickItem *repeater, int expectedCount)
{
    VERIFY(repeater);
    COMPARE_NON_FLOAT(repeater->property("count").toInt(), expectedCount);
    if (expectedCount > 0) {
        for (int delegateIndex = 0; delegateIndex < expectedCount; ++delegateIndex) {
            QQuickItem *delegateItem = nullptr;
            VERIFY(QMetaObject::invokeMethod(repeater, "itemAt", Qt::DirectConnection,
                Q_RETURN_ARG(QQuickItem*, delegateItem), Q_ARG(int, delegateIndex)));
            VERIFY(delegateItem);
            if (!ensureItemVisible(delegateItem, CheckAllVisibleProperties))
                return false;
        }
    }
    return true;
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
    setCursorPosInScenePixels(QPoint(xPosInScenePixels, yPosInScenePixels), verifyWithinWindow);
}

void TestHelper::setCursorPosInScenePixels(const QPoint &posInScenePixels, bool verifyWithinWindow)
{
    cursorPos = posInScenePixels;
    const int integerZoomLevel = canvas->currentPane()->integerZoomLevel();
    const QPointF localZoomedPixelPos = QPointF(
        posInScenePixels.x() * integerZoomLevel,
        posInScenePixels.y() * integerZoomLevel);
    cursorWindowPos = canvas->mapToScene(localZoomedPixelPos).toPoint() + canvas->firstPane()->integerOffset();

    if (verifyWithinWindow) {
        // As with mouseEventOnCentre(), we don't want this to be a e.g. VERIFY2, because then we'd have to
        // verify its return value everywhere we use it, and we use it a lot, so just assert instead.
        Q_ASSERT_X(cursorWindowPos.x() >= 0 && cursorWindowPos.y() >= 0, Q_FUNC_INFO,
            qPrintable(QString::fromLatin1("scene pos %1 results in invalid cursor position %2")
                .arg(QtUtils::toString(posInScenePixels)).arg(QtUtils::toString(cursorWindowPos))));
    }
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
    return imageProject
        ? imageProject->animationSystem()->currentAnimationPlayback()
        : layeredImageProject->animationSystem()->currentAnimationPlayback();
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
    const int value = QKeySequence(sequenceAsString)[0].toCombined();
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
    if (!offscreenPlatform)
        return triggerShortcut("closeShortcut", app.settings()->closeShortcut());

    project->close();
    return true;
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

bool TestHelper::triggerCopy()
{
    return triggerShortcut("copyShortcut", QKeySequence(QKeySequence::Copy).toString());
}

bool TestHelper::triggerPaste()
{
    return triggerShortcut("pasteShortcut", QKeySequence(QKeySequence::Paste).toString());
}

bool TestHelper::triggerSelectAll()
{
    return triggerShortcut("selectAllShortcut", QKeySequence(QKeySequence::SelectAll).toString());
}

bool TestHelper::triggerDelete()
{
#ifdef Q_OS_MACOS
    FAIL("macOS not supported");
//    return triggerShortcut("deleteViaBackspaceShortcut", QKeySequence("Backspace").toString());
#else
    return triggerShortcut("deleteShortcut", QKeySequence(QKeySequence::Delete).toString());
#endif
}

bool TestHelper::triggerFlipHorizontally()
{
    return triggerShortcut("flipHorizontallyShortcut", app.settings()->flipHorizontallyShortcut());
}

bool TestHelper::triggerFlipVertically()
{
    return triggerShortcut("flipVerticallyShortcut", app.settings()->flipVerticallyShortcut());
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

bool TestHelper::setSplitScreen(bool splitScreen)
{
    if (canvas->isSplitScreen() != splitScreen)
        if (!clickButton(splitScreenToolButton))
        return false;
    return canvas->isSplitScreen() == splitScreen;
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

    if (usingAnimation) {
        newAnimationButton = window->findChild<QQuickItem*>("newAnimationButton");
        VERIFY(newAnimationButton);
        duplicateAnimationButton = window->findChild<QQuickItem*>("duplicateAnimationButton");
        VERIFY(duplicateAnimationButton);
        moveAnimationDownButton = window->findChild<QQuickItem*>("moveAnimationDownButton");
        VERIFY(moveAnimationDownButton);
        moveAnimationUpButton = window->findChild<QQuickItem*>("moveAnimationUpButton");
        VERIFY(moveAnimationUpButton);
        deleteAnimationButton = window->findChild<QQuickItem*>("deleteAnimationButton");
        VERIFY(deleteAnimationButton);

        // Ensure that the animation panel is visible and expanded when animation playback is enabled.
        QQuickItem *animationPanel = window->findChild<QQuickItem*>("animationPanel");
        VERIFY(animationPanel);
        VERIFY(animationPanel->property("visible").toBool());
        VERIFY(isPanelExpanded("animationPanel"));
    } else {
        newAnimationButton = nullptr;
        duplicateAnimationButton = nullptr;
        moveAnimationDownButton = nullptr;
        moveAnimationUpButton = nullptr;
        deleteAnimationButton = nullptr;
    }

    return true;
}

bool TestHelper::triggerOptions()
{
    return triggerShortcut("optionsShortcut", app.settings()->optionsShortcut());
}

bool TestHelper::triggerSelectNextLayerUp()
{
    return triggerShortcut("selectNextLayerUpShortcut", app.settings()->selectNextLayerUpShortcut());
}

bool TestHelper::triggerSelectNextLayerDown()
{
    return triggerShortcut("selectNextLayerDownShortcut", app.settings()->selectNextLayerDownShortcut());
}

bool TestHelper::selectLayer(const QString &layerName, int layerIndex)
{
    TRY_VERIFY(findListViewChild("layerListView", layerName));
    QQuickItem *layerDelegate = findListViewChild("layerListView", layerName);
    VERIFY(layerDelegate);
    if (!clickButton(layerDelegate))
        return false;
    VERIFY(layerDelegate->property("checked").toBool());
    if (!verifyCurrentLayer(layerName, layerIndex))
        return false;
    return true;
}

bool TestHelper::verifyCurrentLayer(const QString &layerName, int layerIndex)
{
    VERIFY2(layeredImageProject->currentLayerIndex() == layerIndex,
        qPrintable(QString::fromLatin1("Expected currentLayerIndex to be %1, but it's %2")
            .arg(layerIndex).arg(layeredImageProject->currentLayerIndex())));

    QQuickItem *layerDelegateItem = nullptr;
    if (!verifyLayerName(layerName, &layerDelegateItem))
        return false;
    VERIFY(layerDelegateItem->property("checked").toBool());

    // Verify that the status bar shows the correct current layer name.
    auto currentLayerNameLabel = window->findChild<QQuickItem*>("currentLayerNameLabel");
    VERIFY(currentLayerNameLabel);
    COMPARE_NON_FLOAT(currentLayerNameLabel->property("text").toString(), layerName);
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

bool TestHelper::addNewLayer(const QString &expectedGeneratedLayerName, int expectedIndex)
{
    const ImageLayer *expectedCurrentLayer = layeredImageProject->currentLayer();
    const int oldCurrentLayerIndex = layeredImageProject->currentLayerIndex();
    const int oldLayerCount = layeredImageProject->layerCount();
    if (!clickButton(newLayerButton))
        return false;
    VERIFY(layeredImageProject->layerCount() == oldLayerCount + 1);
    // The current layer shouldn't change..
    VERIFY(layeredImageProject->currentLayer() == expectedCurrentLayer);
    // .. but its index should, as new layers are added above the current layer.
    VERIFY(layeredImageProject->currentLayerIndex() == oldCurrentLayerIndex + 1);
    const QString actualLayerName = layeredImageProject->layerAt(expectedIndex)->name();
    if (actualLayerName != expectedGeneratedLayerName) {
        QString message;
        QDebug stream(&message);
        stream.nospace() << "Expected new layer name to be " << expectedGeneratedLayerName
            << " but it's " << actualLayerName << ". Layers:\n";
        for (int i = 0; i < layeredImageProject->layerCount(); ++i) {
            stream << "    index " << i << ": " << layeredImageProject->layerAt(i)->name() << "\n";
        }
        FAIL(qPrintable(message));
    }
    VERIFY(undoToolButton->isEnabled());
    return true;
}

bool TestHelper::makeCurrentAndRenameLayer(const QString &from, const QString &to)
{
    QQuickItem *layerDelegate = nullptr;
    VERIFY2(verifyLayerName(from, &layerDelegate), failureMessage);

    if (!clickButton(layerDelegate))
        return false;
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
        "Expected layerNameTextField to contain \"%1\" after entering new layer name, but it contains \"%2\"")
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
    VERIFY(oldLayerVisibilityCheckBoxValue == layer->isVisible());

    // If the layer's visibility already matches the target visibility, we have nothing to do.
    if (oldLayerVisibilityCheckBoxValue == !visible)
        return true;

    // Hide the layer.
    if (!clickButton(layerVisibilityCheckBox))
        return false;
    VERIFY(layer->isVisible() == visible);
    VERIFY(layerVisibilityCheckBox->property("checked").toBool() == visible);

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
    qCDebug(lcTestHelper).nospace() << "createNewProject projectType=" << projectType << "args=" << args;

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

    qCDebug(lcTestHelper) << "Triggering new project shortcut";
    if (!triggerNewProject())
        return false;

    // Check that we get prompted to discard any changes.
    if (project && project->hasUnsavedChanges()) {
        qCDebug(lcTestHelper) << "Discarding unsaved changes";
        if (!discardChanges())
            return false;
    }

    // Ensure that the new project popup is visible.
    const QObject *newProjectPopup = findOpenPopupFromTypeName("NewProjectPopup");
    VERIFY(newProjectPopup);
    VERIFY(newProjectPopup->property("visible").toBool());
    // TODO: remove this when https://bugreports.qt.io/browse/QTBUG-53420 is fixed
    newProjectPopup->property("contentItem").value<QQuickItem*>()->forceActiveFocus();
    ENSURE_ACTIVE_FOCUS(newProjectPopup)

    QString newProjectButtonObjectName;
    if (projectType == Project::TilesetType) {
        newProjectButtonObjectName = QLatin1String("tilesetProjectButton");
    } else if (projectType == Project::ImageType) {
        newProjectButtonObjectName = QLatin1String("imageProjectButton");
    } else {
        newProjectButtonObjectName = QLatin1String("layeredImageProjectButton");
    }

    // Click on the appropriate project type button.
    QQuickItem *newProjectButton = newProjectPopup->findChild<QQuickItem*>(newProjectButtonObjectName);
    VERIFY(newProjectButton);
    if (!clickButton(newProjectButton))
        return false;
    VERIFY(newProjectButton->property("checked").toBool());
    // The new project popup should be hidden, and now a project-specific project creation popup should be visible.
    TRY_VERIFY(!newProjectPopup->property("visible").toBool());

    if (projectType == Project::TilesetType) {
        // Create a temporary directory containing a tileset image for us to use.
        qCDebug(lcTestHelper) << "Setting up temporary tileset project directory";
        if (!setupTempTilesetProjectDir())
            return false;

        // Now the New Tileset Project popup should be visible.
        TRY_VERIFY(findOpenPopupFromTypeName("NewTilesetProjectPopup"));
        const QObject *newTilesetProjectPopup = findOpenPopupFromTypeName("NewTilesetProjectPopup");
        TRY_VERIFY2(newTilesetProjectPopup->property("opened").toBool(),
            "NewTilesetProjectPopup should be visible after clicking the new project button");

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
                if (!clickButton(transparentBackgroundCheckBox))
        return false;
                VERIFY(!transparentBackgroundCheckBox->property("checked").toBool());
            }
        }

        // Confirm creation of the project.
        qCDebug(lcTestHelper) << "Confirming tileset project creation by clicking OK button";
        QQuickItem *okButton = newTilesetProjectPopup->findChild<QQuickItem*>("newTilesetProjectOkButton");
        VERIFY(okButton);
        if (!clickButton(okButton))
        return false;
        TRY_VERIFY2(!newTilesetProjectPopup->property("visible").toBool(),
               "NewTilesetProjectPopup should not be visible after confirming project creation");
    } else {
        // projectType != Project::TilesetType

        // Create a temporary directory that we can save into, etc.
        if (projectType == Project::LayeredImageType) {
            qCDebug(lcTestHelper) << "Setting up temporary layered image project directory";
            if (!setupTempLayeredImageProjectDir())
                return false;
        }

        // Now the New [Layered] Image Project popup should be visible.
        QObject *newImageProjectPopup;
        VERIFY(ensureNewImageProjectPopupVisible(projectType, &newImageProjectPopup));

        // Ensure that the popup gets reset each time it's opened.
        const QImage clipboardImage = qGuiApp->clipboard()->image();
        const int expectedImageWidth = !clipboardImage.isNull() ? clipboardImage.width() : 256;
        QQuickItem *imageWidthSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageWidthSpinBox");
        VERIFY(imageWidthSpinBox);
        VERIFY2(imageWidthSpinBox->property("value").toInt() == expectedImageWidth, qPrintable(QString::fromLatin1(
            "Expected value of image width spinbox to be %1, but it's %2")
                .arg(expectedImageWidth).arg(imageWidthSpinBox->property("value").toInt())));

        const int expectedImageHeight = !clipboardImage.isNull() ? clipboardImage.height() : 256;
        QQuickItem *imageHeightSpinBox = newImageProjectPopup->findChild<QQuickItem*>("imageHeightSpinBox");
        VERIFY(imageHeightSpinBox);
        VERIFY2(imageHeightSpinBox->property("value").toInt() == expectedImageHeight, qPrintable(QString::fromLatin1(
            "Expected value of image height spinbox to be %1, but it's %2")
                .arg(expectedImageHeight).arg(imageHeightSpinBox->property("value").toInt())));

        VERIFY(imageWidthSpinBox->setProperty("value", imageWidth));
        VERIFY(imageWidthSpinBox->property("value").toInt() == imageWidth);

        VERIFY(imageHeightSpinBox->setProperty("value", imageHeight));
        VERIFY(imageHeightSpinBox->property("value").toInt() == imageHeight);

        QQuickItem *transparentImageBackgroundCheckBox = newImageProjectPopup->findChild<QQuickItem*>("transparentImageBackgroundCheckBox");
        VERIFY(transparentImageBackgroundCheckBox);
        VERIFY(transparentImageBackgroundCheckBox->property("checked").isValid());
        VERIFY(!transparentImageBackgroundCheckBox->property("checked").toBool());

        if (transparentImageBackground) {
            if (!clickButton(transparentImageBackgroundCheckBox))
        return false;
            VERIFY(transparentImageBackgroundCheckBox->property("checked").toBool() == transparentImageBackground);
        }

        // Confirm creation of the project.
        qCDebug(lcTestHelper) << "Confirming image/layered image project creation by clicking OK button";

        const QString newProjectOkButtonObjectName = projectType == Project::ImageType
            ? "newImageProjectPopupOkButton" : "newLayeredImageProjectPopupOkButton";
        QQuickItem *okButton = newImageProjectPopup->findChild<QQuickItem*>(newProjectOkButtonObjectName);
        VERIFY(okButton);
        QSignalSpy okButtonClickedSpy(okButton, SIGNAL(clicked()));
        VERIFY(okButtonClickedSpy.isValid());

        // This is a bit extreme, but I spent too long an a test failure (Windows, release, Qt 5.13)
        // where the cancel button was somehow clicked instead of OK,
        // even though the OK button's objectName was used to find it...
        const QString newProjectCancelButtonObjectName = projectType == Project::ImageType
            ? "newImageProjectPopupCancelButton" : "newLayeredImageProjectPopupCancelButton";
        QQuickItem *cancelButton = newImageProjectPopup->findChild<QQuickItem*>(newProjectCancelButtonObjectName);
        VERIFY(cancelButton);
        QSignalSpy cancelButtonClickedSpy(cancelButton, SIGNAL(clicked()));
        VERIFY(cancelButtonClickedSpy.isValid());

        // See the comment above. The button's parent item changes in size after this wait on that configuration:
        // QSizeF(201, 40) ... QSizeF(416, 40)
        // This somehow causes the click to hit the cancel button.
        QTest::qWait(20);

        okButton = newImageProjectPopup->findChild<QQuickItem*>(newProjectOkButtonObjectName);
        if (!clickButton(okButton))
        return false;
        VERIFY2(cancelButtonClickedSpy.count() == 0,
            "Did not expect newImageProjectCancelButton's clicked() signal to be emitted, but it was");
        VERIFY2(okButtonClickedSpy.count() == 1,
            "Expected newImageProjectOkButton's clicked() signal to be emitted, but it wasn't");
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

bool TestHelper::ensureNewImageProjectPopupVisible(Project::Type projectType, QObject **popup)
{
    // NewLayeredImageProjectPopup is basically just NewImageProjectPopup,
    // but their file names are different so we have to account for that here.
    const QString newProjectPopupTypeName = projectType == Project::ImageType
            ? "NewImageProjectPopup" : "NewLayeredImageProjectPopup";
    TRY_VERIFY(findOpenPopupFromTypeName(newProjectPopupTypeName));
    QObject *newImageProjectPopup = findOpenPopupFromTypeName(newProjectPopupTypeName);
    VERIFY(newImageProjectPopup);
    TRY_VERIFY(newImageProjectPopup->property("opened").toBool());
    if (popup)
        *popup = newImageProjectPopup;
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

    const QObject *errorPopup = findOpenPopupFromTypeName("ErrorPopup");
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
    TRY_VERIFY(!errorPopup->property("visible").toBool());

    if (projectCreationFailedSpy)
        projectCreationFailedSpy->clear();

    return true;
}

bool TestHelper::updateVariables(bool isNewProject, Project::Type projectType)
{
    qCDebug(lcTestHelper).nospace() << "Updating testhelper variables after creation of project - "
        << "isNewProject=" << isNewProject << " projectType=" << projectType;

    // The projects and canvases that we had references to should have
    // been destroyed by now.
    qCDebug(lcTestHelper).nospace() << "Waiting for the previous project (" << project << ") to be destroyed";
    TRY_VERIFY2(!project, qPrintable(QString::fromLatin1("The last project (%1) should have been destroyed by now")
        .arg(project->url().toString())));
    VERIFY(!imageProject);
    VERIFY(!tilesetProject);

    TRY_VERIFY(!canvas);
    VERIFY(!imageCanvas);
    VERIFY(!tileCanvas);

    project = projectManager->project();
    VERIFY(project);
    qCDebug(lcTestHelper) << "New project set:" << project;

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
        VERIFY(project->creationVersion() == QVersionNumber::fromString(qApp->applicationVersion()));

        // Reset any settings that have changed back to their defaults.
        QVariant settingsAsVariant = qmlEngine(canvas)->rootContext()->contextProperty("settings");
        VERIFY(settingsAsVariant.isValid());
        ApplicationSettings *settings = settingsAsVariant.value<ApplicationSettings*>();
        VERIFY(settings);
        settings->resetShortcutsToDefaults();

        canvas->setRulersVisible(false);

        cursorPos = QPoint();
        cursorWindowPos = QPoint();
    }

    // Sanity check.
    TRY_VERIFY(canvas->window() == canvasSizeToolButton->window());
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
        // Restore the colour panel's contentY to the default.
        QQuickItem *colourPanelFlickable = window->findChild<QQuickItem*>("colourPanelFlickable");
        VERIFY(colourPanelFlickable);
        VERIFY(colourPanelFlickable->setProperty("contentY", 0));
    }

    return true;
}

bool TestHelper::saveChanges(const QString &expectedErrorMessage)
{
    if (project->isNewProject()) {
        FAIL("Cannot save changes because the project is new, " \
             "and we cannot automate interaction with native Save As dialog");
    }

    if (!project->hasUnsavedChanges())
        FAIL("Cannot save changes because there are none");

    const QObject *saveChangesDialog = findOpenPopupFromTypeName("SaveChangesDialog");
    VERIFY(saveChangesDialog);
    TRY_VERIFY(saveChangesDialog->property("opened").toBool());

    QQuickItem *saveChangesButton = findDialogButtonFromObjectName(saveChangesDialog, "saveChangesDialogButton");
    VERIFY(saveChangesButton);
    if (!clickButton(saveChangesButton))
        return false;
    TRY_VERIFY(!saveChangesDialog->property("visible").toBool());

    if (!expectedErrorMessage.isEmpty()) {
        // The save should fail.
        if (!verifyErrorAndDismiss(expectedErrorMessage))
            return false;
        return true;
    }

    VERIFY(!project->hasUnsavedChanges());
    return true;
}

bool TestHelper::discardChanges()
{
    if (!project->hasUnsavedChanges())
        FAIL("Cannot discard changes because there are none");

    const QObject *saveChangesDialog = findOpenPopupFromTypeName("SaveChangesDialog");
    VERIFY(saveChangesDialog);
    TRY_VERIFY(saveChangesDialog->property("opened").toBool());

    QQuickItem *discardChangesButton = findDialogButtonFromObjectName(saveChangesDialog, "discardChangesDialogButton");
    VERIFY(discardChangesButton);
    if (!clickButton(discardChangesButton))
        return false;
    TRY_VERIFY(!saveChangesDialog->property("visible").toBool());
    return true;
}

bool TestHelper::verifyErrorAndDismiss(const QString &expectedErrorMessage)
{
    QObject *errorDialog = findOpenPopupFromTypeName("ErrorPopup");
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
    QObject *errorDialog = findOpenPopupFromTypeName("ErrorPopup");
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
    VERIFY2(tempProjectDir, qPrintable(QString::fromLatin1(
        "Can't copy %1 as tempProjectDir is null").arg(baseName)));

    QFile sourceFile(":/resources/" + baseName);
    VERIFY2(sourceFile.open(QIODevice::ReadOnly), qPrintable(QString::fromLatin1(
        "Failed to open %1 for copying to temp project dir: %2").arg(sourceFile.fileName()).arg(sourceFile.errorString())));
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
        "Error opening copied file at %1: %2").arg(saveFilePath).arg(copiedFile.errorString())));
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

bool TestHelper::openOptionsTab(const QString &tabButtonObjectName, QObject **optionsDialog)
{
    // Open options dialog.
    if (!triggerOptions())
        return false;
    QObject *theOptionsDialog = findOpenPopupFromTypeName("OptionsDialog");
    VERIFY(theOptionsDialog);
    TRY_VERIFY(theOptionsDialog->property("opened").toBool());

    // Open the relevant tab.
    QQuickItem *tabButton = theOptionsDialog->findChild<QQuickItem*>(tabButtonObjectName);
    VERIFY(tabButton);
    if (!clickButton(tabButton))
        return false;

    if (optionsDialog)
        *optionsDialog = theOptionsDialog;
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

    // Not sure why, but it's necessary to wait before expanding too.
    if (!ensurePanelPolished(panel))
        return false;

    const qreal originalHeight = panel->height();
    VERIFY(panel->setProperty("expanded", QVariant(expanded)));
    VERIFY(panel->property("expanded").toBool() == expanded);

    // This is a layout, so we need to ensure that its polish has completed before testing sizes of items.
    if (!ensurePanelPolished(panel))
        return false;

    if (expanded) {
        // Ensure that it has time to grow, otherwise stuff like input events will not work.
        TRY_VERIFY2(panel->height() > originalHeight, qPrintable(QString::fromLatin1(
            "Expected expanded height of \"%1\" to be larger than collapsed height of %2, but it wasn't (%3)")
                .arg(panelObjectName).arg(originalHeight).arg(panel->height())));
    } else {
        // If it's not visible, it's height might not update until it's visible again, apparently.
        // That's fine with us.
        if (panel->isVisible()) {
            // Ensure that the panel isn't visually expanded. We don't want to hard-code what the collapsed height
            // is, so we just make sure it's less than some large height.
            TRY_VERIFY2(panel->height() < 100, qPrintable(QString::fromLatin1(
                "Expected collapsed height of \"%1\" to be small, but it's %2")
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

    // TODO: fix this in qtquickcontrols2 auto tests, which require the same workaround:
    // https://bugreports.qt.io/browse/QTBUG-76294
    QTest::mouseMove(window, handleCentre);
    QTest::mouseMove(window, handleCentre);
    QQmlProperty handleHoveredProperty(handleItem.data(), "SplitHandle.hovered", qmlContext(handleItem.data()));
    VERIFY(handleHoveredProperty.isValid());
    VERIFY2(handleHoveredProperty.read().toBool() == true,
        "Expected SplitView handle to be hovered after moving the mouse onto it");
    QQmlProperty handlePressedProperty(handleItem.data(), "SplitHandle.pressed", qmlContext(handleItem.data()));
    VERIFY(handlePressedProperty.isValid());
    VERIFY(handlePressedProperty.read().toBool() == false);

    QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, handleCentre);
    VERIFY(handlePressedProperty.read().toBool() == true);

    QTest::mouseMove(window, newCentre);
    VERIFY(handlePressedProperty.read().toBool() == true);

    QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, newCentre);
    VERIFY2(handleHoveredProperty.read().toBool() == true,
        "Expected SplitView handle to be hovered after releasing the mouse over it after a drag");
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
        QPoint(panelSplitView->width() / 2, panelIt->second));
}

bool TestHelper::ensurePanelPolished(QQuickItem *panel)
{
    VERIFY(panel);

    auto panelContentItem = panel->property("contentItem").value<QQuickItem*>();
    VERIFY(panelContentItem);
    if (QQuickTest::qIsPolishScheduled(panelContentItem))
        VERIFY(QQuickTest::qWaitForItemPolished(panelContentItem));

    return true;
}

bool TestHelper::switchMode(TileCanvas::Mode mode)
{
    if (tileCanvas->mode() == mode)
        return true;

    if (!clickButton(modeToolButton))
        return false;
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
            if (!clickButton(penToolButton))
        return false;
        } else {
            QTest::keySequence(window, app.settings()->penToolShortcut());
        }
        break;
    case ImageCanvas::EyeDropperTool:
        if (inputType == MouseInputType) {
            VERIFY(eyeDropperToolButton->isEnabled());
            if (!clickButton(eyeDropperToolButton))
        return false;
        } else {
            QTest::keySequence(window, app.settings()->eyeDropperToolShortcut());
        }
        break;
    case ImageCanvas::FillTool:
        if (inputType == MouseInputType) {
            VERIFY(fillToolButton->isEnabled());
            if (!clickButton(fillToolButton))
        return false;
        } else {
            QTest::keySequence(window, app.settings()->fillToolShortcut());
        }
        break;
    case ImageCanvas::TexturedFillTool:
        if (inputType == MouseInputType) {
            VERIFY(fillToolButton->isEnabled());

            // Open the menu.
            mouseEventOnCentre(fillToolButton, MousePress);
            TRY_VERIFY(findOpenPopupFromTypeName("FillToolMenu"));
            QObject *fillToolMenu = findOpenPopupFromTypeName("FillToolMenu");
            VERIFY(fillToolMenu);
            TRY_VERIFY2(fillToolMenu->property("opened").toBool(), "Fill tool menu didn't open");
            mouseEventOnCentre(fillToolButton, MouseRelease);

            // Select the item.
            auto texturedFillToolMenuItem = window->findChild<QQuickItem*>("texturedFillToolMenuItem");
            VERIFY(texturedFillToolMenuItem);
            if (!clickButton(texturedFillToolMenuItem))
        return false;
            TRY_VERIFY2(!fillToolMenu->property("visible").toBool(), "Fill tool menu didn't close");
        } else {
            qWarning() << "No keyboard shortcut for the textured fill tool!";
        }
        break;
    case ImageCanvas::EraserTool:
        if (inputType == MouseInputType) {
            VERIFY(eraserToolButton->isEnabled());
            if (!clickButton(eraserToolButton))
        return false;
        } else {
            QTest::keySequence(window, app.settings()->eraserToolShortcut());
        }
        break;
    case ImageCanvas::SelectionTool:
        if (inputType == MouseInputType) {
            VERIFY(selectionToolButton->isEnabled());
            if (!clickButton(selectionToolButton))
        return false;
        } else {
            QTest::keySequence(window, app.settings()->selectionToolShortcut());
        }
        break;
    case ImageCanvas::NoteTool:
        if (inputType == MouseInputType) {
            VERIFY(noteToolButton->isEnabled());
            if (!clickButton(noteToolButton))
        return false;
        } else {
            QTest::keySequence(window, app.settings()->noteToolShortcut());
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
        const Qt::CursorShape expectedCursorShape =
            canvas->guideIndexAtCursorPos() != -1 ? Qt::OpenHandCursor : Qt::BlankCursor;
        VERIFY2(window->cursor().shape() == expectedCursorShape, qPrintable(QString::fromLatin1(
            "Expected %1 after Qt::Key_Space release, but got %2").arg(
                QtUtils::toString(expectedCursorShape), QtUtils::toString(window->cursor().shape()))));
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
    static const int maxLoops = 1000;
    CanvasPane *currentPane = canvas->currentPane();
    const bool zoomIn = currentPane->zoomLevel() < zoomLevel;
    for (int i = 0; zoomIn ? currentPane->zoomLevel() < zoomLevel : currentPane->zoomLevel() > zoomLevel; ++i) {
        wheelEvent(canvas, pos, zoomIn ? 1 : -1);
        if (i > maxLoops) {
            FAIL(qPrintable(QString::fromLatin1("Exceeed maximum loops (%1) to reach zoom (%2) - current pane zoom: %3")
                .arg(maxLoops).arg(zoomLevel).arg(currentPane->zoomLevel())));
        }
    }
    VERIFY(currentPane->integerZoomLevel() == zoomLevel);
    return true;
}
