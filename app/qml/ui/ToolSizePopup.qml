import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.11

import App 1.0

import "." as Ui

Popup {
    objectName: "toolSizePopup"
    focus: true

    property ImageCanvas canvas

    contentItem: ColumnLayout {

        RowLayout {

            Slider {
                id: lowerToolSizeSlider
                objectName: "toolSizeSlider"
                from: 0
                to: canvas ? canvas.maxToolSize : 1
                stepSize: 1
                visible: canvas && canvas.toolSizeUsePressure
                Layout.fillWidth: true
                value: canvas ? canvas.lowerToolSize : 1

                Binding {
                    target: canvas
                    property: "lowerToolSize"
                    value: lowerToolSizeSlider.value
                    when: canvas
                }
            }

            Slider {
                id: upperToolSizeSlider
                objectName: "upperToolSizeSlider"
                from: 1
                to: canvas ? canvas.maxToolSize : 1
                stepSize: 1
                Layout.fillWidth: true
                value: canvas ? canvas.upperToolSize : 1

                Binding {
                    target: canvas
                    property: "upperToolSize"
                    value: upperToolSizeSlider.value
                    when: canvas
                }
            }

            // Why causing binding loop?
            /*RangeSlider {
                id: toolSizeRangeSlider
                objectName: "toolSizeRangeSlider"
                from: 0
                to: canvas ? canvas.maxToolSize : 1
                stepSize: 1
                visible: canvas && canvas.toolSizeUsePressure
                Layout.fillWidth: true
                first.value: canvas ? canvas.lowerToolSize : 1
                second.value: canvas ? canvas.upperToolSize : 1

                Binding {
                    target: canvas
                    property: "lowerToolSize"
                    value: toolSizeRangeSlider.first.value
                    when: canvas
                }

                Binding {
                    target: canvas
                    property: "upperToolSize"
                    value: toolSizeRangeSlider.second.value
                    when: canvas
                }
            }*/

            Ui.IconToolButton {
                objectName: "usePressureButton"
                text: "\uf040"
                hoverEnabled: true
                focusPolicy: Qt.NoFocus

                checked: canvas && canvas.toolSizeUsePressure == true
                checkable: true

                ToolTip.text: qsTr("Enable tablet stylus pressure sensitivity for tool size")
                ToolTip.visible: hovered

                onClicked: canvas.toolSizeUsePressure = checked
            }
        }

        RowLayout {
            SpinBox {
                id: lowerToolSizeSpinBox
                editable: true
                from: 0
                to: canvas ? canvas.maxToolSize : 1
                stepSize: 1
                visible: canvas && canvas.toolSizeUsePressure == true
                Layout.fillWidth: true
                value: canvas ? canvas.lowerToolSize : 1

                Binding {
                    target: canvas
                    property: "lowerToolSize"
                    value: lowerToolSizeSpinBox.value
                    when: canvas
                }

                ToolTip.text: qsTr("Lower tool size")
                ToolTip.visible: hovered
            }


            SpinBox {
                id: upperToolSizeSpinBox
                editable: true
                from: 1
                to: canvas ? canvas.maxToolSize : 1
                stepSize: 1
                Layout.fillWidth: true
                value: canvas ? canvas.upperToolSize : 1

                Binding {
                    target: canvas
                    property: "upperToolSize"
                    value: upperToolSizeSpinBox.value
                    when: canvas
                }

                ToolTip.text: canvas && canvas.toolSizeUsePressure ? qsTr("Upper tool size") : qsTr("Tool size")
                ToolTip.visible: hovered
            }
        }
    }
}
