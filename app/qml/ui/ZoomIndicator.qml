import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import App

RowLayout {
    id: zoomLevelIndicator

    property CanvasPane pane

    TextMetrics {
        id: maxZoomTextMetrics
        text: pane ? pane.maxZoomLevel.toString() : "999"
    }

    Label {
        text: "\uf002"
        font.family: "FontAwesome"
    }

    Label {
        text: "x"
    }

    Label {
        id: zoomLevelText
        text: pane ? pane.integerZoomLevel : ""

        Layout.minimumWidth: maxZoomTextMetrics.width
        Layout.maximumWidth: maxZoomTextMetrics.width
    }
}
