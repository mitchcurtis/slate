import QtQuick 2.6
import QtQuick.Controls 2.0

import App 1.0

Popup {
    modal: true
    dim: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside

    property TileCanvas canvas

    contentItem: Slider {
        id: toolSizeSlider
        objectName: "toolSizeSlider"
        from: 1
        to: canvas.maxToolSize
        stepSize: 1

        onValueChanged: canvas.toolSize = value

        Binding {
            target: toolSizeSlider
            property: "value"
            value: canvas.toolSize
        }

        ToolTip {
            parent: toolSizeSlider.handle
            visible: toolSizeSlider.pressed
            text: toolSizeSlider.valueAt(toolSizeSlider.position).toFixed(0)
        }
    }
}
