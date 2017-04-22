import QtQuick 2.6
import QtQuick.Controls 2.0

import App 1.0

Popup {
    modal: true
    dim: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside

    property ImageCanvas canvas

    contentItem: Slider {
        id: toolSizeSlider
        objectName: "toolSizeSlider"
        from: 1
        to: canvas ? canvas.maxToolSize : 100
        stepSize: 1

        onValueChanged: if (canvas) canvas.toolSize = value

        Binding {
            target: toolSizeSlider
            property: "value"
            value: canvas ? canvas.toolSize : 1
            when: canvas
        }

        ToolTip {
            parent: toolSizeSlider.handle
            visible: toolSizeSlider.pressed
            text: toolSizeSlider.valueAt(toolSizeSlider.position).toFixed(0)
        }
    }
}
