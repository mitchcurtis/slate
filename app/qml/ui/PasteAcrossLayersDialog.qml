/*
    Copyright 2023, Mitch Curtis

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
    objectName: "pasteAcrossLayersDialog"
    title: qsTr("Paste across layers")
    modal: true
    dim: false
    focus: true

    property LayeredImageProject project
    property ImageCanvas canvas

    // We could check that the layer counts match in the enabled binding for our menu item,
    // but it would be hard to give feedback to the user about that, so we let them trigger it
    // and show an error message.
    function openOrError() {
        if (Clipboard.copiedLayerCount !== root.project.layerCount) {
            project.error(qsTr("Number of layers in clipboard (%1) doesn't match the current number of layers (%2).\n\n"
                + "Please Copy Across Layers again without adding or removing layers until the Paste Across Layers is complete.")
                    .arg(Clipboard.copiedLayerCount).arg(root.project.layerCount))
            return
        }

//        else if (size of project in clipboard doesn't match the current project size)
//            error message
//            return

        root.open()
    }

    function updateLivePreview() {
        project.pasteAcrossLayers(pasteXSpinBox.value, pasteYSpinBox.value, onlyPasteIntoVisibleLayersCheckBox.checked)
    }

    onAboutToShow: {
        if (!project)
            return

        canvas.finaliseSelection()

        pasteXSpinBox.value = 0
        pasteYSpinBox.value = 0
        onlyPasteIntoVisibleLayersCheckBox.checked = true

        pasteXSpinBox.contentItem.forceActiveFocus()

        project.beginLivePreview()
    }

    onAccepted: project.endLivePreview(LayeredImageProject.CommitModificaton)
    onRejected: project.endLivePreview(LayeredImageProject.RollbackModification)
    onClosed: canvas.forceActiveFocus()

    contentItem: ColumnLayout {
        Item {
            Layout.preferredHeight: 4
        }

        GridLayout {
            columns: 2

            Label {
                text: qsTr("Paste X position")
            }
            SpinBox {
                id: pasteXSpinBox
                objectName: "pasteXSpinBox"
                from: root.project ? -root.project.size.width : 0
                value: 0
                to: root.project ? root.project.size.width - 1 : 1
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("The X coordinate at which to paste")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: root.updateLivePreview()
            }

            Label {
                text: qsTr("Paste Y position")
            }
            SpinBox {
                id: pasteYSpinBox
                objectName: "pasteYSpinBox"
                from: root.project ? -root.project.size.height : 0
                value: 0
                to: root.project ? root.project.size.height - 1 : 1
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("The Y coordinate at which to paste")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onValueModified: root.updateLivePreview()
            }

            Label {
                text: qsTr("Only paste into visible layers")
                Layout.fillWidth: true
            }
            CheckBox {
                id: onlyPasteIntoVisibleLayersCheckBox
                objectName: "onlyPasteIntoVisibleLayersCheckBox"
                checked: true

                ToolTip.text: qsTr("Only paste into layers which are currently visible")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                Keys.onReturnPressed: root.accept()

                onToggled: root.updateLivePreview()
            }
        }
    }

    footer: DialogButtonBox {
        DialogButton {
            objectName: root.objectName + "OkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        DialogButton {
            objectName: root.objectName + "CancelButton"
            text: qsTr("Cancel")

            // https://bugreports.qt.io/browse/QTBUG-67168
            // TODO: replace this with DestructiveRole when it works (closes dialog)
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
