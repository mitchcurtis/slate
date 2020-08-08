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

#include <QGuiApplication>
#include <QtTest>
#include <QQuickItemGrabResult>
#include <QQuickWindow>

#include "application.h"
#include "project.h"
#include "swatch.h"
#include "testhelper.h"
#include "utils.h"

class tst_Screenshots : public TestHelper
{
    Q_OBJECT

public:
    tst_Screenshots(int &argc, char **argv);

    Q_INVOKABLE QObject *findAnyChild(QObject *parent, const QString &objectName);

private Q_SLOTS:
    void cleanup();

    void panels_data();
    void panels();
    void toolBarFull();
    void toolBarIcons();
    void animation();
    void texturedFill();

private:
    QString makeImagePath(const QString &contentName);
    QObject *findItemChild(QObject *parent, const QString &objectName);

    QDir mOutputDirectory;
};

tst_Screenshots::tst_Screenshots(int &argc, char **argv) :
    // Note that we're using the application settings from the auto tests;
    // if anything weird starts happening, we should check if we're modifying
    // any settings and make sure that the auto tests reset them before each test.
    TestHelper(argc, argv)
{
    mOutputDirectory.setPath(QGuiApplication::applicationDirPath());
    mOutputDirectory.mkdir("output");
    mOutputDirectory.cd("output");

    // qmlRegisterSingletonType wasn't working, so we do it the hacky way.
    app.qmlEngine()->rootContext()->setContextProperty("findChildHelper", this);

    qInfo() << "Saving screenshots to" << mOutputDirectory.path();
}

void tst_Screenshots::cleanup()
{
    auto deletables = window->findChildren<QQuickItem*>("deleteMe");
    qDeleteAll(deletables);
}

void tst_Screenshots::panels_data()
{
    QTest::addColumn<QString>("projectFileName");
    QTest::addColumn<QString>("panelToMark");
    QTest::addColumn<QStringList>("extraPanelsToExpand");
    QTest::addColumn<QString>("markersQmlFilePath");
    QTest::addColumn<QString>("outputFileName");

    QTest::addRow("colour")
        << QString::fromLatin1("animation-panel.slp")
        << QString::fromLatin1("colourPanel")
        << (QStringList() << QLatin1String("layerPanel"))
        << QString::fromLatin1(":/resources/ColourPanelMarkers.qml")
        << QString::fromLatin1("slate-colour-panel.png");

    QTest::addRow("swatch")
        << QString::fromLatin1("animation-panel.slp")
        << QString::fromLatin1("swatchesPanel")
        << (QStringList() << QLatin1String("colourPanel"))
        << QString::fromLatin1(":/resources/SwatchPanelMarkers.qml")
        << QString::fromLatin1("slate-swatches-panel.png");

    QTest::addRow("layers")
        << QString::fromLatin1("animation-panel.slp")
        << QString::fromLatin1("layerPanel")
        << (QStringList() << QLatin1String("colourPanel"))
        << QString::fromLatin1(":/resources/LayerPanelMarkers.qml")
        << QString::fromLatin1("slate-layers-panel.png");

    QTest::addRow("animation")
        << QString::fromLatin1("animation-panel.slp")
        << QString::fromLatin1("animationPanel")
        << (QStringList() << QLatin1String("colourPanel"))
        << QString::fromLatin1(":/resources/AnimationPanelMarkers.qml")
        << QString::fromLatin1("slate-animation-panel.png");
}

