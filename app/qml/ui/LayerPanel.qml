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
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.12
import QtQuick.Window 2.12

import App 1.0

import "." as Ui

Panel {
    id: root
    objectName: "layerPanel"
    title: qsTr("Layers")
    padding: 0

    property LayeredImageCanvas layeredImageCanvas
    property LayeredImageProject project

    readonly property int minimumUsefulHeight: header.implicitHeight
        // Estimate delegate height since we can't easily know what it is for all styles.
        + 48
        + footer.implicitHeight

    UiStateSerialisation {
        project: root.project
        onReadyToLoad: {
            root.expanded = root.project.uiState.value("layerPanelExpanded", true)
            layerListView.contentY = root.project.uiState.value("layerListViewContentY", 0)
        }
        onReadyToSave: {
            root.project.uiState.setValue("layerPanelExpanded", root.expanded)
            root.project.uiState.setValue("layerListViewContentY", layerListView.contentY)
        }
    }

    ButtonGroup {
        objectName: "layerPanelButtonGroup"
        buttons: layerListView.contentItem.children
    }

    contentItem: ColumnLayout {
        visible: root.expanded
        spacing: 0

        ListView {
            id: layerListView
            objectName: "layerListView"
            boundsBehavior: ListView.StopAtBounds
            // TODO: shouldn't need to null-check at all in this file
            visible: project && project.loaded
            clip: true

            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollBar.vertical: ScrollBar {}

            model: LayerModel {
                layeredImageProject: project
            }

            delegate: LayerDelegate {
                width: layerListView.width
            }
        }

        // Necessary for when there is no loaded project so that the separator
        // doesn't go halfway up the panel.
        Item {
            Layout.fillHeight: layerListView.count == 0
        }

        Ui.VerticalSeparator {
            padding: 6
            topPadding: 0
            bottomPadding: 0

            Layout.fillWidth: true
        }
    }

    footer: RowLayout {
        id: footerRowLayout
        visible: root.expanded

        Button {
            objectName: "newLayerButton"
            text: "+"
            flat: true
            focusPolicy: Qt.NoFocus
            hoverEnabled: true

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.leftMargin: 6

            ToolTip.text: qsTr("Add a new layer")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onClicked: project.addNewLayer()
        }

        Button {
            objectName: "moveLayerDownButton"
            text: "\uf107"
            font.family: "FontAwesome"
            flat: true
            focusPolicy: Qt.NoFocus
            hoverEnabled: true
            enabled: project && project.currentLayerIndex < project.layerCount - 1

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true
            Layout.fillHeight: true

            ToolTip.text: qsTr("Move the current layer down")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onClicked: project.moveCurrentLayerDown()
        }

        Button {
            objectName: "moveLayerUpButton"
            text: "\uf106"
            font.family: "FontAwesome"
            flat: true
            focusPolicy: Qt.NoFocus
            hoverEnabled: true
            enabled: project && project.currentLayerIndex > 0

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true
            Layout.fillHeight: true

            ToolTip.text: qsTr("Move the current layer up")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onClicked: project.moveCurrentLayerUp()
        }

        Button {
            objectName: "duplicateLayerButton"
            text: "\uf24d"
            font.family: "FontAwesome"
            flat: true
            focusPolicy: Qt.NoFocus
            hoverEnabled: true
            enabled: project && project.currentLayerIndex >= 0 && project.currentLayerIndex < project.layerCount

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true
            Layout.fillHeight: true

            ToolTip.text: qsTr("Duplicate the current layer")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onClicked: project.duplicateCurrentLayer()
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Button {
            objectName: "deleteLayerButton"
            text: "\uf1f8"
            font.family: "FontAwesome"
            flat: true
            focusPolicy: Qt.NoFocus
            enabled: project && project.currentLayer && project.layerCount > 1
            hoverEnabled: true

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rightMargin: 6

            ToolTip.text: qsTr("Delete the current layer")
            ToolTip.visible: hovered
            ToolTip.delay: UiConstants.toolTipDelay
            ToolTip.timeout: UiConstants.toolTipTimeout

            onClicked: project.deleteCurrentLayer()
        }
    }
}
