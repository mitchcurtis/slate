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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {
    id: root
    implicitHeight: swatchColourLayout.implicitHeight

    property alias probabilitySlider: texturedFillSwatchColourProbabilitySlider

    signal deleted

    function highlight() {
        highlightAnimation.start()
    }

    Rectangle {
        id: highlightRect
        anchors.fill: parent
        opacity: 0

        NumberAnimation {
            id: highlightAnimation
            target: highlightRect
            property: "opacity"
            from: 0.4
            to: 0
            duration: 1400
            easing.type: Easing.InOutQuad
        }
    }

    RowLayout {
        id: swatchColourLayout
        width: parent.width

        Rectangle {
            id: colourDelegate
            width: 16
            height: 16
            color: model.colour

            ToolTip.text: model.colourHexName
            ToolTip.visible: delegateHoverHandler.hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            HoverHandler {
                id: delegateHoverHandler
            }
        }

        Label {
            text: "Probability"

            Layout.leftMargin: 4
        }

        Slider {
            id: texturedFillSwatchColourProbabilitySlider
            objectName: "texturedFillSwatchColourProbabilitySlider"
            value: model.probability
            live: false

            Layout.fillWidth: true

            ToolTip.text: qsTr("Adjust the probability of this colour appearing in the fill")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout
        }

        Button {
            objectName: "deleteTexturedFillSwatchColourButton"
            text: "\uf1f8"
            font.family: "FontAwesome"
            flat: true
            focusPolicy: Qt.NoFocus

            Layout.maximumWidth: implicitHeight

            ToolTip.text: qsTr("Remove this colour from the swatch")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onClicked: root.deleted()
        }
    }
}
