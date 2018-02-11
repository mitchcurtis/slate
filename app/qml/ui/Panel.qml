import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

import App 1.0

import "." as Ui

Page {
    id: root
    objectName: "panel"
    background: Rectangle {
        color: Ui.CanvasColours.panelColour
    }

    property bool expanded: true

    header: RowLayout {
        Label {
            text: root.title
            font.bold: true
            anchors.verticalCenter: parent.verticalCenter

            Layout.leftMargin: 16
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            text: expanded ? "\uf146" : "\uf0fe"
            font.family: "FontAwesome"
            anchors.verticalCenter: parent.verticalCenter
            focus: Qt.NoFocus

            Layout.preferredWidth: implicitHeight

            onClicked: expanded = !expanded
        }
    }
}
