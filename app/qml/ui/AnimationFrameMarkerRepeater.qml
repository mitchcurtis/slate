import QtQuick 2.12
import QtQuick.Controls 2.12

import App 1.0

Repeater {
    id: root

    property ImageCanvas canvas
    property CanvasPaneItem paneItem
    readonly property Project project: canvas.project

    readonly property bool animationAvailable:
        root.project && !!root.project.usingAnimation && root.project.animationSystem.currentAnimation
    readonly property Animation currentAnimation: animationAvailable ? root.project.animationSystem.currentAnimation : null
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
        width: root.frameWidth
        height: root.frameHeight
        text: index + 1
        font.pixelSize: root.labelFontSize
        color: "white"
        leftPadding: root.labelLeftPadding
        topPadding: root.labelTopPadding
        opacity: !hoverHandler.hovered ? 1 : 0
        visible: opacity > 0 && visibleInPane

        // By using a Repeater instead of GridLayout, we can completely hide
        // items that are not visible, instead of just setting their opacity to 0
        // (which we did in order to avoid the item after the one that was hidden shuffling around).
        // It just means we have to have these extra position bindings.
        readonly property int paneX: paneItem.paneIndex == 1 ? paneItem.pane.size * paneItem.width : 0
        readonly property int relativeX: column * (root.frameWidth * paneItem.pane.integerZoomLevel)
        readonly property int relativeY: row * (root.frameHeight * paneItem.pane.integerZoomLevel)
        required property int index
        readonly property int startIndex: root.currentAnimation ? root.currentAnimation.startIndex(root.project.size.width) : 0
        readonly property int frameIndex: root.currentAnimation ? startIndex + index : -1
        readonly property int row: frameIndex / root.columns
        readonly property int column: frameIndex % root.columns
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
            color: root.canvas.highlightedAnimationFrameIndex === label.index ? Theme.focusColour : "#aa444444"
            width: label.implicitWidth + label.leftPadding
            height: label.implicitHeight + label.topPadding
            z: -1

            Behavior on color {
                ColorAnimation {
                    duration: 140
                    easing.type: Easing.InOutQuad
                }
            }

            // TODO: verify that the bug where clicking on the handler keeps it hovered is fixed in dev
            HoverHandler {
                id: hoverHandler
            }
        }
    }
}
