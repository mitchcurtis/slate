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

private Q_SLOTS:
    void swatch();

private:
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

    qInfo() << "Saving screenshots to" << mOutputDirectory.path();
}

void tst_Screenshots::swatch()
{
    // TODO: load layers.slp
    QVERIFY2(createNewLayeredImageProject(), failureMessage);

    QQuickItem *swatchesPanel = window->findChild<QQuickItem*>("swatchesPanel");
    QVERIFY(swatchesPanel);
    QVERIFY(swatchesPanel->setProperty("expanded", QVariant(true)));
    QVERIFY(QTest::qWaitForWindowExposed(window));

    auto grabResult = swatchesPanel->grabToImage();
    QTRY_VERIFY(!grabResult->image().isNull());
    QVERIFY(grabResult->image().save(mOutputDirectory.absoluteFilePath(QLatin1String("slate-swatches-panel.png"))));
}

int main(int argc, char *argv[])
{
    tst_Screenshots test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "screenshots.moc"
