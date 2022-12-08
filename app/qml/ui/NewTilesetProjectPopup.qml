import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.platform as Platform

import Slate

Dialog {
    id: root
    title: qsTr("New Tileset Project")
    contentWidth: 400
    padding: 20
    modal: true
    closePolicy: Popup.CloseOnEscape
    focus: true

    readonly property bool validExistingFile: useExistingTilesetCheckBox.checked && validator.fileValid
    readonly property bool validExistingFileOrNew: !useExistingTilesetCheckBox.checked || (useExistingTilesetCheckBox.checked && validator.fileValid)
    readonly property int tilesWide: useExistingTilesetCheckBox.checked ? validator.calculatedTilesWide : tilesWideSpinBox.value
    readonly property int tilesHigh: useExistingTilesetCheckBox.checked ? validator.calculatedTilesHigh : tilesHighSpinBox.value
    readonly property bool allValid: useExistingTilesetCheckBox.checked ? validExistingFile && validator.tileWidthValid && validator.tileHeightValid : true

    readonly property url tilesetPath: tilesetPathTextField.text
    readonly property int tileWidth: tileWidthSpinBox.value
    readonly property int tileHeight: tileHeightSpinBox.value
    readonly property int tilesetTilesWide: tilesWide
    readonly property int tilesetTilesHigh: tilesHigh
    readonly property int canvasTilesWide: 10
    readonly property int canvasTilesHigh: 10
    readonly property bool transparentBackground: transparentBackgroundCheckBox.checked

    Platform.FileDialog {
        id: openTilesetDialog
        onAccepted: {
            tilesetPathTextField.text = file;
            okButton.forceActiveFocus();
        }
    }

    NewProjectValidator {
        id: validator
        objectName: "validator"
        url: tilesetPathTextField.text
        tileWidth: tileWidthSpinBox.value
        tileHeight: tileHeightSpinBox.value
    }

    FontMetrics {
        id: fontMetrics
        font: tilesetPathTextField.font
    }

    onAboutToShow: {
        // Reset input controls to default values.
        useExistingTilesetCheckBox.forceActiveFocus();
        useExistingTilesetCheckBox.checked = false;
        tilesetPathTextField.text = "";
        tileWidthSpinBox.value = tileWidthSpinBox.defaultValue;
        tileHeightSpinBox.value = tileHeightSpinBox.defaultValue;
        tilesWideSpinBox.value = tilesWideSpinBox.defaultValue;
        tilesHighSpinBox.value = tilesHighSpinBox.defaultValue;
        transparentBackgroundCheckBox.checked = transparentBackgroundCheckBox.defaultValue;
    }

    contentItem: GridLayout {
        columns: 2
        columnSpacing: 14
        rowSpacing: 0

        CheckBox {
            id: useExistingTilesetCheckBox
            objectName: "useExistingTilesetCheckBox"
            text: qsTr("Use existing tileset")
            padding: 0

            Layout.columnSpan: 2
        }

        RowLayout {
            enabled: useExistingTilesetCheckBox.checked

            Layout.leftMargin: 14

            Label {
                text: qsTr("Tileset")
            }

            ErrorLabel {
                objectName: "invalidFileIcon"
                opacity: !valid

                valid: validator.fileValid
                errorMessage: validator.fileErrorMessage
            }
        }

        RowLayout {
            enabled: useExistingTilesetCheckBox.checked

            TextField {
                id: tilesetPathTextField
                objectName: "tilesetPathTextField"

                Layout.fillWidth: true

                Keys.onReturnPressed: root.accept()
            }

            Button {
                objectName: "chooseTilesetPathButton"
                text: qsTr("...")
                onClicked: openTilesetDialog.open()
                hoverEnabled: true

                ToolTip.text: qsTr("Click to choose the path to a tileset image")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout
            }
        }

        RowLayout {
            Label {
                text: qsTr("Tile Width")
                enabled: validExistingFileOrNew
            }
            ErrorLabel {
                objectName: "invalidTileWidthIcon"
                opacity: validator.fileValid && !validator.tileWidthValid

                errorMessage: validator.tileWidthErrorMessage
            }
        }
        SpinBox {
            id: tileWidthSpinBox
            objectName: "tileWidthSpinBox"
            from: 1
            value: defaultValue
            editable: true
            hoverEnabled: true
            enabled: validExistingFileOrNew

            ToolTip.text: qsTr("How wide each tile is in pixels")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            readonly property int defaultValue: 32

            Keys.onReturnPressed: root.accept()
        }

        RowLayout {
            Label {
                text: qsTr("Tile Height")
                enabled: validExistingFileOrNew
            }
            ErrorLabel {
                objectName: "invalidTileHeightIcon"
                opacity: validator.fileValid && !validator.tileHeightValid

                errorMessage: validator.tileHeightErrorMessage
            }
        }
        SpinBox {
            id: tileHeightSpinBox
            objectName: "tileHeightSpinBox"
            from: 1
            value: defaultValue
            editable: true
            hoverEnabled: true
            enabled: validExistingFileOrNew

            ToolTip.text: qsTr("How high each tile is in pixels")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay

            readonly property int defaultValue: 32

            Keys.onReturnPressed: root.accept()
        }

        Label {
            text: qsTr("Tiles Wide")
            enabled: !useExistingTilesetCheckBox.checked
        }
        SpinBox {
            id: tilesWideSpinBox
            objectName: "tilesWideSpinBox"
            from: 1
            value: defaultValue
            to: 10000
            editable: true
            hoverEnabled: true
            enabled: !useExistingTilesetCheckBox.checked

            ToolTip.text: qsTr("How many tiles should be displayed horizontally")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            readonly property int defaultValue: 10

            Keys.onReturnPressed: root.accept()
        }

        Label {
            text: qsTr("Tiles High")
            enabled: !useExistingTilesetCheckBox.checked
        }
        SpinBox {
            id: tilesHighSpinBox
            objectName: "tilesHighSpinBox"
            from: 1
            value: defaultValue
            to: 10000
            editable: true
            hoverEnabled: true
            enabled: !useExistingTilesetCheckBox.checked

            ToolTip.text: qsTr("How many tiles should be displayed vertically")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            readonly property int defaultValue: 10

            Keys.onReturnPressed: root.accept()
        }

        CheckBox {
            id: transparentBackgroundCheckBox
            objectName: "transparentBackgroundCheckBox"
            text: qsTr("Transparent Background")
            padding: 0
            enabled: !useExistingTilesetCheckBox.checked

            readonly property bool defaultValue: true
        }

        Label {
            text: qsTr("Preview")
            enabled: validator.fileValid

            Layout.columnSpan: 2
            Layout.topMargin: 10
        }

        Frame {
            id: previewFrame
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 128

            enabled: validExistingFileOrNew

            Item {
                clip: true
                anchors.fill: parent

                Flickable {
                    anchors.fill: parent
                    contentWidth: previewContents.width
                    contentHeight: previewContents.height
                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.horizontal: ScrollBar {}
                    ScrollBar.vertical: ScrollBar {}

                    Item {
                        id: previewContents
                        width: previewRect.visible ? previewRect.width : previewImage.width
                        height: previewRect.visible ? previewRect.height : previewImage.height

                        Rectangle {
                            id: previewRect
                            width: tilesWide * tileWidthSpinBox.value
                            height: tilesHigh * tileHeightSpinBox.value
                            color: transparentBackground ? "transparent" : "white"
                            visible: !useExistingTilesetCheckBox.checked
                        }

                        Image {
                            id: previewImage
                            source: enabled ? validator.url : ""
                            sourceSize: Qt.size(width, height)
                        }

                        TileGrid {
                            tileWidth: tileWidthSpinBox.value
                            tileHeight: tileHeightSpinBox.value
                            tilesWide: root.tilesWide
                            tilesHigh: root.tilesHigh
                            anchors.fill: parent
                            anchors.margins: -0.5
                            visible: !useExistingTilesetCheckBox.checked
                                || (validator.tileWidthValid && validator.tileHeightValid)
                        }
                    }
                }
            }
        }
    }

    footer: DialogButtonBox {
        DialogButton {
            id: okButton
            objectName: "newTilesetProjectOkButton"
            text: qsTr("OK")
            enabled: allValid

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        DialogButton {
            objectName: "newTilesetProjectCancelButton"
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
