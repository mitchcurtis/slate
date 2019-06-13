import QtQuick 2.12

import App 1.0

// For access to Theme Singleton
import "."

ImageCanvas {
    id: imageCanvas
    objectName: "imageCanvas"
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

    readonly property int currentPaneZoomLevel: imageCanvas.currentPane ? imageCanvas.currentPane.integerZoomLevel : 1
    readonly property point currentPaneOffset: imageCanvas.currentPane ? imageCanvas.currentPane.integerOffset : Qt.point(0, 0)
    readonly property bool useCrosshairCursor: imageCanvas.tool === TileCanvas.SelectionTool
        || (imageCanvas.toolSize < 4 && imageCanvas.currentPaneZoomLevel <= 3)
    readonly property bool useIconCursor: imageCanvas.tool === TileCanvas.EyeDropperTool

    onErrorOccurred: errorPopup.showError(errorMessage)

    CanvasPaneRepeater {
        canvas: imageCanvas
    }

    SplitterBar {
        canvas: imageCanvas
    }
}
