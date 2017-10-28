import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

import App 1.0

Popup {
    id: popup
    objectName: "imageSizePopup"
    modal: true
    dim: false
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside

    property Project project

    onVisibleChanged: {
        if (visible && project) {
            widthSpinBox.value = project.size.width;
            heightSpinBox.value = project.size.height;
            widthSpinBox.contentItem.forceActiveFocus();
        }
    }

    contentItem: ColumnLayout {
        Label {
            id: titleLabel
            text: qsTr("Choose a size for the image")
        }

        Item {
            Layout.preferredHeight: 4
        }

        GridLayout {
            columns: 2

            Label {
                text: qsTr("Image width")
            }

            SpinBox {
                id: widthSpinBox
                objectName: "changeImageWidthSpinBox"
                from: 1
                to: 1000
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Image width in pixels")
            }
            Label {
                text: qsTr("Image height")
            }

            SpinBox {
                id: heightSpinBox
                objectName: "changeImageHeightSpinBox"
                from: 1
                to: 1000
                editable: true
                stepSize: 1

                ToolTip.text: qsTr("Image height in pixels")
            }

            Item {
                Layout.minimumHeight: 10
                Layout.columnSpan: 2
            }

            RowLayout {
                Layout.columnSpan: 2

                Item {
                    Layout.fillWidth: true
                }

                Button {
                    objectName: "imageSizePopupOkButton"
                    text: "OK"
                    onClicked: {
                        project.resize(widthSpinBox.value, heightSpinBox.value);
                        popup.visible = false;
                    }
                }
                Button {
                    objectName: "imageSizePopupCancelButton"
                    text: "Cancel"
                    onClicked: popup.visible = false
                }
            }
        }
    }
}
