import QtQuick 2.9
import QtQuick.Controls 2.1

import App 1.0

Item {
    id: canvasContainer
    objectName: "canvasContainer"

    property string projectType
    property var project
    property var canvas: loader.item
    property var checkedToolButton

    FontMetrics {
        id: fontMetrics
    }

    Loader {
        id: loader
        objectName: "canvasContainerLoader"
        sourceComponent: projectType.length > 0 ? componentMap[projectType] : null
        focus: true
        anchors.fill: parent
    }

    property var componentMap: {
        "image": imageCanvasComponent,
        "tileset": tileCanvasComponent
    }

    CrosshairCursor {
        id: crosshairCursor
        x: canvas ? canvas.cursorX - width / 2 : 0
        y: canvas ? canvas.cursorY - height / 2 : 0
        z: 1
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
        text: checkedToolButton.iconText
    }

    ZoomIndicator {
        objectName: "firstPaneZoomIndicator"
        x: canvas ? canvas.firstPane.size * canvas.width - width - 16 : 0
        z: 1
        anchors.bottom: canvasContainer.bottom
        anchors.bottomMargin: 16
        visible: project && canvas && project.loaded
        pane: canvas ? canvas.firstPane : null
        fontMetrics: fontMetrics
    }

    ZoomIndicator {
        objectName: "secondPaneZoomIndicator"
        z: 1
        anchors.right: canvasContainer.right
        anchors.bottom: canvasContainer.bottom
        anchors.margins: 16
        visible: project && canvas && project.loaded && canvas.splitScreen
        pane: canvas ? canvas.secondPane : null
        fontMetrics: fontMetrics
    }

    Component {
        id: imageCanvasComponent

        ImageCanvas {
            id: imageCanvas
            objectName: "imageCanvas"
            focus: true
            // Items in Loaders are deleteLater()'d, so we have to make sure
            // that we're still the active item before setting a project that might
            // now be something different than what we're designed for.
            project: loader.item == imageCanvas ? canvasContainer.project : null
            backgroundColour: CanvasColours.backgroundColour
            gridVisible: settings.gridVisible
            gridColour: "#55000000"
            splitScreen: settings.splitScreen
            splitter.enabled: settings.splitScreen && !settings.splitterLocked
            splitter.width: 32
            anchors.fill: parent

            readonly property int currentPaneZoomLevel: imageCanvas.currentPane ? imageCanvas.currentPane.zoomLevel : 1
            readonly property point currentPaneOffset: imageCanvas.currentPane ? imageCanvas.currentPane.offset : Qt.point(0, 0)
            readonly property bool useCrosshairCursor: imageCanvas.toolSize < 4 && imageCanvas.currentPaneZoomLevel <= 3
            readonly property bool useIconCursor: imageCanvas.tool === TileCanvas.EyeDropperTool

            onErrorOccurred: errorPopup.showError(errorMessage)
        }
    }

    Component {
        id: tileCanvasComponent

        TileCanvas {
            id: tileCanvas
            objectName: "tileCanvas"
            focus: true
            project: loader.item == tileCanvas ? canvasContainer.project : null
            backgroundColour: CanvasColours.backgroundColour
            gridVisible: settings.gridVisible
            gridColour: "#55000000"
            splitScreen: settings.splitScreen
            splitter.enabled: settings.splitScreen && !settings.splitterLocked
            splitter.width: 32
            anchors.fill: parent

            readonly property int currentPaneZoomLevel: tileCanvas.currentPane ? tileCanvas.currentPane.zoomLevel : 1
            readonly property point currentPaneOffset: tileCanvas.currentPane ? tileCanvas.currentPane.offset : Qt.point(0, 0)
            readonly property bool useCrosshairCursor: tileCanvas.mode === TileCanvas.TileMode || (tileCanvas.toolSize < 4 && tileCanvas.currentPaneZoomLevel <= 3)
            readonly property bool useIconCursor: tileCanvas.tool === TileCanvas.EyeDropperTool

            onErrorOccurred: errorPopup.showError(errorMessage)

            // TODO: tile pen preview shouldn't be visible with colour picker open
        }
    }
}
