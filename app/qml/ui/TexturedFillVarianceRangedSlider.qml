/*
    Copyright 2019, Mitch Curtis

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

import App 1.0

RangeSlider {
    id: root
    from: -0.1
    to: 0.1
    first.value: parameter.varianceLowerBound
    second.value: parameter.varianceUpperBound

    property string displayName
    property TexturedFillParameter parameter

    ToolTip.text: qsTr("Adjusts the lower and upper bounds for random %1 variance").arg(displayName)
    ToolTip.visible: hovered
    ToolTip.delay: UiConstants.toolTipDelay
    ToolTip.timeout: UiConstants.toolTipTimeout

    function commit() {
        parameter.varianceLowerBound = first.value
        parameter.varianceUpperBound = second.value
    }

    // The plan was to set live to false due to fills being slow:
    // https://github.com/mitchcurtis/slate/issues/45
    // However, setting it to false means that we couldn't use the new moved() signal
    // to update the parameters, because the value hasn't been updated, because it
    // only updates on release.
    // So, we need live to be true, and instead we only update the parameters on release.
    first.onPressedChanged: if (!first.pressed) parameter.varianceLowerBound = first.value
    second.onPressedChanged: if (!second.pressed) parameter.varianceUpperBound = second.value

    ToolTip {
        parent: root.first.handle
        visible: root.first.pressed
        text: root.valueAt(first.position).toFixed(3)
    }
    ToolTip {
        parent: root.second.handle
        visible: root.second.pressed
        text: root.valueAt(second.position).toFixed(3)
    }
}
