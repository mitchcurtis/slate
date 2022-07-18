import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Slate

// For access to Theme Singleton
import "."

Repeater {
    id: root
    objectName: "canvasPaneRepeater"
    model: 2

    property ImageCanvas canvas

    /*
        The stacking order for child items of ImageCanvas is as follows:

         +--------------------------+
         | ImageCanvas (parent)     |
         |   +--------------------------+
         |   | CanvasPaneItem N         |----------|  Checkered transparency pixmap, canvas image content, selection.
         |   |   +--------------------------+
         |   |   | Guides                   |
         |   |   |   +--------------------------+
         |   |   |   | Selection Item           |
         |   |   |   |   +--------------------------+
         +---|   |   |   | Selection Cursor Guide   |
             |   |   |   |   +--------------------------+
             | 1 |   |   |   | First Horizontal Ruler   |
             +---|   |   |   |   +--------------------------+
                 | 2 |   |   |   | First Vertical Ruler     |
                 +---|   |   |   |   |   +--------------------------+
                     | 3 |   |   |   |   | Second Horizontal Ruler  |
                     +---|   |   |   |   |   +--------------------------+
                         | 4 |   |   |   |   | Second Vertical Ruler    |
                         +---|   |   |   |   |   +--------------------------+
                             | 5 |   |   |   |   | Notes                    |
                             +---|   |   |   |   |                          |
                                 | 6 |   |   |   |                          |
                                 +---|   |   |   |                          |
                                     | 7 |   |   |                          |
                                     +---|   |   |                          |
                                         | 8 |   |                          |
                                         +---|   |                          |
                                             | 9 |                          |
                                             +---| 10                       |
                                                 |--------------------------+

        Each item is a direct child of ImageCanvas.

        The Z value of each child is shown in the lower left corner.

        When QTBUG-74320 is implemented, we could support any amount of panes
        by listening to CanvasPaneItem for changes in its children (so that we
        can e.g. keep track of all of the rulers for event handling).
    */
    
    CanvasPaneItem {
        id: paneItem
        objectName: canvas.objectName + "PaneItem" + index
        canvas: root.canvas
        pane: canvas.paneAt(index)
        paneIndex: index
        x: isFirstPane ? 0 : Math.floor(parent.width - width)
        width: Math.floor(paneItem.pane.size * parent.width)
        height: parent.height
        visible: isFirstPane || canvas.splitScreen
        clip: true

        required property int index

        readonly property bool isFirstPane: paneIndex === 0
        readonly property string indexAsWord: isFirstPane ? "first" : "second"

        Rectangle {
            width: Math.floor(paneItem.pane.size * parent.width)
            height: parent.height
            color: Theme.canvasBackgroundColour
            z: -1
        }

        GuideModel {
            id: guideModel
            project: root.canvas.project
            canvas: root.canvas
        }

        Repeater {
            model: root.canvas.guidesVisible ? guideModel : null
            delegate: Guide {
                pane: paneItem.pane
            }
        }

        // Draws a guide being dragged out from the ruler.
        Guide {
            pane: paneItem.pane
            orientation: root.canvas.pressedRuler?.orientation ?? Qt.Horizontal
            position: horizontal ? root.canvas.cursorSceneY : root.canvas.cursorSceneX
            visible: root.canvas.pressedRuler
        }

        NotesItem {
            anchors.fill: parent
            canvas: root.canvas
            pane: paneItem.pane
            paneIndex: paneItem.paneIndex
            visible: root.canvas.notesVisible
        }

        SelectionItem {
            anchors.fill: parent
            canvas: root.canvas
            pane: paneItem.pane
            paneIndex: paneItem.paneIndex
            visible: root.canvas.hasSelection
        }

        SelectionCursorGuide {
            anchors.fill: parent
            canvas: root.canvas
            pane: paneItem.pane
            paneIndex: paneItem.paneIndex
            visible: root.canvas.tool === ImageCanvas.SelectionTool && !root.canvas.hasSelection && root.canvas.containsMouse
        }

        Ruler {
            objectName: paneItem.indexAsWord + "HorizontalRuler"
            width: parent.width
            height: 20
            orientation: Qt.Horizontal
            from: paneItem.pane.integerOffset.x
            zoomLevel: paneItem.pane.integerZoomLevel
            foregroundColour: Theme.rulerForegroundColour
            backgroundColour: Theme.rulerBackgroundColour
            visible: root.canvas.rulersVisible && (isFirstPane || root.canvas.splitScreen)
        }

        Ruler {
            objectName: paneItem.indexAsWord + "VerticalRuler"
            width: 20
            height: parent.height
            orientation: Qt.Vertical
            from: paneItem.pane.integerOffset.y
            zoomLevel: paneItem.pane.integerZoomLevel
            foregroundColour: Theme.rulerForegroundColour
            backgroundColour: Theme.rulerBackgroundColour
            visible: root.canvas.rulersVisible && (isFirstPane || root.canvas.splitScreen)
        }

        AnimationFrameMarkerRepeater {
            canvas: root.canvas
            paneItem: paneItem
        }
    }
}
