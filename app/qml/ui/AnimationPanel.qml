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
    property ImageCanvas canvas
    property AnimationPlayback animationPlayback: project ? project.animationPlayback : null

    settingsPopup: AnimationSettingsPopup {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        parent: root.parent.Window.contentItem
        animationPlayback: root.animationPlayback

        onClosed: canvas.forceActiveFocus()
    }

    contentItem: ColumnLayout {
        spacing: 0

        Flickable {
            interactive: spriteImageContainer.implicitWidth > width || spriteImageContainer.implicitHeight > height
            clip: true
            contentWidth: spriteImageContainer.implicitWidth
            contentHeight: spriteImageContainer.implicitHeight

            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollBar.horizontal: ScrollBar {}
            ScrollBar.vertical: ScrollBar {}

            Item {
                id: spriteImageContainer
                implicitWidth: spriteImage.implicitWidth * spriteImage.scale
                implicitHeight: spriteImage.implicitHeight * spriteImage.scale

                SpriteImage {
                    id: spriteImage
                    project: root.project
                    animationPlayback: root.animationPlayback
                    scale: animationPlayback ? animationPlayback.scale : 1.0
                    smooth: false
                    anchors.centerIn: parent
                }
            }
        }

        RowLayout {
            // We only use one icon from typicons.
            FontLoader {
                id: fontLoader
                source: "qrc:/fonts/Typicons.ttf"
            }

            ToolButton {
                objectName: "animationPlayPauseButton"
                text: root.enabled && animationPlayback.playing ? "\uf04c" : "\uf04b"
                font.family: "FontAwesome"
                focusPolicy: Qt.NoFocus
                onClicked: animationPlayback.playing = !animationPlayback.playing
            }

            ProgressBar {
                value: animationPlayback ? animationPlayback.currentFrameIndex / (animationPlayback.frameCount - 1) : 0

                Layout.fillWidth: true
            }

            ToolButton {
                objectName: "animationLoopButton"
                text: "\ue800"
                checkable: true
                checked: animationPlayback ? animationPlayback.loop : false
                focusPolicy: Qt.NoFocus
                font.family: fontLoader.name
                onClicked: animationPlayback.loop = checked
            }
        }
    }
}
