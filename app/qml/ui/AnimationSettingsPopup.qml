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
            objectName: "animationPreviewScaleSlider"
            from: 0.5
            value: animationPlayback ? animationPlayback.scale : 1.0
            to: 10
            leftPadding: 0
            rightPadding: 0
            focusPolicy: Qt.NoFocus

            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: controlWidth - 30

            ToolTip.text: qsTr("The scale of the animation in the preview")
            ToolTip.visible: hovered

            onMoved: animationPlayback.scale = value
        }
    }
}
