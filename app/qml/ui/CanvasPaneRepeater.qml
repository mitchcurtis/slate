import QtQuick 2.12

import App 1.0

// For access to CanvasColours Singleton
import "."

Repeater {
    id: root
    model: 2

    property ImageCanvas canvas

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
         +---|   |   |   | Selection Item
             |   |   |   |   +--------------------------+
             | 1 |   |   |   | Selection Cursor Guide   |
             +---|   |   |   |   +--------------------------+
                 | 2 |   |   |   | First Horizontal Ruler   |
                 +---|   |   |   |   +--------------------------+
                     | 3 |   |   |   | First Vertical Ruler     |
                     +---|   |   |   |   +--------------------------+
                         | 4 |   |   |   | Second Horizontal Ruler  |
                         +---|   |   |   |   +--------------------------+
                             | 5 |   |   |   | Second Vertical Ruler    |
                             +---|   |   |   |                          |
                                 | 6 |   |   |                          |
                                 +---|   |   |                          |
                                     | 7 |   |                          |
                                     +---|   |                          |
                                         | 8 |                          |
                                         +---|                          |
                                             | 9                        |
                                             +--------------------------+


        Each item is a direct child of ImageCanvas.

        The Z value of each child is shown in the lower left corner.
    */
    
    CanvasPaneItem {
        id: paneItem
        objectName: canvas.objectName + "PaneItem" + index
        canvas: root.canvas
        pane: canvas.paneAt(index)
        paneIndex: index
        anchors.fill: parent
        visible: index === 0 || canvas.splitScreen
        
        Rectangle {
            x: index === 0 ? 0 : Math.floor(parent.width - width)
            width: Math.floor(paneItem.pane.size * parent.width)
            height: parent.height
            color: CanvasColours.backgroundColour
            z: -1
        }
    }
}
