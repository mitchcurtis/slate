/*
    Copyright 2019, Mitch Curtis

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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.1
import QtQuick.Window 2.12

import Qt.labs.platform 1.0 as Platform

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

    readonly property int minimumUsefulHeight: header.implicitHeight + 100

    UiStateSerialisation {
        project: root.project
        onReadyToLoad: root.expanded = root.project.uiState.value("animationPanelExpanded", true)
        onReadyToSave: root.project.uiState.setValue("animationPanelExpanded", root.expanded)
    }

    Platform.FileDialog {
        id: saveGifDialog
        objectName: "saveGifDialog"
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: ["GIF files (*.gif)"]
        defaultSuffix: "gif"
        onAccepted: project.exportGif(file)
    }

    settingsPopup: AnimationSettingsPopup {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        parent: root.parent.Window.contentItem
        animationPlayback: root.animationPlayback

        onClosed: canvas.forceActiveFocus()
    }

    contentItem: ColumnLayout {
        visible: root.expanded
        spacing: 0

        Flickable {
            objectName: "animationPanelFlickable"
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

        Ui.VerticalSeparator {
            padding: 6
            topPadding: 0
            bottomPadding: 0

            Layout.fillWidth: true
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
                objectName: "animationProgressBar"
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

            ToolButton {
                objectName: "exportGifButton"
                text: "\uf019"
                font.family: "FontAwesome"
                focusPolicy: Qt.NoFocus

                ToolTip.text: qsTr("Export this animation as a GIF")
                ToolTip.visible: hovered
                ToolTip.delay: UiConstants.toolTipDelay
                ToolTip.timeout: UiConstants.toolTipTimeout

                onClicked: saveGifDialog.open()
            }
        }
    }
}
