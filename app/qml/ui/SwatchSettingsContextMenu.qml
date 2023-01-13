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

import QtQuick.Controls
import Slate

import Qt.labs.platform as Platform

Menu {
    objectName: "swatchSettingsContextMenu"

    MenuItem {
        id: importSlateSwatchMenuItem
        text: qsTr("Import Slate Swatch...")

        readonly property int swatchFormat: Project.SlateSwatch
        readonly property var nameFilters: ["JSON files (*.json)"]

        onTriggered: importMenuItemClicked(importSlateSwatchMenuItem)
    }

    MenuItem {
        id: importPaintNetSwatchMenuItem
        text: qsTr("Import Paint.NET Swatch...")

        readonly property int swatchFormat: Project.PaintNetSwatch
        readonly property var nameFilters: ["Text files (*.txt)"]

        onTriggered: importMenuItemClicked(importPaintNetSwatchMenuItem)
    }

    MenuItem {
        text: qsTr("Export Swatch...")
        onTriggered: exportDialog.open()
    }

    function importMenuItemClicked(menuItem) {
        importDialog.swatchFormat = menuItem.swatchFormat
        importDialog.nameFilters = menuItem.nameFilters
        importDialog.open()
    }

    Platform.FileDialog {
        id: importDialog
        objectName: "importSwatchDialog"
        defaultSuffix: "json"

        property int swatchFormat

        onAccepted: project.importSwatch(swatchFormat, file)
    }

    Platform.FileDialog {
        id: exportDialog
        objectName: "exportSwatchDialog"
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: ["JSON files (*.json)"]
        defaultSuffix: "json"

        onAccepted: project.exportSwatch(file)
    }
}
