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

import Slate

Dialog {
    id: root
    objectName: "moveContentsDialog"
    title: qsTr("Move layer contents")
    modal: true
    dim: false
    focus: true
    enabled: project

    property LayeredImageProject project

    function updateLivePreview() {
        project.moveContents(xDistanceSpinBox.value, yDistanceSpinBox.value, onlyMoveVisibleLayersCheckBox.checked)
    }

    onAboutToShow: {
        if (!project)
            return

        xDistanceSpinBox.value = 0
        yDistanceSpinBox.value = 0
        xDistanceSpinBox.contentItem.forceActiveFocus()

        project.beginLivePreview()
    }

    onAccepted: project.endLivePreview(LayeredImageProject.CommitModificaton)
    onRejected: project.endLivePreview(LayeredImageProject.RollbackModification)

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

                onValueModified: root.updateLivePreview()

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

                onValueModified: root.updateLivePreview()

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

                onToggled: root.updateLivePreview()

                Keys.onReturnPressed: root.accept()
            }
        }
    }

    footer: DialogButtonBox {
        DialogButton {
            objectName: "moveContentsDialogOkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        DialogButton {
            objectName: "moveContentsDialogCancelButton"
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
