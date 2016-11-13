import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

Popup {
    id: control
    modal: true
    focus: true

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

    contentItem: RowLayout {
        Label {
            id: iconText
            text: icon
            anchors.verticalCenter: parent.verticalCenter
        }
        Label {
            text: control.text
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
