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

#include "splitter.h"

#include <QQuickItem>

#include "tilecanvas.h"

Splitter::Splitter(ImageCanvas *canvas) :
    QObject(canvas),
    mCanvas(canvas),
    mEnabled(false),
    mPosition(0),
    mWidth(0),
    mPressed(false),
    mHovered(false)
{
}

Splitter::~Splitter()
{
}

bool Splitter::isEnabled() const
{
    return mEnabled;
}

void Splitter::setEnabled(bool enabled)
{
    if (enabled == mEnabled)
        return;

    mEnabled = enabled;
    emit enabledChanged();
}

void Splitter::setPosition(qreal position)
{
    // Leave a little bit of room for the smaller side.
    position = qBound<qreal>(0.1, position, 0.9);
    if (!qFuzzyCompare(mPosition, position)) {
        mPosition = position;
        emit positionChanged();
    }
}

qreal Splitter::position() const
{
    return mPosition;
}

int Splitter::width() const
{
    return mWidth;
}

void Splitter::setWidth(int width)
{
    if (width == mWidth)
        return;

    mWidth = width;
    emit widthChanged();
}

bool Splitter::isPressed() const
{
    return mPressed;
}

void Splitter::setPressed(bool pressed)
{
    if (mPressed == pressed)
        return;

    mPressed = pressed;
    emit pressedChanged();
}

bool Splitter::isHovered() const
{
    return mHovered;
}

void Splitter::setHovered(bool hovered)
{
    if (mHovered == hovered)
        return;

    mHovered = hovered;
    emit hoveredChanged();
}
