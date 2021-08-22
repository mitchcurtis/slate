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

#include "eventserialiser.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

QList<CapturedEvent> EventSerialiser::loadEventsFromJson(const QString &jsonPath)
{
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning().nospace() << "Unable to open JSON file " << jsonPath << ": " << file.errorString();
        return {};
    }

    QJsonParseError error;
    const QJsonDocument jsonSave(QJsonDocument::fromJson(file.readAll(), &error));
    if (error.error != QJsonParseError::NoError) {
        qWarning().nospace() << "Could not parse JSON file:" << jsonPath << ":" << error.errorString();
        return {};
    }

    const QJsonArray eventArray = jsonSave.array();
    QList<CapturedEvent> events;
    events.reserve(eventArray.size());
    for (int i = 0; i < eventArray.size(); ++i) {
        CapturedEvent event;
        event.read(eventArray.at(i).toObject());
        events.append(event);
    }

    return events;
}

void EventSerialiser::saveEventsToJson(const QString &jsonPath, const QList<CapturedEvent> &events, QJsonDocument::JsonFormat format)
{
    QFile saveFile(jsonPath);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning().nospace() << "Unable to open JSON file " << jsonPath << ": " << saveFile.errorString();
        return;
    }

    QJsonArray eventArray;
    for (const auto &event : events) {
        QJsonObject eventJson;
        event.write(eventJson);
        eventArray.append(eventJson);
    }

    QJsonDocument jsonSave(eventArray);
    const qint64 bytesWritten = saveFile.write(jsonSave.toJson(format));
    if (bytesWritten != -1)
        qInfo() << "Saved event JSON to" << jsonPath;
    else
        qWarning() << "Failed to write JSON to" << jsonPath;
}
