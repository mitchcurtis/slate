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

import QtQuick 2.8
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQuick.Controls 2.1

import Qt.labs.settings 1.0
import Qt.labs.platform 1.0 as Platform

import App 1.0

import "ui" as Ui

/*
    TODO:
    - cursor ruler that follows the cursor as it moves to make it easy to
      line stuff up
    - fix performance when drawing pixels
    - fix performance when resizing on Windows
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
    title: project && project.loaded
        ? ((project.url.toString().length > 0 ? project.displayUrl : "Untitled") + (project.unsavedChanges ? "*" : ""))
        : ""
    visible: true

//    onActiveFocusItemChanged: print(activeFocusItem)

    property Project project: projectManager.project
    property int projectType: project && projectManager.ready ? project.type : 0
    property ImageCanvas canvas: canvasContainer.canvas
    property alias newProjectPopup: newProjectPopup
    property alias openProjectDialog: openProjectDialog
    property alias saveChangesDialog: discardChangesDialog
    property alias moveContentsDialog: moveContentsDialog
    property int toolTipDelay: 500
    property int toolTipTimeout: 2000

    onClosing: {
        close.accepted = false
        doIfChangesDiscarded(function() { Qt.quit() })
    }

    // If we set the image URL immediately, it can happen before
    // the error popup is ready.
    Component.onCompleted: {
        contentItem.objectName = "applicationWindowContentItem";

        if (settings.loadLastOnStartup && settings.recentFiles.length > 0) {
            loadProject(settings.recentFiles[0])
        } else {
            createNewProject(Project.LayeredImageType)
        }
    }

    function doIfChangesDiscarded(actionFunction, skipChangesConfirmationIfNoProject) {
        if ((skipChangesConfirmationIfNoProject === undefined || skipChangesConfirmationIfNoProject === true) && !project) {
            // If there's no project open, some features should be able to
            // be performed immediately, such as Open.
            actionFunction();
            return;
        }

        if (!project) {
            // Auto tests can skip this function.
            return;
        }

        if (!project.unsavedChanges) {
            actionFunction();
            return;
        }

        function disconnectSignals() {
            saveChangesDialog.accepted.disconnect(dontDiscardChanges);
            saveChangesDialog.discarded.disconnect(discardChanges);
        }

        function discardChanges() {
            actionFunction();
            disconnectSignals();
            // TODO: temporary until https://bugreports.qt.io/browse/QTBUG-67168 is fixed.
            saveChangesDialog.close();
        }

        function dontDiscardChanges() {
            disconnectSignals();
        }

        saveChangesDialog.accepted.connect(dontDiscardChanges);
        saveChangesDialog.discarded.connect(discardChanges);
        saveChangesDialog.open();
    }

    function saveOrSaveAs() {
        if (project.url.toString().length > 0) {
            project.save();
        } else {
            saveAsDialog.open();
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
        target: projectManager.project ? projectManager.project : null
        onErrorOccurred: errorPopup.showError(errorMessage)
    }

    Connections {
        target: projectManager
        onCreationFailed: errorPopup.showError(errorMessage)
    }

    Ui.Shortcuts {
        window: window
        canvasContainer: canvasContainer
        canvas: window.canvas
    }

    Ui.MenuBar {
        id: menuBar
        canvas: window.canvas
        canvasSizePopup: canvasSizePopup
        imageSizePopup: imageSizePopup
        moveContentsDialog: moveContentsDialog
        texturedFillSettingsDialog: texturedFillSettingsDialog
    }

    header: Ui.ToolBar {
        id: iconToolBar
        project: window.project
        canvas: window.canvas
        canvasSizePopup: canvasSizePopup
        imageSizePopup: imageSizePopup

        Layout.fillWidth: true
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Ui.CanvasContainer {
            id: canvasContainer
            focus: true

            checkedToolButton: iconToolBar.toolButtonGroup.checkedButton

            Layout.preferredWidth: window.width / 3
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ColumnLayout {
            id: panelColumnLayout

            Layout.minimumWidth: 240
            Layout.maximumWidth: 240
            Layout.alignment: Qt.AlignTop

            Ui.ColourPanel {
                id: colourPanel
                canvas: window.canvas
                project: window.project

                Layout.fillWidth: true
                Layout.minimumHeight: expanded ? header.implicitHeight + 200 : -1
                Layout.maximumHeight: expanded ? implicitHeight : header.implicitHeight
                Layout.fillHeight: expanded
            }

            Loader {
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
                Layout.fillWidth: true
                Layout.minimumHeight: active && item.expanded ? item.header.implicitHeight + 300 : -1
                Layout.maximumHeight: active ? (item.expanded ? -1 : item.header.implicitHeight) : 0
                Layout.fillHeight: active && item.expanded
            }

            Loader {
                objectName: "layersLoader"
                active: window.projectType === Project.LayeredImageType && window.canvas
                sourceComponent: Ui.LayerPanel {
                    objectName: "layerPanel"
                    layeredImageCanvas: window.canvas
                    project: window.project
                    z: canvasContainer.z - 1
                }

                Layout.preferredWidth: active ? colourPanel.implicitWidth : 0
                Layout.fillWidth: true
                Layout.minimumHeight: active && item.expanded ? item.header.implicitHeight + item.footer.implicitHeight + 100 : -1
                Layout.maximumHeight: active ? (item.expanded ? -1 : item.header.implicitHeight) : 0
                Layout.fillHeight: active && item.expanded
            }

            Ui.AnimationPanel {
                id: animationPanel
                visible: window.project && window.project.loaded && window.projectType === Project.LayeredImageType
                    && window.project.usingAnimation
                project: visible ? window.project : null

                Layout.preferredWidth: visible ? colourPanel.implicitWidth : 0
                Layout.fillWidth: true
                Layout.minimumHeight: expanded ? header.implicitHeight + 200 : -1
                Layout.maximumHeight: visible ? (expanded ? -1 : header.implicitHeight) : 0
                Layout.fillHeight: expanded
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
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2

        onAccepted: createNewProject(Project.ImageType)
    }

    Ui.NewLayeredImageProjectPopup {
        id: newLayeredImageProjectPopup
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2

        onAccepted: createNewProject(Project.LayeredImageType)
    }

    Ui.OptionsDialog {
        id: optionsDialog
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
    }

    Dialog {
        id: discardChangesDialog
        objectName: "discardChangesDialog"
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        title: qsTr("Unsaved changes")
        modal: true

        Label {
            text: qsTr("The action you're about to perform could discard changes.\n\nContinue anyway?")
        }

        // Using a DialogButtonBox allows us to assign objectNames to the buttons,
        // which makes it possible to test them.
        footer: DialogButtonBox {
            Button {
                objectName: "saveChangesDialogButton"
                text: qsTr("Save")
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            }
            Button {
                objectName: "discardChangesDialogButton"
                text: qsTr("Discard")
                DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
            }
        }
    }

    Ui.CanvasSizePopup {
        id: canvasSizePopup
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        project: projectManager.project
    }

    Ui.ImageSizePopup {
        id: imageSizePopup
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        project: projectManager.project
    }

    Ui.MoveContentsDialog {
        id: moveContentsDialog
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        project: projectManager.project
    }

    Ui.TexturedFillSettingsDialog {
        id: texturedFillSettingsDialog
        x: Math.round(parent.width - width) / 2
        y: Math.round(parent.height - height) / 2
        project: projectManager.project
        canvas: window.canvas
    }
}
