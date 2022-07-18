import QtQml.Models 2.12
import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
// TODO: remove in Qt 6
import QtQml 2.15

import Slate 1.0

ColumnLayout {
    id: behaviourTab

    function applyChangesToSettings() {
        settings.loadLastOnStartup = loadLastCheckBox.checked
        settings.gesturesEnabled = enableGesturesCheckBox.checked
        settings.penToolRightClickBehaviour = penToolRightClickBehaviourComboBox.currentValue
        settings.autoSwatchEnabled = enableAutoSwatchCheckBox.checked

        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i)
            if (row.hasChanged) {
                settings[row.shortcutName] = row.newSequence
            }
        }
    }

    function revertToOldSettings() {
        loadLastCheckBox.checked = settings.loadLastOnStartup
        enableGesturesCheckBox.checked = settings.gesturesEnabled
        penToolRightClickBehaviourComboBox.currentIndex =
            penToolRightClickBehaviourComboBox.indexOfValue(settings.penToolRightClickBehaviour)
        enableAutoSwatchCheckBox.checked = settings.autoSwatchEnabled

        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i)
            if (row.hasChanged) {
                row.reset()
            }
        }
    }

    Item {
        Layout.preferredHeight: 10
    }

    ScrollView {
        objectName: "behaviourScrollView"
        clip: true

        ScrollBar.horizontal.policy: ScrollBar.AsNeeded

        Layout.fillWidth: true
        Layout.fillHeight: true

        GridLayout {
            columns: 2
            columnSpacing: 12
            width: parent.width

            Label {
                text: qsTr("General")
                font.bold: true

                Layout.columnSpan: 2
            }
            VerticalSeparator {
                leftPadding: 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0

                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Load last project on startup")
            }
            CheckBox {
                id: loadLastCheckBox
                leftPadding: 0
                checked: settings.loadLastOnStartup
            }

            Label {
                text: qsTr("Enable gestures (macOS only)")
            }
            CheckBox {
                id: enableGesturesCheckBox
                leftPadding: 0
                checked: settings.gesturesEnabled

                ToolTip.text: qsTr("Enables the use of two-finger panning and pinch-to-zoom on macOS")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout
            }

            Label {
                text: qsTr("Pen tool right click behaviour")
            }
            ComboBox {
                id: penToolRightClickBehaviourComboBox
                objectName: "penToolRightClickBehaviourComboBox"
                textRole: "display"
                valueRole: "value"

                // TODO: add icons when QTBUG-73489 is implemented
                model: [
                    {
                        value: ImageCanvas.PenToolRightClickAppliesEraser,
                        display: qsTr("Apply eraser")
                    },
                    {
                        value: ImageCanvas.PenToolRightClickAppliesEyeDropper,
                        display: qsTr("Apply colour picker")
                    },
                    {
                        value: ImageCanvas.PenToolRightClickAppliesBackgroundColour,
                        display: qsTr("Apply background colour")
                    }
                ]

                Layout.fillWidth: true

                Component.onCompleted: currentIndex = indexOfValue(settings.penToolRightClickBehaviour)
            }

            Label {
                text: qsTr("Enable auto swatch (experimental)")
            }
            CheckBox {
                id: enableAutoSwatchCheckBox
                leftPadding: 0
                checked: settings.autoSwatchEnabled

                ToolTip.text: qsTr("Enables the use of a read-only swatch whose colours come from the image")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout
            }

            Label {
                text: qsTr("Shortcuts")
                font.bold: true

                Layout.columnSpan: 2
                Layout.topMargin: 12
            }
            VerticalSeparator {
                leftPadding: 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0

                Layout.columnSpan: 2
                Layout.fillWidth: true
            }

            Repeater {
                model: ObjectModel {
                    id: shortcutModel

                    ShortcutRow {
                        shortcutName: "newShortcut"
                        shortcutDisplayName: qsTr("New Project")
                    }
                    ShortcutRow {
                        shortcutName: "openShortcut"
                        shortcutDisplayName: qsTr("Open Project")
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
                        shortcutName: "exportShortcut"
                        shortcutDisplayName: qsTr("Export Project")
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
                    ShortcutRow {
                        shortcutName: "fullScreenToggleShortcut"
                        shortcutDisplayName: qsTr("Toggle fullscreen")
                    }
                    ShortcutRow {
                        shortcutName: "selectNextLayerUpShortcut"
                        shortcutDisplayName: qsTr("Select Next Layer Up")
                    }
                    ShortcutRow {
                        shortcutName: "selectNextLayerDownShortcut"
                        shortcutDisplayName: qsTr("Select Next Layer Down")
                    }
                }
            }
        }
    }
}
