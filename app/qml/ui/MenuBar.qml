import Qt.labs.platform 1.0 as Platform
import QtQml 2.2

import App 1.0

Platform.MenuBar {
    property ImageCanvas canvas
    property ProjectManager projectManager
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType

    property var canvasSizePopup
    property var imageSizePopup

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
            // Can't use qsTr() in submenu titles: https://bugreports.qt.io/browse/QTBUG-66876
            title: "Recent Files"
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
                text: qsTr("Clear Recent Files")
                onTriggered: settings.clearRecentFiles()
            }
        }

        Platform.MenuItem {
            objectName: "openMenuItem"
            text: qsTr("Open")
            onTriggered: doIfChangesDiscarded(function() { openProjectDialog.open() }, true)
        }

        Platform.MenuItem {
            objectName: "showLocationMenuItem"
            text: qsTr("Show Location")
            enabled: project && project.loaded
            onTriggered: Qt.openUrlExternally(Url.path(project.url))
        }

        Platform.MenuSeparator {}

        Platform.MenuItem {
            objectName: "saveMenuItem"
            text: qsTr("Save")
            enabled: project && project.canSave
            onTriggered: projectManager.saveOrSaveAs()
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
            text: qsTr("Export")
            enabled: project && project.loaded && projectType === Project.LayeredImageType
            onTriggered: exportDialog.open()
        }

        Platform.MenuItem {
            objectName: "autoExportMenuItem"
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
            text: qsTr("Revert")
            enabled: project && project.loaded && project.unsavedChanges
            onTriggered: project.revert()
        }
    }

    Platform.Menu {
        id: editMenu
        objectName: "editMenu"
        title: qsTr("Edit")

        Platform.MenuItem {
            objectName: "undoMenuItem"
            text: qsTr("Undo")
            onTriggered: project.undoStack.undo()
            enabled: project && project.undoStack.canUndo
        }

        Platform.MenuItem {
            objectName: "redoMenuItem"
            text: qsTr("Redo")
            onTriggered: project.undoStack.redo()
            enabled: project && project.undoStack.canRedo
        }

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

        Platform.MenuItem {
            objectName: "changeCanvasSizeMenuItem"
            text: qsTr("Canvas Size...")
            shortcut: settings.resizeCanvasShortcut
            enabled: canvas
            onTriggered: canvasSizePopup.open()
        }

        Platform.MenuItem {
            objectName: "changeImageSizeMenuItem"
            text: qsTr("Image Size...")
            shortcut: settings.resizeImageShortcut
            enabled: canvas && projectType === Project.ImageType
            onTriggered: imageSizePopup.open()
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
            text: qsTr("Centre")
            enabled: canvas
            onTriggered: canvas.centreView()
        }

        Platform.MenuSeparator {}

        Platform.MenuItem {
            objectName: "showGridMenuItem"
            text: qsTr("Show Grid")
            enabled: canvas
            checkable: true
            checked: settings.gridVisible
            onTriggered: settings.gridVisible = checked
        }

        Platform.MenuItem {
            objectName: "showRulersMenuItem"
            text: qsTr("Show Rulers")
            enabled: canvas
            checkable: true
            checked: settings.rulersVisible
            onTriggered: settings.rulersVisible = checked
        }

        Platform.MenuItem {
            objectName: "showGuidesMenuItem"
            text: qsTr("Show Guides")
            enabled: canvas
            checkable: true
            checked: settings.guidesVisible
            onTriggered: settings.guidesVisible = checked
        }

        Platform.MenuItem {
            objectName: "lockGuidesMenuItem"
            text: qsTr("Lock Guides")
            enabled: canvas
            checkable: true
            checked: settings.guidesLocked
            onTriggered: settings.guidesLocked = checked
        }

        Platform.MenuSeparator {}

        Platform.MenuItem {
            objectName: "splitScreenMenuItem"
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
}
