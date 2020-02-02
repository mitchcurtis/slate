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

import "." as Screenshot

Item {
    id: root
    objectName: "deleteMe"

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "animationPanelSettingsToolButton")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        number: 1
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "animationPanelHideShowToolButton")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        number: 2
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "animationPanelFlickable")
        anchors.centerIn: parent
        number: 3
    }

    Screenshot.Marker {
        id: playPauseMarker
        parent: findChildHelper.findAnyChild(root.parent, "animationPlayPauseButton")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.top
        number: 4
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "animationProgressBar")
        anchors.horizontalCenter: parent.horizontalCenter
//        anchors.bottom: playPauseMarker.bottom
        anchors.bottom: parent.top
        anchors.bottomMargin: 12
        number: 5
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "animationLoopButton")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.top
        number: 6
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "exportGifButton")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.top
        number: 7
    }
}
