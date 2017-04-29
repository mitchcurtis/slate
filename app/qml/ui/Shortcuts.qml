import QtQuick 2.6

import App 1.0

Item {
    property var window
    property ProjectManager projectManager
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    property ImageCanvas canvas

    readonly property bool modalPopupsOpen: canvas ? canvas.modalPopupsOpen : false

    Shortcut {
        sequence: settings.quitShortcut
        onActivated: Qt.quit()
    }

    Shortcut {
        id: s
        sequence: settings.newShortcut
        onActivated: doIfChangesDiscarded(function() { newProjectPopup.open() })
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.openShortcut
        onActivated: doIfChangesDiscarded(function() { openProjectDialog.open() }, true)
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.saveShortcut
        onActivated: projectManager.saveOrSaveAs()
        enabled: !modalPopupsOpen && (project ? project.canSave : false)
    }

    Shortcut {
        sequence: settings.closeShortcut
        onActivated: doIfChangesDiscarded(function() { project.close() })
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.revertShortcut
        onActivated: project.revert()
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.undoShortcut
        onActivated: project.undoStack.undo()
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.redoShortcut
        onActivated: project.undoStack.redo()
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.gridVisibleShortcut
        onActivated: settings.gridVisible = !settings.gridVisible
    }

    Shortcut {
        sequence: settings.splitScreenShortcut
        onActivated: settings.splitScreen = !settings.splitScreen
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.centreShortcut
        onActivated: canvas.centreView()
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.optionsShortcut
        onActivated: optionsDialog.open()
        enabled: !modalPopupsOpen
    }

    Shortcut {
        sequence: settings.penToolShortcut
        onActivated: canvas.tool = TileCanvas.PenTool
    }

    Shortcut {
        sequence: settings.eyeDropperToolShortcut
        onActivated: canvas.tool = TileCanvas.EyeDropperTool
    }

    Shortcut {
        sequence: settings.eraserToolShortcut
        onActivated: canvas.tool = TileCanvas.EraserTool
    }

    Shortcut {
        sequence: settings.toolModeShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.mode = (canvas.mode === TileCanvas.TileMode ? TileCanvas.PixelMode : TileCanvas.TileMode)
    }

    Shortcut {
        sequence: settings.decreaseToolSizeShortcut
        onActivated: --canvas.toolSize
    }

    Shortcut {
        sequence: settings.increaseToolSizeShortcut
        onActivated: ++canvas.toolSize
    }

    Shortcut {
        sequence: settings.swatchLeftShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchLeft()
    }

    Shortcut {
        sequence: settings.swatchRightShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchRight()
    }

    Shortcut {
        sequence: settings.swatchUpShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchUp()
    }

    Shortcut {
        sequence: settings.swatchDownShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchDown()
    }
}
