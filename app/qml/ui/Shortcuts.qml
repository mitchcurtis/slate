import QtQuick 2.6

import App 1.0

Item {
    objectName: "shortcuts"

    property var window
    property Project project: projectManager.project
    property int projectType: project ? project.type : 0
    readonly property bool isImageProjectType: projectType === Project.ImageType || projectType === Project.LayeredImageType
    property Item canvasContainer
    property ImageCanvas canvas
    readonly property bool canvasHasActiveFocus: canvas ? canvas.activeFocus : false

    Shortcut {
        sequence: settings.quitShortcut
        onActivated: Qt.quit()
    }

    Shortcut {
        objectName: "newShortcut"
        sequence: settings.newShortcut
        enabled: canvasHasActiveFocus
        onActivated: doIfChangesDiscarded(function() { newProjectPopup.open() }, true)
    }

    Shortcut {
        objectName: "openShortcut"
        sequence: settings.openShortcut
        // There could be no project open, so the canvas won't exist and hence its container will have focus.
        enabled: canvasHasActiveFocus || canvasContainer.activeFocus
        onActivated: doIfChangesDiscarded(function() { openProjectDialog.open() }, true)
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
        onActivated: doIfChangesDiscarded(function() { project.close() })
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
        enabled: canvasHasActiveFocus && project && project.undoStack.canUndo
        onActivated: {
            // A selection should be cleared when Ctrl + Z is pressed, as this is
            // what mspaint does. However, it doesn't make sense for a selection
            // to have its own undo command (as it's cleared after the first undo
            // on selection moves).
            // It's also not even possible to use the undo framework to implement
            // support for mspaint's undo behaviour, because in order for the commands
            // to be mergeable, a macro needs to be used, and when a macro is being composed,
            // it's not even *possible* to undo/redo.
            // So, we let ImageCanvas intercept the undo shortcut
            // to handle this special case ourselves. This has the advantage of
            // being faster by not using an event filter..
            // .. and I couldn't manage to override shortcuts using an event filter.
            if (!canvas.overrideShortcut(sequence))
                project.undoStack.undo()
        }
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
        onActivated: settings.gridVisible = !settings.gridVisible
    }

    Shortcut {
        objectName: "rulersVisibleShortcut"
        sequence: settings.rulersVisibleShortcut
        onActivated: settings.rulersVisible = !settings.rulersVisible
    }

    Shortcut {
        objectName: "guidesVisibleShortcut"
        sequence: settings.guidesVisibleShortcut
        onActivated: settings.guidesVisible = !settings.guidesVisible
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
}
