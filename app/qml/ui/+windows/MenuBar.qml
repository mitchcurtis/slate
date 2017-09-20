import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3

import App 1.0

MenuBar {
    property ImageCanvas canvas
    property ProjectManager projectManager
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType

    // The only reason we use the verbose syntax is so that we can
    // easily test menu interaction.
    MenuBarItem {
        objectName: "fileMenuBarItem"
        focusPolicy: Qt.TabFocus
        menu: Menu {
            title: qsTr("File")

            MenuItem {
                objectName: "newMenuButton"
                text: qsTr("New")
                hoverEnabled: true
                onClicked: doIfChangesDiscarded(function() { newProjectPopup.open() })
            }

            MenuItem {
                objectName: "openMenuButton"
                text: qsTr("Open")
                hoverEnabled: true
                onClicked: doIfChangesDiscarded(function() { openProjectDialog.open() })
            }

            MenuItem {
                objectName: "saveMenuButton"
                text: qsTr("Save")
                enabled: project ? project.canSave : false
                hoverEnabled: true
                onClicked: projectManager.saveOrSaveAs()
            }

            MenuItem {
                objectName: "saveAsMenuButton"
                text: qsTr("Save As")
                enabled: project ? project.loaded : false
                hoverEnabled: true
                onClicked: saveAsDialog.open()
            }

            MenuItem {
                objectName: "exportMenuButton"
                text: qsTr("Export")
                enabled: project ? project.loaded && projectType === Project.LayeredImageType : false
                onClicked: exportDialog.open()
            }

            MenuItem {
                objectName: "closeMenuButton"
                text: qsTr("Close")
                enabled: project ? project.loaded : false
                hoverEnabled: true
                onClicked: doIfChangesDiscarded(function() { project.close() })
            }

            MenuItem {
                objectName: "revertMenuButton"
                text: qsTr("Revert")
                enabled: project ? project.loaded && project.unsavedChanges : false
                hoverEnabled: true
                onClicked: project.revert()
            }
        }
    }

    MenuBarItem {
        objectName: "editMenuBarItem"
        focusPolicy: Qt.TabFocus
        menu: Menu {
            title: qsTr("Edit")

            MenuItem {
                objectName: "undoMenuButton"
                text: qsTr("Undo")
                onClicked: project.undoStack.undo()
                enabled: project ? project.undoStack.canUndo : false
            }

            MenuItem {
                objectName: "redoMenuButton"
                text: qsTr("Redo")
                onClicked: project.undoStack.redo()
                enabled: project ? project.undoStack.canRedo : false
            }

            MenuSeparator {}

            MenuItem {
                objectName: "selectAllMenuButton"
                text: qsTr("Select All")
                onTriggered: canvas.selectAll()
                enabled: isImageProjectType && canvas
            }

            MenuSeparator {}

            MenuItem {
                objectName: "copyMenuButton"
                text: qsTr("Copy")
                onClicked: canvas.copySelection()
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            MenuItem {
                objectName: "pasteMenuButton"
                text: qsTr("Paste")
                onClicked: canvas.paste()
                enabled: isImageProjectType && canvas
            }

            MenuSeparator {}

            MenuItem {
                objectName: "flipHorizontallyMenuButton"
                text: qsTr("Flip Horizontally")
                onClicked: canvas.flipSelection(Qt.Horizontal)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }

            MenuItem {
                objectName: "flipVerticallyMenuButton"
                text: qsTr("Flip Vertically")
                onClicked: canvas.flipSelection(Qt.Vertical)
                enabled: isImageProjectType && canvas && canvas.hasSelection
            }
        }
    }

    MenuBarItem {
        objectName: "viewMenuBarItem"
        focusPolicy: Qt.TabFocus
        menu: Menu {
            title: qsTr("View")

            MenuItem {
                objectName: "centreMenuButton"
                text: qsTr("Centre")
                enabled: canvas
                onClicked: canvas.centreView()
            }

            MenuItem {
                objectName: "zoomInMenuButton"
                text: qsTr("Zoom In")
                enabled: canvas
                onClicked: canvas.zoomIn()
            }

            MenuItem {
                objectName: "zoomOutMenuButton"
                text: qsTr("Zoom Out")
                enabled: canvas
                onClicked: canvas.zoomOut()
            }

            MenuSeparator {}

            MenuItem {
                objectName: "showGridMenuButton"
                text: qsTr("Show Grid")
                enabled: canvas
                checkable: true
                checked: settings.gridVisible
                onCheckedChanged: settings.gridVisible = checked
            }

            MenuItem {
                objectName: "showRulersMenuButton"
                text: qsTr("Show Rulers")
                enabled: canvas
                checkable: true
                checked: settings.rulersVisible
                onCheckedChanged: settings.rulersVisible = checked
            }

            MenuItem {
                objectName: "showGuidesMenuButton"
                text: qsTr("Show Guides")
                enabled: canvas
                checkable: true
                checked: settings.guidesVisible
                onCheckedChanged: settings.guidesVisible = checked
            }

            MenuItem {
                objectName: "lockGuidesMenuButton"
                text: qsTr("Lock Guides")
                enabled: canvas
                checkable: true
                checked: settings.guidesLocked
                onCheckedChanged: settings.guidesLocked = checked
            }

            MenuSeparator {}

            MenuItem {
                objectName: "splitScreenMenuButton"
                text: qsTr("Split Screen")
                enabled: canvas
                checkable: true
                checked: settings.splitScreen
                onCheckedChanged: settings.splitScreen = checked
            }

            MenuItem {
                objectName: "splitterLockedMenuButton"
                text: qsTr("Lock Splitter")
                enabled: canvas && settings.splitScreen
                checkable: true
                checked: settings.splitterLocked
                onCheckedChanged: settings.splitterLocked = checked
            }

            MenuItem {
                objectName: "scrollZoomMenuButton"
                text: qsTr("Scroll Zoom")
                enabled: canvas
                checkable: true
                checked: settings.scrollZoom
                onCheckedChanged: settings.scrollZoom = checked
            }
        }
    }

    MenuBarItem {
        objectName: "optionsMenuBarItem"
        focusPolicy: Qt.TabFocus
        menu: Menu {
            title: qsTr("Tools")

            MenuItem {
                objectName: "settingsMenuButton"
                text: qsTr("Options")
                onClicked: optionsDialog.open()
            }
        }
    }
}
