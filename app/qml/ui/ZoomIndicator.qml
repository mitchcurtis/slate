import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0

import App 1.0

RowLayout {
    id: zoomLevelIndicator

    property CanvasPane pane
    property FontMetrics fontMetrics
    readonly property real pixelSize: fontMetrics.font.pixelSize * 1.5

    opacity: 0.5

    TextMetrics {
        id: maxZoomTextMetrics
        font.pixelSize: pixelSize
        text: pane ? pane.maxZoomLevel.toString() : "99"
    }

    Label {
        text: "\uf002"
        font.family: "FontAwesome"
        font.pixelSize: pixelSize
        color: "#ffffff"
        horizontalAlignment: Label.AlignHCenter
        verticalAlignment: Label.AlignVCenter
        Layout.preferredWidth: 30
    }

    Label {
        text: "x"
        color: "#ffffff"
        font.pixelSize: pixelSize
    }

    Item {
        Layout.minimumWidth: maxZoomTextMetrics.width

        Label {
            id: zoomLevelText
            text: pane ? pane.zoomLevel : ""
            color: "#ffffff"
            font.pixelSize: pixelSize
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
