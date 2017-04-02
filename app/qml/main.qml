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

import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

import Qt.labs.platform 1.0 as Platform

import App 1.0

import "ui" as Ui

/*
    TODO:
    - fix disappearing hue slider indicator
    - native menubars?
    - fix performance when drawing pixels
    - fix performance when resizing
    - make panels resizable
    - tutorial?
    - add "lighten/darken current colour" feature with convenient keyboard shortcuts.
      it could display a fading indicator like the zoom level does, comparing the
      proposed colour next to the old one. could be a mouse wheel shortcut.
*/

ApplicationWindow {
    id: window
    objectName: "window"
    width: 1200
    height: 800
//    width: 1080
//    height: 1920
    title: qtbug53394Title
    visible: true

    property string qtbug53394Title: project.loaded ? ((project.url.length > 0 ? project.url.toString() : "Untitled") + (project.unsavedChanges ? "*" : "")) : ""

    readonly property int controlSpacing: 10

    property alias newProjectPopup: newProjectPopup
    property alias openProjectDialog: openProjectDialog
    property alias saveChangesDialog: discardChangesMessageBox

    FontMetrics {
        id: fontMetrics
    }

    Ui.Shortcuts {
        window: window
        project: project
        canvas: canvas
    }

    // If we set the image URL immediately, it can happen before
    // the error popup is ready.
    Component.onCompleted: {
        if (settings.loadLastOnStartup && settings.lastProjectUrl.toString().length > 0) {
            project.load(settings.lastProjectUrl);
        }
        window.title = Qt.binding(function(){ return qtbug53394Title });
        window.x = Screen.desktopAvailableWidth / 2 - width / 2
        window.y = Screen.desktopAvailableHeight / 2 - height / 2
    }

    Project {
        id: project
        objectName: "project"

        onErrorOccurred: errorPopup.showError(errorMessage)

        onUrlChanged: {
            if (loaded) {
                settings.lastProjectUrl = url;
            }
        }

        function saveOrSaveAs() {
            if (project.url.length > 0) {
                project.save();
            } else {
                saveAsDialog.open();
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        TileCanvas {
            id: canvas
            objectName: "tileCanvas"
            focus: true
            project: project
            backgroundColour: Ui.CanvasColours.backgroundColour
            gridVisible: settings.gridVisible
            gridColour: "#55000000"
            splitScreen: settings.splitScreen
            splitter.enabled: settings.splitScreen && !settings.splitterLocked
            splitter.width: 32

            readonly property int currentPaneZoomLevel: canvas.currentPane ? canvas.currentPane.zoomLevel : 1
            readonly property point currentPaneOffset: canvas.currentPane ? canvas.currentPane.offset : Qt.point(0, 0)
            readonly property bool useCrosshairCursor: canvas.mode === TileCanvas.TileMode || (canvas.toolSize < 4 && canvas.currentPaneZoomLevel <= 3)
            readonly property bool useIconCursor: canvas.tool === TileCanvas.EyeDropperTool

            // TODO: figure out a nicer solution than this.
            property bool ignoreToolChanges: false

            Layout.preferredWidth: window.width / 3
            Layout.fillWidth: true
            Layout.fillHeight: true

            onErrorOccurred: errorPopup.showError(errorMessage)

            // TODO: tile pen preview shouldn't be visible with colour picker open

            onToolChanged: {
                if (ignoreToolChanges)
                    return;

                switch (canvas.tool) {
                case TileCanvas.PenTool:
                    iconToolBar.toolButtonGroup.checkedButton = iconToolBar.penToolButton;
                    break;
                case TileCanvas.EyeDropperTool:
                    iconToolBar.toolButtonGroup.checkedButton = iconToolBar.eyeDropperToolButton;
                    break;
                case TileCanvas.EraserTool:
                    iconToolBar.toolButtonGroup.checkedButton = iconToolBar.eraserToolButton;
                    break;
                }
            }

            Ui.CrosshairCursor {
                id: crosshairCursor
                x: canvas.cursorX - width / 2
                y: canvas.cursorY - height / 2
                visible: canvas.hasBlankCursor && !canvas.useIconCursor && canvas.useCrosshairCursor
                z: header.z + 1
            }

            RectangularCursor {
                id: rectangleCursor
                x: Math.floor(canvas.cursorX - width / 2)
                y: Math.floor(canvas.cursorY - height / 2)
                width: canvas.toolSize * canvas.currentPaneZoomLevel
                height: canvas.toolSize * canvas.currentPaneZoomLevel
                visible: canvas.hasBlankCursor && !canvas.useIconCursor && !canvas.useCrosshairCursor
                z: header.z + 1
            }

            Label {
                id: iconCursor
                x: canvas.cursorX
                y: canvas.cursorY - height + 3
                visible: canvas.hasBlankCursor && canvas.useIconCursor
                text: iconToolBar.toolButtonGroup.checkedButton.icon
                z: header.z + 1
            }

            Ui.ZoomIndicator {
                x: canvas.firstPane.size * canvas.width - width - 16
                anchors.bottom: canvas.bottom
                anchors.bottomMargin: 16
                visible: project.loaded && canvas.splitScreen
                pane: canvas.firstPane
                fontMetrics: fontMetrics
            }

            Ui.ZoomIndicator {
                anchors.right: canvas.right
                anchors.bottom: canvas.bottom
                anchors.margins: 16
                visible: project.loaded
                pane: canvas.secondPane
                fontMetrics: fontMetrics
            }
        }

        ColumnLayout {
            Ui.ColourPanel {
                id: colourPanel
                canvas: canvas
            }

            Ui.TilesetSwatch {
                id: tilesetSwatch
                objectName: "tilesetSwatch"
                tileCanvas: canvas
                // Don't let e.g. the pencil icon go under us.
                z: canvas.z - 1

                Layout.preferredWidth: colourPanel.implicitWidth
                Layout.preferredHeight: window.contentItem.height / 2
                Layout.fillHeight: true
            }
        }
    }

    header: Ui.ToolBar {
        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Ui.MenuBar {
                id: menuBar
                spacing: controlSpacing
                canvas: canvas
            }

            Ui.IconToolBar {
                id: iconToolBar
                project: project
                canvas: canvas
                fontMetrics: fontMetrics

                Layout.fillWidth: true
            }
        }
    }

    Platform.FileDialog {
        id: openProjectDialog
        nameFilters: ["JSON files (*.json)"]
        defaultSuffix: "json"
        onAccepted: project.load(file)
    }

    Platform.FileDialog {
        id: saveAsDialog
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: ["JSON files (*.json)"]
        defaultSuffix: "json"
        onAccepted: project.saveAs(file)
    }

    Ui.ErrorPopup {
        id: errorPopup
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        // TODO: shouldn't have to do this, but not even a FocusScope around TileCanvas worked..
        onVisibleChanged: canvas.focus = true
    }

    Ui.NewProjectPopup {
        id: newProjectPopup
        x: parent.width / 2 - width / 2
        // Avoid binding loop that results from using height.
        // TODO: https://codereview.qt-project.org/#/c/175024/ fixes this
        y: parent.height / 2 - implicitHeight / 2
        onVisibleChanged: canvas.focus = true

        onAccepted: project.createNew(tilesetPath, tileWidth, tileHeight,
            tilesetTilesWide, tilesetTilesHigh, canvasTilesWide, canvasTilesHigh)
    }

    Ui.RecentProjectsPopup {
        id: recentProjectsPopup
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        onVisibleChanged: canvas.focus = true

        onAccepted: project.load(fileName)
    }

    Ui.OptionsDialog {
        id: optionsDialog
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - implicitHeight / 2
        onVisibleChanged: canvas.focus = true
    }

    Dialog {
        id: discardChangesMessageBox
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        title: qsTr("Unsaved changes")
        standardButtons: Dialog.Yes | Dialog.No

        Label {
            text: qsTr("The action you're about to perform could discard changes.\n\nContinue anyway?")
        }
    }
}
