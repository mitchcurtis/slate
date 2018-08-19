import QtQuick 2.11

import App 1.0

import "."

Rectangle {
    x: Math.max(0, Math.floor(canvas.firstPane.size * parent.width) - 1)
    width: 1
    height: parent.height
    color: CanvasColours.splitColour
    z: 4
    visible: canvas && canvas.project && canvas.project.loaded

    property ImageCanvas canvas
}
