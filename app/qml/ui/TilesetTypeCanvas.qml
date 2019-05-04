import QtQuick 2.12

import App 1.0

// For access to Theme Singleton
import "."

TileCanvas {
    id: tileCanvas
    objectName: "tileCanvas"
    focus: true
    project: args.project
    backgroundColour: Theme.canvasBackgroundColour
    checkerColour1: settings.checkerColour1
    checkerColour2: settings.checkerColour2
    gridVisible: settings.gridVisible
    gridColour: "#55000000"
    rulersVisible: settings.rulersVisible
    guidesVisible: settings.guidesVisible
    guidesLocked: settings.guidesLocked
    splitColour: Theme.splitColour
    splitter.width: 32
    rulerForegroundColour: Theme.rulerForegroundColour
    rulerBackgroundColour: Theme.rulerBackgroundColour
    scrollZoom: settings.scrollZoom
    gesturesEnabled: settings.gesturesEnabled
    penToolRightClickBehaviour: settings.penToolRightClickBehaviour
    anchors.fill: parent

    readonly property int currentPaneZoomLevel: tileCanvas.currentPane ? tileCanvas.currentPane.integerZoomLevel : 1
    readonly property point currentPaneOffset: tileCanvas.currentPane ? tileCanvas.currentPane.integerOffset : Qt.point(0, 0)
    readonly property bool useCrosshairCursor: tileCanvas.mode === TileCanvas.TileMode
        || tileCanvas.tool === TileCanvas.SelectionTool || (tileCanvas.toolSize < 4 && tileCanvas.currentPaneZoomLevel <= 3)
    readonly property bool useIconCursor: tileCanvas.tool === TileCanvas.EyeDropperTool

    onErrorOccurred: errorPopup.showError(errorMessage)

    // TODO: tile pen preview shouldn't be visible with colour picker open

    Repeater {
        id: root
        model: 2

        TileCanvasPaneItem {
            id: paneItem
            objectName: "tileCanvasPaneItem" + index
            canvas: tileCanvas
            pane: canvas.paneAt(index)
            paneIndex: index
            anchors.fill: parent
            visible: index === 0 || canvas.splitScreen

            Rectangle {
                x: index === 0 ? 0 : Math.floor(parent.width - width)
                width: Math.floor(paneItem.pane.size * parent.width)
                height: parent.height
                color: Theme.canvasBackgroundColour
                z: -1
            }
        }
    }

    SplitterBar {
        canvas: tileCanvas
    }
}
