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

#include "guide.h"

Guide::Guide() :
    mPosition(0),
    mOrientation(Qt::Vertical)
{
}

Guide::Guide(int position, Qt::Orientation orientation) :
    mPosition(position),
    mOrientation(orientation)
{
}

int Guide::position() const
{
    return mPosition;
}

void Guide::setPosition(int position)
{
    mPosition = position;
}

Qt::Orientation Guide::orientation() const
{
    return mOrientation;
}

void Guide::setOrientation(const Qt::Orientation &orientation)
{
    mOrientation = orientation;
}

bool operator==(const Guide &a, const Guide &b)
{
    return a.position() == b.position()
        && a.orientation() == b.orientation();
}

QDebug operator<<(QDebug debug, const Guide &guide)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "(Guide position=" << guide.position()
        << " orientation=" << guide.orientation()
        << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const QVector<Guide> &guides)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "QVector(";
    for (int i = 0; i < guides.size(); ++i) {
        const auto guide = guides.at(i);
        const bool horizontal = guide.orientation() == Qt::Horizontal;
        debug << '(' << guide.position() << ' ' << (horizontal ? 'h' : 'v') << ')';
        if (i != guides.size() - 1)
            debug << ", ";
    }
    debug << ')';
    return debug;
}

uint qHash(const Guide &guide, uint seed) noexcept
{
    return qHash(guide.position(), seed) ^ guide.orientation();
}
