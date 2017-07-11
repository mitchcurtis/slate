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

import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Controls 2.1

import Qt.labs.platform 1.0 as Platform

import App 1.0

import "ui" as Ui

/*
    TODO:
    - rulers, guides
    - cursor ruler that follows the cursor as it moves to make it easy to
      line stuff up
    - Fill-all-of-this-colour tool
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
    title: qtbug53394Title
    visible: true

//    onActiveFocusItemChanged: print(activeFocusItem)

    property string qtbug53394Title: project && project.loaded
        ? ((project.url.toString().length > 0 ? project.displayUrl : "Untitled") + (project.unsavedChanges ? "*" : ""))
        : ""

    property alias projectManager: projectManager
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    property ImageCanvas canvas: canvasContainer.canvas
    property alias newProjectPopup: newProjectPopup
    property alias openProjectDialog: openProjectDialog
    property alias saveChangesDialog: discardChangesDialog

    onClosing: {
        close.accepted = false;
        doIfChangesDiscarded(function() { Qt.quit() })
    }

    // If we set the image URL immediately, it can happen before
    // the error popup is ready.
    Component.onCompleted: {
        contentItem.objectName = "applicationWindowContentItem";

        if (settings.loadLastOnStartup && settings.lastProjectUrl.toString().length > 0) {
            loadProject(settings.lastProjectUrl)
        } else {
            createNewProject(Project.LayeredImageType)
        }

        window.title = Qt.binding(function(){ return qtbug53394Title });
        window.x = Screen.desktopAvailableWidth / 2 - width / 2
        window.y = Screen.desktopAvailableHeight / 2 - height / 2
    }

    function doIfChangesDiscarded(actionFunction) {
        if (!project) {
            // Auto tests can skip this function.
            return;
        }

        if (!project.unsavedChanges) {
            actionFunction();
            return;
        }

        function disconnectSignals() {
            saveChangesDialog.accepted.disconnect(discardChanges);
            saveChangesDialog.rejected.disconnect(dontDiscardChanges);
        }

        function discardChanges() {
            actionFunction();
            disconnectSignals();
        }

        function dontDiscardChanges() {
            disconnectSignals();
        }

        saveChangesDialog.accepted.connect(discardChanges);
        saveChangesDialog.rejected.connect(dontDiscardChanges);
        saveChangesDialog.open();
    }

    Connections {
        target: projectManager.project ? projectManager.project : null
        onErrorOccurred: errorPopup.showError(errorMessage)
    }

    ProjectManager {
        id: projectManager
        applicationSettings: settings

        function saveOrSaveAs() {
            if (project.url.toString().length > 0) {
                project.save();
            } else {
                saveAsDialog.open();
            }
        }
    }

    FontMetrics {
        id: fontMetrics
    }

    Ui.Shortcuts {
        window: window
        projectManager: window.projectManager
        canvas: window.canvas
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Ui.CanvasContainer {
            id: canvasContainer
            focus: true

            project: projectManager.project
            checkedToolButton: iconToolBar.toolButtonGroup.checkedButton
            fontMetrics: fontMetrics

            Layout.preferredWidth: window.width / 3
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ColumnLayout {
            Layout.alignment: Qt.AlignTop

            Ui.ColourPanel {
                id: colourPanel
                canvas: window.canvas
            }

            SimpleLoader {
                objectName: "tilesetSwatchLoader"
                active: window.projectType === Project.TilesetType && window.canvas
                sourceComponent: Ui.TilesetSwatch {
                    id: tilesetSwatch
                    objectName: "tilesetSwatch"
                    tileCanvas: window.canvas
                    project: window.project
                    // Don't let e.g. the pencil icon go under us.
                    z: canvasContainer.z - 1
                }

                Layout.preferredWidth: active ? colourPanel.implicitWidth : 0
                Layout.preferredHeight: active ? window.contentItem.height / 2 : 0
                Layout.fillHeight: active
            }

            SimpleLoader {
                objectName: "layersLoader"
                active: window.projectType === Project.LayeredImageType && window.canvas
                sourceComponent: Ui.LayerPanel {
                    objectName: "layerPanel"
                    layeredImageCanvas: window.canvas
                    project: window.project
                    z: canvasContainer.z - 1
                }

                Layout.preferredWidth: active ? colourPanel.implicitWidth : 0
                Layout.preferredHeight: active ? window.contentItem.height / 2 : 0
                Layout.fillHeight: active
            }
        }
    }

    header: Ui.ToolBar {
        objectName: "toolBar"

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            Ui.MenuBar {
                id: menuBar
                projectManager: window.projectManager
                canvas: window.canvas
            }

            Ui.IconToolBar {
                id: iconToolBar
                project: window.project
                canvas: window.canvas
                fontMetrics: fontMetrics

                Layout.fillWidth: true
            }
        }
    }

    readonly property var imageFilters: ["PNG files (*.png)", "BMP files (*.bmp)"]
    readonly property string imageDefaultSuffix: "png"
    readonly property var layeredImageFilters: ["SLP files (*.slp)"]
    readonly property string layeredImageDefaultSuffix: "slp"
    readonly property var tilesetFilters: ["STP files (*.stp)"]
    readonly property string tilesetDefaultSuffix: "stp"

    function defaultSuffixForProjectType(projectType) {
        return projectType === Project.ImageType ? imageDefaultSuffix
            : projectType === Project.LayeredImageType ? layeredImageDefaultSuffix
            : tilesetDefaultSuffix;
    }

    function nameFiltersForProjectType(projectType) {
        return projectType === Project.ImageType ? imageFilters
            : projectType === Project.LayeredImageType ? layeredImageFilters
            : tilesetFilters;
    }

    function projectTypeForUrl(url) {
        var urlString = url.toString();
        return url.toString().endsWith(".stp") ? Project.TilesetType
            : url.toString().endsWith(".slp") ? Project.LayeredImageType : Project.ImageType;
    }

    Platform.FileDialog {
        id: openProjectDialog
        objectName: "openProjectDialog"
        nameFilters: ["All files (*)", "PNG files (*.png)", "BMP files (*.bmp)", "SLP files (*.slp)", "STP files (*.stp)"]
        defaultSuffix: imageDefaultSuffix
        onAccepted: loadProject(file)
    }

    Platform.FileDialog {
        id: saveAsDialog
        objectName: "saveAsDialog"
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: nameFiltersForProjectType(projectType)
        defaultSuffix: defaultSuffixForProjectType(projectType)
        onAccepted: project.saveAs(file)
    }

    Platform.FileDialog {
        id: exportDialog
        objectName: "exportAsDialog"
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: imageFilters
        defaultSuffix: imageDefaultSuffix
        onAccepted: project.exportImage(file)
    }

    Ui.ErrorPopup {
        id: errorPopup
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        // TODO: shouldn't have to do this, but not even a FocusScope around TileCanvas worked..
        onVisibleChanged: if (window.canvas) window.canvas.forceActiveFocus()
    }

    Ui.NewProjectPopup {
        id: newProjectPopup
        x: parent.width / 2 - width / 2
        // Avoid binding loop that results from using height.
        // TODO: https://codereview.qt-project.org/#/c/175024/ fixes this
        y: parent.height / 2 - implicitHeight / 2

        onChoseTilesetProject: newTilesetProjectPopup.open()
        onChoseImageProject: newImageProjectPopup.open()
        onChoseLayeredImageProject: newLayeredImageProjectPopup.open()
        onRejected: if (window.canvas) window.canvas.forceActiveFocus()
    }

    function createNewProject(type) {
        projectManager.beginCreation(type);

        if (type === Project.TilesetType) {
            var p = newTilesetProjectPopup;
            projectManager.temporaryProject.createNew(p.tilesetPath, p.tileWidth, p.tileHeight,
                p.tilesetTilesWide, p.tilesetTilesHigh, p.canvasTilesWide, p.canvasTilesHigh,
                p.transparentBackground);
        } else if (type === Project.ImageType) {
            var p = newImageProjectPopup;
            projectManager.temporaryProject.createNew(p.imageWidth, p.imageHeight, p.transparentBackground);
        } else if (type === Project.LayeredImageType) {
            var p = newLayeredImageProjectPopup;
            projectManager.temporaryProject.createNew(p.imageWidth, p.imageHeight, p.transparentBackground);
        }

        projectManager.completeCreation();
    }

    function loadProject(url) {
        var type = projectTypeForUrl(url);
        projectManager.beginCreation(type);
        projectManager.temporaryProject.load(url);
        projectManager.completeCreation();
    }

    Ui.NewTilesetProjectPopup {
        id: newTilesetProjectPopup
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        onVisibleChanged: if (window.canvas) window.canvas.forceActiveFocus()

        onAccepted: createNewProject(Project.TilesetType)
    }

    Ui.NewImageProjectPopup {
        id: newImageProjectPopup
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        onVisibleChanged: if (window.canvas) window.canvas.forceActiveFocus()

        onAccepted: createNewProject(Project.ImageType)
    }

    Ui.NewLayeredImageProjectPopup {
        id: newLayeredImageProjectPopup
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        onVisibleChanged: if (window.canvas) window.canvas.forceActiveFocus()

        onAccepted: createNewProject(Project.LayeredImageType)
    }

    Ui.OptionsDialog {
        id: optionsDialog
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - implicitHeight / 2
        onVisibleChanged: if (window.canvas) window.canvas.forceActiveFocus()
    }

    Dialog {
        id: discardChangesDialog
        objectName: "discardChangesDialog"
        x: parent.width / 2 - width / 2
        y: parent.height / 2 - height / 2
        title: qsTr("Unsaved changes")
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        onVisibleChanged: if (window.canvas) window.canvas.forceActiveFocus()

        Label {
            text: qsTr("The action you're about to perform could discard changes.\n\nContinue anyway?")
        }
    }
}
