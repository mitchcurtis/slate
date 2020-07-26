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
    objectName: "animationPreviewSettingsPopup"
    title: qsTr("Animation Preview Settings")
    modal: true
    standardButtons: Dialog.Save | Dialog.Cancel

    property Project project
    property AnimationSystem animationSystem: project ? project.animationSystem : null
    property AnimationPlayback animationPlayback: animationSystem ? animationSystem.currentAnimationPlayback : null

    property real originalPreviewScale

    readonly property int controlWidth: 180

    onAboutToShow: originalPreviewScale = animationPlayback.scale

    onRejected: animationPlayback.scale = originalPreviewScale

    GridLayout {
        rowSpacing: 0
        columns: 2

        Label {
            text: qsTr("Preview Scale")
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
