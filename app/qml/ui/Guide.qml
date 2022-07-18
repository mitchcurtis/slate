import QtQuick

import Slate

Rectangle {
    x: horizontal ? 0 : (pane.integerZoomLevel * position) + pane.integerOffset.x
    y: horizontal ? (pane.integerZoomLevel * position) + pane.integerOffset.y : 0
    width: horizontal ? parent.width : 1
    height: horizontal ? 1 : parent.height
    color: "cyan"
    // TODO: hide if not visible

    required property CanvasPane pane

    // From model.
    required property int orientation
    required property int position

    readonly property bool horizontal: orientation === Qt.Horizontal
}
