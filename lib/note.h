/*
    Copyright 2019, Mitch Curtis

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

#ifndef NOTE_H
#define NOTE_H

#include <QDebug>
#include <QPoint>
#include <QSize>
#include <QString>

#include "slate-global.h"

class SLATE_EXPORT Note
{
public:
    Note(const QPoint &position, const QString &text);

    QPoint position() const;
    void setPosition(const QPoint &position);

    QString text() const;
    void setText(const QString &text);

    QSize size() const;

private:
    void updateSize();

    friend QDebug operator<<(QDebug debug, const Note &note);

    QPoint mPosition;
    QString mText;
    // Stored here so we can cache it once upon construction,
    // but not used for e.g. comparison, as it should
    // be identical for two notes with the same text.
    QSize mSize;
};

bool operator==(const Note &a, const Note &b);

#endif // NOTE_H
