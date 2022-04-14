pragma Singleton

import QtQuick

QtObject {
    property string styleName: "Basic"

    property SystemPalette systemPalette: SystemPalette {}

    property color focusColour: "red"

    // Use "var" so that we can assign undefined.
    property var toolButtonWidth: undefined
    property var toolButtonHeight: undefined

    property color canvasBackgroundColour: "#666"
    property color splitColour: "#444"
    property color rulerForegroundColour: "#ccc"
    property color rulerBackgroundColour: canvasBackgroundColour

    property color panelColour: systemPalette.window
}
