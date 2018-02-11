/*
    Copyright 2018, Mitch Curtis

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

import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0

import App 1.0

import "." as Ui

Panel {
    id: root
    objectName: "animationPanel"
    title: qsTr("Animation")
    clip: true
    enabled: project && animationPlayback

    property Project project
    property AnimationPlayback animationPlayback: project ? project.animationPlayback : null

    TextMetrics {
        id: labelTextMetrics
        font: frameWidthLabel.font
        text: "Frame Height"
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

        SpriteImage {
            project: root.project
            animationPlayback: root.animationPlayback

            Layout.fillHeight: true
        }

        ColumnLayout {
            spacing: 0

            RowLayout {
                Label {
                    id: frameWidthLabel
                    text: qsTr("Width")

                    Layout.preferredWidth: labelTextMetrics.width
                    Layout.fillWidth: true
                }

                SpinBox {
                    from: 1
                    value: root.enabled ? root.animationPlayback.frameWidth : 0
                    to: 512
                    focusPolicy: Qt.NoFocus

                    Layout.fillWidth: true

                    onValueModified: animationPlayback.frameWidth = value
                }
            }

            RowLayout {
                Label {
                    text: qsTr("Height")

                    Layout.preferredWidth: labelTextMetrics.width
                    Layout.fillWidth: true
                }

                SpinBox {
                    from: 1
                    value: root.enabled ? animationPlayback.frameHeight : 0
                    to: 512
                    focusPolicy: Qt.NoFocus

                    Layout.fillWidth: true

                    onValueModified: animationPlayback.frameHeight = value
                }
            }

            RowLayout {
                Label {
                    text: qsTr("Count")

                    Layout.preferredWidth: labelTextMetrics.width
                    Layout.fillWidth: true
                }

                SpinBox {
                    from: 1
                    value: root.enabled ? animationPlayback.frameCount : 0
                    to: 1000
                    focusPolicy: Qt.NoFocus

                    Layout.fillWidth: true

                    onValueModified: animationPlayback.frameCount = value
                }
            }
        }
    }
}
