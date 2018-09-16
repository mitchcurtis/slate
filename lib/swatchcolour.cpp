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

#include "swatchcolour.h"

#include <QJsonObject>

SwatchColour::SwatchColour()
{
}

SwatchColour::SwatchColour(const QString &name, const QColor &colour) :
    mName(name),
    mColour(colour)
{
}

QString SwatchColour::name() const
{
    return mName;
}

void SwatchColour::setName(const QString &name)
{
    mName = name;
}

QColor SwatchColour::colour() const
{
    return mColour;
}

void SwatchColour::setColour(const QColor &colour)
{
    mColour = colour;
}

void SwatchColour::read(const QJsonObject &json)
{
    mName = json.value(QLatin1String("name")).toString();
    mColour = QColor(json.value(QLatin1String("colour")).toString());
}

void SwatchColour::write(QJsonObject &json) const
{
    json[QLatin1String("name")] = mName;
    json[QLatin1String("colour")] = mColour.name();
}

bool operator==(const SwatchColour &lhs, const SwatchColour &rhs)
{
    return lhs.name() == rhs.name() && lhs.colour() == rhs.colour();
}

QDebug operator<<(QDebug debug, const SwatchColour &colour)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "SwatchColour(name=" << colour.name()
                    << " colour=" << colour.colour().name() << ")";
    return debug;
}
