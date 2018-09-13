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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0

import App 1.0

import "." as Ui

Panel {
    id: root
    objectName: "swatchesPanel"
    title: qsTr("Swatches")
    topPadding: 0
    leftPadding: 0
    rightPadding: 0
    bottomPadding: 6

    property ImageCanvas canvas
    property Project project

    contentItem: ColumnLayout {
        visible: root.expanded
        spacing: 0

        GridView {
            id: autoSwatchGridView
            objectName: "autoSwatchGridView"
            boundsBehavior: ListView.StopAtBounds
            visible: project && project.loaded
            clip: true
            cellWidth: 16
            cellHeight: 16

            Layout.fillWidth: true
            Layout.preferredHeight: 100
            Layout.fillHeight: true

            property real contentYBeforeModelReset
            property bool lockContentY

            // Try to maintain the same contentY between model changes
            // so that the view doesn't fly back to the top every time a new colour is added.
            Binding {
                target: autoSwatchGridView
                property: "contentY"
                value: autoSwatchGridView.contentYBeforeModelReset
                when: autoSwatchGridView.lockContentY
            }

            Connections {
                target: autoSwatchGridView.model
                onModelAboutToBeReset: {
                    autoSwatchGridView.contentYBeforeModelReset = autoSwatchGridView.contentY
                    autoSwatchGridView.lockContentY = true
                }
                onModelReset: {
                    Qt.callLater(function() {
                        autoSwatchGridView.lockContentY = false
                        autoSwatchGridView.contentY = autoSwatchGridView.contentYBeforeModelReset
                    });
                }
            }

            ScrollBar.vertical: ScrollBar {}

            model: AutoSwatchModel {
                canvas: root.canvas
            }
            delegate: ItemDelegate {
                width: 16
                height: 16
                leftPadding: 0
                rightPadding: 0
                topPadding: 0
                bottomPadding: 0
                focusPolicy: Qt.NoFocus
                contentItem: Rectangle {
                    color: model.colour
                }

                onClicked: canvas.penForegroundColour = modelData
            }
        }
    }

    footer: ColumnLayout {
        visible: root.expanded
        spacing: 0

        MenuSeparator {
            padding: 6
            topPadding: 0
            bottomPadding: 0

            Layout.fillWidth: true
        }

        RowLayout {
            id: footerRowLayout

            Button {
                objectName: "newSwatchButton"
                text: "+"
                flat: true
                focusPolicy: Qt.NoFocus
                hoverEnabled: true

                Layout.maximumWidth: implicitHeight
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 6

    //            onClicked: project.addNewLayer()
            }

    //        Button {
    //            objectName: "moveLayerDownButton"
    //            text: "\uf107"
    //            font.family: "FontAwesome"
    //            flat: true
    //            focusPolicy: Qt.NoFocus
    //            hoverEnabled: true
    //            enabled: project && project.currentLayerIndex < project.layerCount - 1

    //            Layout.maximumWidth: implicitHeight
    //            Layout.fillWidth: true
    //            Layout.fillHeight: true

    //            onClicked: project.moveCurrentLayerDown()
    //        }

    //        Button {
    //            objectName: "moveLayerUpButton"
    //            text: "\uf106"
    //            font.family: "FontAwesome"
    //            flat: true
    //            focusPolicy: Qt.NoFocus
    //            hoverEnabled: true
    //            enabled: project && project.currentLayerIndex > 0

    //            Layout.maximumWidth: implicitHeight
    //            Layout.fillWidth: true
    //            Layout.fillHeight: true

    //            onClicked: project.moveCurrentLayerUp()
    //        }

    //        Button {
    //            objectName: "duplicateLayerButton"
    //            text: "\uf24d"
    //            font.family: "FontAwesome"
    //            flat: true
    //            focusPolicy: Qt.NoFocus
    //            hoverEnabled: true
    //            enabled: project && project.currentLayerIndex >= 0 && project.currentLayerIndex < project.layerCount

    //            Layout.maximumWidth: implicitHeight
    //            Layout.fillWidth: true
    //            Layout.fillHeight: true

    //            onClicked: project.duplicateCurrentLayer()
    //        }

    //        Item {
    //            Layout.fillWidth: true
    //            Layout.fillHeight: true
    //        }

    //        Button {
    //            objectName: "deleteLayerButton"
    //            text: "\uf1f8"
    //            font.family: "FontAwesome"
    //            flat: true
    //            focusPolicy: Qt.NoFocus
    //            enabled: project && project.currentLayer && project.layerCount > 1
    //            hoverEnabled: true

    //            Layout.maximumWidth: implicitHeight
    //            Layout.fillWidth: true
    //            Layout.fillHeight: true
    //            Layout.rightMargin: 6

    //            onClicked: project.deleteCurrentLayer()
    //        }
        }
    }
}
