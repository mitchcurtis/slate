/*
    Copyright 2021, Mitch Curtis

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

import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3


/*
    It's likely that ToolSeparator is the tallest item in the
    StatusBar. If all separators were completely hidden (and not laid
    out), the size of the status bar would jump around when the various
    items are shown/hidden. Initially we were solving this by always
    showing the first separator, but setting its opacity to 0 if it
    wouldn't be shown. This works, but requires that every separator to
    the right of it keep track of the visibility of the ones before it
    (so that we didn't end up with two separators next to each other).

    Another solution would be to give an item that's always visible
    (like pointerIconLabel) a minimumHeight that matches the height
    of ToolSeparator, but that would have to be done for each style,
    and isn't that nice.

    So, to avoid having to hard-code sizes and have complex visible
    bindings, we do the opacity trick for each separator, and also
    reduce its width to 0 so it doesn't take up any room.
*/

ToolSeparator {
    topPadding: 0
    bottomPadding: 0
    opacity: shown ? 1 : 0

    property bool shown

    Layout.maximumWidth: shown ? implicitWidth : 0
    Layout.fillHeight: true
    Layout.maximumHeight: 24
}
