import QtQuick 2.11

import App 1.0

// For access to CanvasColours Singleton
import "."

LayeredImageCanvas {
    id: layeredCanvas
    objectName: "layeredImageCanvas"
    focus: true
    project: args.project
    backgroundColour: CanvasColours.backgroundColour
    checkerColour1: settings.checkerColour1
    checkerColour2: settings.checkerColour2
    gridVisible: settings.gridVisible
    gridColour: "#55000000"
    rulersVisible: settings.rulersVisible
    guidesVisible: settings.guidesVisible
    guidesLocked: settings.guidesLocked
    splitColour: CanvasColours.splitColour
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

    CanvasPaneItem {
        id: secondPane
        objectName: "secondLayeredImageCanvasPaneItem"
        canvas: layeredCanvas
        pane: layeredCanvas.secondPane
        paneIndex: 1
        anchors.fill: parent
        visible: layeredCanvas.splitScreen

        Rectangle {
            x: parent.width - width
            width: layeredCanvas.secondPane.size * parent.width
            height: parent.height
            color: CanvasColours.backgroundColour
            z: -1
        }

//        Repeater {
//            model: GuideModel {
//                project: layeredCanvas.project
//            }
//            delegate: Rectangle {
//                x: (secondPane.pane.offset.x + model.xPosition) * secondPane.pane.integerZoomLevel
//                y: (secondPane.pane.offset.y + model.yPosition) * secondPane.pane.integerZoomLevel
//                width: model.orientation === Qt.Vertical ? 1 : parent.width
//                height: model.orientation === Qt.Horizontal ? parent.height : 1
//                color: "cyan"
//                visible: {
//                    // force evaluation
//                    secondPane.pane.offset
//                    // TODO: doesn't work, but not that important
//                    layeredCanvas.isGuideVisibleInPane(secondPane.paneIndex, index)
//                }
//            }
//        }
    }

    CanvasPaneItem {
        id: firstPane
        objectName: "firstLayeredImageCanvasPaneItem"
        canvas: layeredCanvas
        pane: layeredCanvas.firstPane
        paneIndex: 0
        anchors.fill: parent

        Rectangle {
            width: layeredCanvas.firstPane.size * parent.width
            height: parent.height
            color: CanvasColours.backgroundColour
            z: -1
        }
    }

    Rectangle {
        x: layeredCanvas.firstPane.size * parent.width
        width: 1
        height: parent.height
        color: CanvasColours.splitColour
    }
}
