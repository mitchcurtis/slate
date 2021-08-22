/*
    Copyright 2021, Mitch Curtis

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

#include "eventreplayer.h"

#include <QQuickItem>
#include <QQuickWindow>
#include <QtTest>

bool EventReplayer::replayEvents(const QQuickItem *targetItem, const QList<CapturedEvent> &events)
{
    if (!targetItem->window()) {
        qWarning() << "Can't replay events because targetItem has no window associated with it";
        return false;
    }

    for (const auto &event : events) {
        const QPoint eventPosInWindow = targetItem->mapToScene(
            QPointF(event.pos().x(), event.pos().y())).toPoint();

        switch (event.type()) {
        case QEvent::MouseButtonPress:
            QTest::mousePress(targetItem->window(), event.mouseButton(), Qt::NoModifier, eventPosInWindow, event.delay());
            break;
        case QEvent::MouseButtonRelease:
            QTest::mouseRelease(targetItem->window(), event.mouseButton(), Qt::NoModifier, eventPosInWindow, event.delay());
            break;
        case QEvent::MouseButtonDblClick:
            QTest::mouseDClick(targetItem->window(), event.mouseButton(), Qt::NoModifier, eventPosInWindow, event.delay());
            break;
        case QEvent::MouseMove:
            QTest::mouseMove(targetItem->window(), eventPosInWindow, event.delay());
            break;
        default:
            qWarning() << "No support for event type" << event.type();
            return false;
        }
    }

    return true;
}
