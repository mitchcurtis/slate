/*
    Copyright 2020, Mitch Curtis

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
    // TODO: check if we actually need this, and remove from every panel if not
    clip: true
    enabled: project && animationPlayback
    padding: 12
    leftPadding: 0
    rightPadding: 0

    property Project project
    property ImageCanvas canvas
    property AnimationSystem animationSystem: project ? project.animationSystem : null
    property Animation currentAnimation: project ? animationSystem.currentAnimation : null
    property AnimationPlayback animationPlayback: project ? animationSystem.currentAnimationPlayback : null
    readonly property int currentAnimationIndex: animationSystem ? animationSystem.currentAnimationIndex : -1

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

    settingsPopup: AnimationPreviewSettingsPopup {
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        parent: root.parent.Window.contentItem
        project: root.project

        onClosed: canvas.forceActiveFocus()
    }

    AnimationSettingsPopup {
        id: animationSettingsPopup
        anchors.centerIn: Overlay.overlay
        project: root.project

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
            Layout.leftMargin: root.padding
            Layout.rightMargin: root.padding
            Layout.fillHeight: true

            ScrollBar.horizontal: ScrollBar {}
            ScrollBar.vertical: ScrollBar {}

            SpriteImageContainer {
                id: spriteImageContainer
                objectName: "animationPreviewContainer"
                project: root.project
                animationPlayback: root.animationPlayback
            }
        }

        Ui.VerticalSeparator {
            padding: 6
            topPadding: 4
            bottomPadding: 0

            Layout.fillWidth: true
        }

        RowLayout {
            enabled: root.animationSystem && root.animationSystem.currentAnimation

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

            Slider {
                id: animationSeekSlider
                objectName: "animationSeekSlider"
                value: animationPlayback && currentAnimation ? animationPlayback.currentFrameIndex : 0
                to: animationPlayback && currentAnimation ? currentAnimation.frameCount - 1 : 1
                stepSize: 1
                focusPolicy: Qt.NoFocus
                onMoved: animationPlayback.currentFrameIndex = value

                Layout.fillWidth: true

                ToolTip {
                    parent: animationSeekSlider.handle
                    text: qsTr("Frame %1").arg(animationSeekSlider.value + 1)
                    visible: animationPlayback && currentAnimation
                        && (animationSeekSlider.hovered || animationSeekSlider.pressed)
                }
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

        Ui.VerticalSeparator {
            padding: 6
            topPadding: 0
            bottomPadding: 4

            Layout.fillWidth: true
        }

        ButtonGroup {
            objectName: "animationPanelButtonGroup"
            buttons: animationListView.contentItem.children
        }

        ListView {
            id: animationListView
            objectName: "animationListView"
            boundsBehavior: ListView.StopAtBounds
            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollBar.vertical: ScrollBar {}

            model: AnimationModel {
                animationSystem: root.project ? root.project.animationSystem : null
            }

            delegate: AnimationDelegate {
                width: animationListView.width
                project: root.project
                editing: animationSettingsPopup.animationIndex === index

                onRenamed: root.canvas.forceActiveFocus()
                onSettingsRequested: {
                    animationSettingsPopup.animationIndex = animationIndex
                    animationSettingsPopup.animation = animation
                    animationSettingsPopup.open()
                }
            }
        }

        Ui.VerticalSeparator {
            padding: 6
            topPadding: 0
            bottomPadding: 0

            Layout.fillWidth: true
        }
    }

    footer: RowLayout {
        visible: root.expanded

        RowActionButton {
            objectName: "newAnimationButton"

            Layout.leftMargin: 6

            ToolTip.text: qsTr("Add a new animation")

            onClicked: {
                project.addAnimation()
                animationListView.currentIndex = animationListView.count - 1
                animationSystem.currentAnimationIndex = animationListView.currentIndex
            }
        }

        RowActionButton {
            objectName: "moveAnimationDownButton"
            text: "\uf107"
            font.family: "FontAwesome"
            enabled: root.animationSystem && root.currentAnimationIndex < root.animationSystem.animationCount - 1

            ToolTip.text: qsTr("Move the current animation down")

            onClicked: project.moveCurrentAnimationDown()
        }

        RowActionButton {
            objectName: "moveAnimationUpButton"
            text: "\uf106"
            font.family: "FontAwesome"
            enabled: root.animationSystem && root.currentAnimationIndex > 0

            ToolTip.text: qsTr("Move the current animation up")

            onClicked: project.moveCurrentAnimationUp()
        }

        RowActionButton {
            objectName: "duplicateAnimationButton"
            text: "\uf24d"
            font.family: "FontAwesome"
            enabled: root.animationSystem && root.currentAnimationIndex >= 0 && root.currentAnimationIndex < root.animationSystem.animationCount

            ToolTip.text: qsTr("Duplicate the animation layer")

            onClicked: project.duplicateAnimation(root.currentAnimationIndex)
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowActionButton {
            objectName: "deleteAnimationButton"
            text: "\uf1f8"
            font.family: "FontAwesome"
            enabled: currentAnimation

            ToolTip.text: qsTr("Delete the current animation")

            onClicked: project.removeAnimation(currentAnimationIndex)
        }
    }
}
