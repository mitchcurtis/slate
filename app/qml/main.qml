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

import QtQuick 2.13
import QtQuick.Layouts 1.13
import QtQuick.Window 2.13
import QtQuick.Controls 2.13

import Qt.labs.settings 1.0
import Qt.labs.platform 1.0 as Platform

import App 1.0

import "ui" as Ui

ApplicationWindow {
    id: window
    objectName: "window"
    width: 1200
    height: 800
    title: project && project.loaded
        ? ((project.url.toString().length > 0 ? project.displayUrl : "Untitled") + (project.unsavedChanges ? "*" : ""))
        : ""
    opacity: settings.windowOpacity
    visible: true

//    onActiveFocusItemChanged: print("active focus: " + activeFocusItem + ", parent: "
//        + (activeFocusItem ? activeFocusItem.parent : null))

    property Project project: projectManager.project
    readonly property int projectType: project && projectManager.ready ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType
    readonly property bool isLayeredImageProjectType: projectType === Project.LayeredImageType
    property ImageCanvas canvas: canvasContainer.canvas
    property alias newProjectPopup: newProjectPopup
    property alias openProjectDialog: openProjectDialog
    property alias saveChangesDialog: saveChangesDialog
    property alias saveAsDialog: saveAsDialog
    property alias moveContentsDialog: moveContentsDialog
    property int oldWindowVisibility: Window.Windowed

    onClosing: {
        close.accepted = false
        saveChangesDialog.doIfChangesSavedOrDiscarded(function() { Qt.quit() })
    }

    // If we set the image URL immediately, it can happen before
    // the error popup is ready.
    Component.onCompleted: {
        contentItem.parent.objectName = "applicationWindowRootItem"
        contentItem.objectName = "applicationWindowContentItem"

        if (settings.loadLastOnStartup && settings.recentFiles.length > 0) {
            loadProject(settings.recentFiles[0])
        } else {
            createNewProject(Project.LayeredImageType)
        }
    }

    function saveOrSaveAs() {
        if (project.url.toString().length > 0) {
            // Existing project; can save without a dialog.
            project.save();
        } else {
            // New project; need to save as.
            saveAsDialog.open();
        }
    }

    function toggleFullScreen() {
        if (window.visibility === Window.FullScreen) {
            window.visibility = oldWindowVisibility
        }
        else {
            oldWindowVisibility = window.visibility
            window.visibility = Window.FullScreen
        }
    }

    Settings {
        property alias windowX: window.x
        property alias windowY: window.y
        property alias windowWidth: window.width
        property alias windowHeight: window.height
        property alias windowVisibility: window.visibility
    }

    Connections {
        target: projectManager
        function onCreationFailed(errorMessage) { errorPopup.showError(errorMessage) }
    }

    Connections {
        target: projectManager.project ? projectManager.project : null
        function onErrorOccurred(errorMessage) { errorPopup.showError(errorMessage) }
    }

    Connections {
        target: canvas
        function onErrorOccurred(errorMessage) { errorPopup.showError(errorMessage) }
        function onNoteCreationRequested() {
            noteDialog.currentAction = Ui.NoteDialog.NoteAction.Create
            noteDialog.newNoteX = canvas.cursorSceneX
            noteDialog.newNoteY = canvas.cursorSceneY
            noteDialog.open()
        }
        function onNoteModificationRequested(noteIndex) {
            noteDialog.currentAction = Ui.NoteDialog.NoteAction.Modify
            noteDialog.modifyingNoteIndex = noteIndex
            noteDialog.open()
        }
        function onNoteContextMenuRequested(noteIndex) {
            noteContextMenu.noteIndex = noteIndex
            noteContextMenu.popup(canvas.cursorX, canvas.cursorY)
        }
    }

    Ui.UiStateSerialisation {
        project: window.project
        onReadyToLoad: {
            // Old project files and image projects don't have UI state.
            if (project && project.uiState.contains("mainSplitViewState")) {
                // Restore project state.
                mainSplitView.restoreState(project.uiState.base64ToBinary(
                    project.uiState.value("mainSplitViewState")))
                panelSplitView.restoreState(project.uiState.base64ToBinary(
                    project.uiState.value("panelSplitViewState")))
            } else {
                // We should still restore the default sizes for all other cases, though.
                // We only need to restore panelSplitView's preferredWidth, as mainSplitView fills.
                panelSplitView.SplitView.preferredWidth = panelSplitView.defaultPreferredWidth
            }
        }
        onReadyToSave: {
            // Save project state.
            project.uiState.setValue("mainSplitViewState",
                project.uiState.binaryToBase64(mainSplitView.saveState()))
            project.uiState.setValue("panelSplitViewState",
                project.uiState.binaryToBase64(panelSplitView.saveState()))
        }
    }

    Ui.Shortcuts {
        window: window
        canvasContainer: canvasContainer
        canvas: window.canvas
        saveChangesDialog: saveChangesDialog
    }

    menuBar: Ui.MenuBar {
        id: menuBar
        canvas: window.canvas
        hueSaturationDialog: hueSaturationDialog
        opacityDialog: opacityDialog
        canvasSizePopup: canvasSizePopup
        imageSizePopup: imageSizePopup
        moveContentsDialog: moveContentsDialog
        texturedFillSettingsDialog: texturedFillSettingsDialog
        aboutDialog: aboutDialog
        saveChangesDialog: saveChangesDialog
        addGuidesDialog: addGuidesDialog
    }

    header: Ui.ToolBar {
        id: toolBar
        objectName: "toolBar"
        project: window.project
        canvas: window.canvas
        canvasSizePopup: canvasSizePopup
        imageSizePopup: imageSizePopup
    }

    SplitView {
        id: mainSplitView
        objectName: "mainSplitView"
        anchors.fill: parent
        handle: Item {
            implicitWidth: 4
        }

        Layout.fillWidth: true

        Ui.CanvasContainer {
            id: canvasContainer
            focus: true

            checkedToolButton: toolBar.toolButtonGroup.checkedButton

            SplitView.preferredWidth: window.width / 3
            SplitView.fillWidth: true
        }

        SplitView {
            id: panelSplitView
            objectName: "panelSplitView"
            orientation: Qt.Vertical
            handle: Item {
                implicitHeight: 4
            }

            readonly property int defaultPreferredWidth: 240

            SplitView.minimumWidth: 200
            SplitView.preferredWidth: defaultPreferredWidth
            SplitView.maximumWidth: window.width / 3

            Ui.ColourPanel {
                id: colourPanel
                canvas: window.canvas
                project: window.project

                SplitView.minimumHeight: expanded ? minimumUsefulHeight : undefined
                SplitView.maximumHeight: expanded ? implicitHeight : header.implicitHeight
            }

            Ui.SwatchPanel {
                id: swatchesPanel
                canvas: window.canvas
                project: window.project

                SplitView.minimumHeight: expanded ? minimumUsefulHeight : undefined
                SplitView.preferredHeight: minimumUsefulHeight + 100
                SplitView.maximumHeight: expanded ? Infinity : header.implicitHeight
            }

            Loader {
                objectName: "tilesetSwatchLoader"
                active: window.projectType === Project.TilesetType && window.canvas
                visible: active
                sourceComponent: Ui.TilesetSwatchPanel {
                    id: tilesetSwatch
                    tileCanvas: window.canvas
                    project: window.project
                    // Don't let e.g. the pencil icon go under us.
                    z: canvasContainer.z - 1
                }

                SplitView.minimumHeight: active && item.expanded ? item.header.implicitHeight : undefined
                SplitView.maximumHeight: active ? (item.expanded ? Infinity : item.header.implicitHeight) : 0
                SplitView.fillHeight: active && item.expanded
            }

            Loader {
                objectName: "layersLoader"
                active: window.isLayeredImageProjectType && window.canvas
                visible: active
                sourceComponent: Ui.LayerPanel {
                    layeredImageCanvas: window.canvas
                    project: window.project
                    z: canvasContainer.z - 1
                }

                SplitView.minimumHeight: active && item.expanded ? item.minimumUsefulHeight : undefined
                SplitView.maximumHeight: active ? (item.expanded ? Infinity : item.header.implicitHeight) : 0
                SplitView.fillHeight: active && item.expanded
            }

            Ui.AnimationPanel {
                id: animationPanel
                visible: window.project && window.project.loaded && isImageProjectType && window.project.usingAnimation
                project: visible ? window.project : null
                canvas: window.canvas

                SplitView.minimumHeight: expanded ? minimumUsefulHeight : undefined
                SplitView.maximumHeight: visible ? (expanded ? Infinity : header.implicitHeight) : 0
                SplitView.fillHeight: expanded
            }
        }
    }

    readonly property var imageFilters: ["PNG files (*.png)", "BMP files (*.bmp)"]
    readonly property var layeredImageFilters: ["SLP files (*.slp)"]
    readonly property var tilesetFilters: ["STP files (*.stp)"]

    function nameFiltersForProjectType(projectType) {
        return projectType === Project.ImageType ? imageFilters
            : projectType === Project.LayeredImageType ? layeredImageFilters
            : tilesetFilters;
    }

    Platform.FileDialog {
        id: openProjectDialog
        objectName: "openProjectDialog"
        nameFilters: ["All files (*)", "PNG files (*.png)", "BMP files (*.bmp)", "SLP files (*.slp)", "STP files (*.stp)"]
        defaultSuffix: projectManager.projectExtensionForType(Project.ImageType)
        onAccepted: loadProject(file)
        folder: (project && project.loaded) ? project.dirUrl : ""
    }

    Platform.FileDialog {
        id: saveAsDialog
        objectName: "saveAsDialog"
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: nameFiltersForProjectType(projectType)
        defaultSuffix: projectManager.projectExtensionForType(projectType)
        onAccepted: project.saveAs(file)
    }

    Platform.FileDialog {
        id: exportDialog
        objectName: "exportAsDialog"
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: imageFilters
        defaultSuffix: projectManager.projectExtensionForType(Project.ImageType)
        onAccepted: project.exportImage(file)
    }

    Ui.ErrorPopup {
        id: errorPopup
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
    }

    Ui.NewProjectPopup {
        id: newProjectPopup
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2

        onChoseTilesetProject: newTilesetProjectPopup.open()
        onChoseImageProject: newImageProjectPopup.open()
        onChoseLayeredImageProject: newLayeredImageProjectPopup.open()
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
        projectManager.beginCreation(projectManager.projectTypeForUrl(url));
        projectManager.temporaryProject.load(url);
        projectManager.completeCreation();
    }

    Ui.NewTilesetProjectPopup {
        id: newTilesetProjectPopup
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2

        onAccepted: createNewProject(Project.TilesetType)
    }

    Ui.NewImageProjectPopup {
        id: newImageProjectPopup
        objectName: "newImageProjectPopup"
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2

        onAccepted: createNewProject(Project.ImageType)
    }

    Ui.NewLayeredImageProjectPopup {
        id: newLayeredImageProjectPopup
        objectName: "newLayeredImageProjectPopup"
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2

        onAccepted: createNewProject(Project.LayeredImageType)
    }

    Ui.OptionsDialog {
        id: optionsDialog
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
    }

    Ui.SaveChangesDialog {
        id: saveChangesDialog
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        project: projectManager.project
        saveAsDialog: window.saveAsDialog
    }

    Ui.HueSaturationDialog {
        id: hueSaturationDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: projectManager.project
        canvas: window.canvas
    }

    Ui.OpacityDialog {
        id: opacityDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: projectManager.project
        canvas: window.canvas
    }

    Ui.CanvasSizePopup {
        id: canvasSizePopup
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: projectManager.project
        canvas: window.canvas
    }

    Ui.ImageSizePopup {
        id: imageSizePopup
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: projectManager.project
    }

    Ui.MoveContentsDialog {
        id: moveContentsDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: window.isLayeredImageProjectType ? projectManager.project : null
    }

    Ui.TexturedFillSettingsDialog {
        id: texturedFillSettingsDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: projectManager.project
        canvas: window.canvas
    }

    Ui.NoteDialog {
        id: noteDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: projectManager.project
        canvas: window.canvas
    }

    Ui.NoteContextMenu {
        id: noteContextMenu
        canvas: window.canvas
    }

    Ui.AddGuidesDialog {
        id: addGuidesDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        project: projectManager.project
        canvas: window.canvas
    }

    Ui.AboutDialog {
        id: aboutDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
    }
}

