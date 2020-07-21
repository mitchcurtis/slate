import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import App

import "." as Ui

Panel {
    id: root
    title: qsTr("Colour")
    objectName: "colourPanel"
    clip: true
    padding: 16
    topPadding: 0

    property ImageCanvas canvas
    property alias project: picker.project

    readonly property int minimumUsefulHeight: header.implicitHeight
        + picker.minimumUsefulHeight
        + root.bottomPadding

    UiStateSerialisation {
        project: root.project
        onReadyToLoad: root.expanded = root.project.uiState.value("colourPanelExpanded", true)
        onReadyToSave: root.project.uiState.setValue("colourPanelExpanded", root.expanded)
    }

    contentItem: Flickable {
        objectName: root.objectName + "Flickable"
        implicitWidth: picker.implicitWidth
        implicitHeight: picker.implicitHeight
        contentWidth: picker.implicitWidth
        contentHeight: picker.implicitHeight
        clip: true
        flickableDirection: Flickable.AutoFlickIfNeeded

        ScrollBar.vertical: ScrollBar {}

        HslSimplePicker {
            id: picker
            canvas: root.canvas
        }
    }
}
