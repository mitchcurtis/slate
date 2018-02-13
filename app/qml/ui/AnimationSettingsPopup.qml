import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import App 1.0

Dialog {
    id: root
    title: qsTr("Animation Settings")
    modal: true
    standardButtons: Dialog.Save | Dialog.Cancel

    property AnimationPlayback animationPlayback

    property int originalFps
    property int originalFrameCount
    property int originalFrameWidth
    property int originalFrameHeight

    onAboutToShow: {
        originalFps = animationPlayback.fps
        originalFrameCount = animationPlayback.frameCount
        originalFrameWidth = animationPlayback.frameWidth
        originalFrameHeight = animationPlayback.frameHeight
    }

    onRejected: {
        animationPlayback.fps = originalFps
        animationPlayback.frameCount = originalFrameCount
        animationPlayback.frameWidth = originalFrameWidth
        animationPlayback.frameHeight = originalFrameHeight
    }

    TextMetrics {
        id: labelTextMetrics
        font: frameWidthLabel.font
        text: "Frame Height"
    }

    ColumnLayout {
        spacing: 0

        RowLayout {
            Label {
                id: frameWidthLabel
                text: qsTr("Width")

                Layout.preferredWidth: labelTextMetrics.width
                Layout.fillWidth: true
            }

            SpinBox {
                from: 1
                value: animationPlayback ? animationPlayback.frameWidth : 0
                to: 512
                focusPolicy: Qt.NoFocus

                Layout.fillWidth: true

                // Update the actual values as the controls are modified so that
                // the user gets a preview of the changes they're making.
                // If the dialog is cancelled, we revert the changes.
                onValueModified: animationPlayback.frameWidth = value
            }
        }

        RowLayout {
            Label {
                text: qsTr("Height")

                Layout.preferredWidth: labelTextMetrics.width
                Layout.fillWidth: true
            }

            SpinBox {
                from: 1
                value: animationPlayback ? animationPlayback.frameHeight : 0
                to: 512
                focusPolicy: Qt.NoFocus

                Layout.fillWidth: true

                onValueModified: animationPlayback.frameHeight = value
            }
        }

        RowLayout {
            Label {
                text: qsTr("Count")

                Layout.preferredWidth: labelTextMetrics.width
                Layout.fillWidth: true
            }

            SpinBox {
                from: 1
                value: animationPlayback ? animationPlayback.frameCount : 0
                to: 1000
                focusPolicy: Qt.NoFocus

                Layout.fillWidth: true

                onValueModified: animationPlayback.frameCount = value
            }
        }
    }
}
