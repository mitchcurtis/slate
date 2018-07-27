import QtQuick 2.11

import App 1.0

// For access to CanvasColours Singleton
import "."

Repeater {
    id: root
    model: 2

    property ImageCanvas canvas
    
    CanvasPaneItem {
        id: paneItem
        objectName: canvas.objectName + "PaneItem" + index
        canvas: root.canvas
        pane: canvas.paneAt(index)
        paneIndex: index
        anchors.fill: parent
        visible: index === 0 || canvas.splitScreen
        // We want the first pane to be painted on top of the second.
        z: 2 - index
        
        Rectangle {
            x: index === 0 ? 0 : Math.floor(parent.width - width)
            width: Math.floor(paneItem.pane.size * parent.width)
            height: parent.height
            color: CanvasColours.backgroundColour
            z: -1
        }
    }
}
