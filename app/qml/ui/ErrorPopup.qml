import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14

Dialog {
    id: root
    objectName: "errorPopup"
    modal: true
    focus: true
    title: "Error"
    width: 500
    standardButtons: Dialog.Ok

    property string text
    property string icon

    function showError(errorMessage) {
        text = errorMessage;
        open();
    }

    onVisibleChanged: {
        if (!visible) {
            text = "";
        }
    }

    RowLayout {
        anchors.fill: parent

        Label {
            id: iconText
            text: icon
        }
        Label {
            objectName: root.objectName + "ErrorLabel"
            text: root.text
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere

            Layout.fillWidth: true
        }
    }
}
