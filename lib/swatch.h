/*
    Copyright 2020, Mitch Curtis

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
#include <QList>
#include <QQmlEngine>

#include "slate-global.h"
#include "swatchcolour.h"

class QJsonObject;

class SLATE_EXPORT Swatch : public QObject
{
    Q_OBJECT
    QML_ANONYMOUS

public:
    explicit Swatch(QObject *parent = nullptr);

    QVector<SwatchColour> colours() const;

    Q_INVOKABLE void addColour(const QString &name, const QColor &colour);
    void addColours(const QVector<QColor> &colours);
    Q_INVOKABLE void renameColour(int index, const QString &newName);
    Q_INVOKABLE void removeColour(int index);

    int indexOfColour(const QColor &colour) const;

    bool read(const QJsonObject &json, QString &errorMessage);
    void write(QJsonObject &json) const;

    void reset();
    void copy(const Swatch &other);

signals:
    void preColourAdded();
    void preColoursAdded();
    void postColourAdded();
    void postColoursAdded();

    void colourRenamed(int index);

    void preColourRemoved(int index);
    void postColourRemoved();

    void preImported();
    void postImported();

protected:
    virtual void doAddColour(const QString &name, const QColor &colour);
    virtual void doAddColours(const QVector<QColor> &colours);
    virtual void doRemoveColour(int index);
    virtual void doCopy(const Swatch &other);
    virtual bool doRead(const QJsonObject &json, QString &errorMessage);
    virtual void doWrite(QJsonObject &json) const;

    bool isValidIndex(int index) const;

    QList<SwatchColour> mColours;
};

#endif // SWATCH_H
