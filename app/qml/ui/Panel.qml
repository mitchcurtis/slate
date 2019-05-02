import QtQuick 2.12
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12

import App 1.0

import "." as Ui

Page {
    id: root
    objectName: "panel"
    background: Rectangle {
        color: Ui.CanvasColours.panelColour
    }

    property bool expanded: true
    property Popup settingsPopup: null
    property alias settingsPopupToolButton: settingsPopupToolButton

    header: RowLayout {
        objectName: root.objectName + "Header"
        spacing: 0

        Label {
            objectName: parent.objectName + "TitleLabel"
            text: root.title
            font.bold: true

            Layout.leftMargin: 16
        }

        Item {
            Layout.fillWidth: true
        }

        ToolButton {
            id: settingsPopupToolButton
            objectName: root.objectName + "SettingsToolButton"
            text: "\uf013"
            font.family: "FontAwesome"
            focusPolicy: Qt.NoFocus
            visible: settingsPopup

            Layout.preferredWidth: implicitHeight

            onClicked: settingsPopup.open()
        }

        ToolButton {
            objectName: root.objectName + "HideShowToolButton"
            text: expanded ? "\uf146" : "\uf0fe"
            font.family: "FontAwesome"
            focusPolicy: Qt.NoFocus

            Layout.leftMargin: -8
            Layout.preferredWidth: implicitHeight

            onClicked: expanded = !expanded
        }
    }
}
