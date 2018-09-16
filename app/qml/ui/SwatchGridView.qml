import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import App 1.0

GridView {
    id: root
    boundsBehavior: ListView.StopAtBounds
    visible: project && project.loaded
    clip: true
    cellWidth: 16
    cellHeight: 16

    property Menu swatchContextMenu
    property bool readOnly

    delegate: Rectangle {
        id: colourDelegate
        width: 16
        height: 16
        color: model.colour

        // TODO: dragging seems broken on Mac
        /*Drag.active: mouseArea.drag.active
        Drag.dragType: Drag.Automatic
        Drag.onActiveChanged: {
            print("Drag.active changed to", Drag.active)
//                    if (Drag.active)
//                        Drag.startDrag()
        }
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2
        Drag.onDragStarted: {
            print("drag started")
            draggedSwatchIndex = index
        }
        Drag.onDragFinished: {
            print("drag finished")
            draggedSwatchIndex = -1
        }

        states: [
            State {
                when: !mouseArea.drag.active
                ParentChange { target: colourDelegate; parent: autoSwatchGridView }
                PropertyChanges {
                    target: colourDelegate
                    x: 0
                    y: 0
                }
            },
            State {
                when: mouseArea.drag.active
                ParentChange { target: colourDelegate; parent: root }
            }
        ]*/

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#444444"
        }

        MouseArea {
            id: mouseArea
            hoverEnabled: true
            acceptedButtons: readOnly ? Qt.LeftButton : Qt.LeftButton | Qt.RightButton
            anchors.fill: parent
//            drag.target: parent

            onClicked: {
                if (mouse.button === Qt.LeftButton) {
                    canvas.penForegroundColour = model.colour
                } else if (mouse.button === Qt.RightButton) {
                    swatchContextMenu.rightClickedColourIndex = index
                    swatchContextMenu.rightClickedColourName = model.name
                    swatchContextMenu.rightClickedColourX = colourDelegate.x
                    swatchContextMenu.rightClickedColourY = colourDelegate.y
                    swatchContextMenu.open()
                }
            }
        }

        ToolTip.text: readOnly || model.name.length === 0
            ? model.colour : model.name + " (" + model.colour + ")"
        ToolTip.visible: mouseArea.containsMouse
        ToolTip.delay: toolTipDelay
    }
}
