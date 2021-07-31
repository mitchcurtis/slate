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

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls as Controls

import App

Controls.MenuBar {
    spacing: 10

    property ImageCanvas canvas
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType

    property var hueSaturationDialog
    property var opacityDialog
    property var canvasSizePopup
    property var imageSizePopup
    property var moveContentsDialog
    property var texturedFillSettingsDialog
    property var aboutDialog
    property SaveChangesDialog saveChangesDialog
    property AddGuidesDialog addGuidesDialog

    Menu {
        id: fileMenu
        title: qsTr("File")

        // Usually we'd set a NoFocus policy on controls so that we
        // can be sure that shortcuts are only activated when the canvas
        // has focus (i.e. no popups are open), but doing so on MenuItems
        // would prevent them from being navigable with the arrow keys.
        // Instead, we just force focus on the canvas when a menu closes.
        // Note that we need to check if the canvas exists, as the menu
        // can close when e.g. creating a new project without having one
        // already open.
        onClosed: if (canvas) canvas.forceActiveFocus()

        MenuItem {
            objectName: "newMenuButton"
            text: qsTr("New")
            onTriggered: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { newProjectPopup.open() }, true)
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
                    onTriggered: saveChangesDialog.doIfChangesSavedOrDiscarded(function() {
                        // If we load the project immediately, it causes the menu items to be removed immediately,
                        // which means the Menu that owns them will disconnect from the triggered() signal of the
                        // menu item, resulting in the menu not closing:
                        //
                        // https://github.com/mitchcurtis/slate/issues/128
                        //
                        // For some reason, this doesn't happen with native menus, possibly because
                        // the removal and insertion is delayed there.
                        Qt.callLater(function() {
                            loadProject(modelData)
                        })
                    }, true)
                }

                onObjectAdded: (index, object) => recentFilesSubMenu.insertItem(index, object)
                onObjectRemoved: (index, object) => recentFilesSubMenu.removeItem(object)
            }

            MenuSeparator {}

            MenuItem {
                objectName: "clearRecentFilesMenuItem"
                //: Empty the list of recent files in the File menu.
                text: qsTr("Clear Recent Files")
                onTriggered: settings.clearRecentFiles()
            }
        }

        MenuItem {
            objectName: "openMenuItem"
            text: qsTr("Open")
            onTriggered: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { openProjectDialog.open() }, true)
        }

        MenuSeparator {}

        MenuItem {
            objectName: "showLocationMenuItem"
            //: Opens the project directory in the file explorer.
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
            //: Exports the project as a single image.
            objectName: "exportMenuItem"
            text: qsTr("Export")
            enabled: project && project.loaded && projectType === Project.LayeredImageType
            onClicked: exportDialog.open()
        }

        MenuItem {
            objectName: "autoExportMenuItem"
            //: Enables automatic exporting of the project as a single image each time the project is saved.
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
            onClicked: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { project.close() })
        }

        MenuSeparator {}

        MenuItem {
            objectName: "revertMenuItem"
            //: Loads the last saved version of the project, losing any unsaved changes.
            text: qsTr("Revert")
            enabled: project && project.loaded && project.unsavedChanges
            onClicked: project.revert()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "quitMenuItem"
            text: qsTr("Quit Slate")
            onTriggered: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { Qt.quit() })
        }
    }

    Menu {
        id: editMenu
        title: qsTr("Edit")
        width: 240

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
            objectName: "addSelectedColoursToTexturedFillSwatchMenuItem"
            text: qsTr("Add to Textured Fill Swatch...")
            onTriggered: {
                canvas.addSelectedColoursToTexturedFillSwatch()
                canvas.texturedFillParameters.type = TexturedFillParameters.SwatchFillType
                texturedFillSettingsDialog.open()
            }
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "texturedFillSettingsMenuItem"
            //: Opens a dialog that allows the user to change the settings for the Textured Fill tool.
            text: qsTr("Textured Fill Settings...")
            onTriggered: texturedFillSettingsDialog.open()
            enabled: isImageProjectType && canvas
        }

        MenuSeparator {}

        MenuItem {
            objectName: "rotateClockwiseMenuItem"
            //: Rotates the image 90 degrees to the right.
            text: qsTr("Rotate 90° Clockwise")
            onClicked: canvas.rotateSelection(90)
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "rotateCounterClockwiseMenuItem"
            //: Rotates the image 90 degrees to the left.
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

            MenuItem {
                objectName: "opacityMenuItem"
                text: qsTr("Opacity...")
                enabled: isImageProjectType && canvas && canvas.hasSelection
                onTriggered: opacityDialog.open()
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
            enabled: isImageProjectType && canvas
            onTriggered: imageSizePopup.open()
        }

        MenuItem {
            objectName: "cropToSelectionMenuItem"
            text: qsTr("Crop to Selection")
            enabled: isImageProjectType && canvas && canvas.hasSelection
            onTriggered: project.crop(canvas.selectionArea)
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
            //: Moves the current layer up in the list of layers in the Layer panel.
            text: qsTr("Move Layer Up")
            enabled: canvas && project && projectType === Project.LayeredImageType
                && project.currentLayerIndex > 0
            onTriggered: project.moveCurrentLayerUp()
        }

        MenuItem {
            objectName: "moveLayerDownMenuItem"
            //: Moves the current layer down in the list of layers in the Layer panel.
            text: qsTr("Move Layer Down")
            enabled: canvas && project && projectType === Project.LayeredImageType
                && project.currentLayerIndex < project.layerCount - 1
            onTriggered: project.moveCurrentLayerDown()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "mergeLayerUpMenuItem"
            //: Combines the current layer with the layer above it, then removes the current layer.
            text: qsTr("Merge Layer Up")
            enabled: canvas && project && project.currentLayerIndex > 0
            onTriggered: project.mergeCurrentLayerUp()
        }

        MenuItem {
            objectName: "mergeLayerDownMenuItem"
            //: Combines the current layer with the layer below it, then removes the current layer.
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
            text: enabled && !playback.playing ? qsTr("Play") : qsTr("Pause")
            enabled: animationPlaybackMenuItem.checked && playback
            onTriggered: playback.playing = !playback.playing

            readonly property AnimationPlayback playback: project && project.animationSystem
                ? project.animationSystem.currentAnimationPlayback : null
        }
    }

    Menu {
        id: viewMenu
        title: qsTr("View")

        onClosed: canvas.forceActiveFocus()

        MenuItem {
            objectName: "centreMenuItem"
            //: Positions the canvas in the centre of the canvas pane.
            text: qsTr("Centre")
            enabled: canvas
            onClicked: canvas.centreView()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "showGridMenuItem"
            //: Shows a grid that acts as a guide to distinguish tiles from one another.
            text: qsTr("Show Grid")
            enabled: canvas && projectType === Project.TilesetType
            checkable: true
            checked: canvas && !!canvas.gridVisible
            onTriggered: canvas.gridVisible = checked
        }

        MenuItem {
            objectName: "showRulersMenuItem"
            //: Shows rulers on the sides of each canvas pane that can be used to measure in pixels.
            text: qsTr("Show Rulers")
            enabled: canvas
            checkable: true
            checked: canvas && canvas.rulersVisible
            onTriggered: canvas.rulersVisible = checked
        }

        MenuItem {
            objectName: "showGuidesMenuItem"
            //: Shows coloured guides (vertical and horizontal lines) that can be dragged out from the rulers.
            text: qsTr("Show Guides")
            enabled: canvas
            checkable: true
            checked: canvas && canvas.guidesVisible
            onTriggered: canvas.guidesVisible = checked
        }

        MenuItem {
            objectName: "lockGuidesMenuItem"
            //: Prevents the guides from being moved by the user.
            text: qsTr("Lock Guides")
            enabled: canvas
            checkable: true
            checked: canvas && canvas.guidesLocked
            onTriggered: canvas.guidesLocked = checked
        }

        MenuItem {
            objectName: "addGuidesMenuItem"
            //: Opens a dialog that allows adding multiple guides at once.
            text: qsTr("Add Guides...")
            enabled: canvas
            onTriggered: addGuidesDialog.open()
        }

        MenuItem {
            objectName: "deleteAllGuidesMenuItem"
            //: Deletes all guides.
            text: qsTr("Delete All Guides")
            enabled: canvas
            onTriggered: canvas.removeAllGuides()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "splitScreenMenuItem"
            //: Toggles split screen: two canvas panes are shown instead of one.
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

        MenuSeparator {}

        MenuItem {
            objectName: "showAnimationFrameMarkersMenuItem"
            text: qsTr("Show Animation Frame Markers")
            enabled: isImageProjectType && canvas && project.usingAnimation
            checkable: true
            checked: enabled && canvas.animationMarkersVisible
            onTriggered: canvas.animationMarkersVisible = checked
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
            onTriggered: Qt.openUrlExternally("https://github.com/mitchcurtis/slate/blob/release/doc/overview.md")
        }

        MenuItem {
            objectName: "aboutMenuItem"
            text: qsTr("About Slate")
            onTriggered: aboutDialog.open()
        }
    }
}
