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

import QtQuick 2.12

import App 1.0

/*!
    An item whose implicit size is equal to the natural size of each frame in the animation,
    with the sprite being scaled to the scale of the AnimationPlayback and centered in the container.
*/
Item {
    id: root
    implicitWidth: spriteImage.implicitWidth * spriteImage.scale
    implicitHeight: spriteImage.implicitHeight * spriteImage.scale

    property var project
    property AnimationPlayback animationPlayback

    SpriteImage {
        id: spriteImage
        objectName: root.objectName + "SpriteImage"
        project: root.project
        animationPlayback: root.animationPlayback
        scale: animationPlayback ? animationPlayback.scale : 1.0
        smooth: false
        anchors.centerIn: parent
    }
}
