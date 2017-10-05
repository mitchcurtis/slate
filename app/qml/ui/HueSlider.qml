import QtQuick 2.6
import QtQuick.Layouts 1.3

Rectangle {
    id: root
    implicitWidth: 30
    implicitHeight: 200

    property real hue: 0

    signal huePicked()

    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#ff0000"
        }
        GradientStop {
            position: 0.166666
            color: "#ffff00"
        }
        GradientStop {
            position: 0.333333
            color: "#00ff00"
        }
        GradientStop {
            position: 0.5
            color: "#00ffff"
        }
        GradientStop {
            position: 0.666666
            color: "#0000ff"
        }
        GradientStop {
            position: 0.833333
            color: "#ff00ff"
        }
        GradientStop {
            position: 1
            color: "#ff0000"
        }
    }

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
