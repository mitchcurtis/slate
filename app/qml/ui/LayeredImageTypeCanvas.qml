import App 1.0

// For access to CanvasColours Singleton
import "."

LayeredImageCanvas {
    id: layeredCanvas
    objectName: "layeredImageCanvas"
    focus: true
    project: args.project
    backgroundColour: CanvasColours.backgroundColour
    gridVisible: settings.gridVisible
    gridColour: "#55000000"
    rulersVisible: settings.rulersVisible
    guidesVisible: settings.guidesVisible
    guidesLocked: settings.guidesLocked
    splitScreen: settings.splitScreen
    splitColour: CanvasColours.splitColour
    splitter.enabled: settings.splitScreen && !settings.splitterLocked
    splitter.width: 32
    rulerForegroundColour: CanvasColours.rulerForegroundColour
    rulerBackgroundColour: CanvasColours.rulerBackgroundColour
    scrollZoom: settings.scrollZoom
    anchors.fill: parent

    readonly property int currentPaneZoomLevel: layeredCanvas.currentPane ? layeredCanvas.currentPane.integerZoomLevel : 1
    readonly property point currentPaneOffset: layeredCanvas.currentPane ? layeredCanvas.currentPane.offset : Qt.point(0, 0)
    readonly property bool useCrosshairCursor: layeredCanvas.tool === ImageCanvas.SelectionTool
        || (layeredCanvas.toolSize < 4 && layeredCanvas.currentPaneZoomLevel <= 3)
    readonly property bool useIconCursor: layeredCanvas.tool === ImageCanvas.EyeDropperTool

    onErrorOccurred: errorPopup.showError(errorMessage)
}
