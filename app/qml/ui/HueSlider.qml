import QtQuick 2.14
import QtQuick.Layouts 1.14

Rectangle {
    id: root
    objectName: "hueSlider"
    implicitWidth: 30
    implicitHeight: 200

    property real hue: 0

    signal huePicked()

    gradient: HueGradient {}

    Item {
        x: -width + 5
        y: hue * (root.height) - height / 2
        width: 14
        height: 14
        clip: true

        Rectangle {
            x: -parent.width / 2
            rotation: 45
            width: parent.width
            height: parent.height
            color: "#bbb"
        }
    }

    MouseArea {
        anchors.fill: parent
        onMouseYChanged: {
            if (containsMouse && pressed) {
                hue = mouseY / height;
                root.huePicked();
            }
        }
    }
}
