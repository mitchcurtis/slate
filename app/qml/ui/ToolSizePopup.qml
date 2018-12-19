import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.11

import App 1.0

import "." as Ui

Popup {
    objectName: "toolSizePopup"
    modal: true
    dim: false
    focus: true

    property ImageCanvas canvas

    contentItem: ColumnLayout {
        Slider {
            id: toolSizeSlider
            objectName: "toolSizeSlider"
            from: 1
            to: canvas ? canvas.maxToolSize : 1
            stepSize: 1
            visible: canvas && canvas.toolSizeUsePressure == false
            Layout.fillWidth: true

            onValueChanged: if (canvas) canvas.upperToolSize = value

            Binding {
                target: toolSizeSlider
                property: "value"
                value: canvas ? canvas.upperToolSize : 1
                when: canvas
            }
        }

        RangeSlider {
            id: toolSizeRangeSlider
            objectName: "toolSizeRangeSlider"
            from: 1
            to: canvas ? canvas.maxToolSize : 1
            stepSize: 1
            visible: canvas && canvas.toolSizeUsePressure == true
            Layout.fillWidth: true

            first.onValueChanged: if (canvas) canvas.lowerToolSize = first.value
            second.onValueChanged: if (canvas) canvas.upperToolSize = second.value

            Binding {
                target: toolSizeRangeSlider
                property: "first.value"
                value: canvas ? canvas.lowerToolSize : 1
                when: canvas
            }

            Binding {
                target: toolSizeRangeSlider
                property: "second.value"
                value: canvas ? canvas.upperToolSize : 1
                when: canvas
            }
        }

        RowLayout {
            SpinBox {
                editable: true
                from: 1
                to: canvas ? canvas.maxToolSize : 1
                stepSize: 1
                visible: canvas && canvas.toolSizeUsePressure == true

                value: canvas ? canvas.lowerToolSize : 1
                onValueChanged: if (canvas) canvas.lowerToolSize = value

                ToolTip.text: qsTr("Lower tool size")
                ToolTip.visible: hovered
            }

            SpinBox {
                editable: true
                from: 1
                to: canvas ? canvas.maxToolSize : 1
                stepSize: 1

                value: canvas ? canvas.upperToolSize : 1
                onValueChanged: if (canvas) canvas.upperToolSize = value

                ToolTip.text: canvas && canvas.toolSizeUsePressure ? qsTr("Upper tool size") : qsTr("Tool size")
                ToolTip.visible: hovered
            }

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
    }
}
