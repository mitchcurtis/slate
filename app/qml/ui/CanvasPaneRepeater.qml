import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import App 1.0

// For access to Theme Singleton
import "."

Repeater {
    id: root
    objectName: "canvasPaneRepeater"
    model: 2

    property ImageCanvas canvas
    readonly property Project project: canvas.project

    /*
        The stacking order for child items of ImageCanvas is as follows:

         +--------------------------+
         | ImageCanvas (parent)     |
         |   +--------------------------+
         |   | Second Pane              |----------|  Checkered transparency pixmap,
         |   |   +--------------------------+      |  canvas image content, selection.
         |   |   | First Pane               |------|
         |   |   |   +--------------------------+
         |   |   |   | Guides                   |
         |   |   |   |   +--------------------------+
         |   |   |   |   | Selection Item           |
         |   |   |   |   |   +--------------------------+
         +---|   |   |   |   | Selection Cursor Guide   |
             |   |   |   |   |   +--------------------------+
             | 1 |   |   |   |   | First Horizontal Ruler   |
             +---|   |   |   |   |   +--------------------------+
                 | 2 |   |   |   |   | First Vertical Ruler     |
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

        readonly property bool isFirstPane: paneIndex === 0
        readonly property string indexAsWord: isFirstPane ? "first" : "second"

        Rectangle {
            width: Math.floor(paneItem.pane.size * parent.width)
            height: parent.height
            color: Theme.canvasBackgroundColour
            z: -1
        }

        GuidesItem {
            id: guidesItem
            anchors.fill: parent
            canvas: root.canvas
            pane: paneItem.pane
            paneIndex: paneItem.paneIndex
            visible: root.canvas.guidesVisible
        }

        SelectionItem {
            anchors.fill: parent
            canvas: root.canvas
            pane: paneItem.pane
            paneIndex: paneItem.paneIndex
            visible: root.canvas.hasSelection
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

        Rectangle {
            anchors.fill: guidesItem
            color: "transparent"
            border.color: "darkorange"
            z: 1000
        }

        Repeater {
            id: animationFrameTextRepeater

            readonly property bool animationAvailable:
                root.project && !!root.project.usingAnimation && root.project.animationSystem.currentAnimation
            readonly property Animation currentAnimation: root.project ? root.project.animationSystem.currentAnimation : null
            readonly property bool showMarkers: currentAnimation && canvas.animationMarkersVisible

            readonly property int rows: currentAnimation ? project.size.height / currentAnimation.frameHeight : 0
            readonly property int columns: currentAnimation ? project.size.width / currentAnimation.frameWidth : 0
            readonly property int frameWidth: currentAnimation ? currentAnimation.frameWidth : 0
            readonly property int frameHeight: currentAnimation ? currentAnimation.frameHeight : 0

            // At smaller zoom levels we're short on space, so use a smaller padding.
            readonly property int labelLeftPadding: 2 + Math.min(paneItem.pane.integerZoomLevel, 8)
            readonly property int labelTopPadding: Math.min(paneItem.pane.integerZoomLevel, 8)
            // Same goes for font size. Go from the default font size to max 50% larger.
            // At a zoom level of 1, multiply by 0; at 2, multiply by 1.1; etc.
            readonly property int labelFontSize: Qt.application.font.pixelSize * (1.0 + Math.min((paneItem.pane.integerZoomLevel - 1) * 0.1, 0.5))

            model: showMarkers ? currentAnimation.frameCount : 0
            delegate: Label {
                id: label
                x: paneItem.pane.integerOffset.x + relativeX
                y: paneItem.pane.integerOffset.y + relativeY
                z: 1000
                width: animationFrameTextRepeater.frameWidth
                height: animationFrameTextRepeater.frameHeight
                text: modelData + 1
                font.pixelSize: animationFrameTextRepeater.labelFontSize
                color: "white"
                leftPadding: animationFrameTextRepeater.labelLeftPadding
                topPadding: animationFrameTextRepeater.labelTopPadding
                opacity: !hoverHandler.hovered ? 1 : 0
                visible: opacity > 0 && visibleInPane

                // By using a Repeater instead of GridLayout, we can completely hide
                // items that are not visible, instead of just setting their opacity to 0
                // (which we did in order to avoid the item after the one that was hidden shuffling around).
                // It just means we have to have these extra position bindings.
                readonly property int paneX: paneItem.paneIndex == 1 ? paneItem.pane.size * paneItem.width : 0
                readonly property int relativeX: column * (animationFrameTextRepeater.frameWidth * paneItem.pane.integerZoomLevel)
                readonly property int relativeY: row * (animationFrameTextRepeater.frameHeight * paneItem.pane.integerZoomLevel)
                readonly property int frameIndex: animationFrameTextRepeater.currentAnimation
                    ? animationFrameTextRepeater.currentAnimation.startIndex(root.project.size.width) + index : -1
                readonly property int row: frameIndex / animationFrameTextRepeater.columns
                readonly property int column: frameIndex % animationFrameTextRepeater.columns
                readonly property bool visibleInPane: {
                    paneItem.width; paneItem.height; // TODO: fix bug where it's not rendered at startup.
                    paneItem.isRectVisible(Qt.rect(x, y, shadeRect.width, shadeRect.height))
                }

                Behavior on opacity {
                    NumberAnimation {
                        duration: 140
                        easing.type: Easing.InOutQuad
                    }
                }

                Rectangle {
                    id: shadeRect
                    color: "#aa444444"
                    width: label.implicitWidth + label.leftPadding
                    height: label.implicitHeight + label.topPadding
                    z: -1

                    // TODO: verify that the bug where clicking on the handler keeps it hovered is fixed in dev
                    HoverHandler {
                        id: hoverHandler
                    }
                }
            }
        }
    }
}
