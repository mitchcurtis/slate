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
import QtQml 2.2
import QtQuick.Controls.Material 2.1

QtObject {
    // The light theme's background colour is too white for the crosshair cursor.
    property color backgroundColour: Material.theme === Material.Dark ? Material.background : "#ccc"
    property color panelColour: "#424242"
    property color focusColor: Material.accent
}
