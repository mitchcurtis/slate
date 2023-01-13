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

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import Slate 1.0

import "." as Ui

EditableDelegate {
    id: root
    objectName: model.animation.name + "_Delegate"
    checked: animationSystem && animationSystem.currentAnimationIndex === index

    textField.objectName: "animationNameTextField"
    textField.text: model.animation.name

    property var project
    property ImageCanvas canvas
    readonly property AnimationSystem animationSystem: project ? project.animationSystem : null

    property bool editing

    signal renamed
    signal settingsRequested(int animationIndex, var animation)

    onClicked: project.animationSystem.currentAnimationIndex = index

    textField.onAccepted: {
        animationSystem.editAnimation.name = textField.text
        project.renameAnimation(index)
        // We need to handle the accepted and rejected (escape pressed) signals separately,
        // and also take care of relieving ourselves of focus (which is done by the calling
        // code via the renamed signal), otherwise we'd just respond to editingFinished in
        // order to emit this. (editingFinished is only emitted after we lose focus)
        renamed()
    }
    textField.Keys.onEscapePressed: {
        textField.text = model.animation.name

        renamed()
    }

    SpriteImageContainer {
        id: thumbnailPreview
        objectName: root.objectName + "ThumbnailPreview"
        x: 3
        parent: root.leftSectionLayout
        project: root.project
        animationPlayback: AnimationPlayback {
            objectName: root.objectName + "AnimationPlayback"
            animation: root.editing ? animationSystem.editAnimation : model.animation
            // Fit us in the thumbnail.
            scale: Math.min(thumbnailPreview.width / animation.frameWidth, thumbnailPreview.height / animation.frameHeight)
        }

        Layout.preferredWidth: height
        Layout.fillHeight: true
    }

    ToolButton {
        id: settingsPopupToolButton
        objectName: root.objectName + "AnimationSettingsToolButton"
        text: "\uf1de"
        font.family: "FontAwesome"
        focusPolicy: Qt.NoFocus
        parent: root.rightSectionLayout

        ToolTip.text: qsTr("Configure this animation")
        ToolTip.visible: hovered
        ToolTip.delay: UiConstants.toolTipDelay
        ToolTip.timeout: UiConstants.toolTipTimeout

        onClicked: root.settingsRequested(index, model.animation)
    }
}
