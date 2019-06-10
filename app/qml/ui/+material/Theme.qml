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
import QtQuick.Controls.Material 2.12

QtObject {
    readonly property bool darkTheme: Material.theme === Material.Dark
    readonly property color reallyLightBlue: "#f8fdff"

    readonly property color focusColour: darkTheme ? Material.accent : "salmon"

    readonly property var toolButtonWidth: undefined
    readonly property var toolButtonHeight: undefined
    readonly property color toolBarColour: darkTheme ? "#424242" : Material.color(Material.LightBlue, Material.Shade200)

    // The light theme's background colour is too white for the crosshair cursor.
    readonly property color canvasBackgroundColour: darkTheme ? Material.background : "#444"
    readonly property color splitColour: "#444"
    readonly property color rulerForegroundColour: Qt.darker(Material.foreground, 1.4)
    readonly property color rulerBackgroundColour: panelColour

    readonly property color panelColour: darkTheme ? "#424242" : reallyLightBlue

    readonly property color statusBarColour: reallyLightBlue
    readonly property color statusBarPaneEdgeColour: darkTheme ? "#444" : "transparent"

    // Material-only stuff below.

    // We create our own Material ApplicationWindow (plus the Default one which we have to
    // have for the selector to work) so that we can override the background colour.
    // If we didn't, each Theme would have to provide their own windowColour, meaning
    // we'd need to hard-code the window colours from each style.
    readonly property color windowColour: darkTheme ? Material.background : "#eee"
}
