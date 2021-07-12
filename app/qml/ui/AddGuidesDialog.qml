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

import App 1.0

Dialog {
    id: root
    objectName: "addGuidesDialog"
    title: qsTr("Add guides")
    modal: true
    dim: false
    focus: true

    property Project project
    property ImageCanvas canvas

    onAboutToShow: {
        if (project)
            horizontalSpacingSpinBox.contentItem.forceActiveFocus()
    }

    onClosed: canvas.forceActiveFocus()

    onAccepted: canvas.addNewGuides(horizontalSpacingSpinBox.value, verticalSpacingSpinBox.value)

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
                value: 32
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
                value: 32
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
