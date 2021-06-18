/*
    Copyright 2020, Mitch Curtis

    This file is part of Slate.

    Slate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slate. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import App

GridView {
    id: root
    boundsBehavior: ListView.StopAtBounds
    visible: project && project.loaded
    clip: true
    cellWidth: 16
    cellHeight: 16

    property Menu swatchContextMenu
    property bool readOnly

    readonly property int minimumUsefulHeight: 16

    delegate: Rectangle {
        id: colourDelegate
        objectName: root.objectName + "_Delegate_" + index + "_" + model.colourHexName
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

            onClicked: (mouseEvent) => {
                if (mouseEvent.button === Qt.LeftButton) {
                    canvas.penForegroundColour = model.colour
                } else if (mouseEvent.button === Qt.RightButton) {
                    swatchContextMenu.rightClickedColourIndex = index
                    swatchContextMenu.rightClickedColourName = model.name
                    swatchContextMenu.x = colourDelegate.x
                    swatchContextMenu.y = colourDelegate.y + colourDelegate.height
                    swatchContextMenu.open()
                }
            }
        }

        ToolTip.text: readOnly || model.name.length === 0
            ? model.colour : model.name + " (" + model.colour + ")"
        ToolTip.visible: mouseArea.containsMouse
        ToolTip.delay: UiConstants.toolTipDelay
        ToolTip.timeout: UiConstants.toolTipTimeout
    }
}
