import QtQuick 2.0
import QtQuick.Controls 2.0

import App 1.0

Item {
    id: root
    implicitWidth: penForegroundColourToolButton.width * 1.5
    implicitHeight: penForegroundColourToolButton.height * 1.5

    property TileCanvas canvas

    property string currentPenName: penBackgroundColourButton.checked ? "penBackgroundColour" : "penForegroundColour"

    ButtonGroup {
        id: buttonGroup
        buttons: root.children
    }

    ColourButton {
        id: penBackgroundColourButton
        objectName: "penBackgroundColourButton"
        x: penForegroundColourToolButton.width / 2
        y: penForegroundColourToolButton.height / 2
        color: canvas.penBackgroundColour
        width: parent.width / 2
        height: width

        ToolTip.text: qsTr("Set the background colour")
        ToolTip.visible: hovered
    }

    ColourButton {
        id: penForegroundColourToolButton
        objectName: "penForegroundColourButton"
        color: canvas.penForegroundColour
        width: parent.width / 2
        height: width
        checked: true

        ToolTip.text: qsTr("Set the foreground colour")
        ToolTip.visible: hovered
    }
}
