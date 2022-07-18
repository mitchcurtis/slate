import QtQuick
import QtQuick.Controls

import Slate

Popup {
    objectName: "toolSizePopup"
    modal: true
    dim: false
    focus: true
    // Some styles have quite a small slider by default, which makes it harder
    // to be precise when selecting a value (which also affects tests,
    // specifically TestHelper::changeToolSize).
    width: 240

    property ImageCanvas canvas

    contentItem: Slider {
        id: toolSizeSlider
        objectName: "toolSizeSlider"
        from: 1
        to: canvas ? canvas.maxToolSize : 100
        stepSize: 1

        onMoved: if (canvas) canvas.toolSize = value

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
