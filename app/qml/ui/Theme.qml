pragma Singleton

import QtQuick

QtObject {
    property color focusColour: "red"

    // Use "var" so that we can assign undefined.
    property var toolButtonWidth: undefined
    property var toolButtonHeight: undefined

    property color canvasBackgroundColour: "grey"
    property color splitColour: "#444"
    property color rulerForegroundColour: "#ccc"
    property color rulerBackgroundColour: canvasBackgroundColour

    property color panelColour: "#424242"
}
