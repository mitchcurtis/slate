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

import "." as Ui

Dialog {
    id: root
    objectName: "hueSaturationDialog"
    title: qsTr("Hue/Saturation")
    modal: true
    dim: false
    focus: true

    property Project project
    property ImageCanvas canvas

    // We don't want to store these as "color", because the values
    // are not absolute: they are going to be *added* to each pixel's colour.
    property real hslHue
    property real hslSaturation
    property real hslLightness
    property real hslAlpha

    readonly property real sliderStepSize: 0.001

    // TODO: need https://bugreports.qt.io/browse/QTBUG-71317 to be fixed
//    DragHandler {
//        target: ???
//    }

    TextMetrics {
        id: valueTextMetrics
        font: hueTextField.font
        text: "-0.0001"
    }

    function modifySelectionHsl() {
        canvas.modifySelectionHsl(hslHue, hslSaturation, hslLightness)
    }

    onAboutToShow: {
        if (project) {
            hslHue = 0
            hslSaturation = 0
            hslLightness = 0
            hslAlpha = 0

            hueTextField.forceActiveFocus()

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
        columns: 2
        rowSpacing: 0

        Label {
            text: qsTr("Hue")

            Layout.column: 0
            Layout.fillWidth: true
        }
        DoubleTextField {
            id: hueTextField
            objectName: root.objectName + "HueTextField"
            propertySource: root
            propertyName: "hslHue"
            // If the maximum length matches the text metrics text, the sign
            // character will sometimes be slightly cut off, so account for that.
            maximumLength: valueTextMetrics.text.length - 1

            Layout.maximumWidth: valueTextMetrics.width
            Layout.fillWidth: true

            Keys.onReturnPressed: root.accept()

            // We call this here instead of just doing it in e.g. onHslHueChanged
            // because that would require us to block changes that occur when the
            // HSL property values are reset upon showing the dialog. Modifying the
            // selection only on user interaction instead feels nicer.
            onValueModified: modifySelectionHsl()
        }
        Slider {
            id: hueSlider
            objectName: root.objectName + "HueSlider"
            from: -1
            to: 1
            value: hslHue
            stepSize: sliderStepSize
            leftPadding: 0
            rightPadding: 0

            Layout.columnSpan: 2
            Layout.topMargin: -bottomPadding

            ToolTip.text: qsTr("Changes the hue of the image")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            Keys.onReturnPressed: root.accept()

            onMoved: {
                hslHue = value
                modifySelectionHsl()
            }

            background: Item {
                // Default Material values.
                implicitWidth: 200
                implicitHeight: 48

                HorizontalGradientRectangle {
                    width: parent.width
                    gradient: HueGradient {}
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        Label {
            text: qsTr("Saturation")

            Layout.fillWidth: true
        }
        DoubleTextField {
            objectName: root.objectName + "SaturationTextField"
            propertySource: root
            propertyName: "hslSaturation"
            maximumLength: valueTextMetrics.text.length - 1

            Layout.maximumWidth: valueTextMetrics.width
            Layout.fillWidth: true

            Keys.onReturnPressed: root.accept()

            onValueModified: modifySelectionHsl()
        }
        Slider {
            id: saturationSlider
            objectName: root.objectName + "SaturationSlider"
            from: -1
            to: 1
            value: hslSaturation
            stepSize: sliderStepSize
            leftPadding: 0
            rightPadding: 0

            Layout.columnSpan: 2
            Layout.topMargin: -bottomPadding

            ToolTip.text: qsTr("Changes the saturation of the image")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            Keys.onReturnPressed: root.accept()

            onMoved: {
                hslSaturation = value
                modifySelectionHsl()
            }

            background: Item {
                // Default Material values.
                implicitWidth: 200
                implicitHeight: 48

                HorizontalGradientRectangle {
                    width: parent.width
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "grey"
                        }
                        GradientStop {
                            position: 1
                            color: "red"
                        }
                    }
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        Label {
            text: qsTr("Lightness")

            Layout.fillWidth: true
        }
        DoubleTextField {
            objectName: root.objectName + "LightnessTextField"
            propertySource: root
            propertyName: "hslLightness"
            maximumLength: valueTextMetrics.text.length - 1

            Layout.maximumWidth: valueTextMetrics.width
            Layout.fillWidth: true

            Keys.onReturnPressed: root.accept()

            onValueModified: modifySelectionHsl()
        }
        Slider {
            id: lightnessSlider
            objectName: root.objectName + "LightnessSlider"
            from: -1
            to: 1
            value: hslLightness
            stepSize: sliderStepSize
            leftPadding: 0
            rightPadding: 0

            Layout.columnSpan: 2
            Layout.topMargin: -bottomPadding

            ToolTip.text: qsTr("Changes the lightness of the image")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            Keys.onReturnPressed: root.accept()

            onMoved: {
                hslLightness = value
                modifySelectionHsl()
            }

            background: Item {
                // Default Material values.
                implicitWidth: 200
                implicitHeight: 48

                HorizontalGradientRectangle {
                    width: parent.width
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "black"
                        }
                        GradientStop {
                            position: 1
                            color: "white"
                        }
                    }
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "hueSaturationDialogOkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: "hueSaturationDialogCancelButton"
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
