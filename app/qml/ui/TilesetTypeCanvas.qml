import App 1.0

// For access to CanvasColours Singleton
import "."

TileCanvas {
    id: tileCanvas
    objectName: "tileCanvas"
    focus: true
    project: loader.item === tileCanvas ? canvasContainer.project : null
    backgroundColour: CanvasColours.backgroundColour
    gridVisible: settings.gridVisible
    gridColour: "#55000000"
    splitScreen: settings.splitScreen
    splitColour: CanvasColours.splitColour
    splitter.enabled: settings.splitScreen && !settings.splitterLocked
    splitter.width: 32
    scrollZoom: settings.scrollZoom
    anchors.fill: parent

    readonly property int currentPaneZoomLevel: tileCanvas.currentPane ? tileCanvas.currentPane.zoomLevel : 1
    readonly property point currentPaneOffset: tileCanvas.currentPane ? tileCanvas.currentPane.offset : Qt.point(0, 0)
    readonly property bool useCrosshairCursor: tileCanvas.mode === TileCanvas.TileMode
        || tileCanvas.tool === TileCanvas.SelectionTool || (tileCanvas.toolSize < 4 && tileCanvas.currentPaneZoomLevel <= 3)
    readonly property bool useIconCursor: tileCanvas.tool === TileCanvas.EyeDropperTool

    onErrorOccurred: errorPopup.showError(errorMessage)

    // TODO: tile pen preview shouldn't be visible with colour picker open
}
