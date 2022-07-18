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

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import Slate 1.0

Dialog {
    id: root
    objectName: "addGuidesDialog"
    title: qsTr("Add guides")
    modal: true
    dim: false
    focus: true

    property Project project
    property ImageCanvas canvas

    // Whenever we open the dialog, get the last values used for the spin boxes if they were set.
    // If they weren't set, just set a default value that at least results in a grid of guides.
    function setLastOrDefaultValues() {
        const lastAcceptedHorizontalSpacingValue = root.project.uiState.value("addGuidesDialogHorizontalSpacing")
        if (lastAcceptedHorizontalSpacingValue !== undefined) {
            horizontalSpacingSpinBox.value = lastAcceptedHorizontalSpacingValue
            verticalSpacingSpinBox.value = root.project.uiState.value("addGuidesDialogVerticalSpacing")
        } else {
            horizontalSpacingSpinBox.value = 32
            verticalSpacingSpinBox.value = 32
        }
    }

    onAboutToShow: {
        if (!project)
            return

        setLastOrDefaultValues()
        horizontalSpacingSpinBox.contentItem.forceActiveFocus()
    }

    onClosed: canvas.forceActiveFocus()

    onAccepted: {
        canvas.addNewGuides(horizontalSpacingSpinBox.value, verticalSpacingSpinBox.value)

        root.project.uiState.setValue("addGuidesDialogHorizontalSpacing", horizontalSpacingSpinBox.value)
        root.project.uiState.setValue("addGuidesDialogVerticalSpacing", verticalSpacingSpinBox.value)
    }

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
                text: qsTr("Horizontal spacing")
            }

            SpinBox {
                id: horizontalSpacingSpinBox
                objectName: "addGuidesHorizontalSpacingSpinBox"
                from: 1
                to: 10000
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Horizontal spacing between guides")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()
            }
            Label {
                text: qsTr("Vertical spacing")
            }

            SpinBox {
                id: verticalSpacingSpinBox
                objectName: "addGuidesVerticalSpacingSpinBox"
                from: 1
                to: 10000
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Vertical spacing between guides")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()
            }

            Item {
                Layout.minimumHeight: 10
                Layout.columnSpan: 2
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            id: okButton
            objectName: "addGuidesDialogOkButton"
            text: qsTr("OK")
            // Ensure that clicks give us focus, because otherwise, entering text into one of the spin boxes
            // and then clicking OK will result in the value changing _after_ the dialog is accepted (due to
            // the focus change). By doing this, focus will change immediately and ensure that the value is
            // correct before accepting.
            focusPolicy: Qt.StrongFocus

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: "addGuidesDialogCancelButton"
            text: qsTr("Cancel")

            // https://bugreports.qt.io/browse/QTBUG-67168
            // TODO: replace this with DestructiveRole when it works (closes dialog)
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
