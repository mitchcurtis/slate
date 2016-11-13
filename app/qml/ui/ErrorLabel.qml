import QtQuick 2.0
import QtQuick.Controls 2.0

Label {
    text: "\uf06a"
    font.family: "FontAwesome"

    property bool valid: false
    property string errorMessage

    MouseArea {
        hoverEnabled: true
        anchors.fill: parent

        ToolTip.text: errorMessage
        ToolTip.visible: containsMouse
    }
}
