/*
    Copyright 2018, Mitch Curtis

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

import QtQuick.Controls 2.3

import Qt.labs.platform 1.0 as Platform

Menu {
    objectName: "swatchSettingsContextMenu"

    MenuItem {
        text: qsTr("Import Swatch...")
        onTriggered: importDialog.open()
    }

    MenuItem {
        text: qsTr("Export Swatch...")
        onTriggered: exportDialog.open()
    }

    Platform.FileDialog {
        id: importDialog
        objectName: "importSwatchDialog"
        nameFilters: ["JSON files (*.json)"]
        defaultSuffix: "json"
        onAccepted: project.importSwatch(file)
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
