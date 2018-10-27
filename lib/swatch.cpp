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

#include "swatch.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(lcSwatch, "app.swatch")

Swatch::Swatch(QObject *parent) :
    QObject(parent)
{
}

QVector<SwatchColour> Swatch::colours() const
{
    return mColours;
}

void Swatch::addColour(const QString &name, const QColor &colour)
{
    qCDebug(lcSwatch) << "adding colour" << colour.name() << "with name" << name << "to swatch";
    emit preColourAdded();

    mColours.append(SwatchColour(name, colour));

    emit postColourAdded();
}

void Swatch::renameColour(int index, const QString &newName)
{
    if (!isValidIndex(index))
        return;

    SwatchColour &colour = mColours[index];
    qCDebug(lcSwatch) << "renaming colour" << colour.name() << "to" << newName;
    colour.setName(newName);
    emit colourRenamed(index);
}

void Swatch::removeColour(int index)
{
    if (!isValidIndex(index))
        return;

    qCDebug(lcSwatch) << "removing colour" << mColours.at(index).colour().name()
        << "with name" << mColours.at(index).name() << "from swatch";
    emit preColourRemoved(index);

    mColours.removeAt(index);

    emit postColourRemoved();
}

bool Swatch::read(const QJsonObject &json, QString &errorMessage)
{
    if (!json.contains("colours")) {
        errorMessage = QLatin1String("No colours property");
        return false;
    }

    QVector<SwatchColour> colours;

    const QJsonArray colourArray = json.value("colours").toArray();
    for (int i = 0; i < colourArray.size(); ++i) {
        SwatchColour colour;
        if (!colour.read(colourArray.at(i).toObject(), errorMessage))
            return false;

        colours.append(colour);
    }

    mColours = colours;
    return true;
}

void Swatch::write(QJsonObject &json) const
{
    QJsonArray colourArray;
    for (const SwatchColour &swatchColour : qAsConst(mColours)) {
        QJsonObject colourObject;
        swatchColour.write(colourObject);
        colourArray.append(colourObject);
    }
    json["colours"] = colourArray;
}

void Swatch::copy(const Swatch &other)
{
    emit preImported();

    mColours = other.mColours;

    emit postImported();
}

bool Swatch::isValidIndex(int index) const
{
    if (index < 0 || index >= mColours.size()) {
        qWarning() << "Invalid swatch colour index" << index;
        return false;
    }
    return true;
}
