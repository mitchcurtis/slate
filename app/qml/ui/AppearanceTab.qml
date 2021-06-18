import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
// TODO: remove in Qt 6
import QtQml 2.15

ColumnLayout {
    function applyChangesToSettings() {
        settings.language = languageComboBox.model[languageComboBox.currentIndex].value
        settings.checkerColour1 = checkerColour1TextField.colour
        settings.checkerColour2 = checkerColour2TextField.colour
        settings.alwaysShowCrosshair = alwaysShowCrosshairCheckBox.checked
        settings.fpsVisible = showFpsCheckBox.checked
        settings.showCurrentLayerInStatusBar = showCurrentLayerInStatusBarCheckBox.checked
        settings.windowOpacity = windowOpacitySlider.value
    }

    function revertToOldSettings() {
        languageComboBox.currentIndex = languageComboBox.indexForValue(settings.language)
        checkerColour1TextField.text = settings.checkerColour1
        checkerColour2TextField.text = settings.checkerColour2
        showFpsCheckBox.checked = settings.fpsVisible
        showCurrentLayerInStatusBarCheckBox.checked = settings.showCurrentLayerInStatusBar
        alwaysShowCrosshairCheckBox.checked = settings.alwaysShowCrosshair
        windowOpacitySlider.value = settings.windowOpacity
    }

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
                    },
                    {
                        value: "zh_CN",
                        display: "简体中文"
                    }
                ]
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
                    // TODO: remove in Qt 6
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
                text: qsTr("Show current layer in status bar")
            }
            CheckBox {
                id: showCurrentLayerInStatusBarCheckBox
                leftPadding: 0
                checked: settings.showCurrentLayerInStatusBar
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
        }
    }
}
