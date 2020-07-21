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
import QtQuick.Window

import App

import "." as Ui

ItemDelegate {
    objectName: model.layer.name
    checkable: true
    checked: project && project.currentLayerIndex === index
    implicitHeight: Math.max(implicitBackgroundHeight,
        Math.max(visibilityCheckBox.implicitHeight, layerNameTextField.implicitHeight) + topPadding + bottomPadding)
    leftPadding: visibilityCheckBox.width + 18
    topPadding: 0
    bottomPadding: 0
    focusPolicy: Qt.NoFocus

    onClicked: project.currentLayerIndex = index
    onDoubleClicked: layerNameTextField.forceActiveFocus()

    CheckBox {
        id: visibilityCheckBox
        objectName: "layerVisibilityCheckBox"
        x: 14
        text: model.layer.visible ? "\uf06e" : "\uf070"
        font.family: "FontAwesome"
        focusPolicy: Qt.NoFocus
        indicator: null
        anchors.verticalCenter: parent.verticalCenter

        onClicked: project.setLayerVisible(index, !model.layer.visible)
    }

    TextField {
        id: layerNameTextField
        objectName: "layerNameTextField"
        text: model.layer.name
        font.family: "FontAwesome"
        activeFocusOnPress: false
        anchors.left: visibilityCheckBox.right
        anchors.leftMargin: 4
        anchors.right: parent.right
        anchors.rightMargin: parent.rightPadding
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 6
        background.visible: false
        font.pixelSize: 12
        visible: false

        onAccepted: {
            project.setLayerName(index, text)
            layeredImageCanvas.forceActiveFocus()
        }

        Keys.onEscapePressed: {
            text = model.layer.name;
            layeredImageCanvas.forceActiveFocus()
        }
    }

    // We don't want TextField's editable cursor to be visible,
    // so we set visible: false to disable the cursor, and instead
    // render it via this.
    ShaderEffectSource {
        sourceItem: layerNameTextField
        anchors.fill: layerNameTextField
    }

    // Apparently the one above only works for the top level control item,
    // so we also need one for the background.
    ShaderEffectSource {
        sourceItem: layerNameTextField.background
        x: layerNameTextField.x + layerNameTextField.background.x
        y: layerNameTextField.y + layerNameTextField.background.y
        width: layerNameTextField.background.width
        height: layerNameTextField.background.height
        visible: layerNameTextField.activeFocus
    }

    Rectangle {
        id: focusRect
        width: 2
        height: parent.height
        color: Ui.Theme.focusColour
        visible: parent.checked
    }
}
