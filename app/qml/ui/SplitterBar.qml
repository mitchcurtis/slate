import QtQuick

import Slate

import "."

Rectangle {
    objectName: "splitterBar"
    x: Math.floor(canvas.firstPane.size * parent.width)
    width: 1
    height: parent.height
    color: Theme.splitColour
    z: 4
    visible: canvas && canvas.project && canvas.project.loaded && canvas.splitScreen

    property ImageCanvas canvas
}
