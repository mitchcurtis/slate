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

import QtQuick 2.9
import QtQuick.Controls 2.1

TextField {
    text: propertySource[propertyName]
    validator: DoubleValidator {
        bottom: -1.0
        top: 1.0
    }
    // https://bugreports.qt.io/browse/QTBUG-71907
    background.width: width

    property QtObject propertySource
    property string propertyName
    signal valueModified

    Keys.onDownPressed: {
        let newValue = clamp(propertySource[propertyName] - 0.01)
        if (propertySource[propertyName] !== newValue) {
            propertySource[propertyName] = newValue
            valueModified()
        }
    }
    Keys.onUpPressed: {
        let newValue = clamp(propertySource[propertyName] + 0.01)
        if (propertySource[propertyName] !== newValue) {
            propertySource[propertyName] = newValue
            valueModified()
        }
    }

    onTextEdited: {
        // Tried to use inputMask for this, but it seems buggy. Also, even though we
        // have a DoubleValidator with a range specified, it still allows the user
        // to enter e.g. 123, so we clamp it here.

        // As a special case, if the user has started typing a negative number,
        // the text will be "-", which parseFloat() will return NaN for.
        let adjustedText = text === "-" ? "-0" : text
        let newValue = adjustedText.length > 0 ? clamp(parseFloat(adjustedText)) : 0
        if (propertySource[propertyName] !== newValue) {
            propertySource[propertyName] = newValue
            valueModified()
        }
    }
}
