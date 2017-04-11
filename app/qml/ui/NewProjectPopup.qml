import QtGraphicalEffects 1.0
import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1
import Qt.labs.platform 1.0 as Platform

import App 1.0

Dialog {
    id: popup
    modal: true
    focus: true
    padding: 20
    bottomPadding: 0
    contentWidth: tilesetPage.implicitWidth
    contentHeight: 480

    readonly property bool validExistingFile: useExistingTilesetCheckBox.checked && validator.fileValid
    readonly property bool validExistingFileOrNew: !useExistingTilesetCheckBox.checked || (useExistingTilesetCheckBox.checked && validator.fileValid)
    readonly property int tilesWide: useExistingTilesetCheckBox.checked ? validator.calculatedTilesWide : tilesetTilesWideSpinBox.value
    readonly property int tilesHigh: useExistingTilesetCheckBox.checked ? validator.calculatedTilesHigh : tilesetTilesHighSpinBox.value
    readonly property bool allValid: useExistingTilesetCheckBox.checked ? validExistingFile && validator.tileWidthValid && validator.tileHeightValid : true

    readonly property url tilesetPath: tilesetPathTextField.text
    readonly property bool isometric: isometricCheckBox.checked
    readonly property int isometricTileXOffset: isometricTileXOffsetSpinBox.value
    readonly property int isometricTileYOffset: isometricTileYOffsetSpinBox.value
    readonly property int tilesetTileWidth: tileWidthSpinBox.value
    readonly property int tilesetTileHeight: tileHeightSpinBox.value
    readonly property int tilesetTilesWide: tilesWide
    readonly property int tilesetTilesHigh: tilesHigh
    readonly property int canvasTileWidth: 32
    readonly property int canvasTileHeight: 32
    readonly property int canvasTilesWide: 10
    readonly property int canvasTilesHigh: 10

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

    onAboutToShow: stackView.pop(null, StackView.Immediate)

    onOpened: {
        // Reset input controls to default values.
        useExistingTilesetCheckBox.forceActiveFocus();
        useExistingTilesetCheckBox.checked = false;
        tilesetPathTextField.text = "";
        isometricCheckBox.checked = false;
        tileWidthSpinBox.value = tileWidthSpinBox.defaultValue;
        tileHeightSpinBox.value = tileHeightSpinBox.defaultValue;
        tilesetTilesWideSpinBox.value = tilesetTilesWideSpinBox.defaultValue;
        tilesetTilesHighSpinBox.value = tilesetTilesHighSpinBox.defaultValue;
    }

    header: Label {
        text: qsTr("New Project")
        font.pixelSize: fontMetrics.font.pixelSize * 1.5
//        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
        padding: 14
        topPadding: 20
        bottomPadding: 0
    }

    contentItem: StackView {
        id: stackView
        initialItem: tilesetPage
        clip: true

        Page {
            id: tilesetPage
            objectName: "tilesetPage"

            Flickable {
                id: flickable
                implicitWidth: tilesetPageGridLayout.implicitWidth + leftMargin + rightMargin
                implicitHeight: 400
                contentWidth: contentWidth
                contentHeight: tilesetPageGridLayout.implicitHeight
                flickableDirection: Flickable.VerticalFlick
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                // Put some distance between the scroll bar and the content.
                leftMargin: 30
                rightMargin: 30
                // Make room for the hover effects.
                topMargin: 20

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AlwaysOn
                }

                GridLayout {
                    id: tilesetPageGridLayout
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
                            anchors.verticalCenter: parent.verticalCenter

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
                        }

                        Button {
                            text: qsTr("...")
                            onClicked: openTilesetDialog.open()
                            hoverEnabled: true

                            ToolTip.text: qsTr("Click to choose the path to a tileset image")
                            ToolTip.visible: hovered
                        }
                    }

                    CheckBox {
                        id: isometricCheckBox
                        objectName: "isometricCheckBox"
                        text: qsTr("Isometric")
                        padding: 0

                        Layout.columnSpan: 2
                    }

                    Label {
                        text: qsTr("Tile X Offset")
                        enabled: isometric

                        Layout.leftMargin: 14
                    }
                    SpinBox {
                        id: isometricTileXOffsetSpinBox
                        objectName: "isometricTileXOffsetSpinBox"
                        from: 0
                        value: defaultValue
                        editable: true
                        hoverEnabled: true
                        enabled: isometric

                        ToolTip.text: qsTr("The X offset of isometric tiles")
                        ToolTip.visible: hovered

                        readonly property int defaultValue: 0
                    }

                    Label {
                        text: qsTr("Tile Y Offset")
                        enabled: isometric

                        Layout.leftMargin: 14
                    }
                    SpinBox {
                        id: isometricTileYOffsetSpinBox
                        objectName: "isometricTileYOffsetSpinBox"
                        from: 0
                        value: defaultValue
                        editable: true
                        hoverEnabled: true
                        enabled: isometric

                        ToolTip.text: qsTr("The Y offset of isometric tiles")
                        ToolTip.visible: hovered

                        readonly property int defaultValue: 0
                    }

                    Label {
                        text: qsTr("Tileset Width")
                        enabled: !useExistingTilesetCheckBox.checked
                    }
                    SpinBox {
                        id: tilesetTilesWideSpinBox
                        objectName: "tilesetTilesWideSpinBox"
                        from: 1
                        value: defaultValue
                        to: 10000
                        editable: true
                        hoverEnabled: true
                        enabled: !useExistingTilesetCheckBox.checked

                        ToolTip.text: qsTr("How many horizontal tiles there are in the tileset")
                        ToolTip.visible: hovered

                        readonly property int defaultValue: 10
                    }

                    Label {
                        text: qsTr("Tileset Height")
                        enabled: !useExistingTilesetCheckBox.checked
                    }
                    SpinBox {
                        id: tilesetTilesHighSpinBox
                        objectName: "tilesetTilesHighSpinBox"
                        from: 1
                        value: defaultValue
                        to: 10000
                        editable: true
                        hoverEnabled: true
                        enabled: !useExistingTilesetCheckBox.checked

                        ToolTip.text: qsTr("How many vertical tiles there are in the tileset")
                        ToolTip.visible: hovered

                        readonly property int defaultValue: 10
                    }

                    RowLayout {
                        Label {
                            text: qsTr("Tileset Tile Width")
                            enabled: validExistingFileOrNew
                        }
                        ErrorLabel {
                            objectName: "invalidTileWidthIcon"
                            opacity: validator.fileValid && !validator.tileWidthValid
                            anchors.verticalCenter: parent.verticalCenter

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

                        ToolTip.text: qsTr("How wide each tileset tile is in pixels")
                        ToolTip.visible: hovered

                        readonly property int defaultValue: 32
                    }

                    RowLayout {
                        Label {
                            text: qsTr("Tileset Tile Height")
                            enabled: validExistingFileOrNew
                        }
                        ErrorLabel {
                            objectName: "invalidTileHeightIcon"
                            opacity: validator.fileValid && !validator.tileHeightValid
                            anchors.verticalCenter: parent.verticalCenter

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

                        ToolTip.text: qsTr("How high each tileset tile is in pixels")
                        ToolTip.visible: hovered

                        readonly property int defaultValue: 32
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
                                        tilesWide: popup.tilesWide
                                        tilesHigh: popup.tilesHigh
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
            }

            background: null

            footer: Item {
                implicitWidth: rowLayout.implicitWidth + rowLayout.anchors.leftMargin + rowLayout.anchors.rightMargin
                implicitHeight: rowLayout.implicitHeight + rowLayout.anchors.topMargin + rowLayout.anchors.bottomMargin

                RowLayout {
                    id: rowLayout
                    anchors.fill: parent
                    anchors.margins: 20
                    anchors.topMargin: 8

                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: nextButton
                        objectName: "newProjectNextButton"
                        text: "Next"
                        enabled: allValid
                        onClicked: stackView.push(canvasPage, StackView.Immediate)
                    }
                    Button {
                        text: "Cancel"
                        onClicked: popup.reject()
                    }
                }
            }
        }

        Page {
            id: canvasPage
            objectName: "canvasPage"
            leftPadding: 30
            rightPadding: 30
            visible: false

            GridLayout {
                columns: 2
                columnSpacing: 14
                rowSpacing: 0

                Label {
                    text: qsTr("Canvas Width")
                    enabled: !useExistingTilesetCheckBox.checked
                }
                SpinBox {
                    id: canvasTilesWideSpinBox
                    objectName: "canvasTilesWideSpinBox"
                    from: 1
                    value: defaultValue
                    to: 100
                    editable: true
                    hoverEnabled: true
                    enabled: !useExistingTilesetCheckBox.checked

                    ToolTip.text: qsTr("How many horizontal tiles there should be in the canvas")
                    ToolTip.visible: hovered

                    readonly property int defaultValue: 10
                }

                Label {
                    text: qsTr("Canvas Height")
                    enabled: !useExistingTilesetCheckBox.checked
                }
                SpinBox {
                    id: canvasTilesHighSpinBox
                    objectName: "canvasTilesHighSpinBox"
                    from: 1
                    value: defaultValue
                    to: 100
                    editable: true
                    hoverEnabled: true
                    enabled: !useExistingTilesetCheckBox.checked

                    ToolTip.text: qsTr("How many vertical tiles there should be in the canvas")
                    ToolTip.visible: hovered

                    readonly property int defaultValue: 10
                }
            }

            background: null

            footer: Item {
                implicitWidth: canvasFooterLayout.implicitWidth + canvasFooterLayout.anchors.leftMargin + canvasFooterLayout.anchors.rightMargin
                implicitHeight: canvasFooterLayout.implicitHeight + canvasFooterLayout.anchors.topMargin + canvasFooterLayout.anchors.bottomMargin

                RowLayout {
                    id: canvasFooterLayout
                    anchors.fill: parent
                    anchors.margins: 20
                    anchors.topMargin: 8

                    Button {
                        id: backButton
                        objectName: "newProjectBackButton"
                        text: "Back"
                        onClicked: stackView.pop(StackView.Immediate)
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    Button {
                        id: okButton
                        objectName: "newProjectOkButton"
                        text: "OK"
                        enabled: allValid
                        onClicked: popup.accept()
                    }
                    Button {
                        text: "Cancel"
                        onClicked: popup.reject()
                    }
                }
            }
        }
    }
}
