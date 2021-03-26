import QtQuick
import App

SaturationLightnessPickerTemplate {
    id: control
    implicitWidth: Math.max(background ? background.implicitWidth : 0, contentItem.implicitWidth)
    implicitHeight: Math.max(background ? background.implicitHeight : 0, contentItem.implicitHeight)

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 200
        color: control.visualFocus ? (control.pressed ? "#cce0ff" : "#f0f6ff") : (control.pressed ? "#d6d6d6" : "#f6f6f6")
        border.color: "#353637"
    }

    contentItem: ShaderEffect {
        width: 64
        height: 64
        scale: contentItem.width / width
        layer.enabled: true
        layer.smooth: true
        anchors.centerIn: parent
        property alias hue: control.hue

        fragmentShader: "qrc:/qml/ui/shaders/SaturationLightness.frag.qsb"
    }
    //! [contentItem]

    handle: Rectangle {
        implicitWidth: 20
        implicitHeight: 20
        radius: 10
        color: "transparent"
        border.color: control.visualFocus ? "#0066ff" : (control.pressed ? "#36383a" : "#454647")
        border.width: 1

        Rectangle {
            x: 1
            y: 1
            width: 18
            height: 18
            radius: 9
            color: "transparent"
            border.color: control.visualFocus ? "#0066ff" : (control.pressed ? "#86888a" : "#959697")
            border.width: 2
        }
    }
}
