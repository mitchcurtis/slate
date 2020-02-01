import QtQml.Models 2.2
import QtQuick 2.14
// TODO: move this back to the top once QTBUG-81787 is fixed.
import QtQml 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14

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
    // We used to use onRejected here, but if the settings changes outside of our control
    // (e.g. through being reset during testing) then some controls will contain outdated
    // values since clearChanges() won't be called in that case.
    onAboutToShow: clearChanges()

    function applyAllSettings() {
        settings.language = languageComboBox.model[languageComboBox.currentIndex].value
        settings.loadLastOnStartup = loadLastCheckBox.checked
        settings.gesturesEnabled = enableGesturesCheckBox.checked
        settings.penToolRightClickBehaviour =
            penToolRightClickBehaviourComboBox.model[penToolRightClickBehaviourComboBox.currentIndex].value
        settings.autoSwatchEnabled = enableAutoSwatchCheckBox.checked
        settings.checkerColour1 = checkerColour1TextField.colour
        settings.checkerColour2 = checkerColour2TextField.colour
        settings.alwaysShowCrosshair = alwaysShowCrosshairCheckBox.checked
        settings.fpsVisible = showFpsCheckBox.checked
        settings.windowOpacity = windowOpacitySlider.value

        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i)
            if (row.hasChanged) {
                settings[row.shortcutName] = row.newSequence
            }
        }
    }

    function clearChanges() {
        languageComboBox.currentIndex = languageComboBox.indexForValue(settings.language)
        loadLastCheckBox.checked = settings.loadLastOnStartup
        enableGesturesCheckBox.checked = settings.gesturesEnabled
        penToolRightClickBehaviourComboBox.currentIndex =
            penToolRightClickBehaviourComboBox.indexForValue(settings.penToolRightClickBehaviour)
        enableAutoSwatchCheckBox.checked = settings.autoSwatchEnabled
        checkerColour1TextField.text = settings.checkerColour1
        checkerColour2TextField.text = settings.checkerColour2
        showFpsCheckBox.checked = settings.fpsVisible
        alwaysShowCrosshairCheckBox.checked = settings.alwaysShowCrosshair
        windowOpacitySlider.value = settings.windowOpacity

        for (var i = 0; i < shortcutModel.count; ++i) {
            var row = shortcutModel.get(i)
            if (row.hasChanged) {
                row.reset()
            }
        }
    }

    header: TabBar {
        id: tabBar
        // For the Universal style.
        clip: true

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

            ScrollView {
                clip: true

                ScrollBar.horizontal.policy: ScrollBar.AsNeeded

                Layout.fillWidth: true
                Layout.fillHeight: true

                GridLayout {
                    columns: 2
                    columnSpacing: 12

                    width: parent.width

                    Label {
                        text: qsTr("Language")
                    }
                    ComboBox {
                        id: languageComboBox
                        objectName: "languageComboBox"
                        leftPadding: 0
                        textRole: "display"
                        currentIndex: indexForValue(settings.language)

                        Layout.fillWidth: true

                        function indexForValue(value) {
                            for (var i = 0; i < model.length; ++i) {
                                if (model[i].value === value)
                                    return i;
                            }
                            return -1;
                        }

                        model: [
                            {
                                value: "en_GB",
                                display: "English"
                            },
                            {
                                value: "nb_NO",
                                display: "Norsk"
                            },
                            {
                                value: "ar_EG",
                                display: "العربيه"
                            }
                        ]
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
                        leftPadding: 0
                        textRole: "display"
                        // TODO: use findValue() when QTBUG-73491 is implemented
                        currentIndex: indexForValue(settings.penToolRightClickBehaviour)

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

                        function indexForValue(value) {
                            for (var i = 0; i < model.length; ++i) {
                                if (model[i].value === value)
                                    return i;
                            }
                            return -1;
                        }
                    }

                    Label {
                        text: qsTr("Window opacity")
                    }
                    Slider {
                        id: windowOpacitySlider
                        from: 0.5
                        value: settings.windowOpacity
                        to: 1
                        stepSize: 0.05
                        objectName: "windowOpacitySlider"

                        ToolTip.text: qsTr("Changes the opacity of the window. Useful for tracing over an image in another window.")
                        ToolTip.visible: hovered
                        ToolTip.delay: UiConstants.toolTipDelay
                        ToolTip.timeout: UiConstants.toolTipTimeout

                        Binding {
                            target: dialog.ApplicationWindow.window
                            property: "opacity"
                            value: windowOpacitySlider.value
                            when: windowOpacitySlider.pressed
                            restoreMode: Binding.RestoreBindingOrValue
                        }

                        ToolTip {
                            y: -implicitHeight - 12
                            parent: windowOpacitySlider.handle
                            visible: windowOpacitySlider.pressed
                            text: windowOpacitySlider.value.toFixed(1)
                        }
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

                    Label {
                        text: qsTr("Show FPS")
                    }
                    CheckBox {
                        id: showFpsCheckBox
                        leftPadding: 0
                        checked: settings.fpsVisible
                    }

                    Label {
                        text: qsTr("Always show crosshair cursor")
                    }
                    CheckBox {
                        id: alwaysShowCrosshairCheckBox
                        leftPadding: 0
                        checked: settings.alwaysShowCrosshair

                        ToolTip.text: qsTr("Don't hide crosshair cursor when rectangle cursor is visible")
                        ToolTip.visible: hovered
                        ToolTip.delay: UiConstants.toolTipDelay
                        ToolTip.timeout: UiConstants.toolTipTimeout
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

                ViewBorder {
                    anchors.top: parent.top
                }

                ViewBorder {
                    anchors.bottom: parent.bottom
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
                    ShortcutRow {
                        shortcutName: "fullScreenToggleShortcut"
                        shortcutDisplayName: qsTr("Toggle fullscreen")
                    }
                }
            }
        }
    }
}
