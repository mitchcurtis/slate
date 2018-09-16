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

void Swatch::removeColour(int index)
{
    if (index < 0 || index >= mColours.size()) {
        qWarning() << "Invalid swatch colour index" << index;
        return;
    }

    qCDebug(lcSwatch) << "removing colour" << mColours.at(index).colour().name()
        << "with name" << mColours.at(index).name() << "from swatch";
    emit preColourRemoved(index);

    mColours.removeAt(index);

    emit postColourRemoved();
}

void Swatch::read(const QJsonObject &json)
{
    mColours.clear();

    const QJsonArray colourArray = json.value("colours").toArray();
    for (int i = 0; i < colourArray.size(); ++i) {
        SwatchColour colour;
        colour.read(colourArray.at(i).toObject());
        mColours.append(colour);
    }
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
