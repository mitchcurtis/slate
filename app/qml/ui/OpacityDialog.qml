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

import "." as Ui

Dialog {
    id: root
    objectName: "opacityDialog"
    title: qsTr("Opacity")
    modal: true
    dim: false
    focus: true

    property Project project
    property ImageCanvas canvas

    property real hslAlpha

    TextMetrics {
        id: valueTextMetrics
        font: opacityTextField.font
        text: "-0.0001"
    }

    function modifySelectionHsl() {
        var flags = ImageCanvas.DefaultAlphaAdjustment
        if (doNotModifyFullyTransparentPixelsCheckBox.checked)
            flags |= ImageCanvas.DoNotModifyFullyTransparentPixels
        if (doNotModifyFullyOpaquePixelsCheckBox.checked)
            flags |= ImageCanvas.DoNotModifyFullyOpaquePixels
        canvas.modifySelectionHsl(0, 0, 0, hslAlpha, flags)
    }

    onAboutToShow: {
        if (project) {
            hslAlpha = 0

            opacityTextField.forceActiveFocus()

            canvas.beginModifyingSelectionHsl()
        }
    }

    onClosed: canvas.forceActiveFocus()

    onAccepted: canvas.endModifyingSelectionHsl(ImageCanvas.CommitAdjustment)

    onRejected: {
        if (canvas.adjustingImage) {
            canvas.endModifyingSelectionHsl(ImageCanvas.RollbackAdjustment)
        }
    }

    contentItem: GridLayout {
        columns: 3
        columnSpacing: 24
        rowSpacing: 0

        Label {
            text: qsTr("Opacity")

            Layout.fillWidth: true
        }
        OpacitySlider {
            id: opacitySlider
            objectName: root.objectName + "OpacitySlider"
            from: -1
            value: hslAlpha
            to: 1
            stepSize: 0.001

            ToolTip.text: qsTr("Changes the opacity of the image")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            Keys.onReturnPressed: root.accept()

            onMoved: {
                hslAlpha = value
                modifySelectionHsl()
            }
        }
        DoubleTextField {
            id: opacityTextField
            objectName: root.objectName + "OpacityTextField"
            propertySource: root
            propertyName: "hslAlpha"
            // If the maximum length matches the text metrics text, the sign
            // character will sometimes be slightly cut off, so account for that.
            maximumLength: valueTextMetrics.text.length - 1

            Layout.maximumWidth: valueTextMetrics.width
            Layout.fillWidth: true

            onAccepted: root.accept()

            // We call this here instead of just doing it in e.g. onHslHueChanged
            // because that would require us to block changes that occur when the
            // HSL property values are reset upon showing the dialog. Modifying the
            // selection only on user interaction instead feels nicer.
            onValueModified: modifySelectionHsl()
        }

        CheckBox {
            id: doNotModifyFullyTransparentPixelsCheckBox
            objectName: "doNotModifyFullyTransparentPixelsCheckBox"
            text: qsTr("Do not modify fully transparent pixels")
            checked: true

            Layout.columnSpan: 3

            ToolTip.text: qsTr("Only change the alpha if it's non-zero to prevent fully transparent pixels from gaining opacity.")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            Keys.onReturnPressed: root.accept()

            onClicked: modifySelectionHsl()
        }

        CheckBox {
            id: doNotModifyFullyOpaquePixelsCheckBox
            objectName: "doNotModifyFullyOpaquePixelsCheckBox"
            text: qsTr("Do not modify fully opaque pixels")
            checked: true

            Layout.columnSpan: 3

            ToolTip.text: qsTr("Only change the alpha if it's less than one to prevent fully opaque pixels from losing opacity.")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            Keys.onReturnPressed: root.accept()

            onClicked: modifySelectionHsl()
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "opacityDialogOkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: "opacityDialogCancelButton"
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
