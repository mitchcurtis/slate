import QtQuick 2.14

Item {
    width: 13
    height: 13

    property int size: 1
    property color colour: defaultColour
    property color defaultColour: "#aa000000"

    Rectangle {
        x: 6
        width: 1
        height: 4
        color: colour
    }

    Rectangle {
        x: parent.width / 2 - width / 2
        y: parent.height - height
        width: 1
        height: 4
        color: colour
    }

    Rectangle {
        y: parent.height / 2 - height / 2
        width: 4
        height: 1
        color: colour
    }

    Rectangle {
        x: parent.width - width
        y: parent.height / 2 - height / 2
        width: 4
        height: 1
        color: colour
    }

    Rectangle {
        x: parent.width - width
        y: parent.height / 2 - height / 2
        width: 4
        height: 1
        color: colour
    }

    Rectangle {
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        width: 1
        height: 1
        color: "#aaffffff"
    }
}
