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
    return a.position() == b.position() && a.orientation() == b.orientation();
}
