import QtQml.Models 2.2
import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1

import App 1.0

Dialog {
    id: dialog
    objectName: "optionsDialog"
    modal: true
    focus: true
    implicitWidth: 500
    implicitHeight: 400

    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: applyAllSettings()
    onRejected: clearChanges()

    function applyAllSettings() {
        settings.loadLastOnStartup = loadLastCheckBox.checked
        settings.checkerColour1 = checkerColour1TextField.colour
        settings.checkerColour2 = checkerColour2TextField.colour

        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i)
            if (row.hasChanged) {
                settings[row.shortcutName] = row.newSequence
            }
        }
    }

    function clearChanges() {
        loadLastCheckBox.checked = settings.loadLastOnStartup
        checkerColour1TextField.text = settings.checkerColour1
        checkerColour2TextField.text = settings.checkerColour2

        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i)
            if (row.hasChanged) {
                row.reset()
            }
        }
    }

    header: TabBar {
        id: tabBar

        TabButton {
            objectName: "generalTabButton"
            text: qsTr("General")
        }

        TabButton {
            objectName: "shortcutsTabButton"
            text: qsTr("Shortcuts")
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        ColumnLayout {
            id: generalTab

            Item {
                Layout.preferredHeight: 10
            }

            GridLayout {
                columns: 2
                columnSpacing: 12

                Label {
                    text: qsTr("Load last project on startup")
                }
                CheckBox {
                    id: loadLastCheckBox
                    leftPadding: 0
                    checked: settings.loadLastOnStartup
                }

                Label {
                    text: qsTr("Transparency grid colours")
                }
                RowLayout {
                    spacing: 8

                    Layout.alignment: Qt.AlignHCenter

                    TextMetrics {
                        id: colourInputFontMetrics
                        // '.' is never part of the text, but it gives us some wiggle room.
                        text: "444444."
                        font: checkerColour1TextField.font
                    }

                    Item {
                        implicitWidth: 32
                        implicitHeight: 32

                        Flow {
                            anchors.fill: parent

                            Repeater {
                                model: 16
                                delegate: Rectangle {
                                    width: 8
                                    height: 8
                                    color: index % 2 == 0
                                        ? (evenRow ? checkerColour2TextField.colour : checkerColour1TextField.colour)
                                        : (evenRow ? checkerColour1TextField.colour : checkerColour2TextField.colour)

                                    readonly property int evenRow: Math.floor(index / 4) % 2 == 0
                                }
                            }
                        }
                    }
                    TextField {
                        id: checkerColour1TextField
                        objectName: "checkerColour1TextField"
                        implicitWidth: colourInputFontMetrics.width
                        text: settings.checkerColour1
                        inputMask: "hhhhhh"
                        selectByMouse: true

                        readonly property color colour: "#" + text
                    }
                    TextField {
                        id: checkerColour2TextField
                        objectName: "checkerColour2TextField"
                        text: settings.checkerColour2
                        implicitWidth: colourInputFontMetrics.width
                        inputMask: "hhhhhh"
                        selectByMouse: true

                        readonly property color colour: "#" + text
                    }
                }
            }
        }

        Item {
            id: shortcutListViewContainer

            ListView {
                id: shortcutListView
                anchors.fill: parent
                anchors.rightMargin: ScrollBar.vertical.width
                clip: true

                ScrollBar.vertical: ScrollBar {
                    id: verticalScrollBar
                    parent: shortcutListView.parent
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    anchors.top: parent.top
                    color: "#1a000000"
                }

                Rectangle {
                    width: parent.width
                    height: 1
                    anchors.bottom: parent.bottom
                    color: "#1a000000"
                }

                model: ObjectModel {
                    id: shortcutModel

                    ShortcutRow {
                        shortcutName: "newShortcut"
                        shortcutDisplayName: qsTr("New Project")
                    }
                    ShortcutRow {
                        shortcutName: "closeShortcut"
                        shortcutDisplayName: qsTr("Close Project")
                    }
                    ShortcutRow {
                        shortcutName: "saveShortcut"
                        shortcutDisplayName: qsTr("Save Project")
                    }
                    ShortcutRow {
                        shortcutName: "saveAsShortcut"
                        shortcutDisplayName: qsTr("Save Project As")
                    }
                    ShortcutRow {
                        shortcutName: "closeShortcut"
                        shortcutDisplayName: qsTr("Close Project")
                    }
                    ShortcutRow {
                        shortcutName: "revertShortcut"
                        shortcutDisplayName: qsTr("Revert To Last Save")
                    }
                    ShortcutRow {
                        shortcutName: "undoShortcut"
                        shortcutDisplayName: qsTr("Undo Action")
                    }
                    ShortcutRow {
                        shortcutName: "redoShortcut"
                        shortcutDisplayName: qsTr("Redo Action")
                    }
                    ShortcutRow {
                        shortcutName: "flipHorizontallyShortcut"
                        shortcutDisplayName: qsTr("Flip Horizontally")
                    }
                    ShortcutRow {
                        shortcutName: "flipVerticallyShortcut"
                        shortcutDisplayName: qsTr("Flip Vertically")
                    }
                    ShortcutRow {
                        shortcutName: "resizeCanvasShortcut"
                        shortcutDisplayName: qsTr("Resize Canvas")
                    }
                    ShortcutRow {
                        shortcutName: "resizeImageShortcut"
                        shortcutDisplayName: qsTr("Resize Image")
                    }
                    ShortcutRow {
                        shortcutName: "moveContentsShortcut"
                        shortcutDisplayName: qsTr("Move Contents")
                    }
                    ShortcutRow {
                        shortcutName: "centreShortcut"
                        shortcutDisplayName: qsTr("Centre View")
                    }
                    ShortcutRow {
                        shortcutName: "zoomInShortcut"
                        shortcutDisplayName: qsTr("Zoom In")
                    }
                    ShortcutRow {
                        shortcutName: "zoomOutShortcut"
                        shortcutDisplayName: qsTr("Zoom Out")
                    }
                    ShortcutRow {
                        shortcutName: "gridVisibleShortcut"
                        shortcutDisplayName: qsTr("Toggle Grid Visibility")
                    }
                    ShortcutRow {
                        shortcutName: "rulersVisibleShortcut"
                        shortcutDisplayName: qsTr("Toggle Ruler Visibility")
                    }
                    ShortcutRow {
                        shortcutName: "guidesVisibleShortcut"
                        shortcutDisplayName: qsTr("Toggle Guide Visibility")
                    }
                    ShortcutRow {
                        shortcutName: "splitScreenShortcut"
                        shortcutDisplayName: qsTr("Toggle Split Screen")
                    }
                    ShortcutRow {
                        shortcutName: "animationPlaybackShortcut"
                        shortcutDisplayName: qsTr("Animation Playback")
                    }
                    ShortcutRow {
                        shortcutName: "optionsShortcut"
                        shortcutDisplayName: qsTr("Options")
                    }
                    ShortcutRow {
                        shortcutName: "penToolShortcut"
                        shortcutDisplayName: qsTr("Pen Tool")
                    }
                    ShortcutRow {
                        shortcutName: "eyeDropperToolShortcut"
                        shortcutDisplayName: qsTr("Eye Dropper Tool")
                    }
                    ShortcutRow {
                        shortcutName: "fillToolShortcut"
                        shortcutDisplayName: qsTr("Fill Tool")
                    }
                    ShortcutRow {
                        shortcutName: "eraserToolShortcut"
                        shortcutDisplayName: qsTr("Eraser Tool")
                    }
                    ShortcutRow {
                        shortcutName: "selectionToolShortcut"
                        shortcutDisplayName: qsTr("Selection Tool")
                    }
                    ShortcutRow {
                        shortcutName: "toolModeShortcut"
                        shortcutDisplayName: qsTr("Toggle Tool Mode")
                    }
                    ShortcutRow {
                        shortcutName: "decreaseToolSizeShortcut"
                        shortcutDisplayName: qsTr("Decrease Tool Size")
                    }
                    ShortcutRow {
                        shortcutName: "increaseToolSizeShortcut"
                        shortcutDisplayName: qsTr("Increase Tool Size")
                    }
                    ShortcutRow {
                        shortcutName: "swatchLeftShortcut"
                        shortcutDisplayName: qsTr("Move Swatch Selection Left")
                    }
                    ShortcutRow {
                        shortcutName: "swatchRightShortcut"
                        shortcutDisplayName: qsTr("Move Swatch Selection Right")
                    }
                    ShortcutRow {
                        shortcutName: "swatchUpShortcut"
                        shortcutDisplayName: qsTr("Move Swatch Selection Up")
                    }
                    ShortcutRow {
                        shortcutName: "swatchDownShortcut"
                        shortcutDisplayName: qsTr("Move Swatch Selection Down")
                    }
                }
            }
        }
    }
}
