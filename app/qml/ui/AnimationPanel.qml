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
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.3

import App 1.0

import "." as Ui

Panel {
    id: root
    objectName: "animationPanel"
    title: qsTr("Animation")
    clip: true
    enabled: project && animationPlayback
    padding: 12

    property Project project
    property AnimationPlayback animationPlayback: project ? project.animationPlayback : null

    settingsPopup: AnimationSettingsPopup {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        parent: root.parent.Window.contentItem
        animationPlayback: root.animationPlayback
    }

    contentItem: Flickable {
        implicitWidth: spritePlayPauseButton.implicitWidth
        implicitHeight: spritePlayPauseButton.implicitHeight
        interactive: spritePlayPauseButton.implicitWidth > width || spritePlayPauseButton.implicitHeight > height

        Button {
            id: spritePlayPauseButton
            focusPolicy: Qt.NoFocus
            width: Math.max(parent.width, implicitWidth)
            height: Math.max(parent.height, implicitHeight)

            onClicked: animationPlayback.playing = !animationPlayback.playing

            background: null
            contentItem: Item {
                implicitWidth: spriteImage.implicitWidth
                implicitHeight: spriteImage.implicitHeight

                SpriteImage {
                    id: spriteImage
                    project: root.project
                    animationPlayback: root.animationPlayback
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                }

                Rectangle {
                    anchors.fill: parent
                    color: "#44000000"
                    opacity: spritePlayPauseButton.hovered ? 1 : 0

                    Behavior on opacity {
                        OpacityAnimator {
                            duration: 150
                        }
                    }

                    Label {
                        id: playPauseIcon
                        text: root.enabled && animationPlayback.playing ? "\uf04c" : "\uf04b"
                        font.family: "FontAwesome"
                        font.pixelSize: 30
                        fontSizeMode: Label.Fit
                        horizontalAlignment: Label.AlignHCenter
                        verticalAlignment: Label.AlignVCenter
                        anchors.fill: parent
                        anchors.margins: 4
                    }
                }
            }
        }
    }
}
