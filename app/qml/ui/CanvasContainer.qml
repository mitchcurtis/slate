import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import App 1.0

Item {
    id: canvasContainer
    objectName: "canvasContainer"

    property ProjectManager projectManager
    property Project project: projectManager.project
    property ImageCanvas canvas: loader.item
    property var checkedToolButton
    property FontMetrics fontMetrics

    Loader {
        id: loader
        objectName: "canvasContainerLoader"
        source: project && project.typeString.length > 0 ? project.typeString + "Canvas.qml" : ""
        focus: true
        anchors.fill: parent

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
        visible: canvas && canvas.hasBlankCursor && !canvas.useIconCursor && canvas.useCrosshairCursor
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
        text: visible && checkedToolButton && checkedToolButton.hasOwnProperty("iconText") ? checkedToolButton.iconText : ""
        font.family: "FontAwesome"
        color: "#ccc"
    }

    StatusBar {
        id: statusBar
        z: 1
        width: parent.width
        anchors.bottom: parent.bottom
        project: canvasContainer.project
        canvas: canvasContainer.canvas
        fontMetrics: canvasContainer.fontMetrics
    }
}
