/*
    Copyright 2019, Mitch Curtis

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

import Qt.labs.platform 1.0 as Platform
import QtQml 2.2
import QtQuick 2.12

import App 1.0

Item {
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

    Platform.MenuBar {
        Platform.Menu {
            id: fileMenu
            objectName: "fileMenu"
            title: qsTr("File")

            Platform.MenuItem {
                objectName: "newMenuItem"
                text: qsTr("New")
                onTriggered: doIfChangesDiscarded(function() { newProjectPopup.open() }, true)
            }

            Platform.MenuSeparator {}

            Platform.Menu {
                id: recentFilesSubMenu
                objectName: "recentFilesSubMenu"
                title: qsTr("Recent Files")
                enabled: recentFilesInstantiator.count > 0

                Instantiator {
                    id: recentFilesInstantiator
                    objectName: "recentFilesInstantiator"
                    model: settings.recentFiles
                    delegate: Platform.MenuItem {
                        objectName: text + "MenuItem"
                        text: settings.displayableFilePath(modelData)
                        onTriggered: doIfChangesDiscarded(function() { loadProject(modelData) }, true)
                    }

                    onObjectAdded: recentFilesSubMenu.insertItem(index, object)
                    onObjectRemoved: recentFilesSubMenu.removeItem(object)
                }

                Platform.MenuSeparator {}

                Platform.MenuItem {
                    objectName: "clearRecentFilesMenuItem"
                    //: Empty the list of recent files in the File menu.
                    text: qsTr("Clear Recent Files")
                    onTriggered: settings.clearRecentFiles()
                }
            }

            Platform.MenuItem {
                objectName: "openMenuItem"
                text: qsTr("Open")
                onTriggered: doIfChangesDiscarded(function() { openProjectDialog.open() }, true)
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "showLocationMenuItem"
                //: Opens the project directory in the file explorer.
                text: qsTr("Show Location")
                enabled: project && project.loaded
                onTriggered: Qt.openUrlExternally(project.dirUrl)
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "saveMenuItem"
                text: qsTr("Save")
                enabled: project && project.canSave
                onTriggered: saveOrSaveAs()
            }

            Platform.MenuItem {
                objectName: "saveAsMenuItem"
                text: qsTr("Save As")
                enabled: project && project.loaded
                onTriggered: saveAsDialog.open()
            }

            Platform.MenuItem {
                id: exportMenuItem
                objectName: "exportMenuItem"
                //: Exports the project as a single image.
                text: qsTr("Export")
                enabled: project && project.loaded && projectType === Project.LayeredImageType
                onTriggered: exportDialog.open()
            }

            Platform.MenuItem {
                objectName: "autoExportMenuItem"
                //: Enables automatic exporting of the project as a single image each time the project is saved.
                text: qsTr("Auto Export")
                checkable: true
                checked: enabled && project.autoExportEnabled
                enabled: exportMenuItem.enabled
                onTriggered: project.autoExportEnabled = !project.autoExportEnabled
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "closeMenuItem"
                text: qsTr("Close")
                enabled: project && project.loaded
                onTriggered: doIfChangesDiscarded(function() { project.close() })
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "revertMenuItem"
                //: Loads the last saved version of the project, losing any unsaved changes.
                text: qsTr("Revert")
                enabled: project && project.loaded && project.unsavedChanges
                onTriggered: project.revert()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "quitMenuItem"
                text: qsTr("Quit Slate")
                onTriggered: doIfChangesDiscarded(function() { Qt.quit() })
            }
        }

        Platform.Menu {
            id: editMenu
            objectName: "editMenu"
            title: qsTr("Edit")

            Platform.MenuItem {
                objectName: "undoMenuItem"
                text: qsTr("Undo")
                // See Shortcuts.qml for why we do it this way.
                enabled: project && canvas && (project.undoStack.canUndo || canvas.hasModifiedSelection)
                onTriggered: canvas.undo()
            }

            Platform.MenuItem {
                objectName: "redoMenuItem"
                text: qsTr("Redo")
                onTriggered: project.undoStack.redo()
                enabled: project && project.undoStack.canRedo
            }

            // https://bugreports.qt.io/browse/QTBUG-67310
            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "copyMenuItem"
                text: qsTr("Copy")
                onTriggered: canvas.copySelection()
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            Platform.MenuItem {
                objectName: "pasteMenuItem"
                text: qsTr("Paste")
                onTriggered: canvas.paste()
                enabled: isImageProjectType && canvas
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "selectAllMenuItem"
                text: qsTr("Select All")
                onTriggered: canvas.selectAll()
                enabled: isImageProjectType && canvas
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "addSelectedColoursToTexturedFillSwatchMenuItem"
                text: qsTr("Add to Textured Fill Swatch...")
                onTriggered: {
                    canvas.addSelectedColoursToTexturedFillSwatch()
                    canvas.texturedFillParameters.type = TexturedFillParameters.SwatchFillType
                    texturedFillSettingsDialog.open()
                }
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            Platform.MenuItem {
                objectName: "texturedFillSettingsMenuItem"
                //: Opens a dialog that allows the user to change the settings for the Textured Fill tool.
                text: qsTr("Textured Fill Settings...")
                onTriggered: texturedFillSettingsDialog.open()
                enabled: isImageProjectType && canvas
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "rotateClockwiseMenuItem"
                //: Rotates the image 90 degrees to the right.
                text: qsTr("Rotate 90° Clockwise")
                onTriggered: canvas.rotateSelection(90)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            Platform.MenuItem {
                objectName: "rotateCounterClockwiseMenuItem"
                //: Rotates the image 90 degrees to the left.
                text: qsTr("Rotate 90° Counter Clockwise")
                onTriggered: canvas.rotateSelection(-90)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            Platform.MenuItem {
                objectName: "flipHorizontallyMenuItem"
                text: qsTr("Flip Horizontally")
                onTriggered: canvas.flipSelection(Qt.Horizontal)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            Platform.MenuItem {
                objectName: "flipVerticallyMenuItem"
                text: qsTr("Flip Vertically")
                onTriggered: canvas.flipSelection(Qt.Vertical)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }
        }

        Platform.Menu {
            objectName: "imageMenuBarItem"
            title: qsTr("Image")

            Platform.Menu {
                objectName: "adjustmentsMenuItem"
                title: qsTr("Adjustments")

                Platform.MenuItem {
                    objectName: "hueSaturationMenuItem"
                    text: qsTr("Hue/Saturation...")
                    enabled: isImageProjectType && canvas && canvas.hasSelection
                    onTriggered: hueSaturationDialog.open()
                }

                Platform.MenuItem {
                    objectName: "opacityMenuItem"
                    text: qsTr("Opacity...")
                    enabled: isImageProjectType && canvas && canvas.hasSelection
                    onTriggered: opacityDialog.open()
                }
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "changeCanvasSizeMenuItem"
                text: qsTr("Canvas Size...")
                enabled: canvas
                onTriggered: canvasSizePopup.open()
            }

            Platform.MenuItem {
                objectName: "changeImageSizeMenuItem"
                text: qsTr("Image Size...")
                enabled: isImageProjectType && canvas
                onTriggered: imageSizePopup.open()
            }

            Platform.MenuItem {
                objectName: "cropToSelectionMenuItem"
                text: qsTr("Crop to Selection")
                enabled: isImageProjectType && canvas && canvas.hasSelection
                onTriggered: project.crop(canvas.selectionArea)
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "moveContentsMenuItem"
                text: qsTr("Move Contents...")
                enabled: canvas && projectType === Project.LayeredImageType
                onTriggered: moveContentsDialog.open()
            }
        }

        Platform.Menu {
            objectName: "layersMenuBarItem"
            title: qsTr("Layers")

            Platform.MenuItem {
                objectName: "moveLayerUpMenuItem"
                //: Moves the current layer up in the list of layers in the Layer panel.
                text: qsTr("Move Layer Up")
                enabled: canvas && project && projectType === Project.LayeredImageType
                    && project.currentLayerIndex > 0
                onTriggered: project.moveCurrentLayerUp()
            }

            Platform.MenuItem {
                objectName: "moveLayerDownMenuItem"
                //: Moves the current layer down in the list of layers in the Layer panel.
                text: qsTr("Move Layer Down")
                enabled: canvas && project && projectType === Project.LayeredImageType
                    && project.currentLayerIndex < project.layerCount - 1
                onTriggered: project.moveCurrentLayerDown()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "mergeLayerUpMenuItem"
                //: Combines the current layer with the layer above it, then removes the current layer.
                text: qsTr("Merge Layer Up")
                enabled: canvas && project && project.currentLayerIndex > 0
                onTriggered: project.mergeCurrentLayerUp()
            }

            Platform.MenuItem {
                objectName: "mergeLayerDownMenuItem"
                //: Combines the current layer with the layer below it, then removes the current layer.
                text: qsTr("Merge Layer Down")
                enabled: canvas && project && projectType === Project.LayeredImageType
                    && project.currentLayerIndex < project.layerCount - 1
                onTriggered: project.mergeCurrentLayerDown()
            }
        }

        Platform.Menu {
            id: animationMenu
            objectName: "animationMenu"
            title: qsTr("Animation")

            Platform.MenuItem {
                id: animationPlaybackMenuItem
                objectName: "animationPlaybackMenuItem"
                text: qsTr("Animation Playback")
                enabled: isImageProjectType && canvas
                checkable: true
                checked: isImageProjectType && project.usingAnimation
                onTriggered: project.usingAnimation = checked
            }

            Platform.MenuItem {
                objectName: "animationPlayMenuItem"
                text: enabled && !project.animationPlayback.playing ? qsTr("Play") : qsTr("Pause")
                enabled: animationPlaybackMenuItem.checked
                onTriggered: project.animationPlayback.playing = !project.animationPlayback.playing
            }
        }

        Platform.Menu {
            id: viewMenu
            objectName: "viewMenu"
            title: qsTr("View")

            Platform.MenuItem {
                objectName: "centreMenuItem"
                //: Positions the canvas in the centre of the canvas pane.
                text: qsTr("Centre")
                enabled: canvas
                onTriggered: canvas.centreView()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "showGridMenuItem"
                //: Shows a grid that acts as a guide to distinguish tiles from one another.
                text: qsTr("Show Grid")
                enabled: canvas && projectType === Project.TilesetType
                checkable: true
                checked: settings.gridVisible
                onTriggered: settings.gridVisible = checked
            }

            Platform.MenuItem {
                objectName: "showRulersMenuItem"
                //: Shows rulers on the sides of each canvas pane that can be used to measure in pixels.
                text: qsTr("Show Rulers")
                enabled: canvas
                checkable: true
                checked: settings.rulersVisible
                onTriggered: settings.rulersVisible = checked
            }

            Platform.MenuItem {
                objectName: "showGuidesMenuItem"
                //: Shows coloured guides (vertical and horizontal lines) that can be dragged out from the rulers.
                text: qsTr("Show Guides")
                enabled: canvas
                checkable: true
                checked: settings.guidesVisible
                onTriggered: settings.guidesVisible = checked
            }

            Platform.MenuItem {
                objectName: "lockGuidesMenuItem"
                //: Prevents the guides from being moved by the user.
                text: qsTr("Lock Guides")
                enabled: canvas
                checkable: true
                checked: settings.guidesLocked
                onTriggered: settings.guidesLocked = checked
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "splitScreenMenuItem"
                //: Toggles split screen: two canvas panes are shown instead of one.
                text: qsTr("Split Screen")
                enabled: canvas
                checkable: true
                checked: canvas && canvas.splitScreen
                onTriggered: canvas.splitScreen = checked
            }

            Platform.MenuItem {
                objectName: "splitterLockedMenuItem"
                text: qsTr("Lock Splitter")
                enabled: canvas && canvas.splitScreen
                checkable: true
                checked: canvas && canvas.splitter.enabled
                onTriggered: canvas.splitter.enabled = checked
            }

            Platform.MenuItem {
                objectName: "scrollZoomMenuItem"
                text: qsTr("Scroll Zoom")
                enabled: canvas
                checkable: true
                checked: settings.scrollZoom
                onTriggered: settings.scrollZoom = checked
            }
        }

        Platform.Menu {
            id: settingsMenu
            title: qsTr("Tools")

            Platform.MenuItem {
                objectName: "optionsMenuItem"
                text: qsTr("Options")
                onTriggered: optionsDialog.open()
            }
        }

        Platform.Menu {
            id: helpMenu
            title: qsTr("Help")

            Platform.MenuItem {
                objectName: "onlineDocumentationMenuItem"
                text: qsTr("Online Documentation...")
                onTriggered: Qt.openUrlExternally("https://github.com/mitchcurtis/slate/blob/master/doc/overview.md")
            }

            Platform.MenuItem {
                objectName: "aboutMenuItem"
                text: qsTr("About Slate")
                onTriggered: aboutDialog.open()
            }
        }
    }
}
