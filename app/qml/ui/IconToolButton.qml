import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

ToolButton {
    id: control
    focusPolicy: Qt.TabFocus

    Layout.preferredWidth: implicitWidth
    Layout.maximumWidth: Layout.preferredWidth
    Layout.preferredHeight: implicitHeight
    Layout.maximumHeight: Layout.preferredHeight

    property string iconText

    text: iconText
    font.family: "FontAwesome"
}
