/*
    Copyright 2018, Mitch Curtis

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

import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3

import App 1.0

Dialog {
    id: popup
    objectName: "canvasSizePopup"
    title: qsTr("Choose a size for the canvas")
    modal: true
    dim: false
    focus: true

    property Project project
    property ImageCanvas canvas
    readonly property bool isTilesetProject: project && project.type === Project.TilesetType

    onAboutToShow: {
        if (project) {
            widthSpinBox.value = project.size.width
            heightSpinBox.value = project.size.height
            widthSpinBox.contentItem.forceActiveFocus()
        }
    }

    onClosed: canvas.forceActiveFocus()

    contentItem: ColumnLayout {
        Item {
            Layout.preferredHeight: 4
        }

        GridLayout {
            columns: 2

            Label {
                text: qsTr("Canvas width")
            }

            SpinBox {
                id: widthSpinBox
                objectName: "changeCanvasWidthSpinBox"
                from: 1
                to: isTilesetProject ? 1024 : 4096
                editable: true
                stepSize: 1

                ToolTip.text: isTilesetProject ? tilesetText : imageText

                readonly property string tilesetText: qsTr("Number of horizontal tiles")
                readonly property string imageText: qsTr("Canvas width in pixels")
            }
            Label {
                text: qsTr("Canvas height")
            }

            SpinBox {
                id: heightSpinBox
                objectName: "changeCanvasHeightSpinBox"
                from: 1
                to: isTilesetProject ? 1024 : 4096
                editable: true
                stepSize: 1

                ToolTip.text: isTilesetProject ? tilesetText : imageText

                readonly property string tilesetText: qsTr("Number of vertical tiles")
                readonly property string imageText: qsTr("Canvas height in pixels")
            }

            Item {
                Layout.minimumHeight: 10
                Layout.columnSpan: 2
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "canvasSizePopupOkButton"
            text: "OK"

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

            onClicked: {
                project.size = Qt.size(widthSpinBox.value, heightSpinBox.value);
                popup.visible = false;
            }
        }
        Button {
            objectName: "canvasSizePopupCancelButton"
            text: "Cancel"

            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole

            onClicked: popup.visible = false
        }
    }
}
