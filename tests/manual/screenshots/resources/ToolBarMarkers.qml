/*
    Copyright 2018, Mitch Curtis

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

import QtQuick 2.7

import "." as Screenshot

Item {
    id: root

    Repeater {
        model: [
            "canvasSizeButton",
            "imageSizeButton",
            "undoButton",
            "redoButton",
            "modeToolButton",
            "penToolButton",
            "eyeDropperToolButton",
            "eraserToolButton",
            "fillToolButton",
            "selectionToolButton",
            "toolSizeButton",
            "rotate90CcwToolButton",
            "rotate90CwToolButton",
            "showRulersToolButton",
            "showGuidesToolButton",
            "lockGuidesToolButton",
            "splitScreenToolButton",
            "lockSplitterToolButton"
        ]
        delegate: Screenshot.Marker {
            parent: findChildHelper.findAnyChild(root.parent, modelData)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom
            anchors.topMargin: -5
            number: index + 1
            small: true
        }
    }
}
