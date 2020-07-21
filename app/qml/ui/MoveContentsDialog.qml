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
import QtQuick.Layouts
import QtQuick.Controls

import App

Dialog {
    id: root
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

    onAccepted: project.moveContents(xDistanceSpinBox.value, yDistanceSpinBox.value, onlyMoveVisibleLayersCheckBox.checked)

    contentItem: ColumnLayout {
        GridLayout {
            columns: 2

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
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()
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
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()
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
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()
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
                        x: Math.ceil((xDistanceSpinBox.value * widthScale) - parent.x)
                        y: Math.ceil((yDistanceSpinBox.value * heightScale) - parent.y)
                        width: imagePreviewBackground.width
                        height: imagePreviewBackground.height
                        fillMode: Image.PreserveAspectFit
                        smooth: false
                        // Ensure that we get up-to-date images from the provider.
                        cache: false

                        // The image is scaled to fit the viewport (imagePreviewBackground),
                        // so our x and y position must be scaled to account for that.
                        readonly property int widthScale: paintedWidth / implicitWidth
                        readonly property int heightScale: paintedHeight / implicitHeight
                    }
                }
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "moveContentsDialogOkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: "moveContentsDialogCancelButton"
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
