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
import QtQuick.Layouts 1.3

import App 1.0

Dialog {
    id: root
    objectName: "animationSettingsPopup"
    title: qsTr("Animation Settings")
    modal: true
    standardButtons: Dialog.Save | Dialog.Cancel

    property Project project
    property AnimationSystem animationSystem: project ? project.animationSystem : null
    property int animationIndex: -1
    property Animation animation

    property bool ignoreChanges
    // We want to use Keys.onReturnPressed to accept the dialog,
    // but by the time it's emitted, the value hasn't been updated,
    // and e.g. callLater() happens too soon to be used as a workaround.
    // We also can't use valueFromText() because of https://bugreports.qt.io/browse/QTBUG-64151.
    property bool acceptOnNextModify

    readonly property int controlWidth: 180

    onAboutToShow: {
        // Unfortunately we need to do this, as SpinBox's valueModified signal
        // isn't emitted when text is edited, only when it's accepted, and we
        // want a live update, so we need to respond to valueChanged instead.
        // Use textEdited eventually: https://bugreports.qt.io/browse/QTBUG-85739
        ignoreChanges = true

        animationSystem.editAnimation.name = animation.name
        animationSystem.editAnimation.fps = animation.fps
        animationSystem.editAnimation.frameCount = animation.frameCount
        animationSystem.editAnimation.frameX = animation.frameX
        animationSystem.editAnimation.frameY = animation.frameY
        animationSystem.editAnimation.frameWidth = animation.frameWidth
        animationSystem.editAnimation.frameHeight = animation.frameHeight
        animationSystem.editAnimation.reverse = animation.reverse

        ignoreChanges = false
    }

    onAccepted: project.modifyAnimation(animationIndex)

    onClosed: {
        animationIndex = -1
        animation = null
        acceptOnNextModify = false
    }

    TextMetrics {
        id: labelTextMetrics
        font: frameWidthLabel.font
        text: "Frame Height......"
    }

    GridLayout {
        rowSpacing: 0
        columns: 2
        enabled: root.animation

        Label {
            id: frameXLabel
            text: qsTr("Frame X")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameXSpinBox"
            from: 0
            value: animationSystem ? animationSystem.editAnimation.frameX : 0
            to: 4096
            editable: true
            focusPolicy: Qt.TabFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("X coordinate of the first frame to animate")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            // TODO: use textEdited when implemented: https://bugreports.qt.io/browse/QTBUG-85739
            onValueModified: {
                animationSystem.editAnimation.frameX = value
                if (acceptOnNextModify)
                    root.accept()
            }

            Keys.onReturnPressed: acceptOnNextModify = true
        }

        Label {
            id: frameYLabel
            text: qsTr("Frame Y")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            id: animationFrameYSpinBox
            objectName: "animationFrameYSpinBox"
            from: 0
            value: animationSystem ? animationSystem.editAnimation.frameY : 0
            to: 4096
            editable: true
            focusPolicy: Qt.TabFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Y coordinate of the first frame to animate")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: {
                animationSystem.editAnimation.frameY = value
                if (acceptOnNextModify)
                    root.accept()
            }

            Keys.onReturnPressed: acceptOnNextModify = true
        }

        Label {
            id: frameWidthLabel
            text: qsTr("Frame Width")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameWidthSpinBox"
            from: 1
            value: animationSystem ? animationSystem.editAnimation.frameWidth : 0
            to: 512
            editable: true
            focusPolicy: Qt.TabFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Width of each frame")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: {
                animationSystem.editAnimation.frameWidth = value
                if (acceptOnNextModify)
                    root.accept()
            }

            Keys.onReturnPressed: acceptOnNextModify = true
        }

        Label {
            text: qsTr("Frame Height")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameHeightSpinBox"
            from: 1
            value: animationSystem ? animationSystem.editAnimation.frameHeight : 0
            to: 512
            editable: true
            focusPolicy: Qt.TabFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Height of each frame")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: {
                animationSystem.editAnimation.frameHeight = value
                if (acceptOnNextModify)
                    root.accept()
            }

            Keys.onReturnPressed: acceptOnNextModify = true
        }

        Label {
            text: qsTr("Frame Count")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameCountSpinBox"
            from: 1
            value: animationSystem ? animationSystem.editAnimation.frameCount : 0
            to: 1000
            editable: true
            focusPolicy: Qt.TabFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("The number of frames to animate")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: {
                animationSystem.editAnimation.frameCount = value
                if (acceptOnNextModify)
                    root.accept()
            }

            Keys.onReturnPressed: acceptOnNextModify = true
        }

        Label {
            text: qsTr("FPS")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFpsSpinBox"
            from: 1
            value: animationSystem ? animationSystem.editAnimation.fps : 0
            to: 60
            editable: true
            focusPolicy: Qt.TabFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Frames per second")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: {
                animationSystem.editAnimation.fps = value
                if (acceptOnNextModify)
                    root.accept()
            }

            Keys.onReturnPressed: acceptOnNextModify = true
        }

        Label {
            text: qsTr("Reverse")
        }

        CheckBox {
            objectName: "animationReverseCheckBox"
            checked: animationSystem ? animationSystem.editAnimation.reverse : false
            focusPolicy: Qt.TabFocus

            ToolTip.text: qsTr("Reverse the animation")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onClicked: animationSystem.editAnimation.reverse = checked
        }
    }
}
