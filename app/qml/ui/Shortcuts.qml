import QtQuick 2.6

import App 1.0

Item {
    objectName: "shortcuts"

    property var window
    property ProjectManager projectManager
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    property ImageCanvas canvas
    readonly property bool canvasHasActiveFocus: canvas ? canvas.activeFocus : false

    Shortcut {
        sequence: settings.quitShortcut
        onActivated: Qt.quit()
    }

    Shortcut {
        sequence: settings.newShortcut
        onActivated: doIfChangesDiscarded(function() { newProjectPopup.open() })
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.openShortcut
        onActivated: doIfChangesDiscarded(function() { openProjectDialog.open() }, true)
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.saveShortcut
        onActivated: projectManager.saveOrSaveAs()
        enabled: canvasHasActiveFocus && (project ? project.canSave : false)
    }

    Shortcut {
        sequence: settings.closeShortcut
        onActivated: doIfChangesDiscarded(function() { project.close() })
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.revertShortcut
        onActivated: project.revert()
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.undoShortcut
        onActivated: project.undoStack.undo()
        enabled: canvasHasActiveFocus && project && project.undoStack.canUndo
    }

    Shortcut {
        sequence: settings.redoShortcut
        onActivated: project.undoStack.redo()
        enabled: canvasHasActiveFocus && project && project.undoStack.canRedo
    }

    Shortcut {
        sequence: StandardKey.Copy
        onActivated: canvas.copySelection()
        enabled: projectType === Project.ImageType && canvasHasActiveFocus && canvas.hasSelection
    }

    Shortcut {
        sequence: StandardKey.Paste
        onActivated: canvas.paste()
        enabled: projectType === Project.ImageType && canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.gridVisibleShortcut
        onActivated: settings.gridVisible = !settings.gridVisible
    }

    Shortcut {
        sequence: settings.splitScreenShortcut
        onActivated: settings.splitScreen = !settings.splitScreen
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.centreShortcut
        onActivated: canvas.centreView()
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.zoomInShortcut
        onActivated: canvas.zoomIn()
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.zoomOutShortcut
        onActivated: canvas.zoomOut()
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.optionsShortcut
        onActivated: optionsDialog.open()
        enabled: canvasHasActiveFocus
    }

    Shortcut {
        sequence: settings.penToolShortcut
        onActivated: canvas.tool = ImageCanvas.PenTool
    }

    Shortcut {
        sequence: settings.eyeDropperToolShortcut
        onActivated: canvas.tool = ImageCanvas.EyeDropperTool
    }

    Shortcut {
        sequence: settings.eraserToolShortcut
        onActivated: canvas.tool = ImageCanvas.EraserTool
    }

    Shortcut {
        sequence: settings.selectionToolShortcut
        onActivated: canvas.tool = ImageCanvas.SelectionTool
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
