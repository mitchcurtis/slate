import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.3

import App 1.0

Dialog {
    id: popup
    objectName: "canvasSizePopup"
    title: qsTr("Choose a size for the canvas")
    modal: true
    dim: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnReleaseOutside

    property Project project
    readonly property bool isTilesetProject: project && project.type === Project.TilesetType

    onVisibleChanged: {
        if (visible && project) {
            widthSpinBox.value = project.size.width;
            heightSpinBox.value = project.size.height;
            widthSpinBox.contentItem.forceActiveFocus();
        }
    }

    contentItem: ColumnLayout {
        Item {
            Layout.preferredHeight: 4
        }

        GridLayout {
            columns: 2

            Label {
                text: qsTr("Canvas width")
            }

            SpinBox {
                id: widthSpinBox
                objectName: "changeCanvasWidthSpinBox"
                from: 1
                to: isTilesetProject ? 1024 : 4096
                editable: true
                stepSize: 1

                ToolTip.text: isTilesetProject ? tilesetText : imageText

                readonly property string tilesetText: qsTr("Number of horizontal tiles")
                readonly property string imageText: qsTr("Canvas width in pixels")
            }
            Label {
                text: qsTr("Canvas height")
            }

            SpinBox {
                id: heightSpinBox
                objectName: "changeCanvasHeightSpinBox"
                from: 1
                to: isTilesetProject ? 1024 : 4096
                editable: true
                stepSize: 1

                ToolTip.text: isTilesetProject ? tilesetText : imageText

                readonly property string tilesetText: qsTr("Number of vertical tiles")
                readonly property string imageText: qsTr("Canvas height in pixels")
            }

            Item {
                Layout.minimumHeight: 10
                Layout.columnSpan: 2
            }
        }
    }

    footer: DialogButtonBox {
        Button {
            objectName: "canvasSizePopupOkButton"
            text: "OK"

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

            onClicked: {
                project.size = Qt.size(widthSpinBox.value, heightSpinBox.value);
                popup.visible = false;
            }
        }
        Button {
            objectName: "canvasSizePopupCancelButton"
            text: "Cancel"

            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole

            onClicked: popup.visible = false
        }
    }
}
