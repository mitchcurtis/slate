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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window

import Slate

import "." as Ui

AbstractButton {
    id: root
    implicitWidth: implicitContentWidth + leftPadding + rightPadding
    implicitHeight: implicitContentHeight + topPadding + bottomPadding
    checkable: true
    leftPadding: 0
    rightPadding: 4
    focusPolicy: Qt.NoFocus

    property alias textField: textField

    property alias leftSectionLayout: leftSectionLayout
    property alias rightSectionLayout: rightSectionLayout

    onDoubleClicked: textField.forceActiveFocus()

    contentItem: RowLayout {
        spacing: 0

        Rectangle {
            id: focusRect
            width: 2
            color: Ui.Theme.focusColour
            // We want it to take up space when it's not checked
            opacity: root.checked ? 1 : 0

            Layout.fillHeight: true
        }

        RowLayout {
            id: leftSectionLayout
            spacing: 0
        }

        Ui.TextField {
            id: textField
            font.family: "FontAwesome"
            activeFocusOnPress: false
            font.pixelSize: 12
            // Don't show the outline when we don't have focus.
            background.visible: activeFocus

            Layout.fillWidth: true

            // We want single clicks to emit clicked on the root delegate, not give the text field focus.
            // Instead, double clicks should give the text field focus.
            MouseArea {
                anchors.fill: parent
                visible: !textField.activeFocus
                onClicked: root.clicked()
                onDoubleClicked: root.doubleClicked()
            }
        }

        RowLayout {
            id: rightSectionLayout
            spacing: 0
        }
    }
}
