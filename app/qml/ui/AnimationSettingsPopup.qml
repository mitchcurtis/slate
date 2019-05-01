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

import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import App 1.0

Dialog {
    id: root
    objectName: "animationSettingsPopup"
    title: qsTr("Animation Settings")
    modal: true
    standardButtons: Dialog.Save | Dialog.Cancel

    property AnimationPlayback animationPlayback

    property int originalFps
    property int originalFrameCount
    property int originalFrameX
    property int originalFrameY
    property int originalFrameWidth
    property int originalFrameHeight
    property real originalPreviewScale
    readonly property int controlWidth: 180

    onAboutToShow: {
        originalFps = animationPlayback.fps
        originalFrameCount = animationPlayback.frameCount
        originalFrameX = animationPlayback.frameX
        originalFrameY = animationPlayback.frameY
        originalFrameWidth = animationPlayback.frameWidth
        originalFrameHeight = animationPlayback.frameHeight
        originalPreviewScale = animationPlayback.scale
    }

    onRejected: {
        animationPlayback.fps = originalFps
        animationPlayback.frameCount = originalFrameCount
        animationPlayback.frameX = originalFrameX
        animationPlayback.frameY = originalFrameY
        animationPlayback.frameWidth = originalFrameWidth
        animationPlayback.frameHeight = originalFrameHeight
        animationPlayback.scale = originalPreviewScale
    }

    TextMetrics {
        id: labelTextMetrics
        font: frameWidthLabel.font
        text: "Frame Height......"
    }

    GridLayout {
        rowSpacing: 0
        columns: 2

        Label {
            id: frameXLabel
            text: qsTr("Frame X")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameXSpinBox"
            from: 0
            value: animationPlayback ? animationPlayback.frameX : 0
            to: 4096
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("X coordinate of the first frame to animate")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onValueModified: animationPlayback.frameX = value
        }

        Label {
            id: frameYLabel
            text: qsTr("Frame Y")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameYSpinBox"
            from: 0
            value: animationPlayback ? animationPlayback.frameY : 0
            to: 4096
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Y coordinate of the first frame to animate")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onValueModified: animationPlayback.frameY = value
        }

        Label {
            id: frameWidthLabel
            text: qsTr("Frame Width")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameWidthSpinBox"
            from: 1
            value: animationPlayback ? animationPlayback.frameWidth : 0
            to: 512
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Width of each frame")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onValueModified: animationPlayback.frameWidth = value
        }

        Label {
            text: qsTr("Frame Height")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameHeightSpinBox"
            from: 1
            value: animationPlayback ? animationPlayback.frameHeight : 0
            to: 512
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Height of each frame")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onValueModified: animationPlayback.frameHeight = value
        }

        Label {
            text: qsTr("Frame Count")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFrameCountSpinBox"
            from: 1
            value: animationPlayback ? animationPlayback.frameCount : 0
            to: 1000
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("The number of frames to animate")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            onValueModified: animationPlayback.frameCount = value
        }

        Label {
            text: qsTr("FPS")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFpsSpinBox"
            from: 1
            value: animationPlayback ? animationPlayback.fps : 0
            to: 60
            editable: true
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            ToolTip.text: qsTr("Frames per second")
            ToolTip.visible: hovered
            ToolTip.delay: toolTipDelay

            // Update the actual values as the controls are modified so that
            // the user gets a preview of the changes they're making.
            // If the dialog is cancelled, we revert the changes.
            onValueModified: animationPlayback.fps = value
        }

        Label {
            text: qsTr("Preview Scale")

            Layout.preferredWidth: labelTextMetrics.width
        }

        Slider {
            id: animationPreviewScaleSlider
            objectName: "animationPreviewScaleSlider"
            from: 0.5
            value: animationPlayback ? animationPlayback.scale : 1.0
            to: 10
            stepSize: 0.01
            leftPadding: 0
            rightPadding: 0
            focusPolicy: Qt.NoFocus
            transformOrigin: Item.TopLeft

            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: controlWidth - 30

            onMoved: animationPlayback.scale = value

            ToolTip {
                parent: animationPreviewScaleSlider.handle
                visible: animationPreviewScaleSlider.hovered
                text: animationPreviewScaleSlider.valueAt(animationPreviewScaleSlider.position).toFixed(2)
            }
        }
    }
}
