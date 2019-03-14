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

#ifndef SERIALISABLESTATE_H
#define SERIALISABLESTATE_H

#include <QObject>
#include <QVariant>

#include "slate-global.h"

class SLATE_EXPORT SerialisableState : public QObject
{
    Q_OBJECT

public:
    SerialisableState();

    Q_INVOKABLE bool contains(const QString &key) const;
    Q_INVOKABLE QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);

    // QTBUG-71511: we want to serialise SplitView's state to JSON, which means
    // we need it as a string, not a binary byte array. QML has functions
    // for this, but they're completely broken, so we have our own.
    // While we're at it, we also give them proper, non-four-letter names.
    Q_INVOKABLE QByteArray base64ToBinary(const QString &base64String);
    Q_INVOKABLE QString binaryToBase64(const QByteArray &binaryByteArray);

    QVariantMap map() const;
    void reset(const QVariantMap &map);

private:
    QVariantMap mState;
};

#endif // SERIALISABLESTATE_H
