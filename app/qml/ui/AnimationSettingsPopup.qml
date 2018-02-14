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
    property int originalFrameWidth
    property int originalFrameHeight
    property real originalPreviewScale
    readonly property int controlWidth: 180

    onAboutToShow: {
        originalFps = animationPlayback.fps
        originalFrameCount = animationPlayback.frameCount
        originalFrameWidth = animationPlayback.frameWidth
        originalFrameHeight = animationPlayback.frameHeight
        originalPreviewScale = animationPlayback.scale
    }

    onRejected: {
        animationPlayback.fps = originalFps
        animationPlayback.frameCount = originalFrameCount
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
            text: qsTr("FPS")

            Layout.preferredWidth: labelTextMetrics.width
        }

        SpinBox {
            objectName: "animationFpsSpinBox"
            from: 1
            value: animationPlayback ? animationPlayback.fps : 0
            to: 60
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            // Update the actual values as the controls are modified so that
            // the user gets a preview of the changes they're making.
            // If the dialog is cancelled, we revert the changes.
            onValueModified: animationPlayback.fps = value
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
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

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
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

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
            focusPolicy: Qt.NoFocus

            Layout.preferredWidth: controlWidth

            onValueModified: animationPlayback.frameCount = value
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

            onMoved: animationPlayback.scale = value
        }
    }
}
