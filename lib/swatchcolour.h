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

#ifndef SWATCHCOLOUR_H
#define SWATCHCOLOUR_H

#include <QString>
#include <QColor>
#include <QDebug>

#include "slate-global.h"

class QJsonObject;

class SLATE_EXPORT SwatchColour
{
public:
    SwatchColour();
    explicit SwatchColour(const QString &name, const QColor &colour);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString name() const;
    void setName(const QString &name);

    QColor colour() const;
    void setColour(const QColor &colour);

private:
    QString mName;
    QColor mColour;
};

SLATE_EXPORT bool operator==(const SwatchColour &lhs, const SwatchColour &rhs);
SLATE_EXPORT QDebug operator<<(QDebug debug, const SwatchColour &colour);

#endif // SWATCHCOLOUR_H
