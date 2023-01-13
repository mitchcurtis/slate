/*
    Copyright 2023, Mitch Curtis

    This file is part of Slate.

    Slate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Slate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Slate. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

import Slate

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
        // Ideally we would set spacing here to avoid having to set margins in individual items,
        // but since we use the zero-width trick to ensure the height of the status bar doesn't jump around,
        // we run into https://bugreports.qt.io/browse/QTBUG-93765.
        spacing: 0

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

        OptionalToolSeparator {
            shown: selectionIcon.visible
        }
        Image {
            id: selectionIcon
            source: "qrc:/images/selection.png"
            visible: canvas && canvas.tool === ImageCanvas.SelectionTool
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
            Layout.leftMargin: 5

            TextMetrics {
                id: selectionAreaMaxTextMetrics
                font: selectionSizeLabel.font
                text: "9999 x 9999"
            }
        }

        OptionalToolSeparator {
            shown: lineLengthLabel.visible
        }
        Rectangle {
            objectName: "lineLengthIcon"
            implicitWidth: 16
            implicitHeight: 1
            visible: canvas && canvas.lineVisible
            color: lineLengthLabel.color

            Rectangle {
                y: -1
                width: 1
                height: 3
                color: lineLengthLabel.color
            }

            Rectangle {
                y: -1
                width: 1
                height: 3
                anchors.right: parent.right
                color: lineLengthLabel.color
            }
        }
        Label {
            id: lineLengthLabel
            objectName: "lineLengthLabel"
            text: canvas ? canvas.lineLength : ""
            visible: canvas && canvas.lineVisible

            Layout.minimumWidth: lineLengthMaxTextMetrics.width
            Layout.maximumWidth: lineLengthMaxTextMetrics.width
            Layout.leftMargin: 5

            TextMetrics {
                id: lineLengthMaxTextMetrics
                font: lineLengthLabel.font
                text: Screen.desktopAvailableWidth
            }
        }

        OptionalToolSeparator {
            shown: lineAngleLabel.visible
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
            objectName: "lineAngleLabel"
            text: canvas ? canvas.lineAngle.toFixed(2) : ""
            visible: canvas && canvas.lineVisible

            Layout.minimumWidth: lineAngleMaxTextMetrics.width
            Layout.maximumWidth: lineAngleMaxTextMetrics.width
            Layout.leftMargin: 5

            TextMetrics {
                id: lineAngleMaxTextMetrics
                font: lineAngleLabel.font
                text: "360.00"
            }
        }

        OptionalToolSeparator {
            shown: fpsCounter.visible
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

        OptionalToolSeparator {
            shown: currentLayerNameLabel.visible
        }
        Image {
            source: "qrc:/images/current-layer.png"
            visible: currentLayerNameLabel.visible
        }
        Label {
            id: currentLayerNameLabel
            objectName: "currentLayerNameLabel"
            text: visible ? project.currentLayer.name : ""
            visible: settings.showCurrentLayerInStatusBar
                && project && project.type === Project.LayeredImageType && project.currentLayer

            Layout.leftMargin: 3
        }

        OptionalToolSeparator {
            shown: toolsForbiddenReasonLabel.visible
        }
        Label {
            text: "\uf06a"
            font.family: "FontAwesome"
            visible: toolsForbiddenReasonLabel.visible
        }
        Label {
            id: toolsForbiddenReasonLabel
            objectName: "toolsForbiddenReasonLabel"
            text: canvas ? canvas.toolsForbiddenReason : ""
            visible: canvas && canvas.toolsForbidden

            Layout.leftMargin: 5
        }

        Item {
            Layout.fillWidth: true
        }

        ZoomIndicator {
            objectName: "firstPaneZoomIndicator"
            pane: canvas ? canvas.firstPane : null

            Layout.rightMargin: 4
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
