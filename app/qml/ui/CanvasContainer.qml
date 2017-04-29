import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import App 1.0

Item {
    id: canvasContainer
    objectName: "canvasContainer"

    property Project project
    property ImageCanvas canvas: loader.item
    property var checkedToolButton

    FontMetrics {
        id: fontMetrics
    }

    TextMetrics {
        id: cursorMaxTextMetrics
        font.pixelSize: fontMetrics.font.pixelSize
        text: "999, 999"
    }

    Loader {
        id: loader
        objectName: "canvasContainerLoader"
        sourceComponent: project && project.typeString.length > 0 ? componentMap[project.typeString] : null
        focus: true
        anchors.fill: parent
    }

    property var componentMap: {
        "ImageType": imageCanvasComponent,
        "TilesetType": tileCanvasComponent
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

    Pane {
        id: statusBarPane
        z: 1
        objectName: "statusBarPane"
        width: parent.width
        contentHeight: statusBarLayout.implicitHeight
        anchors.bottom: parent.bottom

        Rectangle {
            parent: statusBarPane.background
            width: parent.width
            height: 1
            color: "#444"
        }

        Rectangle {
            parent: statusBarPane.background
            x: parent.width
            width: 1
            height: parent.height
            color: "#444"
        }

        RowLayout {
            id: statusBarLayout
            width: canvas ? canvas.firstPane.size * canvas.width - statusBarPane.padding * 2 : parent.width
            visible: project && canvas && project.loaded
            anchors.verticalCenter: parent.verticalCenter

            Label {
                id: pointerIconLabel
                text: "\uf245"
                font.family: "FontAwesome"
                font.pixelSize: fontMetrics.font.pixelSize * 1.2
                horizontalAlignment: Label.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter

                Layout.preferredWidth: Math.max(26, implicitWidth)
            }

            Label {
                objectName: "cursorTilePixelPosLabel"
                text: {
                    if (!canvas)
                        return "-1, -1";

                    if (canvas.hasOwnProperty("cursorTilePixelX"))
                        return canvas.cursorTilePixelX + ", " + canvas.cursorTilePixelY;

                    return canvas.cursorSceneX + ", " + canvas.cursorSceneY;
                }
                Layout.preferredWidth: Math.max(cursorMaxTextMetrics.width, implicitWidth)
                anchors.verticalCenter: parent.verticalCenter
            }

            Item {
                Layout.fillWidth: true
            }

            ZoomIndicator {
                objectName: "firstPaneZoomIndicator"
                pane: canvas ? canvas.firstPane : null
                fontMetrics: fontMetrics
            }
        }

        ZoomIndicator {
            objectName: "secondPaneZoomIndicator"
            z: 1
            anchors.right: parent.right
            visible: project && canvas && project.loaded && canvas.splitScreen
            pane: canvas ? canvas.secondPane : null
            fontMetrics: fontMetrics
        }
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
