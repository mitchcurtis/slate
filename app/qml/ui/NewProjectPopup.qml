import QtGraphicalEffects 1.0
import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1
import QtQuick.Templates 2.1 as T
import Qt.labs.platform 1.0 as Platform

import App 1.0

Dialog {
    id: popup
    objectName: "newProjectPopup"
    modal: true
    closePolicy: Popup.CloseOnEscape
    focus: true
    padding: 20
    contentWidth: 600
    contentHeight: 400

    onAboutToShow: {
        buttonGroup.checkedButton = null;
        tilesetProjectButton.forceActiveFocus();
    }

    signal choseTilesetProject
    signal choseImageProject

    onAccepted: {
        if (buttonGroup.checkedButton === tilesetProjectButton)
            choseTilesetProject()
        else
            choseImageProject()
    }

    contentItem: ColumnLayout {
        spacing: 14

        ButtonGroup {
            id: buttonGroup
            buttons: popup.contentItem.children
        }

        ProjectTemplateButton {
            id: tilesetProjectButton
            objectName: "tilesetProjectButton"
            titleText: qsTr("New Tileset")
            descriptionText: qsTr("Creates a new tileset bitmap image for editing. "
               + "Paint tiles from an image onto a grid. "
               + "An accompanying project file is created to save the contents of the grid.")
            radius: popup.background.radius
            iconBackgroundColour: Qt.darker(popup.background.color, 1.15)

            onClicked: popup.accept()

            Layout.fillWidth: true

            Label {
                text: "\uf00a"
                font.family: "FontAwesome"
                font.pixelSize: 200
                fontSizeMode: Label.Fit
                verticalAlignment: Label.AlignVCenter

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Label {
                text: "\uf1c5"
                font.family: "FontAwesome"
                font.pixelSize: 200
                fontSizeMode: Label.Fit
                verticalAlignment: Label.AlignVCenter

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }

        ProjectTemplateButton {
            objectName: "imageProjectButton"
            titleText: qsTr("New Image")
            descriptionText: qsTr("Creates a new bitmap image for editing.")
            radius: popup.background.radius
            iconBackgroundColour: Qt.darker(popup.background.color, 1.15)

            onClicked: popup.accept()

            Layout.fillWidth: true

            Label {
                text: "\uf1c5"
                font.family: "FontAwesome"
                font.pixelSize: 200
                fontSizeMode: Label.Fit
                verticalAlignment: Label.AlignVCenter

                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
