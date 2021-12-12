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

pragma Singleton

import QtQml
import QtQuick.Controls.Material

QtObject {
    property color focusColour: Material.accent

    property var toolButtonWidth: undefined
    property var toolButtonHeight: undefined

    // The light theme's background colour is too white for the crosshair cursor.
    property color canvasBackgroundColour: Material.theme === Material.Dark ? Material.background : "#ccc"
    property color splitColour: Qt.lighter(rulerBackgroundColour, 1.2)
    property color rulerForegroundColour: Qt.darker(Material.foreground, 1.4)
    property color rulerBackgroundColour: panelColour

    property color panelColour: "#424242"
}
