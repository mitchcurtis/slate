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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import App 1.0

import "." as Ui

Menu {
    id: root
    objectName: "swatchContextMenu"
    modal: true
    dim: false

    property Project project
    property Dialog renameSwatchColourDialog
    // The root objects in the application aren't loaded if we use SwatchPanel as the type here,
    // and there's no error message...
    property var swatchPanel
    property int rightClickedColourIndex
    property string rightClickedColourName

    onClosed: {
        rightClickedColourIndex = -1
        rightClickedColourName = ""
    }

    MenuItem {
        id: renameMenuItem
        objectName: "renameSwatchColourMenuItem"
        text: qsTr("Rename")
        onTriggered: {
            renameSwatchColourDialog.colourIndex = root.rightClickedColourIndex
            renameSwatchColourDialog.oldName = root.rightClickedColourName
            renameSwatchColourDialog.open()
        }
    }

    MenuItem {
        objectName: "deleteSwatchColourMenuItem"
        text: qsTr("Delete")
        onTriggered: project.swatch.removeColour(root.rightClickedColourIndex)
    }
}
