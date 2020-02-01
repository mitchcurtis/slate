import QtQuick 2.12

import App 1.0

Item {
    objectName: "shortcuts"

    property var window
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType
    property Item canvasContainer
    property ImageCanvas canvas
    readonly property bool canvasHasActiveFocus: canvas && canvas.activeFocus
    property SaveChangesDialog saveChangesDialog

    Shortcut {
        sequence: settings.quitShortcut
        onActivated: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { Qt.quit() })
    }

    Shortcut {
        objectName: "newShortcut"
        sequence: settings.newShortcut
        // There could be no project open, so the canvas won't exist and hence its container will have focus.
        enabled: canvasHasActiveFocus || canvasContainer.activeFocus
        onActivated: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { newProjectPopup.open() }, true)
    }

    Shortcut {
        objectName: "openShortcut"
        sequence: settings.openShortcut
        enabled: canvasHasActiveFocus || canvasContainer.activeFocus
        onActivated: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { openProjectDialog.open() }, true)
    }

    Shortcut {
        objectName: "saveShortcut"
        sequence: settings.saveShortcut
        enabled: canvasHasActiveFocus && project && project.canSave
        onActivated: saveOrSaveAs()
    }

    Shortcut {
        objectName: "saveAsShortcut"
        sequence: settings.saveAsShortcut
        enabled: canvasHasActiveFocus && project
        onActivated: saveAsDialog.open()
    }

    Shortcut {
        objectName: "exportShortcut"
        sequence: settings.exportShortcut
        enabled: project && project.loaded && projectType === Project.LayeredImageType
        onActivated: exportDialog.open()
    }

    Shortcut {
        objectName: "closeShortcut"
        sequence: settings.closeShortcut
        enabled: canvasHasActiveFocus
        onActivated: saveChangesDialog.doIfChangesSavedOrDiscarded(function() { project.close() })
    }

    Shortcut {
        objectName: "revertShortcut"
        sequence: settings.revertShortcut
        enabled: canvasHasActiveFocus
        onActivated: project.revert()
    }

    Shortcut {
        objectName: "undoShortcut"
        sequence: settings.undoShortcut
        // This is a bit hacky, but we want two things:
        // #1: the user to be able to undo selection modifications before confirming them, as mspaint does
        // #2: an undo command for selection modifications should only be pushed onto the stack when the
        //     selection is confirmed (or "undone"; see ImageCanvas::interceptUndo()).
        // Since these conflict with each other, we cheat a little bit and allow
        // undos as long as the selection contents have been modified have to add the || canvas.hasModifiedSelection.
        enabled: canvasHasActiveFocus && project && (project.undoStack.canUndo || canvas.hasModifiedSelection)
        onActivated: canvas.undo()
    }

    Shortcut {
        objectName: "redoShortcut"
        sequence: settings.redoShortcut
        enabled: canvasHasActiveFocus && project && project.undoStack.canRedo
        onActivated: project.undoStack.redo()
    }

    Shortcut {
        objectName: "copyShortcut"
        sequence: StandardKey.Copy
        enabled: isImageProjectType && canvasHasActiveFocus && canvas.hasSelection
        onActivated: canvas.copySelection()
    }

    Shortcut {
        objectName: "pasteShortcut"
        sequence: StandardKey.Paste
        enabled: isImageProjectType && canvasHasActiveFocus
        onActivated: canvas.paste()
    }

    Shortcut {
        id: primaryDeleteShortcut
        objectName: "deleteShortcut"
        sequence: StandardKey.Delete
        enabled: isImageProjectType && canvasHasActiveFocus
        onActivated: canvas.deleteSelectionOrContents()
    }

    // StandardKey.Delete doesn't work on a MacBook Pro keyboard;
    // Qt uses Fn+Delete instead, and "Backspace" is the only thing that seems to work,
    // so we need two shortcuts. See: https://bugreports.qt.io/browse/QTBUG-67430
    Shortcut {
        objectName: "deleteViaBackspaceShortcut"
        sequence: "Backspace"
        enabled: primaryDeleteShortcut.enabled
        onActivated: canvas.deleteSelectionOrContents()
    }

    Shortcut {
        objectName: "selectAllShortcut"
        sequence: StandardKey.SelectAll
        enabled: isImageProjectType && canvasHasActiveFocus
        onActivated: canvas.selectAll()
    }

    Shortcut {
        objectName: "flipHorizontallyShortcut"
        sequence: settings.flipHorizontallyShortcut
        enabled: isImageProjectType && canvasHasActiveFocus && canvas.hasSelection
        onActivated: canvas.flipSelection(Qt.Horizontal)
    }

    Shortcut {
        objectName: "flipVerticallyShortcut"
        sequence: settings.flipVerticallyShortcut
        enabled: isImageProjectType && canvasHasActiveFocus && canvas.hasSelection
        onActivated: canvas.flipSelection(Qt.Vertical)
    }

    Shortcut {
        objectName: "resizeCanvasShortcut"
        sequence: settings.resizeCanvasShortcut
        enabled: canvasHasActiveFocus && canvas.hasSelection
        onActivated: canvasSizePopup.open()
    }

    Shortcut {
        objectName: "resizeImageShortcut"
        sequence: settings.resizeImageShortcut
        enabled: isImageProjectType && canvasHasActiveFocus && canvas.hasSelection
        onActivated: imageSizePopup.open()
    }

    Shortcut {
        objectName: "moveContentsShortcut"
        sequence: settings.moveContentsShortcut
        enabled: projectType === Project.LayeredImageType && canvasHasActiveFocus
        onActivated: moveContentsDialog.open()
    }

    Shortcut {
        objectName: "gridVisibleShortcut"
        sequence: settings.gridVisibleShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.gridVisible = !canvas.gridVisible
    }

    Shortcut {
        objectName: "rulersVisibleShortcut"
        sequence: settings.rulersVisibleShortcut
        onActivated: canvas.rulersVisible = !canvas.rulersVisible
    }

    Shortcut {
        objectName: "guidesVisibleShortcut"
        sequence: settings.guidesVisibleShortcut
        onActivated: canvas.guidesVisible = !canvas.guidesVisible
    }

    Shortcut {
        objectName: "splitScreenShortcut"
        sequence: settings.splitScreenShortcut
        enabled: canvasHasActiveFocus
        onActivated: canvas.splitScreen = !canvas.splitScreen
    }

    Shortcut {
        objectName: "splitterLockedShortcut"
        sequence: settings.splitterLockedShortcut
        enabled: canvasHasActiveFocus
        onActivated: canvas.splitter.enabled = !canvas.splitter.enabled
    }

    Shortcut {
        objectName: "centreShortcut"
        sequence: settings.centreShortcut
        enabled: canvasHasActiveFocus
        onActivated: canvas.centreView()
    }

    Shortcut {
        objectName: "zoomInShortcut"
        sequence: settings.zoomInShortcut
        enabled: canvasHasActiveFocus
        onActivated: canvas.zoomIn()
    }

    Shortcut {
        objectName: "zoomOutShortcut"
        sequence: settings.zoomOutShortcut
        enabled: canvasHasActiveFocus
        onActivated: canvas.zoomOut()
    }

    Shortcut {
        objectName: "animationPlaybackShortcut"
        sequence: settings.animationPlaybackShortcut
        enabled:  isImageProjectType && canvasHasActiveFocus
        onActivated: project.usingAnimation = !project.usingAnimation
    }

    Shortcut {
        objectName: "optionsShortcut"
        sequence: settings.optionsShortcut
        enabled: canvasHasActiveFocus
        onActivated: optionsDialog.open()
    }

    Shortcut {
        objectName: "penToolShortcut"
        sequence: settings.penToolShortcut
        onActivated: canvas.tool = ImageCanvas.PenTool
    }

    Shortcut {
        objectName: "eyeDropperToolShortcut"
        sequence: settings.eyeDropperToolShortcut
        onActivated: canvas.tool = ImageCanvas.EyeDropperTool
    }

    Shortcut {
        objectName: "fillToolShortcut"
        sequence: settings.fillToolShortcut
        onActivated: canvas.tool = canvas.lastFillToolUsed
    }

    Shortcut {
        objectName: "cycleFillToolsShortcut"
        sequence: "Shift+" + settings.fillToolShortcut
        onActivated: canvas.cycleFillTools()
    }

    Shortcut {
        objectName: "eraserToolShortcut"
        sequence: settings.eraserToolShortcut
        onActivated: canvas.tool = ImageCanvas.EraserTool
    }

    Shortcut {
        objectName: "selectionToolShortcut"
        sequence: settings.selectionToolShortcut
        enabled: isImageProjectType
        onActivated: canvas.tool = ImageCanvas.SelectionTool
    }

    Shortcut {
        objectName: "noteToolShortcut"
        sequence: settings.noteToolShortcut
        enabled: isImageProjectType
        onActivated: canvas.tool = ImageCanvas.NoteTool
    }

    Shortcut {
        objectName: "toolModeShortcut"
        sequence: settings.toolModeShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.mode = (canvas.mode === TileCanvas.TileMode ? TileCanvas.PixelMode : TileCanvas.TileMode)
    }

    Shortcut {
        objectName: "decreaseToolSizeShortcut"
        sequence: settings.decreaseToolSizeShortcut
        onActivated: --canvas.toolSize
    }

    Shortcut {
        objectName: "increaseToolSizeShortcut"
        sequence: settings.increaseToolSizeShortcut
        onActivated: ++canvas.toolSize
    }

    Shortcut {
        objectName: "swatchLeftShortcut"
        sequence: settings.swatchLeftShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchLeft()
    }

    Shortcut {
        objectName: "swatchRightShortcut"
        sequence: settings.swatchRightShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchRight()
    }

    Shortcut {
        objectName: "swatchUpShortcut"
        sequence: settings.swatchUpShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchUp()
    }

    Shortcut {
        objectName: "swatchDownShortcut"
        sequence: settings.swatchDownShortcut
        enabled: projectType === Project.TilesetType
        onActivated: canvas.swatchDown()
    }

    Shortcut {
        objectName: "fullScreenToggleShortcut"
        sequence: settings.fullScreenToggleShortcut
        onActivated: toggleFullScreen()
    }
}
