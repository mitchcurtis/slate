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

    property var canvasSizePopup
    property var imageSizePopup

    delegate: MenuBarItem {
        focusPolicy: Qt.NoFocus
        objectName: menu.title.toLowerCase() + "MenuBarItem"
    }

    Menu {
        title: qsTr("File")

        MenuItem {
            objectName: "newMenuButton"
            text: qsTr("New")
            onTriggered: doIfChangesDiscarded(function() { newProjectPopup.open() })
        }

        MenuItem {
            objectName: "openMenuButton"
            text: qsTr("Open")
            onTriggered: doIfChangesDiscarded(function() { openProjectDialog.open() })
        }

        MenuItem {
            objectName: "saveMenuButton"
            text: qsTr("Save")
            enabled: project ? project.canSave : false
            onTriggered: projectManager.saveOrSaveAs()
        }

        MenuItem {
            objectName: "saveAsMenuButton"
            text: qsTr("Save As")
            enabled: project ? project.loaded : false
            onTriggered: saveAsDialog.open()
        }

        MenuItem {
            id: exportMenuButton
            objectName: "exportMenuButton"
            text: qsTr("Export")
            enabled: project && project.loaded && projectType === Project.LayeredImageType
            onTriggered: exportDialog.open()
        }

        MenuItem {
            objectName: "autoExportMenuButton"
            text: qsTr("Auto Export")
            checkable: true
            checked: enabled && project.autoExportEnabled
            enabled: exportMenuButton.enabled
            onTriggered: project.autoExportEnabled = !project.autoExportEnabled
        }

        MenuItem {
            objectName: "closeMenuButton"
            text: qsTr("Close")
            enabled: project ? project.loaded : false
            onTriggered: doIfChangesDiscarded(function() { project.close() })
        }

        MenuItem {
            objectName: "revertMenuButton"
            text: qsTr("Revert")
            enabled: project ? project.loaded && project.unsavedChanges : false
            onTriggered: project.revert()
        }
    }

    Menu {
        title: qsTr("Edit")

        MenuItem {
            objectName: "undoMenuButton"
            text: qsTr("Undo")
            onTriggered: project.undoStack.undo()
            enabled: project ? project.undoStack.canUndo : false
        }

        MenuItem {
            objectName: "redoMenuButton"
            text: qsTr("Redo")
            onTriggered: project.undoStack.redo()
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
            onTriggered: canvas.copySelection()
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "pasteMenuButton"
            text: qsTr("Paste")
            onTriggered: canvas.paste()
            enabled: isImageProjectType && canvas
        }

        MenuSeparator {}

        MenuItem {
            objectName: "flipHorizontallyMenuButton"
            text: qsTr("Flip Horizontally")
            onTriggered: canvas.flipSelection(Qt.Horizontal)
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }

        MenuItem {
            objectName: "flipVerticallyMenuButton"
            text: qsTr("Flip Vertically")
            onTriggered: canvas.flipSelection(Qt.Vertical)
            enabled: isImageProjectType && canvas && canvas.hasSelection
        }
    }

    Menu {
        title: qsTr("Image")

        MenuItem {
            objectName: "changeCanvasSizeMenuButton"
            text: qsTr("Canvas Size...")
            enabled: canvas
            onTriggered: canvasSizePopup.open()
        }

        MenuItem {
            objectName: "changeImageSizeMenuButton"
            text: qsTr("Image Size...")
            enabled: canvas && projectType === Project.ImageType
            onTriggered: imageSizePopup.open()
        }
    }

    Menu {
        title: qsTr("Animation")

        MenuItem {
            id: animationPlaybackMenuButton
            objectName: "animationPlaybackMenuButton"
            text: qsTr("Animation Playback")
            enabled: isImageProjectType && canvas
            checkable: true
            checked: isImageProjectType && project.usingAnimation
            onTriggered: project.usingAnimation = checked
        }

        MenuItem {
            objectName: "animationPlayMenuButton"
            text: enabled && !project.animationPlayback.playing ? qsTr("Play") : qsTr("Pause")
            enabled: animationPlaybackMenuButton.checked
            onTriggered: project.animationPlayback.playing = !project.animationPlayback.playing
        }
    }

    Menu {
        title: qsTr("View")

        MenuItem {
            objectName: "centreMenuButton"
            text: qsTr("Centre")
            enabled: canvas
            onTriggered: canvas.centreView()
        }

        MenuItem {
            objectName: "zoomInMenuButton"
            text: qsTr("Zoom In")
            enabled: canvas
            onTriggered: canvas.zoomIn()
        }

        MenuItem {
            objectName: "zoomOutMenuButton"
            text: qsTr("Zoom Out")
            enabled: canvas
            onTriggered: canvas.zoomOut()
        }

        MenuSeparator {}

        MenuItem {
            objectName: "showGridMenuButton"
            text: qsTr("Show Grid")
            enabled: canvas
            checkable: true
            checked: settings.gridVisible
            onTriggered: settings.gridVisible = checked
        }

        MenuItem {
            objectName: "showRulersMenuButton"
            text: qsTr("Show Rulers")
            enabled: canvas
            checkable: true
            checked: settings.rulersVisible
            onTriggered: settings.rulersVisible = checked
        }

        MenuItem {
            objectName: "showGuidesMenuButton"
            text: qsTr("Show Guides")
            enabled: canvas
            checkable: true
            checked: settings.guidesVisible
            onTriggered: settings.guidesVisible = checked
        }

        MenuItem {
            objectName: "lockGuidesMenuButton"
            text: qsTr("Lock Guides")
            enabled: canvas
            checkable: true
            checked: settings.guidesLocked
            onTriggered: settings.guidesLocked = checked
        }

        MenuSeparator {}

        MenuItem {
            objectName: "splitScreenMenuButton"
            text: qsTr("Split Screen")
            enabled: canvas
            checkable: true
            checked: settings.splitScreen
            onTriggered: settings.splitScreen = checked
        }

        MenuItem {
            objectName: "splitterLockedMenuButton"
            text: qsTr("Lock Splitter")
            enabled: canvas && settings.splitScreen
            checkable: true
            checked: settings.splitterLocked
            onTriggered: settings.splitterLocked = checked
        }

        MenuItem {
            objectName: "scrollZoomMenuButton"
            text: qsTr("Scroll Zoom")
            enabled: canvas
            checkable: true
            checked: settings.scrollZoom
            onTriggered: settings.scrollZoom = checked
        }
    }

    Menu {
        title: qsTr("Tools")

        MenuItem {
            objectName: "optionsMenuButton"
            text: qsTr("Options")
            onTriggered: optionsDialog.open()
        }
    }
}
