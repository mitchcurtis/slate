import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1

import App 1.0

RowLayout {
    id: root

    property TileCanvas canvas
    property alias colourSelector: selector

    ColourSelector {
        id: selector
        canvas: root.canvas

        Layout.preferredWidth: 32
        Layout.preferredHeight: Layout.preferredWidth
    }

    Label {
        text: qsTr("#")
    }

    TextField {
        id: hexTextField
        objectName: "hexTextField"
        text: canvas[selector.currentPenName].toString().substr(1)
        readOnly: true // see tst_app::colourPickerHexField
        inputMask: "HHHHHH"
        selectByMouse: true
        onAccepted: canvas[selector.currentPenName] = inputColour

        property color inputColour: "#" + text

        Layout.fillWidth: true
    }
}
