import QtQuick
import QtQuick.Controls

import App

Item {
    id: root
    objectName: "colourSelector"
    implicitWidth: penForegroundColourToolButton.implicitWidth * 1.5
    implicitHeight: penForegroundColourToolButton.implicitWidth * 1.5

    property ImageCanvas canvas

    property string currentPenName: penBackgroundColourButton.checked ? qsTr("background") : qsTr("foreground")
    property string currentPenPropertyName: penBackgroundColourButton.checked ? "penBackgroundColour" : "penForegroundColour"

    ButtonGroup {
        id: buttonGroup
        buttons: root.children
    }

    ColourButton {
        id: penBackgroundColourButton
        objectName: "penBackgroundColourButton"
        x: penForegroundColourToolButton.width / 2
        y: penForegroundColourToolButton.height / 2
        color: canvas ? canvas.penBackgroundColour : "white"
        width: parent.width / 2
        height: width

        ToolTip.text: qsTr("Set the background colour")
        ToolTip.visible: hovered
        ToolTip.delay: UiConstants.toolTipDelay
        ToolTip.timeout: UiConstants.toolTipTimeout
    }

    ColourButton {
        id: penForegroundColourToolButton
        objectName: "penForegroundColourButton"
        color: canvas ? canvas.penForegroundColour : "black"
        // Account for the SwatchFocusRectangle being beyond the colourPanel Flickable's clipping boundary.
        x: 1
        width: parent.width / 2
        height: width
        checked: true
        checkable: true

        ToolTip.text: qsTr("Set the foreground colour")
        ToolTip.visible: hovered
        ToolTip.delay: UiConstants.toolTipDelay
        ToolTip.timeout: UiConstants.toolTipTimeout
    }
}
