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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Slate

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

    // Only enable the slider if there's something to show. For the swatch fill type,
    // that means ensuring that we have swatches and their probabilities add to more than zero.
    readonly property bool previewEnabled: previewItem.parameters.type !== TexturedFillParameters.SwatchFillType
        || previewItem.parameters.swatch.nonZeroProbabilitySum

    onAboutToShow: {
        if (project) {
            // Copy the canvas' current parameters into ours. If the user accepts the dialog,
            // we'll copy our updated ones back into canvas.
            previewItem.parameters.copy(canvas.texturedFillParameters)
        }
    }

    contentItem: GridLayout {
        id: mainGridLayout
        columns: 2

        Layout.topMargin: 4

        Label {
            text: qsTr("Fill type")

            Layout.fillWidth: true
        }
        ComboBox {
            id: fillTypeComboBox
            objectName: "fillTypeComboBox"
            currentIndex: previewItem.parameters.type
            // TODO: use valueRole in 5.14
            model: [ qsTr("Variance"), qsTr("Swatch") ]

            Layout.fillWidth: true

            onActivated: previewItem.parameters.type = currentIndex
        }

        StackLayout {
            id: typeSettingsStackLayout
            currentIndex: fillTypeComboBox.currentIndex

            Layout.columnSpan: 2
            Layout.fillHeight: true

            GridLayout {
                id: varianceGridLayout
                columns: 2

                Layout.columnSpan: 2

                CheckBox {
                    id: hueVarianceCheckBox
                    objectName: "hueVarianceCheckBox"
                    text: qsTr("Hue variance")
                    checked: previewItem.parameters.hue.enabled

                    Layout.fillWidth: true

                    ToolTip.text: qsTr("Enable random variance in hue")
                    ToolTip.visible: hovered
                    ToolTip.delay: UiConstants.toolTipDelay
                    ToolTip.timeout: UiConstants.toolTipTimeout

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
                    ToolTip.delay: UiConstants.toolTipDelay
                    ToolTip.timeout: UiConstants.toolTipTimeout

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
                    ToolTip.delay: UiConstants.toolTipDelay
                    ToolTip.timeout: UiConstants.toolTipTimeout

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
            }

            Item {
                // This item is here to ensure that the scroll bars can sit outside of the view,
                // and also fixes what appears to be a bug with the ViewBorders being
                // too close together because they seem to be using ListView's original (smaller) height.
                id: swatchListViewContainer
                implicitWidth: swatchListView.implicitWidth
                implicitHeight: swatchListView.implicitHeight

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredHeight: varianceGridLayout.height
                Layout.columnSpan: 2

                ListView {
                    id: swatchListView
                    objectName: "texturedFillSwatchListView"
                    clip: true
                    anchors.fill: parent

                    function highlightColour(colour) {
                        let colourIndex = model.indexOfColour(colour)
                        if (colourIndex === -1) {
                            console.warn("Index of colour " + colour + " does not exist in ProbabilitySwatchModel")
                            return
                        }

                        positionViewAtIndex(colourIndex, ListView.Center)
                        let item = itemAtIndex(colourIndex)
                        item.highlight()
                    }

                    Label {
                        text: qsTr("No colours in swatch")
                        font.pixelSize: Qt.application.font.pixelSize * 1.1
                        anchors.centerIn: parent
                        visible: swatchListView.count === 0
                    }

                    ViewBorder {
                        anchors.top: swatchListView.top
                    }

                    ViewBorder {
                        anchors.bottom: swatchListView.bottom
                    }

                    ScrollBar.vertical: ScrollBar {
                        parent: swatchListViewContainer
                        anchors.left: parent.right
                        height: parent.height
                    }

                    model: ProbabilitySwatchModel {
                        objectName: "texturedFillProbabilitySwatchModel"
                        swatch: previewItem.parameters.swatch
                    }
                    delegate: TexturedFillSwatchDelegate {
                        width: swatchListView.width

                        probabilitySlider.onPressedChanged: {
                            if (!probabilitySlider.pressed)
                                previewItem.parameters.swatch.setProbability(index, probabilitySlider.value)
                        }
                        onDeleted: previewItem.parameters.swatch.removeColour(index)
                    }
                }
            }
        }

        Label {
            text: qsTr("Preview scale")

            Layout.fillWidth: true
        }
        Slider {
            id: previewScaleSlider
            objectName: "previewScaleSlider"
            from: 1
            to: 30
            enabled: dialog.previewEnabled

            Layout.fillWidth: true
            Layout.maximumWidth: lightnessVarianceSlider.width

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
                enabled: dialog.previewEnabled

                onColourClicked: swatchListView.highlightColour(colour)
            }
        }
    }

    footer: DialogButtonBox {
        DialogButton {
            objectName: "texturedFillSettingsDialogOkButton"
            text: qsTr("OK")

            onClicked: {
                canvas.texturedFillParameters.copy(previewItem.parameters)
                dialog.visible = false
            }
        }
        DialogButton {
            objectName: "texturedFillSettingsCancelButton"
            text: qsTr("Cancel")

            onClicked: dialog.visible = false
        }
    }
}
