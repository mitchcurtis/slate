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

import Slate

import "." as Ui

ItemDelegate {
    id: root
    objectName: model.layer.name
    checkable: true
    checked: project && project.currentLayerIndex === index
    leftPadding: visibilityCheckBox.width + 18
    focusPolicy: Qt.NoFocus

    Binding {
        target: root
        property: "topPadding"
        value: 0
        when: Ui.Theme.styleName === "Material"
    }

    Binding {
        target: root
        property: "bottomPadding"
        value: 0
        when: Ui.Theme.styleName === "Material"
    }

    Binding {
        target: root
        property: "implicitHeight"
        value: Math.max(implicitBackgroundHeight,
            Math.max(visibilityCheckBox.implicitHeight, layerNameTextField.implicitHeight) + topPadding + bottomPadding)
        when: Ui.Theme.styleName === "Material"
    }

    onClicked: project.currentLayerIndex = index
    onDoubleClicked: layerNameTextField.forceActiveFocus()

    AbstractButton {
        id: visibilityCheckBox
        objectName: "layerVisibilityCheckBox"
        x: 14
        anchors.verticalCenter: parent.verticalCenter
        leftPadding: 10
        rightPadding: 10
        focusPolicy: Qt.NoFocus
        checkable: true
        checked: model.layer.visible
        contentItem: Label {
            text: visibilityCheckBox.checked ? "\uf06e" : "\uf070"
            font.family: "FontAwesome"
        }

        onClicked: project.setLayerVisible(index, checked)
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
        anchors.verticalCenterOffset: Ui.Theme.styleName === "Material" ? 6 : 0
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
