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
        settings.loadLastOnStartup = loadLastCheckBox.checked;

        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i);
            if (row.hasChanged) {
                settings[row.shortcutName] = row.newSequence;
            }
        }
    }

    function clearChanges() {
        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i);
            if (row.hasChanged) {
                row.reset();
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

                Label {
                    text: qsTr("Load last project on startup")
                }
                CheckBox {
                    id: loadLastCheckBox
                    checked: settings.loadLastOnStartup
                }

                Item {
                    Layout.columnSpan: 2
                    Layout.preferredHeight: 10
                }
            }
        }

        ListView {
            id: shortcutListView
            clip: true

            ScrollBar.vertical: ScrollBar {
                id: verticalScrollBar
                Binding {
                    target: verticalScrollBar
                    property: "active"
                    value: verticalScrollBar.hovered
                }
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
                    shortcutName: "centreShortcut"
                    shortcutDisplayName: qsTr("Centre View")
                }
                ShortcutRow {
                    shortcutName: "gridVisibleShortcut"
                    shortcutDisplayName: qsTr("Toggle Grid Visibility")
                }
                ShortcutRow {
                    shortcutName: "splitScreenShortcut"
                    shortcutDisplayName: qsTr("Toggle Split Screen")
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
