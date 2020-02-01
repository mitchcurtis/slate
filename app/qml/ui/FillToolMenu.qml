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

import App 1.0

Menu {
    objectName: "fillToolMenu"
    width: 260

    property ImageCanvas canvas
    
    MenuItem {
        objectName: "fillToolMenuItem"
        text: qsTr("Fill Tool")
        icon.source: "qrc:/images/fill.png"
        autoExclusive: true
        checkable: true
        checked: canvas && canvas.lastFillToolUsed === ImageCanvas.FillTool
        onTriggered: canvas.tool = ImageCanvas.FillTool
    }
    MenuItem {
        objectName: "texturedFillToolMenuItem"
        text: qsTr("Textured Fill Tool")
        icon.source: "qrc:/images/textured-fill.png"
        autoExclusive: true
        checkable: true
        checked: canvas && canvas.lastFillToolUsed === ImageCanvas.TexturedFillTool
        onTriggered: canvas.tool = ImageCanvas.TexturedFillTool
    }
}
