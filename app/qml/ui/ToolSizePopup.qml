import QtQuick 2.12
import QtQuick.Controls 2.12
// TODO: remove in Qt 6
import QtQml 2.15

import App 1.0

Popup {
    objectName: "toolSizePopup"
    modal: true
    dim: false
    focus: true

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
            // TODO: remove in Qt 6
            restoreMode: Binding.RestoreBindingOrValue
        }

        ToolTip {
            parent: toolSizeSlider.handle
            visible: toolSizeSlider.pressed
            text: toolSizeSlider.valueAt(toolSizeSlider.position).toFixed(0)
        }
    }
}
