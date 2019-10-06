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

import QtQuick 2.12

import "." as Screenshot

Item {
    id: root
    objectName: "deleteMe"

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "swatchesPanelHideShowToolButton")
        anchors.right: parent.left
        anchors.verticalCenter: parent.verticalCenter
        number: 1
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "autoSwatchGridView")
        anchors.centerIn: parent
        number: 2
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "swatchGridView")
        anchors.centerIn: parent
        number: 3
    }

    Screenshot.Marker {
        parent: findChildHelper.findAnyChild(root.parent, "newSwatchColourButton")
        anchors.left: parent.right
        anchors.verticalCenter: parent.verticalCenter
        number: 4
    }
}
