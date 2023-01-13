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

pragma Singleton

import QtQml
import QtQuick.Controls.Material

QtObject {
    property string styleName: "Imagine"

    property color focusColour: "white"

    property var toolButtonWidth: undefined
    property var toolButtonHeight: undefined

    property color canvasBackgroundColour: "#ddd"
    property color splitColour: "#4fc1e9"
    property color rulerForegroundColour: "white"
    property color rulerBackgroundColour: Qt.darker(_imagineColour, 1.12)

    property color panelColour: Qt.darker(_imagineColour, 1.12)

    property color _imagineColour: "#4fc1e9"
}
