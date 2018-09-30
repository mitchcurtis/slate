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

#include "application.h"
#include "project.h"
#include "testhelper.h"
#include "utils.h"

class tst_MemoryUsage : public TestHelper
{
    Q_OBJECT

public:
    tst_MemoryUsage(int &argc, char **argv);


private Q_SLOTS:
    void pen();
};

tst_MemoryUsage::tst_MemoryUsage(int &argc, char **argv) :
    TestHelper(argc, argv)
{
}

void tst_MemoryUsage::pen()
{
    QVERIFY2(createNewLayeredImageProject(1000, 1000), failureMessage);

    QVERIFY2(togglePanel("layerPanel", true), failureMessage);

    mouseEventOnCentre(newLayerButton, MouseClick);
    QCOMPARE(layeredImageProject->layerCount(), 2);
    QVERIFY2(selectLayer("Layer 2", 0), failureMessage);

    QVERIFY2(panTopLeftTo(100, 100), failureMessage);

    // Draw horizontal lines.
    for (int y = 1; y < project->heightInPixels() - 1; ++y) {
        setCursorPosInScenePixels(1, y);
        QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);

        for (int x = 1; x < project->widthInPixels() - 1; ++x) {
            setCursorPosInScenePixels(x, y);
            QTest::mouseMove(window, cursorWindowPos);
        }

        QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, cursorWindowPos);
    }

    QVERIFY2(drawPixelAtCursorPos(), failureMessage);

    QTest::qWait(200000);
}

int main(int argc, char *argv[])
{
    tst_MemoryUsage test(argc, argv);
    return QTest::qExec(&test, argc, argv);
}

#include "memory-usage.moc"
