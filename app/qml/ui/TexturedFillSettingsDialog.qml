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
    objectName: "texturedFillSettingsDialog"
    title: qsTr("Textured Fill settings")
    modal: true
    dim: false
    focus: true
    enabled: canvas && project && (project.type === Project.ImageType || project.type === Project.LayeredImageType)

    property Project project
    property ImageCanvas canvas

    onAboutToShow: {
        if (project) {
            // Copy the canvas' current parameters into ours. If the user accepts the dialog,
            // we'll copy our updated ones back into canvas.
            previewItem.parameters.copy(canvas.texturedFillParameters)

//            hueVarianceCheckBox.contentItem.forceActiveFocus()
        }
    }

    contentItem: GridLayout {
        columns: 2

        Layout.topMargin: 4

        CheckBox {
            id: hueVarianceCheckBox
            objectName: "hueVarianceCheckBox"
            text: qsTr("Hue variance")
            checked: previewItem.parameters.hue.enabled

            Layout.fillWidth: true

            ToolTip.text: qsTr("Enable random variance in hue")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onToggled: previewItem.parameters.hue.enabled = checked
        }

        // https://bugreports.qt.io/browse/QTBUG-67311
        TexturedFillVarianceRangedSlider {
            id: hueVarianceSlider
            objectName: "hueVarianceSlider"
            displayName: "hue"
            parameter: previewItem.parameters.hue
            enabled: hueVarianceCheckBox.checked

            Layout.fillWidth: true
        }

        CheckBox {
            id: saturationVarianceCheckBox
            objectName: "saturationVarianceCheckBox"
            text: qsTr("Saturation variance")
            checked: previewItem.parameters.saturation.enabled

            Layout.fillWidth: true

            ToolTip.text: qsTr("Enable random variance in saturation")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onToggled: previewItem.parameters.saturation.enabled = checked
        }

        TexturedFillVarianceRangedSlider {
            id: saturationVarianceSlider
            objectName: "saturationVarianceSlider"
            displayName: "saturation"
            parameter: previewItem.parameters.saturation
            enabled: saturationVarianceCheckBox.checked

            Layout.fillWidth: true
        }

        CheckBox {
            id: lightnessVarianceCheckBox
            objectName: "lightnessVarianceCheckBox"
            text: qsTr("Lightness variance")
            checked: previewItem.parameters.lightness.enabled

            Layout.fillWidth: true

            ToolTip.text: qsTr("Enable random variance in lightness")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onToggled: previewItem.parameters.lightness.enabled = checked
        }

        TexturedFillVarianceRangedSlider {
            id: lightnessVarianceSlider
            objectName: "lightnessVarianceSlider"
            displayName: "lightness"
            parameter: previewItem.parameters.lightness
            enabled: lightnessVarianceCheckBox.checked

            Layout.fillWidth: true
        }

        Label {
            text: qsTr("Preview scale")
        }

        Slider {
            id: previewScaleSlider
            objectName: "previewScaleSlider"
            from: 1
            to: 30

            ToolTip {
                parent: previewScaleSlider.handle
                text: previewScaleSlider.value.toFixed(1)
                visible: previewScaleSlider.pressed
            }
        }

        Rectangle {
            id: texturedFillPreviewBackground
            objectName: "texturedFillPreviewBackground"
            color: "black"
            clip: true

            Layout.columnSpan: 2
            Layout.preferredWidth: 400
            Layout.preferredHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true

            TexturedFillPreviewItem {
                id: previewItem
                objectName: "texturedFillSettingsPreviewItem"
                anchors.fill: parent
                scale: previewScaleSlider.value
                smooth: false
                canvas: dialog.canvas
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "texturedFillSettingsDialogOkButton"
            text: qsTr("OK")

            onClicked: {
                canvas.texturedFillParameters.copy(previewItem.parameters)
                dialog.visible = false
            }
        }
        Button {
            objectName: "texturedFillSettingsCancelButton"
            text: qsTr("Cancel")

            onClicked: dialog.visible = false
        }
    }
}
