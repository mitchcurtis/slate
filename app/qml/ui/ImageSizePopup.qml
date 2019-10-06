/*
    Copyright 2019, Mitch Curtis

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

import QtQuick 2.12
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import App 1.0

Dialog {
    id: root
    objectName: "imageSizePopup"
    title: qsTr("Choose a size for the image")
    modal: true
    dim: false
    focus: true

    property Project project

    onVisibleChanged: {
        if (visible && project) {
            widthSpinBox.value = project.size.width;
            heightSpinBox.value = project.size.height;
            smoothCheckBox.checked = false;
            widthSpinBox.contentItem.forceActiveFocus();
        }
    }

    onAccepted: project.resize(widthSpinBox.value, heightSpinBox.value, smoothCheckBox.checked)

    contentItem: ColumnLayout {
        Item {
            Layout.preferredHeight: 4
        }

        GridLayout {
            columns: 3

            Item {
                id: preserveContainer

                Layout.row: 0
                Layout.rowSpan: 2
                Layout.column: 0
                Layout.preferredWidth: preserveAspectRatioButton.implicitWidth
                Layout.fillHeight: true

                Rectangle {
                    width: 1
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: preserveContainer.height / 4
                    anchors.bottom: parent.verticalCenter
                    anchors.bottomMargin: (preserveAspectRatioButton.height - 16) / 2 + 4

                    Rectangle {
                        width: preserveAspectRatioButton.width / 2 - 6
                        height: 1
                        anchors.left: parent.left
                    }
                }

                Rectangle {
                    width: 1
                    anchors.top: parent.verticalCenter
                    anchors.topMargin: (preserveAspectRatioButton.height - 16) / 2 + 4
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: preserveContainer.height / 4
                    anchors.horizontalCenter: parent.horizontalCenter

                    Rectangle {
                        width: preserveAspectRatioButton.width / 2 - 6
                        height: 1
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                    }
                }

                IconToolButton {
                    id: preserveAspectRatioButton
                    objectName: "preserveAspectRatioButton"
                    text: "\uf0c1"
                    checked: true
                    topPadding: 0
                    bottomPadding: 0
                    anchors.verticalCenter: parent.verticalCenter

                    //: Keep the aspect ratio (width to height) when resizing the image.
                    ToolTip.text: qsTr("Preserve aspect ratio")
                    ToolTip.visible: hovered
                    ToolTip.delay: UiConstants.toolTipDelay
                    ToolTip.timeout: UiConstants.toolTipTimeout
                }
            }

            Label {
                text: qsTr("Image width")
                Layout.row: 0
                Layout.column: 1
            }

            SpinBox {
                id: widthSpinBox
                objectName: "changeImageWidthSpinBox"
                from: 1
                to: 5120
                editable: true
                stepSize: 1

                Layout.row: 0
                Layout.column: 2

                ToolTip.text: qsTr("Image width in pixels")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: valueUpdated(value)

                // https://bugreports.qt.io/browse/QTBUG-64151
//                Connections {
//                    target: widthSpinBox.contentItem
//                    onTextEdited: widthSpinBox.valueUpdated(widthSpinBox.valueFromText(widthSpinBox.contentItem.text, widthSpinBox.locale))
//                }

                function valueUpdated(value) {
                    if (preserveAspectRatioButton.checked) {
                        var aspectRatio = project.size.width / project.size.height
                        heightSpinBox.value = value / aspectRatio
                    }
                }
            }

            Label {
                text: qsTr("Image height")
                Layout.row: 1
                Layout.column: 1
            }

            SpinBox {
                id: heightSpinBox
                objectName: "changeImageHeightSpinBox"
                from: 1
                to: 5120
                editable: true
                stepSize: 1

                Layout.row: 1
                Layout.column: 2

                ToolTip.text: qsTr("Image height in pixels")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: valueUpdated(value)

                // https://bugreports.qt.io/browse/QTBUG-64151
//                Connections {
//                    target: heightSpinBox.contentItem
//                    onTextEdited: heightSpinBox.valueUpdated(heightSpinBox.valueFromText(heightSpinBox.contentItem.text, heightSpinBox.locale))
//                }

                function valueUpdated(value) {
                    if (preserveAspectRatioButton.checked) {
                        var aspectRatio = project.size.width / project.size.height
                        widthSpinBox.value = value * aspectRatio
                    }
                }
            }

            Label {
                text: qsTr("Smooth")
                Layout.row: 2
                Layout.column: 1
            }

            CheckBox {
                id: smoothCheckBox
                objectName: "smoothCheckBox"

                Layout.row: 2
                Layout.column: 2

                ToolTip.text: qsTr("Resize smoothly using bilinear filtering")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout
            }

            Item {
                Layout.row: 2
                Layout.rowSpan: 2
                Layout.column: 0
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "imageSizePopupOkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: "imageSizePopupCancelButton"
            text: qsTr("Cancel")

            // https://bugreports.qt.io/browse/QTBUG-67168
            // TODO: replace this with DestructiveRole when it works (closes dialog)
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