void tst_Screenshots::panels()
{
    QFETCH(QString, projectFileName);
    QFETCH(QString, panelToMark);
    QFETCH(QStringList, extraPanelsToExpand);
    QFETCH(QString, markersQmlFilePath);
    QFETCH(QString, outputFileName);

    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Roll back to the previous value at the end of this test.
    const QSize originalWindowSize = window->size();
    Utils::ScopeGuard windowSizeGuard([=](){
        window->resize(originalWindowSize);
    });
    // Set the optimal vertical size for the window for these screenshots.
    window->resize(1000, 900);

    // Copy the project file from resources into our temporary directory.
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Try to load the project; there shouldn't be any errors.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    QVERIFY2(loadProject(QUrl::fromLocalFile(absolutePath)), failureMessage);

    app.settings()->setAutoSwatchEnabled(true);

    // Wait a frame, otherwise the toggle panels check fails with no change in height (e.g. 394 vs 394).
    QTest::qWait(0);
    QVERIFY2(togglePanels(extraPanelsToExpand, true), failureMessage);

    QQuickItem *panel = window->findChild<QQuickItem*>(panelToMark);
    QVERIFY(panel);
    QVERIFY(panel->setProperty("expanded", QVariant(true)));

    // Load markers into scene.
    QQmlComponent markerOverlayComponent(app.qmlEngine(), markersQmlFilePath);
    QVERIFY2(markerOverlayComponent.isReady(), qPrintable(markerOverlayComponent.errorString()));

    QQuickItem *markerOverlay = qobject_cast<QQuickItem*>(markerOverlayComponent.beginCreate(qmlContext(panel)));
    markerOverlay->setParent(panel);
    markerOverlay->setParentItem(panel);
    markerOverlayComponent.completeCreate();
    QVERIFY(markerOverlay);

    QVERIFY(QTest::qWaitForWindowExposed(window));

    // If we don't do this, the scene isn't ready.
    QSignalSpy swappedSpy(window, SIGNAL(frameSwapped()));
    QTRY_VERIFY(!swappedSpy.isEmpty());

    // Grab the image.
    auto grabResult = panel->grabToImage();
    QTRY_VERIFY(!grabResult->image().isNull());
    QVERIFY(grabResult->image().save(mOutputDirectory.absoluteFilePath(outputFileName)));
}

void tst_Screenshots::toolBarFull()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Roll back to the previous value at the end of this test.
    const QSize originalWindowSize = window->size();
    Utils::ScopeGuard windowSizeGuard([=](){
        window->resize(originalWindowSize);
    });
    // Set the optimal horizontal size for the window for these screenshots.
    window->resize(1400, 900);

    // Copy the project file from resources into our temporary directory.
    const QString projectFileName = QLatin1String("animation-panel.slp");
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Try to load the project; there shouldn't be any errors.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    QVERIFY2(loadProject(QUrl::fromLocalFile(absolutePath)), failureMessage);

    QQuickItem *toolBar = window->findChild<QQuickItem*>("toolBar");
    QVERIFY(toolBar);

    // Load markers into scene.
    QQmlComponent markerOverlayComponent(app.qmlEngine(), QLatin1String(":/resources/ToolBarMarkers.qml"));
    QVERIFY2(markerOverlayComponent.isReady(), qPrintable(markerOverlayComponent.errorString()));

    QQuickItem *markerOverlay = qobject_cast<QQuickItem*>(markerOverlayComponent.beginCreate(qmlContext(toolBar)));
    markerOverlay->setParent(toolBar);
    markerOverlay->setParentItem(toolBar);
    markerOverlayComponent.completeCreate();
    QVERIFY(markerOverlay);

    QVERIFY(QTest::qWaitForWindowExposed(window));

    // Hide stuff that we don't want to be visible in the screenshot.
    // Use opacity instead of visible so that the layouts don't change.
    QQuickItem *panelSplitView = window->findChild<QQuickItem*>("panelSplitView");
    QVERIFY(panelSplitView);
    panelSplitView->setOpacity(0);
    auto panelSplitViewOpacityRollback = qScopeGuard([=](){ panelSplitView->setOpacity(1); });

    if (canvas->areGuidesVisible()) {
        QVERIFY2(triggerGuidesVisible(), failureMessage);
        QCOMPARE(canvas->areGuidesVisible(), false);
    }

    QQuickItem *splitterBar = window->findChild<QQuickItem*>("splitterBar");
    QVERIFY(splitterBar);
    splitterBar->setOpacity(0);
    auto splitterBarOpacityRollback = qScopeGuard([=](){ splitterBar->setOpacity(1); });

    // If we don't do this, the scene isn't ready.
    QSignalSpy swappedSpy(window, SIGNAL(frameSwapped()));
    QTRY_VERIFY(!swappedSpy.isEmpty());

    // We want the tool bar, but we also need to leave some space under it to
    // fit the markers in. So, grab the whole scene and then cut out a portion of it.
    auto grabResult = window->contentItem()->grabToImage();
    QTRY_VERIFY(!grabResult->image().isNull());
    const QImage toolBarGrab = grabResult->image().copy(QRect(0, 0, toolBar->width(), toolBar->height() + 32));
    QVERIFY(toolBarGrab.save(mOutputDirectory.absoluteFilePath(QLatin1String("slate-tool-bar.png"))));

    QVERIFY2(triggerGuidesVisible(), failureMessage);
    QCOMPARE(canvas->areGuidesVisible(), true);
}

