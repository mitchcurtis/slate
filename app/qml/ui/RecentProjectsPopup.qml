import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

Popup {
    id: popup
    modal: true
    focus: true
    padding: 10
    contentWidth: 300

    signal accepted(string fileName)

    contentItem: ColumnLayout {
        id: columnLayout
        spacing: 14

        Label {
            text: qsTr("Load Recent Project")
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 30

            ListView {
                id: listView
                anchors.fill: parent
                spacing: 14
                visible: count > 0

                delegate: ItemDelegate {
                    text: modelData
                }
            }

            Label {
                text: qsTr("No recent projects")
                visible: !listView.visible
                color: "#aaa"
                anchors.centerIn: parent
            }
        }

        RowLayout {
            Item {
                Layout.preferredWidth: popup.contentWidth / 2
            }

            Button {
                id: okButton
                objectName: "loadRecentProjectOkButton"
                text: "OK"
                enabled: listView.currentIndex != -1

                Layout.fillWidth: true

                onClicked: {
                    popup.visible = false;
                    accepted(currentItem.text);
                }
            }
            Button {
                text: "Cancel"

                Layout.fillWidth: true

                onClicked: popup.visible = false
            }
        }
    }
}
