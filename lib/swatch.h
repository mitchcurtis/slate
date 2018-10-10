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

#ifndef SWATCH_H
#define SWATCH_H

#include <QObject>
#include <QVector>

#include "slate-global.h"
#include "swatchcolour.h"

class QJsonObject;

class SLATE_EXPORT Swatch : public QObject
{
    Q_OBJECT

public:
    explicit Swatch(QObject *parent = nullptr);

    QVector<SwatchColour> colours() const;

    Q_INVOKABLE void addColour(const QString &name, const QColor &colour);
    Q_INVOKABLE void renameColour(int index, const QString &newName);
    Q_INVOKABLE void removeColour(int index);

    bool read(const QJsonObject &json, QString &errorMessage);
    void write(QJsonObject &json) const;

    void reset();
    void copy(const Swatch &other);

signals:
    void preColourAdded();
    void postColourAdded();

    void colourRenamed(int index);

    void preColourRemoved(int index);
    void postColourRemoved();

    void preImported();
    void postImported();

private:
    bool isValidIndex(int index) const;

    QVector<SwatchColour> mColours;
};

#endif // SWATCH_H
