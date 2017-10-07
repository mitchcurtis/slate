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

    property ImageCanvas canvas

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

            onHuePicked: canvas[hexColourRowLayout.colourSelector.currentPenName] = saturationLightnessPicker.color

            function updateOurColour() {
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
            alpha: transparencySlider.value

            function updateOurColour() {
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

            onColorPicked: {
                if (!canvas)
                    return;

                canvas[hexColourRowLayout.colourSelector.currentPenName] = saturationLightnessPicker.color
            }
        }
    }

    Image {
        id: opacityLabel
        source: "qrc:/images/opacity.png"

        Layout.alignment: Qt.AlignHCenter

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true

            ToolTip.visible: containsMouse
            ToolTip.text: qsTr("Opacity")
        }
    }

    Slider {
        id: transparencySlider
        objectName: "transparencySlider"
        value: canvas ? canvas[hexColourRowLayout.colourSelector.currentPenName].a : 1
        focusPolicy: Qt.NoFocus

        Layout.fillWidth: true
        Layout.preferredWidth: lightnessRowLayout.implicitWidth

        property bool ignoreChanges: false

        onValueChanged: {
            if (!canvas)
                return;

            ignoreChanges = true;
            canvas[hexColourRowLayout.colourSelector.currentPenName].a = transparencySlider.value;
            ignoreChanges = false;
        }

        function updateOurValue() {
            if (ignoreChanges)
                return;

            transparencySlider.value = canvas[hexColourRowLayout.colourSelector.currentPenName].a;
        }

        Connections {
            target: hexColourRowLayout.colourSelector
            onCurrentPenNameChanged: {
                transparencySlider.ignoreChanges = true;
                transparencySlider.updateOurValue()
                transparencySlider.ignoreChanges = false;
            }
        }

        Connections {
            target: canvas
            onPenForegroundColourChanged: transparencySlider.updateOurValue()
            onPenBackgroundColourChanged: transparencySlider.updateOurValue()
        }
    }

    MenuSeparator {
        topPadding: 0
        bottomPadding: 0

        Layout.columnSpan: 2
        Layout.fillWidth: true
    }

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
        id: lightnessRowLayout

        Button {
            objectName: "lighterButton"
            text: qsTr("-")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: saturationLightnessPicker.decreaseLightness()
        }

        Button {
            objectName: "darkerButton"
            text: qsTr("+")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: saturationLightnessPicker.increaseLightness()
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
            GradientStop { position: 0; color: Ui.CanvasColours.focusColour }
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

            onClicked: saturationLightnessPicker.decreaseSaturation()
        }

        Button {
            objectName: "saturateButton"
            text: qsTr("+")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: saturationLightnessPicker.increaseSaturation()
        }
    }

    Item {
        Layout.columnSpan: 2
        Layout.fillHeight: true
    }
}
