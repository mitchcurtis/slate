/*
    Copyright 2021, Mitch Curtis

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

import Slate 1.0

Dialog {
    id: root
    objectName: "rearrangeContentsIntoGridDialog"
    title: qsTr("Rearrange contents into a grid")
    modal: true
    dim: false
    focus: true

    property Project project
    property ImageCanvas canvas

    function updateLivePreview() {
        project.rearrangeContentsIntoGrid(
            cellWidthSpinBox.value,
            cellHeightSpinBox.value,
            columnsSpinBox.value,
            rowsSpinBox.value)
    }

    // Whenever we open the dialog, get the last values used for the spin boxes if they were set.
    // If they weren't set, just set a default value that at least results in a grid.
    function setLastOrDefaultValues() {
        const lastAcceptedCellWidthValue = root.project.uiState.value("rearrangeContentsDialogCellWidth")
        if (lastAcceptedCellWidthValue !== undefined) {
            cellWidthSpinBox.value = lastAcceptedCellWidthValue
            cellHeightSpinBox.value = root.project.uiState.value("rearrangeContentsDialogCellHeight")
            columnsSpinBox.value = root.project.uiState.value("rearrangeContentsDialogColumns")
            rowsSpinBox.value = root.project.uiState.value("rearrangeContentsDialogRows")
        } else {
            cellWidthSpinBox.value = project.size.width / 2
            cellHeightSpinBox.value = project.size.height / 2
            columnsSpinBox.value = 2
            rowsSpinBox.value = 2
        }
    }

    onAboutToShow: {
        if (!project)
            return

        setLastOrDefaultValues()
        cellWidthSpinBox.contentItem.forceActiveFocus()

        project.beginLivePreview()
    }

    onAccepted: {
        project.endLivePreview(LayeredImageProject.CommitModificaton)

        root.project.uiState.setValue("rearrangeContentsDialogCellWidth", cellWidthSpinBox.value)
        root.project.uiState.setValue("rearrangeContentsDialogCellHeight", cellHeightSpinBox.value)
        root.project.uiState.setValue("rearrangeContentsDialogColumns", columnsSpinBox.value)
        root.project.uiState.setValue("rearrangeContentsDialogRows", rowsSpinBox.value)
    }

    onRejected: project.endLivePreview(LayeredImageProject.RollbackModification)

    onClosed: canvas.forceActiveFocus()

    UiStateSerialisation {
        project: root.project
        onReadyToLoad: setLastOrDefaultValues()
        // We don't use onReadyToSave, but instead store the values whenever the dialog is accepted.
    }

    contentItem: ColumnLayout {
        Item {
            Layout.preferredHeight: 4
        }

        GridLayout {
            columns: 2

            Label {
                text: qsTr("Cell width")
            }
            SpinBox {
                id: cellWidthSpinBox
                objectName: "cellWidthSpinBox"
                from: 1
                to: 1024
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Cell width in pixels")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: root.updateLivePreview()
            }

            Label {
                text: qsTr("Cell height")
            }
            SpinBox {
                id: cellHeightSpinBox
                objectName: "cellHeightSpinBox"
                from: 1
                to: 1024
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Cell height in pixels")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: root.updateLivePreview()
            }

            Label {
                text: qsTr("Columns")
            }
            SpinBox {
                id: columnsSpinBox
                objectName: "columnsSpinBox"
                from: 1
                to: 1024
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Number of columns in the grid")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: root.updateLivePreview()
            }

            Label {
                text: qsTr("Rows")
            }
            SpinBox {
                id: rowsSpinBox
                objectName: "rowsSpinBox"
                from: 1
                to: 1024
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Number of rows in the grid")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: root.updateLivePreview()
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: root.objectName + "OkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: root.objectName + "CancelButton"
            text: qsTr("Cancel")

            // https://bugreports.qt.io/browse/QTBUG-67168
            // TODO: replace this with DestructiveRole when it works (closes dialog)
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
