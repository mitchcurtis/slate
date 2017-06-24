import QtQuick 2.6
import QtQuick.Controls 2.1

import App 1.0

import "." as Ui

Item {
    id: root
    objectName: "iconToolBar"
    implicitWidth: toolbarRow.implicitWidth
    implicitHeight: toolbarRow.implicitHeight

    property Project project
    property int projectType: project ? project.type : 0
    property ImageCanvas canvas
    property FontMetrics fontMetrics

    property alias toolButtonGroup: toolButtonGroup

    function switchTool(tool) {
        root.ignoreToolChanges = true;
        canvas.tool = tool;
        root.ignoreToolChanges = false;
    }

    // TODO: figure out a nicer solution than this.
    property bool ignoreToolChanges: false

    Connections {
        target: canvas
        onToolChanged: {
            if (root.ignoreToolChanges)
                return;

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
        enabled: canvas
        anchors.fill: parent
        // Make sure that we don't end up on a sub-pixel position.
        anchors.leftMargin: Math.round(toolSeparator.implicitWidth / 2)

        ToolButton {
            id: canvasSizeButton
            objectName: "canvasSizeButton"
            enabled: project ? project.loaded : false
            hoverEnabled: true
            focusPolicy: Qt.TabFocus

            icon.source: "qrc:/images/change-size.png"

            ToolTip.text: qsTr("Change the size of the canvas")
            ToolTip.visible: hovered && !canvasSizePopup.visible

            onClicked: canvasSizePopup.visible = !canvasSizePopup.visible

            CanvasSizePopup {
                id: canvasSizePopup
                x: parent.x
                y: parent.height + 10
                project: root.project
            }
        }

        ToolSeparator {}

        Row {
            spacing: 5

            Ui.IconToolButton {
                objectName: "undoButton"
                iconText: "\uf0e2"
                enabled: project ? project.undoStack.canUndo : false
                hoverEnabled: true

                ToolTip.text: qsTr("Undo the last canvas operation")
                ToolTip.visible: hovered

                onClicked: project.undoStack.undo()
            }

            Ui.IconToolButton {
                objectName: "redoButton"
                iconText: "\uf01e"
                enabled: project ? project.undoStack.canRedo : false
                hoverEnabled: true

                ToolTip.text: qsTr("Redo the last undone canvas operation")
                ToolTip.visible: hovered

                onClicked: project.undoStack.redo()
            }

            ToolSeparator {}
        }

        Ui.IconToolButton {
            id: modeToolButton
            objectName: "modeToolButton"
            iconText: "\uf044"
            checked: canvas && canvas.mode === TileCanvas.TileMode
            checkable: true
            hoverEnabled: true
            enabled: canvas && projectType === Project.TilesetType
            visible: enabled

            ToolTip.text: qsTr("Operate on either pixels or whole tiles")
            ToolTip.visible: hovered

            onClicked: {
                root.ignoreToolChanges = true;
                canvas.mode = checked ? TileCanvas.TileMode : TileCanvas.PixelMode;
                root.ignoreToolChanges = false;
            }
        }

        ToolSeparator {
            visible: modeToolButton.visible
        }

        ButtonGroup {
            id: toolButtonGroup
            buttons: toolLayout.children
        }

        Row {
            id: toolLayout
            spacing: 5

            Ui.IconToolButton {
                id: penToolButton
                objectName: "penToolButton"
                iconText: "\uf040"
                checked: true
                hoverEnabled: true

                ToolTip.text: qsTr("Draw pixels or tiles on the canvas")
                ToolTip.visible: hovered

                onClicked: switchTool(ImageCanvas.PenTool)
            }

            Ui.IconToolButton {
                id: eyeDropperToolButton
                objectName: "eyeDropperToolButton"
                iconText: "\uf1fb"
                checkable: true
                hoverEnabled: true

                ToolTip.text: qsTr("Select colours or tiles from the canvas")
                ToolTip.visible: hovered

                onClicked: switchTool(ImageCanvas.EyeDropperTool)
            }

            Ui.IconToolButton {
                id: eraserToolButton
                objectName: "eraserToolButton"
                iconText: "\uf12d"
                checkable: true
                hoverEnabled: true

                ToolTip.text: qsTr("Erase pixels or tiles from the canvas")
                ToolTip.visible: hovered

                onClicked: switchTool(ImageCanvas.EraserTool)
            }

            Ui.IconToolButton {
                id: fillToolButton
                objectName: "fillToolButton"
                iconText: "\uf0c3"
                checkable: true
                hoverEnabled: true

                ToolTip.text: qsTr("Fill a contiguous area with pixels or tiles")
                ToolTip.visible: hovered

                onClicked: switchTool(ImageCanvas.FillTool)
            }

            // TODO: https://bugreports.qt.io/browse/QTBUG-60807 affects icon colour
            ToolButton {
                id: selectionToolButton
                objectName: "selectionToolButton"
                checkable: true
                hoverEnabled: true
                focusPolicy: Qt.TabFocus
                enabled: projectType === Project.ImageType

                icon.source: "qrc:/images/selection.png"

                ToolTip.text: qsTr("Select pixels within an area and move them")
                ToolTip.visible: hovered

                onClicked: switchTool(ImageCanvas.SelectionTool)
            }

            ToolSeparator {}
        }

        Ui.IconToolButton {
            id: toolSizeButton
            objectName: "toolSizeButton"
            hoverEnabled: true

            ToolTip.text: qsTr("Change the size of drawing tools")
            ToolTip.visible: hovered && !toolSizeSliderPopup.visible

            onClicked: toolSizeSliderPopup.visible = !toolSizeSliderPopup.visible

            Item {
                objectName: "toolSizeButtonIcon"
                width: 12
                height: 12
                anchors.centerIn: parent

                Ui.IconRectangle {
                    width: parent.width
                    height: 1
                }

                Ui.IconRectangle {
                    y: 4
                    width: parent.width
                    height: 2
                }

                Ui.IconRectangle {
                    y: 9
                    width: parent.width
                    height: 3
                }
            }

            ToolSizePopup {
                id: toolSizeSliderPopup
                x: parent.width / 2 - width / 2
                y: parent.height
                canvas: root.canvas
            }
        }

        ToolSeparator {
            id: toolSeparator
        }
    }
}
