/*
    Copyright 2018, Mitch Curtis

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

    Q_INVOKABLE QObject *findAnyChild(QObject *parent, const QString &objectName)
    {
        // First, search the visual item hierarchy.
        QObject *child = findItemChild(parent, objectName);
        if (child)
            return child;

        // If it's not a visual child, it might be a QObject child.
        return parent ? parent->findChild<QObject*>(objectName) : nullptr;
    }

private Q_SLOTS:
    void panels_data();
    void panels();

private:
    QObject *findItemChild(QObject *parent, const QString &objectName)
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

    QDir mOutputDirectory;
};

tst_Screenshots::tst_Screenshots(int &argc, char **argv) :
    // Note that we're using the application settings from the auto tests;
    // if anything weird starts happening, we should check if we're modifying
    // any settings and make sure that the auto tests reset them before each test.
    TestHelper(argc, argv)
{
    mOutputDirectory = QGuiApplication::applicationDirPath();
    mOutputDirectory.mkdir("output");
    mOutputDirectory.cd("output");

    // qmlRegisterSingletonType wasn't working, so we do it the hacky way.
    app.qmlEngine()->rootContext()->setContextProperty("findChildHelper", this);

    qInfo() << "Saving screenshots to" << mOutputDirectory.path();
}

void tst_Screenshots::panels_data()
{
    QTest::addColumn<QString>("projectFileName");
    QTest::addColumn<QString>("panelToMark");
    QTest::addColumn<QStringList>("panelsToExpand");
    QTest::addColumn<QString>("markersQmlFilePath");
    QTest::addColumn<QString>("outputFileName");

    QTest::addRow("colour")
        << QString::fromLatin1("animation.slp")
        << QString::fromLatin1("colourPanel")
        << (QStringList() << QLatin1String("layerPanel"))
        << QString::fromLatin1(":/resources/ColourMarkers.qml")
        << QString::fromLatin1("slate-colour-panel.png");

    QTest::addRow("swatch")
        << QString::fromLatin1("animation.slp")
        << QString::fromLatin1("swatchesPanel")
        << (QStringList() << QLatin1String("colourPanel"))
        << QString::fromLatin1(":/resources/SwatchMarkers.qml")
        << QString::fromLatin1("slate-swatches-panel.png");

    QTest::addRow("layers")
        << QString::fromLatin1("animation.slp")
        << QString::fromLatin1("layerPanel")
        << (QStringList() << QLatin1String("colourPanel"))
        << QString::fromLatin1(":/resources/LayerMarkers.qml")
        << QString::fromLatin1("slate-layers-panel.png");

    QTest::addRow("animation")
        << QString::fromLatin1("animation.slp")
        << QString::fromLatin1("animationPanel")
        << (QStringList() << QLatin1String("colourPanel") << QLatin1String("layerPanel"))
        << QString::fromLatin1(":/resources/AnimationMarkers.qml")
        << QString::fromLatin1("slate-animation-panel.png");
}

void tst_Screenshots::panels()
{
    QFETCH(QString, projectFileName);
    QFETCH(QString, panelToMark);
    QFETCH(QStringList, panelsToExpand);
    QFETCH(QString, markersQmlFilePath);
    QFETCH(QString, outputFileName);

    // Ensure that we have a temporary directory.
    QVERIFY2(setupTempLayeredImageProjectDir(), failureMessage);

    // Copy the project file from resources into our temporary directory.
    QVERIFY2(copyFileFromResourcesToTempProjectDir(projectFileName), failureMessage);

    // Try to load the project; there shouldn't be any errors.
    const QString absolutePath = QDir(tempProjectDir->path()).absoluteFilePath(projectFileName);
    QVERIFY2(loadProject(QUrl::fromLocalFile(absolutePath)), failureMessage);

    app.settings()->setAutoSwatchEnabled(true);

    for (const QString &panelObjectName : qAsConst(panelsToExpand)) {
        QQuickItem *panelToExpand = window->findChild<QQuickItem*>(panelObjectName);
        QVERIFY2(panelToExpand, qPrintable(panelObjectName));
        QVERIFY(panelToExpand->setProperty("expanded", QVariant(true)));
    }

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

int main(int argc, char *argv[])
{
    tst_Screenshots test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "screenshots.moc"
