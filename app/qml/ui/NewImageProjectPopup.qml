import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform as Platform

import App

Dialog {
    id: root
    title: qsTr("New Image Project")
    contentWidth: 400
    padding: 20
    modal: true
    closePolicy: Popup.CloseOnEscape
    focus: true

    readonly property int imageWidth: imageWidthSpinBox.value
    readonly property int imageHeight: imageHeightSpinBox.value
    readonly property bool transparentBackground: transparentImageBackgroundCheckBox.checked

    onAboutToShow: {
        // Reset input controls to default values.
        var useClipboardImageSize = Clipboard.image().width > 0 && Clipboard.image().height > 0
        imageWidthSpinBox.value = useClipboardImageSize ? Clipboard.image().width : imageWidthSpinBox.defaultValue
        imageHeightSpinBox.value = useClipboardImageSize ? Clipboard.image().height : imageHeightSpinBox.defaultValue
        transparentImageBackgroundCheckBox.checked = transparentImageBackgroundCheckBox.defaultValue

        imageWidthSpinBox.contentItem.forceActiveFocus()
    }

    contentItem: GridLayout {
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
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            readonly property int defaultValue: 256

            Keys.onReturnPressed: root.accept()
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
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            readonly property int defaultValue: 256

            Keys.onReturnPressed: root.accept()
        }

        CheckBox {
            id: transparentImageBackgroundCheckBox
            objectName: "transparentImageBackgroundCheckBox"
            text: qsTr("Transparent Background")
            padding: 0

            readonly property bool defaultValue: false
        }
    }

    footer: DialogButtonBox {
        Button {
            id: okButton
            objectName: root.objectName + "OkButton"
            text: qsTr("OK")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: root.objectName + "CancelButton"
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
