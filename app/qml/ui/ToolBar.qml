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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Templates 2.12 as T
import QtQuick.Window 2.12

import App 1.0

import "." as Ui

ToolBar {
    id: root
    objectName: "toolBar"

    property Project project
    property ImageCanvas canvas
    property T.Popup canvasSizePopup
    property T.Popup imageSizePopup

    property alias toolButtonGroup: toolButtonGroup

    readonly property int projectType: project ? project.type : 0
    readonly property bool isTilesetProject: projectType === Project.TilesetType
    readonly property bool isImageProject: projectType === Project.ImageType || projectType === Project.LayeredImageType
    readonly property bool projectLoaded: canvas && project && project.loaded

    Connections {
        target: canvas
        onToolChanged: {
            switch (canvas.tool) {
            case TileCanvas.PenTool:
                toolButtonGroup.checkedButton = penToolButton;
                break;
            case TileCanvas.EyeDropperTool:
                toolButtonGroup.checkedButton = eyeDropperToolButton;
                break;
            case TileCanvas.EraserTool:
                toolButtonGroup.checkedButton = eraserToolButton;
                break;
            case TileCanvas.FillTool:
            case TileCanvas.TexturedFillTool:
                toolButtonGroup.checkedButton = fillToolButton;
                break;
            case TileCanvas.SelectionTool:
                toolButtonGroup.checkedButton = selectionToolButton;
                break;
            }
        }
    }

    Row {
        id: toolbarRow
        anchors.fill: parent
        // Make sure that we don't end up on a sub-pixel position.
        anchors.leftMargin: Math.round(toolSeparator.implicitWidth / 2)

        Ui.ToolButton {
            id: canvasSizeToolButton
            objectName: "canvasSizeToolButton"
            enabled: projectLoaded

            icon.source: "qrc:/images/change-canvas-size.png"

            ToolTip.text: qsTr("Change the size of the canvas")
            ToolTip.visible: hovered && !canvasSizePopup.visible

            onClicked: canvasSizePopup.open()
        }

        Ui.ToolButton {
            id: imageSizeToolButton
            objectName: "imageSizeToolButton"
            enabled: projectLoaded && !isTilesetProject

            icon.source: "qrc:/images/change-image-size.png"

            ToolTip.text: qsTr("Change the size of the image")
            ToolTip.visible: hovered && !imageSizePopup.visible

            onClicked: imageSizePopup.open()
        }

        Ui.ToolButton {
            id: cropToSelectionToolButton
            objectName: "cropToSelectionToolButton"
            enabled: projectLoaded && !isTilesetProject && canvas.hasSelection

            icon.source: "qrc:/images/crop-to-selection.png"

            ToolTip.text: qsTr("Crop the image to the current selection")
            ToolTip.visible: hovered

            onClicked: project.crop(canvas.selectionArea)
        }

        ToolSeparator {}

        Row {
            height: parent.height
            spacing: 5

            Ui.IconToolButton {
                objectName: "undoToolButton"
                text: "\uf0e2"
                enabled: projectLoaded && (project.undoStack.canUndo || canvas.hasModifiedSelection)

                ToolTip.text: qsTr("Undo the last canvas operation")

                onClicked: canvas.undo()
            }

            Ui.IconToolButton {
                objectName: "redoToolButton"
                text: "\uf01e"
                enabled: projectLoaded && project.undoStack.canRedo

                ToolTip.text: qsTr("Redo the last undone canvas operation")

                onClicked: project.undoStack.redo()
            }

            ToolSeparator {}
        }

        Ui.IconToolButton {
            id: modeToolButton
            objectName: "modeToolButton"
            text: "\uf044"
            checked: projectLoaded && canvas.mode === TileCanvas.TileMode
            checkable: true
            enabled: projectLoaded && projectType === Project.TilesetType
            visible: enabled

            ToolTip.text: qsTr("Operate on either pixels or whole tiles")

            onClicked: canvas.mode = checked ? TileCanvas.TileMode : TileCanvas.PixelMode
        }

        ToolSeparator {
            visible: modeToolButton.visible
        }

        ButtonGroup {
            id: toolButtonGroup
            objectName: "toolBarButtonGroup"
            buttons: toolLayout.children
        }

        Row {
            id: toolLayout
            height: parent.height
            spacing: 5

            Ui.IconToolButton {
                id: penToolButton
                objectName: "penToolButton"
                text: "\uf040"
                checked: true
                enabled: projectLoaded

                ToolTip.text: qsTr("Draw pixels%1 on the canvas").arg(isTilesetProject ? qsTr(" or tiles") : "")

                onClicked: canvas.tool = ImageCanvas.PenTool
            }

            Ui.IconToolButton {
                id: eyeDropperToolButton
                objectName: "eyeDropperToolButton"
                text: "\uf1fb"
                checkable: true
                enabled: projectLoaded

                ToolTip.text: qsTr("Pick colours%1 from the canvas").arg(isTilesetProject ? qsTr(" or tiles") : "")

                onClicked: canvas.tool = ImageCanvas.EyeDropperTool
            }

            Ui.IconToolButton {
                id: eraserToolButton
                objectName: "eraserToolButton"
                text: "\uf12d"
                checkable: true
                enabled: projectLoaded

                ToolTip.text: qsTr("Erase pixels%1 from the canvas").arg(isTilesetProject ? qsTr(" or tiles") : "")

                onClicked: canvas.tool = ImageCanvas.EraserTool
            }

            Ui.ToolButton {
                id: fillToolButton
                objectName: "fillToolButton"
                checkable: true
                enabled: projectLoaded

                readonly property bool regularFill: canvas && canvas.lastFillToolUsed === ImageCanvas.FillTool
                readonly property string imageProjectToolTipText:
                    qsTr("Fill a contiguous area with %1pixels.\nHold Shift to fill all pixels matching the target colour.")
                        .arg(!regularFill ? "semi-randomised " : "")

                icon.source: regularFill ? "qrc:/images/fill.png" : "qrc:/images/textured-fill.png"

                ToolTip.text: isTilesetProject ? qsTr("Fill a contiguous area with pixels or tiles") : imageProjectToolTipText

                onClicked: canvas.tool = canvas.lastFillToolUsed
                onPressAndHold: if (!isTilesetProject) fillMenu.open()
                // TODO: respond to right clicks when https://bugreports.qt.io/browse/QTBUG-67331 is implemented

                ToolButtonMenuIndicator {
                    color: fillToolButton.icon.color
                    anchors.right: parent.contentItem.right
                    anchors.bottom: parent.contentItem.bottom
                    anchors.margins: 6
                    visible: !isTilesetProject
                }

                Menu {
                    id: fillMenu
                    y: fillToolButton.height
                    width: 260

                    MenuItem {
                        text: qsTr("Fill Tool")
                        icon.source: "qrc:/images/fill.png"
                        autoExclusive: true
                        checkable: true
                        checked: canvas && canvas.lastFillToolUsed === ImageCanvas.FillTool
                        onTriggered: canvas.tool = ImageCanvas.FillTool
                    }
                    MenuItem {
                        text: qsTr("Textured Fill Tool")
                        icon.source: "qrc:/images/textured-fill.png"
                        autoExclusive: true
                        checkable: true
                        checked: canvas && canvas.lastFillToolUsed === ImageCanvas.TexturedFillTool
                        onTriggered: canvas.tool = ImageCanvas.TexturedFillTool
                    }
                }
            }

            Ui.ToolButton {
                id: selectionToolButton
                objectName: "selectionToolButton"
                checkable: true
                visible: projectType === Project.ImageType || projectType === Project.LayeredImageType
                enabled: projectLoaded
                icon.source: "qrc:/images/selection.png"

                ToolTip.text: qsTr("Select pixels within an area and move them")

                onClicked: canvas.tool = ImageCanvas.SelectionTool
            }

            ToolSeparator {}
        }

        Ui.ToolButton {
            id: toolSizeButton
            objectName: "toolSizeButton"
            enabled: projectLoaded
            icon.source: "qrc:/images/change-tool-size.png"

            ToolTip.text: qsTr("Change the size of drawing tools")
            ToolTip.visible: hovered && !toolSizeSliderPopup.visible

            onClicked: toolSizeSliderPopup.visible = !toolSizeSliderPopup.visible

            ToolSizePopup {
                id: toolSizeSliderPopup
                x: parent.width / 2 - width / 2
                y: parent.height
                canvas: root.canvas
            }
        }

        Ui.ToolButton {
            id: toolShapeButton
            objectName: "toolShapeButton"
            enabled: projectLoaded

            readonly property bool squareShape: canvas && canvas.toolShape === ImageCanvas.SquareToolShape
            icon.source: squareShape ? "qrc:/images/square-tool-shape.png" : "qrc:/images/circle-tool-shape.png"

            ToolTip.text: qsTr("Choose brush shape")

            onClicked: toolShapeMenu.visible = !toolShapeMenu.visible

            ToolButtonMenuIndicator {
                color: toolShapeButton.icon.color
                anchors.right: parent.contentItem.right
                anchors.bottom: parent.contentItem.bottom
                anchors.margins: 6
            }

            Menu {
                id: toolShapeMenu
                objectName: "toolShapeMenu"
                y: toolShapeButton.height
                width: 260

                MenuItem {
                    objectName: "squareToolShapeMenuItem"
                    text: qsTr("Square")
                    icon.source: "qrc:/images/square-tool-shape.png"
                    autoExclusive: true
                    checkable: true
                    checked: canvas && canvas.toolShape === ImageCanvas.SquareToolShape
                    onTriggered: canvas.toolShape = ImageCanvas.SquareToolShape
                }
                MenuItem {
                    objectName: "circleToolShapeMenuItem"
                    text: qsTr("Circle")
                    icon.source: "qrc:/images/circle-tool-shape.png"
                    autoExclusive: true
                    checkable: true
                    checked: canvas && canvas.toolShape === ImageCanvas.CircleToolShape
                    onTriggered: canvas.toolShape = ImageCanvas.CircleToolShape
                }
            }
        }

        ToolSeparator {
            id: toolSeparator
            height: parent.height
        }

        Row {
            id: transformLayout
            height: parent.height
            spacing: 5
            visible: projectType === Project.ImageType || projectType === Project.LayeredImageType
            enabled: projectLoaded

            Ui.ToolButton {
                id: rotate90CcwToolButton
                objectName: "rotate90CcwToolButton"
                enabled: isImageProject && canvas && canvas.hasSelection
                icon.source: "qrc:/images/rotate-90-ccw.png"

                ToolTip.text: qsTr("Rotate the selection by 90 degrees counter-clockwise")

                onClicked: canvas.rotateSelection(-90)
            }

            Ui.ToolButton {
                id: rotate90CwToolButton
                objectName: "rotate90CwToolButton"
                enabled: isImageProject && canvas && canvas.hasSelection
                icon.source: "qrc:/images/rotate-90-cw.png"

                ToolTip.text: qsTr("Rotate the selection by 90 degrees clockwise")

                onClicked: canvas.rotateSelection(90)
            }

            Ui.ToolButton {
                id: flipHorizontallyToolButton
                objectName: "flipHorizontallyToolButton"
                enabled: isImageProject && canvas && canvas.hasSelection
                icon.source: "qrc:/images/flip-horizontally.png"

                ToolTip.text: qsTr("Flip the selection horizontally")

                onClicked: canvas.flipSelection(Qt.Horizontal)
            }

            Ui.ToolButton {
                id: flipVerticallyToolButton
                objectName: "flipVerticallyToolButton"
                enabled: isImageProject && canvas && canvas.hasSelection
                icon.source: "qrc:/images/flip-vertically.png"

                ToolTip.text: qsTr("Flip the selection vertically")

                onClicked: canvas.flipSelection(Qt.Vertical)
            }

            ToolSeparator {
                height: parent.height
            }
        }

        Row {
            id: viewLayout
            height: parent.height
            enabled: projectLoaded
            spacing: 5

            Ui.ToolButton {
                objectName: "showRulersToolButton"
                checkable: true
                checked: settings.rulersVisible
                icon.source: "qrc:/images/show-rulers.png"

                ToolTip.text: qsTr("Show rulers")

                onClicked: settings.rulersVisible = checked
            }

            Ui.ToolButton {
                objectName: "showGuidesToolButton"
                checkable: true
                checked: settings.guidesVisible
                icon.source: "qrc:/images/show-guides.png"

                ToolTip.text: qsTr("Show guides")

                onClicked: settings.guidesVisible = checked
            }

            Ui.ToolButton {
                objectName: "lockGuidesToolButton"
                checkable: true
                checked: settings.guidesLocked
                icon.source: "qrc:/images/lock-guides.png"

                ToolTip.text: qsTr("Lock guides")

                onClicked: settings.guidesLocked = checked
            }
        }

        ToolSeparator {
            height: parent.height
        }

        Row {
            id: viewSplitscreenLayout
            height: parent.height
            enabled: projectLoaded
            spacing: 5

            Ui.ToolButton {
                objectName: "splitScreenToolButton"
                checkable: true
                checked: canvas && canvas.splitScreen
                icon.source: "qrc:/images/splitscreen.png"

                ToolTip.text: qsTr("Split Screen")

                onClicked: canvas.splitScreen = checked
            }

            Ui.ToolButton {
                objectName: "lockSplitterToolButton"
                checkable: true
                checked: canvas && !canvas.splitter.enabled
                enabled: canvas && canvas.splitScreen
                icon.source: "qrc:/images/lock-splitter.png"

                ToolTip.text: qsTr("Lock Splitter")

                onClicked: canvas.splitter.enabled = !checked
            }
        }

        ToolSeparator {
            height: parent.height
        }

        Ui.IconToolButton {
            id: fullScreenToolButton
            objectName: "fullScreenToolButton"
            text: "\uF108"
            checkable: true
            checked: window.visibility === Window.FullScreen

            ToolTip.text: qsTr("Toggle fullscreen window")

            onClicked: toggleFullScreen()
        }
    }
}
