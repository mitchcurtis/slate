import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import App 1.0

Item {
    id: canvasContainer
    objectName: "canvasContainer"

    property Project project
    property ImageCanvas canvas: loader.item
    property var checkedToolButton

    FontMetrics {
        id: fontMetrics
    }

    Loader {
        id: loader
        objectName: "canvasContainerLoader"
        source: project && project.typeString.length > 0 ? project.typeString + "Canvas.qml" : ""
        focus: true
        anchors.fill: parent
    }

    CrosshairCursor {
        id: crosshairCursor
        x: canvas ? canvas.cursorX - width / 2 : 0
        y: canvas ? canvas.cursorY - height / 2 : 0
        z: 1
        visible: canvas && canvas.hasBlankCursor && !canvas.useIconCursor && canvas.useCrosshairCursor
    }

    RectangularCursor {
        id: rectangleCursor
        x: canvas ? Math.floor(canvas.cursorX - width / 2) : 0
        y: canvas ? Math.floor(canvas.cursorY - height / 2) : 0
        z: 1
        width: canvas ? canvas.toolSize * canvas.currentPaneZoomLevel : 0
        height: canvas ? canvas.toolSize * canvas.currentPaneZoomLevel : 0
        visible: canvas && canvas.hasBlankCursor && !canvas.useIconCursor && !canvas.useCrosshairCursor
    }

    Label {
        id: iconCursor
        x: canvas ? canvas.cursorX : 0
        y: canvas ? canvas.cursorY - height + 3 : 0
        z: 1
        visible: canvas && canvas.hasBlankCursor && canvas.useIconCursor
        text: visible && checkedToolButton ? checkedToolButton.iconText : ""
        font.family: "FontAwesome"
        color: "#ccc"
    }

    Pane {
        id: statusBarPane
        objectName: "statusBarPane"
        z: 1
        width: parent.width
        contentHeight: statusBarLayout.implicitHeight
        padding: 6
        anchors.bottom: parent.bottom

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
                fontMetrics: fontMetrics
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        ZoomIndicator {
            objectName: "secondPaneZoomIndicator"
            z: 1
            anchors.right: parent.right
            visible: project && canvas && project.loaded && canvas.splitScreen
            pane: canvas ? canvas.secondPane : null
            fontMetrics: fontMetrics
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
