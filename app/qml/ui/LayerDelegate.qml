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

EditableDelegate {
    id: root
    objectName: model.layer.name
    checked: project && project.currentLayerIndex === index

    textField.objectName: "layerNameTextField"
    textField.text: model.layer.name
    textField.onAccepted: {
        project.setLayerName(index, textField.text)
        layeredImageCanvas.forceActiveFocus()
    }
    textField.Keys.onEscapePressed: {
        textField.text = model.layer.name
        layeredImageCanvas.forceActiveFocus()
    }

    onClicked: project.currentLayerIndex = index

    AbstractButton {
        id: visibilityCheckBox
        objectName: "layerVisibilityCheckBox"
        leftPadding: 16
        padding: 10
        rightPadding: 4
        focusPolicy: Qt.NoFocus
        checkable: true
        checked: model.layer.visible
        parent: root.leftSectionLayout
        contentItem: Label {
            text: visibilityCheckBox.checked ? "\uf06e" : "\uf070"
            font.family: "FontAwesome"
        }

        onClicked: project.setLayerVisible(index, checked)
    }
}
