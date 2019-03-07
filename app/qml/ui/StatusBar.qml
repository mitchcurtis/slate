import QtQuick 2.9
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import App 1.0

Pane {
    id: statusBarPane
    objectName: "statusBarPane"
    contentHeight: statusBarLayout.implicitHeight
    padding: 6

    property Project project
    property ImageCanvas canvas

    Rectangle {
        parent: statusBarPane.background
        width: parent.width
        height: 1
        color: "#444"
    }

    Rectangle {
        parent: statusBarPane.background
        x: parent.width - 1
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
            font.pixelSize: Qt.application.font.pixelSize * 1.2
            horizontalAlignment: Label.AlignHCenter

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

            // Specify a fixed size to avoid causing items to the right of us jumping
            // around when we would be resized due to changes in our text.
            Layout.minimumWidth: cursorMaxTextMetrics.width
            Layout.maximumWidth: cursorMaxTextMetrics.width

            TextMetrics {
                id: cursorMaxTextMetrics
                font: cursorPixelPosLabel.font
                text: "9999, 9999"
            }
        }

        ToolSeparator {
            padding: 0
            // Use opacity rather than visible, as it's an easy way of ensuring that the RowLayout
            // always has a minimum height equal to the tallest item (assuming that that's us)
            // and hence doesn't jump around when we become hidden.
            // There's always at least one label and icon visible at all times (cursor pos),
            // so we don't have to worry about those.
            opacity: (fpsCounter.visible || lineLengthLabel.visible || selectionSizeLabel.visible) ? 1 : 0

            Layout.fillHeight: true
            Layout.maximumHeight: 24
        }

        Image {
            id: selectionIcon
            source: "qrc:/images/selection.png"
            visible: canvas && canvas.tool === ImageCanvas.SelectionTool

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
            visible: canvas && canvas.tool === ImageCanvas.SelectionTool

            Layout.minimumWidth: selectionAreaMaxTextMetrics.width
            Layout.maximumWidth: selectionAreaMaxTextMetrics.width

            TextMetrics {
                id: selectionAreaMaxTextMetrics
                font: selectionSizeLabel.font
                text: "9999 x 9999"
            }
        }

        Rectangle {
            implicitWidth: 16
            implicitHeight: 1
            visible: canvas && canvas.lineVisible

            Rectangle {
                y: -1
                width: 1
                height: 3
            }

            Rectangle {
                y: -1
                width: 1
                height: 3
                anchors.right: parent.right
            }
        }

        Label {
            id: lineLengthLabel
            text: canvas ? canvas.lineLength : ""
            visible: canvas && canvas.lineVisible

            Layout.minimumWidth: lineLengthMaxTextMetrics.width
            Layout.maximumWidth: lineLengthMaxTextMetrics.width

            TextMetrics {
                id: lineLengthMaxTextMetrics
                font: lineLengthLabel.font
                text: Screen.desktopAvailableWidth
            }
        }

        Canvas {
            implicitWidth: 16
            implicitHeight: 16
            visible: canvas && canvas.lineVisible

            onPaint: {
                var ctx = getContext("2d");
                ctx.moveTo(0, height - 1);
                ctx.lineTo(width, height - 1);
                ctx.strokeStyle = lineLengthLabel.color;
                ctx.stroke();
                ctx.closePath();

                ctx.beginPath();
                ctx.moveTo(0, height - 1);
                ctx.lineTo(Math.round(width * 0.8), Math.round(height * 0.2));
                ctx.stroke();
                ctx.closePath();

                ctx.beginPath();
                ctx.arc(width * 0.3, height * .8, width / 3, -1.1, -5.8, false);
                ctx.stroke();
                ctx.closePath();
            }
        }

        Label {
            id: lineAngleLabel
            text: canvas ? canvas.lineAngle.toFixed(2) : ""
            visible: canvas && canvas.lineVisible

            Layout.minimumWidth: lineAngleMaxTextMetrics.width
            Layout.maximumWidth: lineAngleMaxTextMetrics.width

            TextMetrics {
                id: lineAngleMaxTextMetrics
                font: lineAngleLabel.font
                text: "360.00"
            }
        }

        ToolSeparator {
            padding: 0
            visible: fpsCounter.visible && (lineLengthLabel.visible || selectionSizeLabel.visible)

            Layout.fillHeight: true
            Layout.maximumHeight: 24
        }

        FpsCounter {
            id: fpsCounter
            visible: settings.fpsVisible

            Layout.minimumWidth: fpsMaxTextMetrics.width
            Layout.maximumWidth: fpsMaxTextMetrics.width

            TextMetrics {
                id: fpsMaxTextMetrics
                font: fpsCounter.font
                text: "100 FPS"
            }
        }

        Item {
            Layout.fillWidth: true
        }

        ZoomIndicator {
            objectName: "firstPaneZoomIndicator"
            pane: canvas ? canvas.firstPane : null
        }
    }

    ZoomIndicator {
        objectName: "secondPaneZoomIndicator"
        z: 1
        anchors.right: parent.right
        visible: project && canvas && project.loaded && canvas.splitScreen
        pane: canvas ? canvas.secondPane : null
        anchors.verticalCenter: parent.verticalCenter
    }
}
