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

Item {
    id: root
    anchors.centerIn: parent
    implicitWidth: 100
    implicitHeight: 3

    property alias gradient: rect.gradient

    Rectangle {
        id: rect
        width: root.height
        height: root.width
        transformOrigin: Item.TopLeft
        rotation: -90
        y: width

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#ff0000"
            }
            GradientStop {
                position: 0.166666
                color: "#ffff00"
            }
            GradientStop {
                position: 0.333333
                color: "#00ff00"
            }
            GradientStop {
                position: 0.5
                color: "#00ffff"
            }
            GradientStop {
                position: 0.666666
                color: "#0000ff"
            }
            GradientStop {
                position: 0.833333
                color: "#ff00ff"
            }
            GradientStop {
                position: 1
                color: "#ff0000"
            }
        }
    }
}
