import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: control
    checkable: true
    hoverEnabled: true
    focusPolicy: Qt.NoFocus
    topInset: 0
    bottomInset: 0

    property alias color: rect.color

    background: Rectangle {
        id: rect
        implicitWidth: 24
        implicitHeight: 24

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "#444444"
        }

        Rectangle {
            anchors.fill: parent
            anchors.margins: 1
            color: "transparent"
            border.color: "#dddddd"
        }

        SwatchFocusRectangle {
            x: -1
            width: 1
            height: parent.height
            color: focusColour
            visible: control.checked
        }
    }
}
