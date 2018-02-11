import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

import App 1.0

import "." as Ui

Rectangle {
    id: root
    objectName: "animationPanel"
    implicitWidth: 800
    implicitHeight: 200
    color: Ui.CanvasColours.panelColour
    clip: true
    enabled: project && animationPlayback

    property Project project
    property AnimationPlayback animationPlayback: project ? project.animationPlayback : null

    TextMetrics {
        id: labelTextMetrics
        font: frameWidthLabel.font
        text: "Frame Height..."
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12

        RowLayout {
            ToolButton {
                text: root.enabled && animationPlayback.playing ? "\uf04c" : "\uf04b"
                font.family: "FontAwesome"
                focusPolicy: Qt.NoFocus
                onClicked: animationPlayback.playing = !animationPlayback.playing
            }
        }

        RowLayout {
            ColumnLayout {
                spacing: 0

                RowLayout {
                    Label {
                        id: frameWidthLabel
                        text: qsTr("Frame Width")

                        Layout.preferredWidth: labelTextMetrics.width
                    }

                    SpinBox {
                        from: 1
                        value: root.enabled ? root.animationPlayback.frameWidth : 0
                        to: 512
                        focusPolicy: Qt.NoFocus

                        Layout.preferredWidth: 160

                        onValueModified: animationPlayback.frameWidth = value
                    }
                }

                RowLayout {
                    Label {
                        text: qsTr("Frame Height")

                        Layout.preferredWidth: labelTextMetrics.width
                    }

                    SpinBox {
                        from: 1
                        value: root.enabled ? animationPlayback.frameHeight : 0
                        to: 512
                        focusPolicy: Qt.NoFocus

                        Layout.preferredWidth: 160

                        onValueModified: animationPlayback.frameHeight = value
                    }
                }

                RowLayout {
                    Label {
                        text: qsTr("Frame Count")

                        Layout.preferredWidth: labelTextMetrics.width
                    }

                    SpinBox {
                        from: 1
                        value: root.enabled ? animationPlayback.frameCount : 0
                        to: 1000
                        focusPolicy: Qt.NoFocus

                        Layout.preferredWidth: 160

                        onValueModified: animationPlayback.frameCount = value
                    }
                }
            }

            SpriteImage {
                project: root.project
                animationPlayback: root.animationPlayback

                Layout.fillHeight: true
            }
        }
    }
}
