import QtQuick 2.9
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.4
import QtQuick.Window 2.0

import App 1.0

import "." as Ui

Dialog {
    id: root
    objectName: "renameSwatchColourDialog"
    title: qsTr("Rename swatch colour")
    modal: true
    dim: false
    focus: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    property Project project
    property string oldName
    property int colourIndex

    onAboutToShow: {
        nameTextField.text = oldName
        nameTextField.selectAll()
        nameTextField.forceActiveFocus()
    }
    onAccepted: project.swatch.renameColour(colourIndex, nameTextField.text)

    TextField {
        id: nameTextField
        objectName: "swatchNameTextField"
        width: parent.width
        onAccepted: root.accept()
    }
}
