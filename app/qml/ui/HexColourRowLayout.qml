import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1

import App 1.0

RowLayout {
    id: root
    objectName: "HexColourRowLayout"

    property ImageCanvas canvas
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
        text: currentColour.toString().substr(1)
//        readOnly: true // see tst_app::colourPickerHexField
        inputMask: "hhhhhhhh"
        selectByMouse: true
        onAccepted: {
            canvas[selector.currentPenPropertyName] = inputColour
            canvas.forceActiveFocus()
        }
        onActiveFocusChanged: if (activeFocus) textBeforeEditing = text
        onEditingFinished: canvas.forceActiveFocus()

        Keys.onEscapePressed: {
            text = textBeforeEditing
            canvas.forceActiveFocus()
        }

        property color inputColour: "#" + text
        property color currentColour: canvas ? canvas[selector.currentPenPropertyName] : Qt.rgba(0, 0, 0, 1)
        property string textBeforeEditing

        Layout.fillWidth: true
    }
}
