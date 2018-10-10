import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0 as Platform

import App 1.0

Dialog {
    id: popup
    title: qsTr("New Image Project")
    contentWidth: 400
    padding: 20
    bottomPadding: 0
    modal: true
    closePolicy: Popup.CloseOnEscape
    focus: true

    readonly property int imageWidth: imageWidthSpinBox.value
    readonly property int imageHeight: imageHeightSpinBox.value
    readonly property bool transparentBackground: transparentImageBackgroundCheckBox.checked

    onAboutToShow: {
        // Reset input controls to default values.
        imageWidthSpinBox.value = imageWidthSpinBox.defaultValue;
        imageHeightSpinBox.value = imageHeightSpinBox.defaultValue;
        transparentImageBackgroundCheckBox.checked = transparentImageBackgroundCheckBox.defaultValue;

        imageWidthSpinBox.contentItem.forceActiveFocus();
    }

    contentItem: ColumnLayout {
        spacing: 14

        GridLayout {
            columns: 2
            columnSpacing: 14
            rowSpacing: 0

            Label {
                text: qsTr("Image Width")
            }
            SpinBox {
                id: imageWidthSpinBox
                objectName: "imageWidthSpinBox"
                from: 1
                value: defaultValue
                to: 10000
                editable: true
                hoverEnabled: true
                stepSize: 1

                ToolTip.text: qsTr("The height of the image")
                ToolTip.visible: hovered

                readonly property int defaultValue: 256
            }

            Label {
                text: qsTr("Image Height")
            }
            SpinBox {
                id: imageHeightSpinBox
                objectName: "imageHeightSpinBox"
                from: 1
                to: 10000
                value: defaultValue
                editable: true
                hoverEnabled: true
                stepSize: 1

                ToolTip.text: qsTr("The height of the image")
                ToolTip.visible: hovered

                readonly property int defaultValue: 256
            }

            CheckBox {
                id: transparentImageBackgroundCheckBox
                objectName: "transparentImageBackgroundCheckBox"
                text: qsTr("Transparent Background")
                padding: 0

                readonly property bool defaultValue: false
            }
        }
    }

    footer: Item {
        implicitWidth: footerLayout.implicitWidth + footerLayout.anchors.leftMargin + footerLayout.anchors.rightMargin
        implicitHeight: footerLayout.implicitHeight + footerLayout.anchors.topMargin + footerLayout.anchors.bottomMargin

        RowLayout {
            id: footerLayout
            anchors.fill: parent
            anchors.margins: 20
            anchors.topMargin: 8

            Item {
                Layout.fillWidth: true
            }

            Button {
                id: okButton
                objectName: "newImageProjectOkButton"
                text: "OK"
                onClicked: popup.accept()
            }
            Button {
                objectName: "newImageProjectCancelButton"
                text: "Cancel"
                onClicked: popup.reject()
            }
        }
    }
}
