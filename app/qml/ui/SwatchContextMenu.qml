import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import QtQuick.Window 2.0

import App 1.0

import "." as Ui

Menu {
    id: root
    objectName: "swatchContextMenu"
    x: rightClickedColourIndex !== -1 ? rightClickedColourPos.x : 0
    y: rightClickedColourIndex !== -1 ? rightClickedColourPos.y : 0
    modal: true
    dim: false

    property Project project
    property Dialog renameSwatchColourDialog
    // The root objects in the application aren't loaded if we use SwatchPanel as the type here,
    // and there's no error message...
    property var swatchPanel
    property int rightClickedColourIndex
    property string rightClickedColourName
    property int rightClickedColourX
    property int rightClickedColourY

    readonly property point rightClickedColourPos: rightClickedColourIndex !== -1
        ? swatchGridView.contentItem.mapToItem(swatchPanel, rightClickedColourX, rightClickedColourY) : Qt.point(0, 0)

    onClosed: {
        rightClickedColourIndex = -1
        rightClickedColourName = ""
    }

    MenuItem {
        id: renameMenuItem
        objectName: "renameSwatchColourMenuItem"
        text: qsTr("Rename")
        onTriggered: {
            renameSwatchColourDialog.colourIndex = root.rightClickedColourIndex
            renameSwatchColourDialog.oldName = root.rightClickedColourName
            renameSwatchColourDialog.open()
        }
    }

    MenuItem {
        objectName: "deleteSwatchColourMenuItem"
        text: qsTr("Delete")
        onTriggered: project.swatch.removeColour(root.rightClickedColourIndex)
    }
}
