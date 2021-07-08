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
import QtQuick.Controls
import QtQuick.Layouts

import App

Item {
    id: canvasContainer
    objectName: "canvasContainer"

    property Project project: projectManager.project
    property ImageCanvas canvas: loader.item
    property var checkedToolButton

    Loader {
        id: loader
        objectName: "canvasContainerLoader"
        source: project && project.typeString.length > 0 ? project.typeString + "Canvas.qml" : ""
        focus: true
        width: parent.width
        height: parent.height - statusBar.height

        property QtObject args: QtObject {
            // We want this property to be updated before "source" above, as we need
            // the canvas to disconnect its signals to e.g. the window before the new canvas is created,
            // otherwise e.g. focus changes can cause crashes, as the code would access a dangling project pointer.
            property Project project: projectManager.ready ? canvasContainer.project : null
        }
    }

    CrosshairCursor {
        id: crosshairCursor
        x: canvas ? canvas.cursorX - width / 2 : 0
        y: canvas ? canvas.cursorY - height / 2 : 0
        z: 1
        colour: canvas ? canvas.invertedCursorPixelColour : defaultColour
        visible: canvas && canvas.hasBlankCursor && !canvas.useIconCursor && (canvas.useCrosshairCursor || settings.alwaysShowCrosshair)
    }

    RectangularCursor {
        id: rectangleCursor
        x: canvas ? Math.floor(canvas.cursorX - width / 2) : 0
        y: canvas ? Math.floor(canvas.cursorY - height / 2) : 0
        z: 1
        width: canvas ? canvas.toolSize * canvas.currentPaneZoomLevel : 0
        height: canvas ? canvas.toolSize * canvas.currentPaneZoomLevel : 0
        visible: canvas && canvas.hasBlankCursor && !canvas.useIconCursor && !canvas.useCrosshairCursor
    }

    Label {
        id: iconCursor
        x: canvas ? canvas.cursorX : 0
        y: canvas ? canvas.cursorY - height + 3 : 0
        z: 1
        visible: canvas && canvas.hasBlankCursor && canvas.useIconCursor
        text: visible && checkedToolButton ? checkedToolButton.text : ""
        font.family: "FontAwesome"
        color: canvas ? canvas.invertedCursorPixelColour : crosshairCursor.defaultColour
    }

    StatusBar {
        id: statusBar
        parent: ApplicationWindow.window.contentItem
        width: canvasContainer.width
        anchors.left: settings.panelPosition === Qt.RightEdge ? parent.left : undefined
        anchors.right: settings.panelPosition === Qt.RightEdge ? undefined : parent.right
        anchors.bottom: parent.bottom
        project: canvasContainer.project
        canvas: canvasContainer.canvas
    }
}
