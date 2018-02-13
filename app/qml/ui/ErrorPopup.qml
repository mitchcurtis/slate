import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3

Dialog {
    id: control
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
            text: control.text
            wrapMode: Label.WrapAtWordBoundaryOrAnywhere

            Layout.fillWidth: true
        }
    }
}
