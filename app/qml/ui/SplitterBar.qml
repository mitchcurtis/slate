import QtQuick

import App

import "."

Rectangle {
    objectName: "splitterBar"
    x: Math.max(0, Math.floor(canvas.firstPane.size * parent.width) - 1)
    width: 1
    height: parent.height
    color: Theme.splitColour
    z: 4
    visible: canvas && canvas.project && canvas.project.loaded && canvas.splitScreen

    property ImageCanvas canvas
}
