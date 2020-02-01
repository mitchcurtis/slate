import QtGraphicalEffects 1.0
import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import QtQuick.Templates 2.14 as T
import Qt.labs.platform 1.0 as Platform

import App 1.0

Dialog {
    id: popup
    objectName: "newProjectPopup"
    modal: true
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
    signal choseLayeredImageProject

    onAccepted: {
        if (buttonGroup.checkedButton === tilesetProjectButton)
            choseTilesetProject()
        else if (buttonGroup.checkedButton === imageProjectButton)
            choseImageProject()
        else if (buttonGroup.checkedButton === layeredImageProjectButton)
            choseLayeredImageProject()
    }

    contentItem: ColumnLayout {
        spacing: 14

        ButtonGroup {
            id: buttonGroup
            objectName: "newProjectPopupButtonGroup"
            buttons: popup.contentItem.children
        }

        ProjectTemplateButton {
            id: layeredImageProjectButton
            objectName: "layeredImageProjectButton"
            icon.source: "qrc:/images/layered-image-project.png"
            titleText: qsTr("New Layered Image")
            descriptionText: qsTr("Creates a new layered image project. Several images are drawn on "
                + "top of each other, and can be exported as a single image.")
            radius: popup.background.radius
            iconBackgroundColour: Qt.darker(popup.background.color, 1.15)

            onClicked: popup.accept()

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ProjectTemplateButton {
            id: imageProjectButton
            objectName: "imageProjectButton"
            icon.source: "qrc:/images/image-project.png"
            titleText: qsTr("New Image")
            descriptionText: qsTr("Creates a new bitmap image for direct editing, with no layer support.")
            radius: popup.background.radius
            iconBackgroundColour: Qt.darker(popup.background.color, 1.15)

            onClicked: popup.accept()

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ProjectTemplateButton {
            id: tilesetProjectButton
            objectName: "tilesetProjectButton"
            icon.source: "qrc:/images/tileset-project.png"
            titleText: qsTr("New Tileset")
            descriptionText: qsTr("Creates a new tileset bitmap image for editing. "
               + "Paint tiles from an image onto a grid. "
               + "An accompanying project file is created to save the contents of the grid.")
            radius: popup.background.radius
            iconBackgroundColour: Qt.darker(popup.background.color, 1.15)

            onClicked: popup.accept()

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
