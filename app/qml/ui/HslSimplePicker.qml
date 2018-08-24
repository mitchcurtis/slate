import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import App 1.0

import "." as Ui

GridLayout {
    id: root
    columns: 2
    rowSpacing: 0

    property ImageCanvas canvas
    property Project project

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
        Layout.alignment: Qt.AlignRight

        HueSlider {
            id: hueSlider
            implicitHeight: saturationLightnessPicker.height

            onHuePicked: canvas[hexColourRowLayout.colourSelector.currentPenPropertyName] = saturationLightnessPicker.color

            function updateOurColour() {
                hueSlider.hue = project && canvas ? canvas[hexColourRowLayout.colourSelector.currentPenPropertyName].hslHue : 0;
            }

            Connections {
                target: canvas
                onPenForegroundColourChanged: hueSlider.updateOurColour()
                onPenBackgroundColourChanged: hueSlider.updateOurColour()
            }

            Connections {
                target: root
                onProjectChanged: hueSlider.updateOurColour()
            }
        }
        SaturationLightnessPicker {
            id: saturationLightnessPicker
            objectName: "saturationLightnessPicker"
            implicitWidth: 156
            implicitHeight: 156
            hue: hueSlider.hue
            alpha: transparencySlider.value

            function updateOurColour() {
                saturationLightnessPicker.color = canvas[hexColourRowLayout.colourSelector.currentPenPropertyName];
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

                canvas[hexColourRowLayout.colourSelector.currentPenPropertyName] = saturationLightnessPicker.color
            }
        }
    }

    Control {
        Layout.alignment: Qt.AlignHCenter

        ToolTip.visible: hovered
        ToolTip.text: qsTr("Opacity")

        background: Image {
            source: "qrc:/images/opacity.png"
        }
    }

    Slider {
        id: transparencySlider
        objectName: "transparencySlider"
        value: canvas ? canvas[hexColourRowLayout.colourSelector.currentPenPropertyName].a : 1
        focusPolicy: Qt.NoFocus

        Layout.fillWidth: true
        Layout.preferredWidth: lightnessRowLayout.implicitWidth

        property bool ignoreChanges: false

        onValueChanged: {
            if (!canvas)
                return;

            ignoreChanges = true;
            canvas[hexColourRowLayout.colourSelector.currentPenPropertyName].a = transparencySlider.value;
            ignoreChanges = false;
        }

        function updateOurValue() {
            if (ignoreChanges)
                return;

            transparencySlider.value = canvas[hexColourRowLayout.colourSelector.currentPenPropertyName].a;
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

        ToolTip {
            parent: transparencySlider.handle
            visible: transparencySlider.hovered || transparencySlider.pressed
            text: (transparencySlider.valueAt(transparencySlider.position) * 100).toFixed(1) + "%"
        }
    }

    MenuSeparator {
        topPadding: 0
        bottomPadding: 0

        Layout.columnSpan: 2
        Layout.fillWidth: true
    }

    Control {
        id: lightnessLabel

        contentItem: Label {
            text: "\uf185"
            font.family: "FontAwesome"
            horizontalAlignment: Text.AlignHCenter
        }

        Layout.fillWidth: true

        ToolTip.visible: hovered
        ToolTip.text: qsTr("Lightness")
    }

    RowLayout {
        id: lightnessRowLayout

        Button {
            objectName: "darkerButton"
            text: qsTr("-")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            ToolTip.text: qsTr("Darken the %1 colour").arg(hexColourRowLayout.colourSelector.currentPenName)
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay
            ToolTip.timeout: toolTipTimeout

            onClicked: saturationLightnessPicker.decreaseLightness()
        }

        Button {
            objectName: "lighterButton"
            text: qsTr("+")
            autoRepeat: true
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            ToolTip.text: qsTr("Lighten the %1 colour").arg(hexColourRowLayout.colourSelector.currentPenName)
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay
            ToolTip.timeout: toolTipTimeout

            onClicked: saturationLightnessPicker.increaseLightness()
        }
    }

    MenuSeparator {
        topPadding: 0
        bottomPadding: 0

        Layout.columnSpan: 2
        Layout.fillWidth: true
    }

    Control {
        id: saturationLabel
        implicitWidth: lightnessLabel.implicitWidth
        implicitHeight: lightnessLabel.implicitHeight

        Layout.alignment: Qt.AlignHCenter

        ToolTip.visible: hovered
        ToolTip.text: qsTr("Saturation")

        background: Rectangle {
            gradient: Gradient {
                GradientStop { position: 0; color: Ui.CanvasColours.focusColour }
                GradientStop { position: 1; color: "#ccc" }
            }
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

            ToolTip.text: qsTr("Desaturate the %1 colour").arg(hexColourRowLayout.colourSelector.currentPenName)
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay
            ToolTip.timeout: toolTipTimeout

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

            ToolTip.text: qsTr("Saturate the %1 colour").arg(hexColourRowLayout.colourSelector.currentPenName)
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay
            ToolTip.timeout: toolTipTimeout

            onClicked: saturationLightnessPicker.increaseSaturation()
        }
    }

    Item {
        Layout.columnSpan: 2
        Layout.fillHeight: true
    }
}
