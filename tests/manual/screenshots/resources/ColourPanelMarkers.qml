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

import QtQuick 2.14

import "." as Screenshot

Item {
    id: root
    objectName: "deleteMe"

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "colourPanelHideShowToolButton")
        anchors.right: parent.left
        anchors.verticalCenter: parent.verticalCenter
        number: 1
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "colourPanelHeaderTitleLabel")
        anchors.right: parent.right
        anchors.top: parent.bottom
        anchors.topMargin: 4
        number: 2
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "hexTextField")
        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        number: 3
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "hueSlider")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        number: 4
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "saturationLightnessPicker")
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        number: 5
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "opacitySlider")
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        number: 6
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "darkerButton")
        anchors.left: parent.right
        anchors.leftMargin: -4
        anchors.verticalCenter: parent.verticalCenter
        number: 7
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "lighterButton")
        anchors.left: parent.right
        anchors.leftMargin: -4
        anchors.verticalCenter: parent.verticalCenter
        number: 8
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "desaturateButton")
        anchors.left: parent.right
        anchors.leftMargin: -4
        anchors.verticalCenter: parent.verticalCenter
        number: 9
        small: true
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "saturateButton")
        anchors.left: parent.right
        anchors.leftMargin: -4
        anchors.verticalCenter: parent.verticalCenter
        number: 10
        small: true
    }
}
