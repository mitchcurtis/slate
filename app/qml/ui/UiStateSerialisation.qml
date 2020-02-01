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

import QtQuick 2.14

import App 1.0

// Can't use Connections as the root, because onProjectChanged doesn't work there.
// Can't use QtObject as the root either,
// because we can't declare objects (like Connections) as children of those.
Item {
    property Project project

    signal readyToSave
    signal readyToLoad

    onProjectChanged: Qt.callLater(function() {
        // During tests (e.g. for layer list view), project can be null briefly.
        // It seems to happen when going from .slp to .slp.
        if (project)
            readyToLoad()
    })

    Connections {
        target: project
        onPreProjectSaved: readyToSave()
    }
}

