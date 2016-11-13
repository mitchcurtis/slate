import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import PickAWinner 1.0

import App 1.0

import "." as Ui

GridLayout {
    id: root
    columns: 2
    rowSpacing: 0

    property TileCanvas canvas

    readonly property real spinBoxFactor: 1000
    readonly property real spinBoxStepSize: 10
    readonly property var spinBoxTextFromValueFunc: function(value) {
        return (value / spinBoxFactor).toFixed(2);
    }

    HexColourRowLayout {
        id: hexColourRowLayout
        canvas: root.canvas

        Layout.columnSpan: 2
    }

    RowLayout {
        Layout.columnSpan: 2
        Layout.topMargin: 8
        Layout.bottomMargin: 8

        HueSlider {
            id: hueSlider
            implicitHeight: saturationLightnessPicker.height

            // I know that this is ugly, but I'm just so sick of binding loops that I can't be bothered anymore.
            property bool ignoreChanges: false

            onColorChanged: {
                ignoreChanges = true;
                canvas[hexColourRowLayout.colourSelector.currentPenName] = saturationLightnessPicker.color
                ignoreChanges = false;
            }

            function updateOurColour() {
                if (hueSlider.ignoreChanges) {
                    return;
                }

                hueSlider.hue = canvas[hexColourRowLayout.colourSelector.currentPenName].hslHue;
            }

            Connections {
                target: canvas
                onPenForegroundColourChanged: hueSlider.updateOurColour()
                onPenBackgroundColourChanged: hueSlider.updateOurColour()
            }
        }
        SaturationLightnessPicker {
            id: saturationLightnessPicker
            objectName: "saturationLightnessPicker"
            implicitWidth: 134
            implicitHeight: 134
            focusPolicy: Qt.NoFocus
            hue: hueSlider.hue

            function updateOurColour() {
                if (saturationLightnessPicker.ignoreChanges) {
                    return;
                }

                saturationLightnessPicker.color = canvas[hexColourRowLayout.colourSelector.currentPenName];
            }

            Connections {
                target: canvas
                onPenForegroundColourChanged: saturationLightnessPicker.updateOurColour()
                onPenBackgroundColourChanged: saturationLightnessPicker.updateOurColour()
            }

            Connections {
                target: hexColourRowLayout.colourSelector
                onCurrentPenNameChanged: saturationLightnessPicker.updateOurColour()
            }

            property bool ignoreChanges: false

            onColorChanged: {
                ignoreChanges = true;
                canvas[hexColourRowLayout.colourSelector.currentPenName] = saturationLightnessPicker.color
                ignoreChanges = false;
            }
        }
    }

    property real changeAmount: 0.01

    Label {
        id: lightnessLabel
        text: "\uf185"
        font.family: "FontAwesome"
        horizontalAlignment: Text.AlignHCenter

        Layout.fillWidth: true

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            ToolTip.visible: containsMouse
            ToolTip.text: qsTr("Lightness")
        }
    }

    RowLayout {
        Button {
            objectName: "lighterButton"
            text: qsTr("-")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: saturationLightnessPicker.lightness -= changeAmount
        }

        Button {
            objectName: "darkerButton"
            text: qsTr("+")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: saturationLightnessPicker.lightness += changeAmount
        }
    }

    MenuSeparator {
        topPadding: 0
        bottomPadding: 0

        Layout.columnSpan: 2
        Layout.fillWidth: true
    }

    Rectangle {
        id: saturationLabel
        implicitWidth: lightnessLabel.implicitWidth
        implicitHeight: lightnessLabel.implicitHeight
        gradient: Gradient {
            GradientStop { position: 0; color: Ui.CanvasColours.focusColor }
            GradientStop { position: 1; color: "#ccc" }
        }

        Layout.alignment: Qt.AlignHCenter

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            ToolTip.visible: containsMouse
            ToolTip.text: qsTr("Saturation")
        }
    }

    RowLayout {
        Button {
            objectName: "desaturateButton"
            text: qsTr("-")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: saturationLightnessPicker.saturation -= changeAmount
        }

        Button {
            objectName: "saturateButton"
            text: qsTr("+")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: saturationLightnessPicker.saturation += changeAmount
        }
    }

    Item {
        Layout.columnSpan: 2
        Layout.fillHeight: true
    }
}
