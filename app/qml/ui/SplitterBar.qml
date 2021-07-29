import QtQuick 2.12

import App 1.0

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
