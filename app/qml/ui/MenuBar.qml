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

import QtQml 2.2
import QtQuick 2.9
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3
import QtQuick.Controls 2.3 as Controls

import App 1.0

Controls.MenuBar {
    spacing: 10

    property ImageCanvas canvas
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType

    property var hueSaturationDialog
    property var canvasSizePopup
    property var imageSizePopup
    property var moveContentsDialog
    property var texturedFillSettingsDialog
    property var aboutDialog

    Menu {
        id: fileMenu
        title: qsTr("File")

        // Usually we'd set a NoFocus policy on controls so that we
        // can be sure that shortcuts are only activated when the canvas
        // has focus (i.e. no popups are open), but doing so on MenuItems
        // would prevent them from being navigable with the arrow keys.
        // Instead, we just force focus on the canvas when a menu closes.
        onClosed: canvas.forceActiveFocus()

        MenuItem {
            objectName: "newMenuButton"
            text: qsTr("New")
            onTriggered: doIfChangesDiscarded(function() { newProjectPopup.open() }, true)
        }

        MenuSeparator {}

        Menu {
            id: recentFilesSubMenu
            objectName: "recentFilesSubMenu"
            title: qsTr("Recent Files")
            // This can use LayoutGroup if it's ever implemented: https://bugreports.qt.io/browse/QTBUG-44078
            width: 400
            enabled: recentFilesInstantiator.count > 0

            onClosed: canvas.forceActiveFocus()

            Instantiator {
                id: recentFilesInstantiator
                objectName: "recentFilesInstantiator"
                model: settings.recentFiles
                delegate: MenuItem {
                    // We should elide on the right when it's possible without losing the styling:
                    // https://bugreports.qt.io/browse/QTBUG-70961
                    objectName: text + "MenuItem"
                    text: settings.displayableFilePath(modelData)
                    onTriggered: doIfChangesDiscarded(function() { loadProject(modelData) }, true)
                }

                onObjectAdded: recentFilesSubMenu.insertItem(index, object)
                onObjectRemoved: recentFilesSubMenu.removeItem(object)
            }

            MenuSeparator {}

            MenuItem {
                objectName: "clearRecentFilesMenuItem"
                text: qsTr("Clear Recent Files")
                onTriggered: settings.clearRecentFiles()
            }
        }

        MenuItem {
            objectName: "openMenuItem"
            text: qsTr("Open")
            onTriggered: doIfChangesDiscarded(function() { openProjectDialog.open() }, true)
        }

        MenuSeparator {}

        MenuItem {
            objectName: "showLocationMenuItem"
            text: qsTr("Show Location")
            enabled: project && project.loaded
            onTriggered: Qt.openUrlExternally(project.dirUrl)
        }

        MenuSeparator {}

        MenuItem {
            objectName: "saveMenuItem"
            text: qsTr("Save")
            enabled: project && project.canSave
            onClicked: projectManager.saveOrSaveAs()
        }

        MenuItem {
            objectName: "saveAsMenuItem"
            text: qsTr("Save As")
            enabled: project && project.loaded
            onClicked: saveAsDialog.open()
        }

        MenuItem {
            id: exportMenuItem
            objectName: "exportMenuItem"
            text: qsTr("Export")
            enabled: project && project.loaded && projectType === Project.LayeredImageType
            onClicked: exportDialog.open()
        }

        MenuItem {
            objectName: "autoExportMenuItem"
            text: qsTr("Auto Export")
            checkable: true
            checked: enabled && project.autoExportEnabled
            enabled: exportMenuItem.enabled
            onTriggered: project.autoExportEnabled = !project.autoExportEnabled
        }

        MenuSeparator {}

        MenuItem {
            objectName: "closeMenuItem"
            text: qsTr("Close")
            enabled: project && project.loaded
            onClicked: doIfChangesDiscarded(function() { project.close() })
        }

        MenuSeparator {}

        MenuItem {
            objectName: "revertMenuItem"
            text: qsTr("Revert")
            enabled: project && project.loaded && project.unsavedChanges
            onClicked: project.revert()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "quitMenuItem"
            text: qsTr("Quit Slate")
            onTriggered: doIfChangesDiscarded(function() { Qt.quit() })
        }
    }

    Menu {
        id: editMenu
        title: qsTr("Edit")

        onClosed: canvas.forceActiveFocus()

        MenuItem {
            objectName: "undoMenuItem"
            text: qsTr("Undo")
            // See Shortcuts.qml for why we do it this way.
            enabled: project && canvas && (project.undoStack.canUndo || canvas.hasModifiedSelection)
            onClicked: canvas.undo()
        }

        MenuItem {
            objectName: "redoMenuItem"
            text: qsTr("Redo")
            enabled: project && project.undoStack.canRedo
            onClicked: project.undoStack.redo()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "copyMenuItem"
            text: qsTr("Copy")
            onClicked: canvas.copySelection()
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "pasteMenuItem"
            text: qsTr("Paste")
            onClicked: canvas.paste()
            enabled: isImageProjectType && canvas
        }

        MenuSeparator {}

        MenuItem {
            objectName: "selectAllMenuItem"
            text: qsTr("Select All")
            onTriggered: canvas.selectAll()
            enabled: isImageProjectType && canvas
        }

        MenuSeparator {}

        MenuItem {
            objectName: "texturedFillSettingsMenuItem"
            text: qsTr("Textured Fill Settings...")
            onTriggered: texturedFillSettingsDialog.open()
            enabled: isImageProjectType && canvas
        }

        MenuSeparator {}

        MenuItem {
            objectName: "rotateClockwiseMenuItem"
            text: qsTr("Rotate 90° Clockwise")
            onClicked: canvas.rotateSelection(90)
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "rotateCounterClockwiseMenuItem"
            text: qsTr("Rotate 90° Counter Clockwise")
            onClicked: canvas.rotateSelection(-90)
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "flipHorizontallyMenuItem"
            text: qsTr("Flip Horizontally")
            onClicked: canvas.flipSelection(Qt.Horizontal)
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "flipVerticallyMenuItem"
            text: qsTr("Flip Vertically")
            onClicked: canvas.flipSelection(Qt.Vertical)
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }
    }

    Menu {
        id: imageMenu
        title: qsTr("Image")

        onClosed: canvas.forceActiveFocus()

        Menu {
            objectName: "adjustmentsMenuItem"
            title: qsTr("Adjustments")

            MenuItem {
                objectName: "hueSaturationMenuItem"
                text: qsTr("Hue/Saturation...")
                enabled: isImageProjectType && canvas && canvas.hasSelection
                onTriggered: hueSaturationDialog.open()
            }
        }

        MenuSeparator {}

        MenuItem {
            objectName: "changeCanvasSizeMenuItem"
            text: qsTr("Canvas Size...")
            enabled: canvas
            onTriggered: canvasSizePopup.open()
        }

        MenuItem {
            objectName: "changeImageSizeMenuItem"
            text: qsTr("Image Size...")
            enabled: canvas && projectType === Project.ImageType
            onTriggered: imageSizePopup.open()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "moveContentsMenuItem"
            text: qsTr("Move Contents...")
            enabled: canvas && projectType === Project.LayeredImageType
            onTriggered: moveContentsDialog.open()
        }
    }

    Menu {
        id: layersMenu
        title: qsTr("Layers")

        onClosed: canvas.forceActiveFocus()

        MenuItem {
            objectName: "moveLayerUpMenuItem"
            text: qsTr("Move Layer Up")
            enabled: canvas && project && projectType === Project.LayeredImageType
                && project.currentLayerIndex > 0
            onTriggered: project.moveCurrentLayerUp()
        }

        MenuItem {
            objectName: "moveLayerDownMenuItem"
            text: qsTr("Move Layer Down")
            enabled: canvas && project && projectType === Project.LayeredImageType
                && project.currentLayerIndex < project.layerCount - 1
            onTriggered: project.moveCurrentLayerDown()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "mergeLayerUpMenuItem"
            text: qsTr("Merge Layer Up")
            enabled: canvas && project && project.currentLayerIndex > 0
            onTriggered: project.mergeCurrentLayerUp()
        }

        MenuItem {
            objectName: "mergeLayerDownMenuItem"
            text: qsTr("Merge Layer Down")
            enabled: canvas && project && projectType === Project.LayeredImageType
                && project.currentLayerIndex < project.layerCount - 1
            onTriggered: project.mergeCurrentLayerDown()
        }
    }

    Menu {
        id: animationMenu
        title: qsTr("Animation")
        // Give a bit of extra width for the longer menu item text.
        width: implicitWidth + 50

        onClosed: canvas.forceActiveFocus()

        MenuItem {
            id: animationPlaybackMenuItem
            objectName: "animationPlaybackMenuItem"
            text: qsTr("Animation Playback")
            enabled: isImageProjectType && canvas
            checkable: true
            checked: isImageProjectType && project.usingAnimation
            onTriggered: project.usingAnimation = checked
        }

        MenuItem {
            objectName: "animationPlayMenuItem"
            text: enabled && !project.animationPlayback.playing ? qsTr("Play") : qsTr("Pause")
            enabled: animationPlaybackMenuItem.checked
            onTriggered: project.animationPlayback.playing = !project.animationPlayback.playing
        }
    }

    Menu {
        id: viewMenu
        title: qsTr("View")

        onClosed: canvas.forceActiveFocus()

        MenuItem {
            objectName: "centreMenuItem"
            text: qsTr("Centre")
            enabled: canvas
            onClicked: canvas.centreView()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "showGridMenuItem"
            text: qsTr("Show Grid")
            enabled: canvas && projectType === Project.TilesetType
            checkable: true
            checked: settings.gridVisible
            onCheckedChanged: settings.gridVisible = checked
        }

        MenuItem {
            objectName: "showRulersMenuItem"
            text: qsTr("Show Rulers")
            enabled: canvas
            checkable: true
            checked: settings.rulersVisible
            onCheckedChanged: settings.rulersVisible = checked
        }

        MenuItem {
            objectName: "showGuidesMenuItem"
            text: qsTr("Show Guides")
            enabled: canvas
            checkable: true
            checked: settings.guidesVisible
            onTriggered: settings.guidesVisible = checked
        }

        MenuItem {
            objectName: "lockGuidesMenuItem"
            text: qsTr("Lock Guides")
            enabled: canvas
            checkable: true
            checked: settings.guidesLocked
            onTriggered: settings.guidesLocked = checked
        }

        MenuSeparator {}

        MenuItem {
            objectName: "splitScreenMenuItem"
            text: qsTr("Split Screen")
            enabled: canvas
            checkable: true
            checked: canvas && canvas.splitScreen
            onTriggered: canvas.splitScreen = checked
        }

        MenuItem {
            objectName: "splitterLockedMenuItem"
            text: qsTr("Lock Splitter")
            enabled: canvas && canvas.splitScreen
            checkable: true
            checked: canvas && canvas.splitter.enabled
            onTriggered: canvas.splitter.enabled = checked
        }

        MenuItem {
            objectName: "scrollZoomMenuItem"
            text: qsTr("Scroll Zoom")
            enabled: canvas
            checkable: true
            checked: settings.scrollZoom
            onTriggered: settings.scrollZoom = checked
        }
    }

    Menu {
        id: settingsMenu
        title: qsTr("Tools")

        onClosed: canvas.forceActiveFocus()

        MenuItem {
            objectName: "settingsMenuItem"
            text: qsTr("Options")
            onClicked: optionsDialog.open()
        }
    }

    Menu {
        id: helpMenu
        title: qsTr("Help")

        onClosed: canvas.forceActiveFocus()

        MenuItem {
            objectName: "onlineDocumentationMenuItem"
            text: qsTr("Online Documentation...")
            onTriggered: Qt.openUrlExternally("https://github.com/mitchcurtis/slate/blob/master/doc/overview.md")
        }

        MenuItem {
            objectName: "aboutMenuItem"
            text: qsTr("About Slate")
            onTriggered: aboutDialog.open()
        }
    }
}
