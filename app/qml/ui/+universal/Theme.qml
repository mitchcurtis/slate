/*
    Copyright 2016, Mitch Curtis

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

import QtQml 2.12
import QtQuick.Controls.Universal 2.12

QtObject {
    property color focusColour: Universal.accent

    property var toolButtonWidth: 48
    property var toolButtonHeight: 38

    property color canvasBackgroundColour: "#222"
    property color splitColour: "#444"
    property color rulerForegroundColour: Universal.foreground
    property color rulerBackgroundColour: "#333"

    property color panelColour: "#282828"
}