void tst_Screenshots::toolBarIcons()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Enable selection-related tool buttons to make their icons easier to see.
    QVERIFY2(selectArea(QRect(0, 0, 30, 30)), failureMessage);

    const QRegularExpression toolButtonRegex(QString::fromLatin1(".*ToolButton"));
    QSet<QQuickItem*> toolButtonsGrabbed;

    auto grabToolButtonImages = [=, &toolButtonsGrabbed](){
        const auto toolButtons = toolBar->findChildren<QQuickItem*>(toolButtonRegex);
        for (QQuickItem *toolButton : qAsConst(toolButtons)) {
            if (toolButtonsGrabbed.contains(toolButton) || !toolButton->isVisible())
                continue;

            // We want the tool bar's background because otherwise we just get icons on a transparent background.
            auto grabResult = window->contentItem()->grabToImage();
            QTRY_VERIFY(!grabResult->image().isNull());
            const int imageSize = 24;
            const QRectF imageRect((toolButton->width() - imageSize) / 2,
                (toolButton->height() - imageSize) / 2, imageSize, imageSize);
            const QRectF toolButtonSceneRect = toolButton->mapRectToScene(imageRect);
            const QImage toolBarGrab = grabResult->image().copy(toolButtonSceneRect.toRect());
            const QString imagePath = makeImagePath(QLatin1String("toolbar-") + toolButton->objectName());
            QVERIFY(toolBarGrab.save(imagePath));

            toolButtonsGrabbed.insert(toolButton);
        }
    };

    // Grab the tool buttons that are visible for image/layer projects.
    grabToolButtonImages();

    // Now grab the buttons that are only visible for tileset projects.
    QVERIFY2(createNewTilesetProject(), failureMessage);
    grabToolButtonImages();
}
#include <QScreen>
void tst_Screenshots::animation()
{
    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Roll back to the previous value at the end of this test.
    const QSize originalWindowSize = window->size();
    Utils::ScopeGuard windowSizeGuard([=](){
        window->resize(originalWindowSize);
    });
    // This is the optimal size for the window for the tutorial.
    window->resize(1401, 675);

    // Copy the project files from resources into our temporary directory.
    QStringList projectFileNames;
    for (int i = 1; i <= 3; ++i) {
        const QString projectFileName = QString::fromLatin1("animation-tutorial-%1.slp").arg(i);
        projectFileNames.append(projectFileName);
        QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);
    }

    // Chapter 1.
    QString projectPath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileNames.at(0));
    QVERIFY2(loadProject(QUrl::fromLocalFile(projectPath)), failureMessage);

    QStringList panelsToExpand;
    panelsToExpand << QLatin1String("layerPanel");
    QVERIFY2(togglePanels(panelsToExpand, true), failureMessage);

    QString screenshotPath = QLatin1String("slate-animation-tutorial-1.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Oepn the canvas size dialog.
    QVERIFY2(changeCanvasSize(216, 38, DoNotCloseDialog), failureMessage);
    // Take a screenshot.
    screenshotPath = QLatin1String("slate-animation-tutorial-1.1.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));
    // Close it.
    QTest::keyClick(window, Qt::Key_Escape, Qt::NoModifier, 100);
    const QObject *canvasSizePopup = findPopupFromTypeName("CanvasSizePopup");
    QVERIFY(canvasSizePopup);
    QTRY_VERIFY2(!canvasSizePopup->property("visible").toBool(), "Failed to cancel CanvasSizePopup");

    // Move the mouse away to avoid getting the tooltip in the next screenshot.
    QQuickItem *toolBar = window->findChild<QQuickItem*>("toolBar");
    QVERIFY(toolBar);
    setCursorPosInPixels(QPoint(10, toolBar->height() + 100));
    QTest::mouseMove(window, cursorWindowPos);
    QQuickItem *canvasSizeToolButton = window->findChild<QQuickItem*>("canvasSizeToolButton");
    QVERIFY(canvasSizeToolButton);

    QVERIFY2(triggerCloseProject(), failureMessage);

    // Chapter 2.
    projectPath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileNames.at(1));
    QVERIFY2(loadProject(QUrl::fromLocalFile(projectPath)), failureMessage);

    QVERIFY2(togglePanels(panelsToExpand, true), failureMessage);

    screenshotPath = QLatin1String("slate-animation-tutorial-2.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Select the background layer and fill it.
    QVERIFY2(selectLayer("background", 2), failureMessage);
    QVERIFY2(switchTool(TileCanvas::FillTool), failureMessage);
    setCursorPosInScenePixels(QPoint(project->widthInPixels() - 1, 0));
    canvas->setPenForegroundColour(Qt::white);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    screenshotPath = QLatin1String("slate-animation-tutorial-2.1.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Turn on rulers and drag some guides out.
    if (!canvas->areRulersVisible()) {
        QVERIFY2(triggerRulersVisible(), failureMessage);
        QCOMPARE(canvas->areRulersVisible(), true);
    }

    for (int i = 1; i <= 5; ++i) {
        QVERIFY2(addNewGuide(Qt::Vertical, i * 36), qPrintable(
            QString::fromLatin1("When dragging guide %1: ").arg(i) + QString::fromLatin1(failureMessage)));
    }

    QVERIFY2(addNewGuide(Qt::Horizontal, 25), failureMessage);

    screenshotPath = QLatin1String("slate-animation-tutorial-2.2.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    QVERIFY2(triggerCloseProject(), failureMessage);
    QVERIFY2(discardChanges(), failureMessage);

    // Chapter 3.
    projectPath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileNames.at(2));
    QVERIFY2(loadProject(QUrl::fromLocalFile(projectPath)), failureMessage);

    panelsToExpand << QLatin1String("animationPanel");
    QVERIFY2(togglePanels(panelsToExpand, true), failureMessage);

    // Take a screenshot of the Animation panel.
    QQuickItem *animationPanel = window->findChild<QQuickItem*>("animationPanel");
    QVERIFY(animationPanel);
    screenshotPath = QLatin1String("slate-animation-tutorial-3.png");
    QVERIFY(imageGrabber.requestImage(window->contentItem()));
    QTRY_VERIFY(imageGrabber.isReady());
    QImage panelGrab = imageGrabber.takeImage();
    QRectF panelRectInScene(0, 0, animationPanel->width(), animationPanel->height());
    panelRectInScene = animationPanel->mapRectToScene(panelRectInScene).toRect();
    panelGrab = panelGrab.copy(panelRectInScene.toRect());
    QVERIFY(panelGrab.save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Close the Layers panel to make more room.
    QVERIFY2(togglePanel("layerPanel", false), failureMessage);

    // Open the preview settings popup to modify the scale.
    QQuickItem *animationPanelSettingsToolButton = window->findChild<QQuickItem*>("animationPanelSettingsToolButton");
    QVERIFY(animationPanelSettingsToolButton);
    mouseEventOnCentre(animationPanelSettingsToolButton, MouseClick);
    QObject *animationPreviewSettingsPopup = findPopupFromTypeName("AnimationPreviewSettingsPopup");
    QVERIFY(animationPreviewSettingsPopup);
    QTRY_COMPARE(animationPreviewSettingsPopup->property("opened").toBool(), true);

    // Cheat a bit and modify it manually, then take the screenshot.
    AnimationPlayback *currentAnimationPlayback = TestHelper::animationPlayback();
    currentAnimationPlayback->setScale(4);
    screenshotPath = QLatin1String("slate-animation-tutorial-3-1.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));
    // Close it.
    QVERIFY2(clickDialogFooterButton(animationPreviewSettingsPopup, "OK"), failureMessage);

    // Open the animation settings popup for the current animation and take a screenshot.
    QObject *animationSettingsPopup = nullptr;
    QVERIFY2(openAnimationSettingsPopupForCurrentAnimation(&animationSettingsPopup), failureMessage);
    screenshotPath = QLatin1String("slate-animation-tutorial-3-2.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));
    // Close it.
    QVERIFY2(clickDialogFooterButton(animationSettingsPopup, "OK"), failureMessage);

    QVERIFY2(triggerCloseProject(), failureMessage);
}

void tst_Screenshots::texturedFill()
{
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Roll back to the previous value at the end of this test.
    const QSize originalWindowSize = window->size();
    Utils::ScopeGuard windowSizeGuard([=](){
        window->resize(originalWindowSize);
    });
    // This is the optimal size for the window for the tutorial.
    // TODO
//    window->resize(1401, 675);

    // Copy the project files from resources into our temporary directory.
    QStringList projectFileNames;
    for (int i = 1; i <= 2; ++i) {
        const QString projectFileName = QString::fromLatin1("textured-fill-tool-tutorial-%1.slp").arg(i);
        projectFileNames.append(projectFileName);
        QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);
    }

    // Chapter 1.
    QString projectPath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileNames.at(0));
    QVERIFY2(loadProject(QUrl::fromLocalFile(projectPath)), failureMessage);

    QStringList panelsToExpand;
    panelsToExpand << QLatin1String("colourPanel") << QLatin1String("layerPanel");
    QVERIFY2(togglePanels(panelsToExpand, true), failureMessage);

    QString screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-1.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Select the base colour using the eyedropper.
    QVERIFY2(switchTool(ImageCanvas::EyeDropperTool), failureMessage);
    setCursorPosInScenePixels(32, 32);
    QTest::mouseMove(window, cursorWindowPos);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    QCOMPARE(canvas->penForegroundColour(), QColor::fromRgb(0x7f7f7f));

    // Open the textured fill settings dialog.
    QObject *settingsDialog = window->findChild<QObject*>("texturedFillSettingsDialog");
    QVERIFY(settingsDialog);
    QVERIFY(QMetaObject::invokeMethod(settingsDialog, "open"));
    QVERIFY(settingsDialog->property("visible").toBool());

    // Take a screenshot.
    QTest::qWait(200);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-1.1.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Modify the values.
    QQuickItem *lightnessVarianceSlider = settingsDialog->findChild<QQuickItem*>("lightnessVarianceSlider");
    QVERIFY(lightnessVarianceSlider);
    const QPoint firstHandlePressPos(lightnessVarianceSlider->width() * 0.25, lightnessVarianceSlider->height() / 2);
    const QPoint firstHandleReleasePos(lightnessVarianceSlider->width() * 0.0, lightnessVarianceSlider->height() / 2);
    const QPoint secondHandlePressPos(lightnessVarianceSlider->width() * 0.75, lightnessVarianceSlider->height() / 2);
    const QPoint secondHandleReleasePos(lightnessVarianceSlider->width() * 0.4, lightnessVarianceSlider->height() / 2);
    mouseEvent(lightnessVarianceSlider, firstHandlePressPos, MousePress);
    mouseEvent(lightnessVarianceSlider, firstHandleReleasePos, MouseRelease);
    mouseEvent(lightnessVarianceSlider, secondHandlePressPos, MousePress);
    mouseEvent(lightnessVarianceSlider, secondHandleReleasePos, MouseRelease);

    // Move the mouse away to hide any tooltips.
    QTest::mouseMove(window, lightnessVarianceSlider->mapToScene(
        QPoint(lightnessVarianceSlider->width() / 2, lightnessVarianceSlider->height() + 1)).toPoint());

    // Take a screenshot.
    QTest::qWait(1000);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-1.2.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Increase the preview scale.
    QQuickItem *previewScaleSlider = settingsDialog->findChild<QQuickItem*>("previewScaleSlider");
    QVERIFY(previewScaleSlider);
    const QPoint scaleSliderHandlePos(previewScaleSlider->width() * 0.25, previewScaleSlider->height() / 2);
    mouseEvent(previewScaleSlider, scaleSliderHandlePos, MouseClick);

    // Move the mouse away to hide any tooltips.
    QTest::mouseMove(window, previewScaleSlider->mapToScene(
        QPoint(previewScaleSlider->width() + 1, previewScaleSlider->height() / 2)).toPoint());

    // Take a screenshot.
    QTest::qWait(1000);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-1.3.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Confirm the settings.
    QQuickItem *okButton = settingsDialog->findChild<QQuickItem*>("texturedFillSettingsDialogOkButton");
    QVERIFY(okButton);
    mouseEventOnCentre(okButton, MouseClick);
    QTRY_VERIFY(!settingsDialog->property("opened").toBool());

    // Switch to the textured fill tool and fill.
    QTest::qWait(1000);
    QVERIFY2(switchTool(ImageCanvas::TexturedFillTool), failureMessage);
    setCursorPosInScenePixels(32, 32);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    // Take a screenshot.
    QTest::qWait(500);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-1.4.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Zoom out and centre the view.
    canvas->setSplitScreen(false);
    canvas->currentPane()->setZoomLevel(1);
    QVERIFY2(triggerShortcut("centreShortcut", app.settings()->centreShortcut()), failureMessage);

    // Take another screenshot.
    QTest::qWait(100);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-1.5.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Chapter 2.
    projectPath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileNames.at(1));
    QVERIFY2(loadProject(QUrl::fromLocalFile(projectPath)), failureMessage);
    QVERIFY2(togglePanels(panelsToExpand, true), failureMessage);

    // Take a screenshot.
    QTest::qWait(200);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-2.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Select the pixels.
    QVERIFY2(selectArea(QRect(30, 21, 4, 2)), failureMessage);

    // Take a screenshot.
    QTest::qWait(200);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-2.1.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Add the colours from the selection manually since we can't open the menu.
    QVERIFY2(addSelectedColoursToTexturedFillSwatch(), failureMessage);

    // Take a screenshot.
    QTest::qWait(200);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-2.2.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Adjust the probability of the yellow-ish colour.
    QQuickItem *texturedFillSwatchListView = settingsDialog->findChild<QQuickItem*>("texturedFillSwatchListView");
    QVERIFY(texturedFillSwatchListView);
    QCOMPARE(texturedFillSwatchListView->property("count").toInt(), 7);
    QQuickItem *swatchDelegate = nullptr;
    // Centre the delegate so we can click on the slider.
    QVERIFY(QMetaObject::invokeMethod(texturedFillSwatchListView, "positionViewAtIndex",
        Qt::DirectConnection, Q_ARG(int, 4), Q_ARG(int, 1)));
    // Get the delegate.
    QVERIFY(QMetaObject::invokeMethod(texturedFillSwatchListView, "itemAtIndex", Qt::DirectConnection,
        Q_RETURN_ARG(QQuickItem*, swatchDelegate), Q_ARG(int, 4)));
    QQuickItem *probabilitySlider = swatchDelegate->findChild<QQuickItem*>("texturedFillSwatchColourProbabilitySlider");
    QVERIFY(probabilitySlider);
    // Click on the slider.
    mouseEvent(probabilitySlider, QPoint(probabilitySlider->width() * 0.25, probabilitySlider->height() / 2), MouseClick);
    const qreal sliderValue = probabilitySlider->property("value").toReal();
    QVERIFY(sliderValue < 0.5);

    // Take a screenshot.
    QTest::qWait(200);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-2.3.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Confirm the settings.
    mouseEventOnCentre(okButton, MouseClick);
    QTRY_VERIFY(!settingsDialog->property("opened").toBool());

    // Switch to the textured fill tool and fill.
    QTest::qWait(1000);
    QVERIFY2(switchTool(ImageCanvas::TexturedFillTool), failureMessage);
    setCursorPosInScenePixels(32, 32);
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

    // Take a screenshot.
    QTest::qWait(500);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-2.4.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    // Zoom out and centre the view.
    canvas->setSplitScreen(false);
    canvas->currentPane()->setZoomLevel(1);
    QVERIFY2(triggerShortcut("centreShortcut", app.settings()->centreShortcut()), failureMessage);

    // Take another screenshot.
    QTest::qWait(200);
    screenshotPath = QLatin1String("slate-textured-fill-tool-tutorial-2.5.png");
    QVERIFY(window->grabWindow().save(mOutputDirectory.absoluteFilePath(screenshotPath)));

    project->close();
}

QString tst_Screenshots::tst_Screenshots::makeImagePath(const QString &contentName)
{
    return mOutputDirectory.absoluteFilePath(QLatin1String("slate-") + contentName + QLatin1String(".png"));
}

QObject *tst_Screenshots::findItemChild(QObject *parent, const QString &objectName)
{
    if (!parent)
        return nullptr;

    QQuickItem *parentItem = qobject_cast<QQuickItem*>(parent);
    if (!parentItem)
        return nullptr;

    auto childItems = parentItem->childItems();
    for (int i = 0; i < childItems.size(); ++i) {
        // Is this direct child of ours the child we're after?
        QQuickItem *child = childItems.at(i);
        if (child->objectName() == objectName)
            return child;
    }

    for (int i = 0; i < childItems.size(); ++i) {
        // Try the direct child's children.
        auto child = findItemChild(childItems.at(i), objectName);
        if (child)
            return child;
    }
    return nullptr;
}

QObject *tst_Screenshots::findAnyChild(QObject *parent, const QString &objectName)
{
    // First, search the visual item hierarchy.
    QObject *child = findItemChild(parent, objectName);
    if (child)
        return child;

    // If it's not a visual child, it might be a QObject child.
    return parent ? parent->findChild<QObject*>(objectName) : nullptr;
}

int main(int argc, char *argv[])
{
    tst_Screenshots test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "screenshots.moc"
