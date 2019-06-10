pragma Singleton

import QtQml 2.12

QtObject {
    readonly property color focusColour: "salmon"

    // Use "var" so that we can assign undefined.
    readonly property var toolButtonWidth: undefined
    readonly property var toolButtonHeight: undefined
    readonly property color toolBarColour: "#424242"

    readonly property color canvasBackgroundColour: "grey"
    readonly property color splitColour: "#444"
    readonly property color rulerForegroundColour: "#ccc"
    readonly property color rulerBackgroundColour: canvasBackgroundColour

    readonly property color panelColour: "#424242"

    readonly property color statusBarPaneEdgeColour: "#444"
}
