import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

import App 1.0

import "." as Ui

Rectangle {
    id: root
    objectName: "animationPanel"
    implicitWidth: 800
    implicitHeight: 200
    color: Ui.CanvasColours.panelColour
    clip: true

    property Project project

    Image {
        source: ""
    }
}
