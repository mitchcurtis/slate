/*
    Copyright 2023, Mitch Curtis

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

#include "note.h"

#include <QFontMetrics>

Note::Note(const QPoint &position, const QString &text) :
    mPosition(position),
    mText(text)
{
    updateSize();
}

QPoint Note::position() const
{
    return mPosition;
}

void Note::setPosition(const QPoint &position)
{
    mPosition = position;
}

QString Note::text() const
{
    return mText;
}

void Note::setText(const QString &text)
{
    if (text == mText)
        return;

    mText = text;
    updateSize();
}

QSize Note::size() const
{
    return mSize;
}

void Note::updateSize()
{
    const QRect boundingRect = QFontMetrics(QFont()).tightBoundingRect(mText);
    static const int margins = 8;
    mSize = QSize(boundingRect.width() + margins, boundingRect.height() + margins);
}

QDebug operator<<(QDebug debug, const Note &note)
{
    debug.nospace() << "(Note position=" << note.mPosition
        << " text=" << note.mText
        << " size=" << note.mSize
        << ")";
    return debug.space();
}

bool operator==(const Note &a, const Note &b)
{
    return a.position() == b.position() && a.text() == b.text();
}
