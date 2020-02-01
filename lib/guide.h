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

#ifndef GUIDE_H
#define GUIDE_H

#include <Qt>

#include "slate-global.h"

class SLATE_EXPORT Guide
{
public:
    Guide();
    Guide(int position, Qt::Orientation orientation);

    int position() const;
    void setPosition(int position);

    Qt::Orientation orientation() const;
    void setOrientation(const Qt::Orientation &orientation);

private:
    int mPosition;
    Qt::Orientation mOrientation;
};

bool operator==(const Guide &a, const Guide &b);

#endif // GUIDE_H
