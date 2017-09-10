import Qt.labs.platform 1.0 as Platform
import QtQuick 2.0

import App 1.0

Item {
    property ImageCanvas canvas
    property ProjectManager projectManager
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType

    Platform.MenuBar {
        Platform.Menu {
            id: fileMenu
            objectName: "fileMenu"
            title: qsTr("File")

            Platform.MenuItem {
                objectName: "newMenuButton"
                text: qsTr("New")
                onTriggered: doIfChangesDiscarded(function() { newProjectPopup.open() })
            }

            Platform.MenuItem {
                objectName: "openMenuButton"
                text: qsTr("Open")
                onTriggered: doIfChangesDiscarded(function() { openProjectDialog.open() })
            }

            Platform.MenuItem {
                objectName: "saveMenuButton"
                text: qsTr("Save")
                enabled: project ? project.canSave : false
                onTriggered: projectManager.saveOrSaveAs()
            }

            Platform.MenuItem {
                objectName: "saveAsMenuButton"
                text: qsTr("Save As")
                enabled: project ? project.loaded : false
                onTriggered: saveAsDialog.open()
            }

            Platform.MenuItem {
                objectName: "exportMenuButton"
                text: qsTr("Export")
                enabled: project ? project.loaded && projectType === Project.LayeredImageType : false
                onTriggered: exportDialog.open()
            }

            Platform.MenuItem {
                objectName: "closeMenuButton"
                text: qsTr("Close")
                enabled: project ? project.loaded : false
                onTriggered: doIfChangesDiscarded(function() { project.close() })
            }

            Platform.MenuItem {
                objectName: "revertMenuButton"
                text: qsTr("Revert")
                enabled: project ? project.loaded && project.unsavedChanges : false
                onTriggered: project.revert()
            }
        }

        Platform.Menu {
            id: editMenu
            objectName: "editMenu"
            title: qsTr("Edit")

            Platform.MenuItem {
                objectName: "undoMenuButton"
                text: qsTr("Undo")
                onTriggered: project.undoStack.undo()
                enabled: project ? project.undoStack.canUndo : false
            }

            Platform.MenuItem {
                objectName: "redoMenuButton"
                text: qsTr("Redo")
                onTriggered: project.undoStack.redo()
                enabled: project ? project.undoStack.canRedo : false
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "copyMenuButton"
                text: qsTr("Copy")
                onTriggered: canvas.copySelection()
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            Platform.MenuItem {
                objectName: "pasteMenuButton"
                text: qsTr("Paste")
                onTriggered: canvas.paste()
                enabled: isImageProjectType && canvas
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "selectAllMenuButton"
                text: qsTr("Select All")
                onTriggered: canvas.selectAll()
                enabled: isImageProjectType && canvas
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "flipHorizontallyMenuButton"
                text: qsTr("Flip Horizontally")
                onTriggered: canvas.flipSelection(Qt.Horizontal)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            Platform.MenuItem {
                objectName: "flipVerticallyMenuButton"
                text: qsTr("Flip Vertically")
                onTriggered: canvas.flipSelection(Qt.Vertical)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }
        }

        Platform.Menu {
            id: viewMenu
            objectName: "viewMenu"
            title: qsTr("View")

            Platform.MenuItem {
                objectName: "centreMenuButton"
                text: qsTr("Centre")
                enabled: canvas
                onTriggered: canvas.centreView()
            }

            Platform.MenuSeparator {}

            Platform.MenuItem {
                objectName: "showGridMenuButton"
                text: qsTr("Show Grid")
                enabled: canvas
                checkable: true
                checked: settings.gridVisible
                onCheckedChanged: settings.gridVisible = checked
            }

            Platform.MenuItem {
                objectName: "showRulersMenuButton"
                text: qsTr("Show Rulers")
                enabled: canvas
                checkable: true
                checked: settings.rulersVisible
                onCheckedChanged: settings.rulersVisible = checked
            }

            Platform.MenuItem {
                objectName: "showGuidesMenuButton"
                text: qsTr("Show Guides")
                enabled: canvas
                checkable: true
                checked: settings.guidesVisible
                onCheckedChanged: settings.guidesVisible = checked
            }

            Platform.MenuItem {
                objectName: "splitScreenMenuButton"
                text: qsTr("Split Screen")
                enabled: canvas
                checkable: true
                checked: settings.splitScreen
                onCheckedChanged: settings.splitScreen = checked
            }

            Platform.MenuItem {
                objectName: "splitterLockedMenuButton"
                text: qsTr("Lock Splitter")
                enabled: canvas && settings.splitScreen
                checkable: true
                checked: settings.splitterLocked
                onCheckedChanged: settings.splitterLocked = checked
            }

            Platform.MenuItem {
                objectName: "scrollZoomMenuButton"
                text: qsTr("Scroll Zoom")
                enabled: canvas
                checkable: true
                checked: settings.scrollZoom
                onCheckedChanged: settings.scrollZoom = checked
            }
        }

        Platform.Menu {
            id: settingsMenu
            title: qsTr("Tools")

            Platform.MenuItem {
                objectName: "settingsMenuButton"
                text: qsTr("Options")
                onTriggered: optionsDialog.open()
            }
        }
    }
}
