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

#ifndef EVENTSERIALISER_H
#define EVENTSERIALISER_H

#include <QList>
#include <QJsonDocument>

#include "capturedevent.h"

class EventSerialiser
{
public:
    static QList<CapturedEvent> loadEventsFromJson(const QString &jsonPath);
    static void saveEventsToJson(const QString &jsonPath, const QList<CapturedEvent> &events,
        QJsonDocument::JsonFormat format = QJsonDocument::Compact);
};

#endif // EVENTSERIALISER_H
