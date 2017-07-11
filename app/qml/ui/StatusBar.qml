import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import App 1.0

Pane {
    id: statusBarPane
    objectName: "statusBarPane"
    contentHeight: statusBarLayout.implicitHeight
    padding: 6

    property Project project
    property ImageCanvas canvas
    property FontMetrics fontMetrics

    Rectangle {
        parent: statusBarPane.background
        width: parent.width
        height: 1
        color: "#444"
    }

    Rectangle {
        parent: statusBarPane.background
        x: parent.width
        width: 1
        height: parent.height
        color: "#444"
    }

    RowLayout {
        id: statusBarLayout
        objectName: "statusBarLayout"
        width: canvas ? canvas.firstPane.size * canvas.width - statusBarPane.padding * 2 : parent.width
        visible: project && canvas && project.loaded
        anchors.verticalCenter: parent.verticalCenter

        Label {
            id: pointerIconLabel
            text: "\uf245"
            font.family: "FontAwesome"
            font.pixelSize: fontMetrics.font.pixelSize * 1.2
            horizontalAlignment: Label.AlignHCenter
            anchors.verticalCenter: parent.verticalCenter

            Layout.preferredWidth: Math.max(26, implicitWidth)
        }

        Label {
            id: cursorPixelPosLabel
            objectName: "cursorPixelPosLabel"
            text: {
                if (!canvas)
                    return "-1, -1";

                if (canvas.hasOwnProperty("cursorTilePixelX"))
                    return canvas.cursorTilePixelX + ", " + canvas.cursorTilePixelY;

                return canvas.cursorSceneX + ", " + canvas.cursorSceneY;
            }
            anchors.verticalCenter: parent.verticalCenter

            Layout.minimumWidth: cursorMaxTextMetrics.width
            Layout.maximumWidth: cursorMaxTextMetrics.width

            TextMetrics {
                id: cursorMaxTextMetrics
                font: cursorPixelPosLabel.font
                text: "999, 999"
            }
        }

        ToolSeparator {
            padding: 0
            anchors.verticalCenter: parent.verticalCenter

            Layout.fillHeight: true
            Layout.maximumHeight: 24
        }

        Image {
            id: selectionIcon
            source: "qrc:/images/selection.png"
            anchors.verticalCenter: parent.verticalCenter
            Layout.rightMargin: 6
        }

        Label {
            id: selectionSizeLabel
            objectName: "selectionSizeLabel"
            text: {
                if (!canvas)
                    return "0 x 0";

                return canvas.selectionArea.width + " x " + canvas.selectionArea.height
            }
            anchors.verticalCenter: parent.verticalCenter

            Layout.minimumWidth: selectionAreaMaxTextMetrics.width
            Layout.maximumWidth: selectionAreaMaxTextMetrics.width

            TextMetrics {
                id: selectionAreaMaxTextMetrics
                font: selectionSizeLabel.font
                text: "999 x 999"
            }
        }

        Item {
            Layout.fillWidth: true
        }

        ZoomIndicator {
            objectName: "firstPaneZoomIndicator"
            pane: canvas ? canvas.firstPane : null
            fontMetrics: statusBarPane.fontMetrics
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    ZoomIndicator {
        objectName: "secondPaneZoomIndicator"
        z: 1
        anchors.right: parent.right
        visible: project && canvas && project.loaded && canvas.splitScreen
        pane: canvas ? canvas.secondPane : null
        fontMetrics: statusBarPane.fontMetrics
        anchors.verticalCenter: parent.verticalCenter
    }
}
