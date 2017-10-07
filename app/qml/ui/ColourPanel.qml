import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

import App 1.0

import "." as Ui

Rectangle {
    id: panel
    objectName: "colourPanel"
    implicitWidth: picker.implicitWidth
    implicitHeight: picker.implicitHeight + picker.anchors.topMargin + picker.anchors.bottomMargin
    color: Ui.CanvasColours.panelColour
    clip: true

    property ImageCanvas canvas
    property alias project: picker.project

    HslSimplePicker {
        id: picker
        canvas: panel.canvas
        anchors.fill: parent
        anchors.margins: 16
        anchors.bottomMargin: 2
    }
}
