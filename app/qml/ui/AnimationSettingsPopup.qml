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
    property Animation animation

    property int originalFps
    property int originalFrameCount
    property int originalFrameX
    property int originalFrameY
    property int originalFrameWidth
    property int originalFrameHeight

    readonly property int controlWidth: 180

    onAboutToShow: {
        originalFps = animation.fps
        originalFrameCount = animation.frameCount
        originalFrameX = animation.frameX
        originalFrameY = animation.frameY
        originalFrameWidth = animation.frameWidth
        originalFrameHeight = animation.frameHeight
    }

    onRejected: {
        animation.fps = originalFps
        animation.frameCount = originalFrameCount
        animation.frameX = originalFrameX
        animation.frameY = originalFrameY
        animation.frameWidth = originalFrameWidth
        animation.frameHeight = originalFrameHeight
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
            value: animation ? animation.frameX : 0
            to: 4096
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("X coordinate of the first frame to animate")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: animation.frameX = value
        }

        Label {
            id: frameYLabel
            text: qsTr("Frame Y")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameYSpinBox"
            from: 0
            value: animation ? animation.frameY : 0
            to: 4096
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Y coordinate of the first frame to animate")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: animation.frameY = value
        }

        Label {
            id: frameWidthLabel
            text: qsTr("Frame Width")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameWidthSpinBox"
            from: 1
            value: animation ? animation.frameWidth : 0
            to: 512
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Width of each frame")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: animation.frameWidth = value
        }

        Label {
            text: qsTr("Frame Height")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameHeightSpinBox"
            from: 1
            value: animation ? animation.frameHeight : 0
            to: 512
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Height of each frame")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: animation.frameHeight = value
        }

        Label {
            text: qsTr("Frame Count")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameCountSpinBox"
            from: 1
            value: animation ? animation.frameCount : 0
            to: 1000
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("The number of frames to animate")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onValueModified: animation.frameCount = value
        }

        Label {
            text: qsTr("FPS")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFpsSpinBox"
            from: 1
            value: animation ? animation.fps : 0
            to: 60
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Frames per second")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            // Update the actual values as the controls are modified so that
            // the user gets a preview of the changes they're making.
            // If the dialog is cancelled, we revert the changes.
            onValueModified: animation.fps = value
        }
    }
}
