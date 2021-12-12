import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

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
