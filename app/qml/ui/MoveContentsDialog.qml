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

import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

import App 1.0

Dialog {
    id: dialog
    objectName: "moveContentsDialog"
    title: qsTr("Move layer contents")
    modal: true
    dim: false
    focus: true
    enabled: isLayeredImageProject

    property Project project
    property bool isLayeredImageProject: project && project.type === Project.LayeredImageType

    onAboutToShow: {
        if (project) {
            xDistanceSpinBox.value = 0
            yDistanceSpinBox.value = 0
            xDistanceSpinBox.contentItem.forceActiveFocus()
            imagePreview.source = "image://project"
        }
    }

    onAboutToHide: {
        // Ensure that an up-to-date image is assign when we open by clearing it when we close.
        imagePreview.source = ""
    }

    contentItem: ColumnLayout {
        GridLayout {
            columns: 2

            // TODO: necessary?
            Layout.topMargin: 4

            Label {
                text: qsTr("Horizontal distance")
                Layout.fillWidth: true
            }

            SpinBox {
                id: xDistanceSpinBox
                objectName: "moveContentsXSpinBox"
                from: -10000
                to: 10000
                editable: true
                stepSize: 1

                Layout.fillWidth: true

                ToolTip.text: qsTr("Horizontal distance to move the contents by (in pixels)")
                ToolTip.visible: hovered
            }

            Label {
                text: qsTr("Vertical distance")
                Layout.fillWidth: true
            }

            SpinBox {
                id: yDistanceSpinBox
                objectName: "moveContentsYSpinBox"
                from: -10000
                to: 10000
                editable: true
                stepSize: 1

                Layout.fillWidth: true

                ToolTip.text: qsTr("Vertical distance to move the contents by (in pixels)")
                ToolTip.visible: hovered
            }

            Label {
                text: qsTr("Only move visible layers")
                Layout.fillWidth: true
            }

            CheckBox {
                id: onlyMoveVisibleLayersCheckBox
                objectName: "onlyMoveVisibleLayersCheckBox"
                checked: true

                ToolTip.text: qsTr("Only move contents of visible layers")
                ToolTip.visible: hovered
            }

            Rectangle {
                id: imagePreviewBackground
                objectName: "imagePreviewBackground"
                color: "black"
                clip: true

                Layout.columnSpan: 2
                Layout.preferredWidth: 400
                Layout.preferredHeight: 400
                Layout.fillWidth: true
                Layout.fillHeight: true

                Image {
                    objectName: "imagePreviewOldBounds"
                    // QQuickImage uses ceil.
                    x: Math.ceil((parent.width - imagePreview.paintedWidth) / 2)
                    y: Math.ceil((parent.height - imagePreview.paintedHeight) / 2)
                    width: imagePreview.paintedWidth
                    height: imagePreview.paintedHeight
                    source: "qrc:/images/checker.png"
                    fillMode: Image.Tile
                    smooth: false
                    clip: true

                    // We want the project's image to be clipped by its old bounds,
                    // as that's what it will actually look like after moving.
                    // That's why it's a child of the old bounds image.
                    Image {
                        id: imagePreview
                        // Our parent has to be positioned correctly based on our painted size,
                        // which means we must deduct their position to get ours.
                        x: Math.ceil(xDistanceSpinBox.value) - parent.x
                        y: Math.ceil(yDistanceSpinBox.value) - parent.y
                        width: imagePreviewBackground.width
                        height: imagePreviewBackground.height
                        fillMode: Image.PreserveAspectFit
                        smooth: false
                        // Ensure that we get up-to-date images from the provider.
                        cache: false
                    }
                }
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "moveContentsDialogOkButton"
            text: qsTr("OK")

            onClicked: {
                project.moveContents(xDistanceSpinBox.value, yDistanceSpinBox.value, onlyMoveVisibleLayersCheckBox.checked)
                dialog.visible = false
            }
        }
        Button {
            objectName: "moveContentsDialogCancelButton"
            text: qsTr("Cancel")

            onClicked: dialog.visible = false
        }
    }
}
