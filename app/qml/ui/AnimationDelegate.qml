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
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import App 1.0

import "." as Ui

ItemDelegate {
    id: root
    objectName: model.animation.name + "_Delegate"
    checkable: false
    checked: animationSystem && animationSystem.currentAnimationIndex === index
    implicitHeight: Math.max(implicitBackgroundHeight,
        Math.max(animationNameTextField.implicitHeight, settingsPopupToolButton.implicitHeight) + topPadding + bottomPadding)
    leftPadding: thumbnailPreview.width + 18
    topPadding: 0
    bottomPadding: 0
    focusPolicy: Qt.NoFocus

    property var project
    property ImageCanvas canvas
    readonly property AnimationSystem animationSystem: project ? project.animationSystem : null

    property bool editing

    signal renamed
    signal settingsRequested(int animationIndex, var animation)

    // The checked binding gets broken when clicking on an already-current item, and even Binding
    // is not enough to restore it apparently (though somehow, LayerDelegate works just fine).
    // TODO: investigate this ^
    // So, we just allow having no current animation, even when there is more than one.
    onClicked: project.animationSystem.currentAnimationIndex = project.animationSystem.currentAnimationIndex === index ? -1 : index
    onDoubleClicked: animationNameTextField.forceActiveFocus()

    SpriteImageContainer {
        id: thumbnailPreview
        objectName: root.objectName + "ThumbnailPreview"
        project: root.project
        animationPlayback: AnimationPlayback {
            objectName: root.objectName + "AnimationPlayback"
            animation: root.editing ? animationSystem.editAnimation : model.animation
            // Fit us in the thumbnail.
            scale: Math.min(thumbnailPreview.width / animation.frameWidth, thumbnailPreview.height / animation.frameHeight)
        }
        width: height
        height: parent.height
    }

    // TODO: try to move this whole delegate into EditableDelegate.qml
    // so LayerDelegate and AnimationDelegate can reuse the code
    TextField {
        id: animationNameTextField
        objectName: "animationNameTextField"
        text: model.animation.name
        font.family: "FontAwesome"
        activeFocusOnPress: false
        font.pixelSize: 12
        visible: false
        anchors.left: thumbnailPreview.right
        anchors.leftMargin: 4
        anchors.right: settingsPopupToolButton.left
        anchors.rightMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 6
        background.visible: false

        onAccepted: {
            model.animation.name = text
            animationSystem.animationModified(index)
            // We need to handle the accepted and rejected (escape pressed) signals separately,
            // and also take care of relieving ourselves of focus (which is done by the calling
            // code via the renamed signal), otherwise we'd just respond to editingFinished in
            // order to emit this. (editingFinished is only emitted after we lose focus)
            root.renamed()
        }

        Keys.onEscapePressed: {
            text = model.animation.name

            root.renamed()
        }
    }

    ToolButton {
        id: settingsPopupToolButton
        objectName: root.objectName + "AnimationSettingsToolButton"
        text: "\uf1de"
        font.family: "FontAwesome"
        focusPolicy: Qt.NoFocus
        width: implicitHeight
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        ToolTip.text: qsTr("Configure this animation")
        ToolTip.visible: hovered
        ToolTip.delay: UiConstants.toolTipDelay
        ToolTip.timeout: UiConstants.toolTipTimeout

        onClicked: root.settingsRequested(index, model.animation)
    }

    // We don't want TextField's editable cursor to be visible,
    // so we set visible: false to disable the cursor, and instead
    // render it via this.
    ShaderEffectSource {
        sourceItem: animationNameTextField
        anchors.fill: animationNameTextField
    }

    // Apparently the one above only works for the top level control item,
    // so we also need one for the background.
    ShaderEffectSource {
        sourceItem: animationNameTextField.background
        x: animationNameTextField.x + animationNameTextField.background.x
        y: animationNameTextField.y + animationNameTextField.background.y
        width: animationNameTextField.background.width
        height: animationNameTextField.background.height
        visible: animationNameTextField.activeFocus
    }

    Rectangle {
        id: focusRect
        width: 2
        height: parent.height
        color: Ui.Theme.focusColour
        visible: parent.checked
    }
}
