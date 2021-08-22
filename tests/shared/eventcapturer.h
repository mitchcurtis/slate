/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Copyright (C) 2021 Mitch Curtis
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef EVENTCAPTURER_H
#define EVENTCAPTURER_H

#include <QObject>
#include <QElapsedTimer>
#include <QEvent>
#include <QList>
#include <QPoint>
#include <QSet>

#include "capturedevent.h"

class EventCapturer : public QObject
{
    Q_OBJECT

public:
    EventCapturer(QObject *parent = 0);

    enum MoveEventTrimFlag
    {
        TrimNone = 0x0,
        TrimLeading = 0x1,
        TrimTrailing = 0x2,
        TrimAfterReleases = 0x4,
        TrimBelowThreshold = 0x8,
        TrimAll = TrimLeading | TrimTrailing | TrimAfterReleases | TrimBelowThreshold
    };

    Q_DECLARE_FLAGS(MoveEventTrimFlags, MoveEventTrimFlag)

    void setStopCaptureKey(Qt::Key stopCaptureKey);
    void setMoveEventTrimFlags(MoveEventTrimFlags trimFlags);
    void setMoveEventTrimThreshold(int thresholdInMs);

    void setMaximumDelay(int maximumDelayInMs);

    void setEventSourceName(const QString &name);

    void startCapturing(QObject *eventSource, int duration);

    QSet<QEvent::Type> capturedEventTypes();
    void setCapturedEventTypes(QSet<QEvent::Type> types);

    QList<CapturedEvent> capturedEvents() const;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void stopCapturing();

private:
    void captureEvent(const QEvent *event);

    QObject *mEventSource;
    QSet<QEvent::Type> mCapturedEventTypes;
    Qt::Key mStopCaptureKey;
    MoveEventTrimFlags mMoveEventTrimFlags;
    int mMoveEventTrimThreshold;
    int mMaximumDelay;
    QElapsedTimer mDelayTimer;
    QList<CapturedEvent> mEvents;
    int mDuration;
    int mLastCaptureTime;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EventCapturer::MoveEventTrimFlags)

#endif // EVENTCAPTURER_H
