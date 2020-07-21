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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Label {
    id: root
    objectName: "fpsCounter"
    text: qsTr("%1 FPS").arg(root.fps)

    property int fps: 0

    Item {
        id: rotationItem
        objectName: "fpsCounterRotationItem"

        property int timesUpdated: 0

        NumberAnimation on rotation {
            from: 0
            to: 360
            duration: 500
            loops: Animation.Infinite
        }

        onRotationChanged: ++timesUpdated
    }

    Timer {
        interval: 1000
        repeat: true
        running: root.visible
        onTriggered: {
            root.fps = rotationItem.timesUpdated / (interval / 1000)
            rotationItem.timesUpdated = 0
        }
    }
}
