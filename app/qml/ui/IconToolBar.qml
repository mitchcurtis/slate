import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.1

import App 1.0

import "." as Ui

Item {
    id: root
    implicitWidth: toolbarRow.implicitWidth
    implicitHeight: toolbarRow.implicitHeight

    property Project project
    property TileCanvas canvas
    property FontMetrics fontMetrics

    property alias toolButtonGroup: toolButtonGroup
    property alias penToolButton: penToolButton
    property alias eyeDropperToolButton: eyeDropperToolButton
    property alias eraserToolButton: eraserToolButton

    TextMetrics {
        id: cursorMaxTextMetrics
        font.pixelSize: fontMetrics.font.pixelSize
        text: "999, 999"
    }

    function switchTool(tool) {
        canvas.ignoreToolChanges = true;
        canvas.tool = tool;
        canvas.ignoreToolChanges = false;
    }

    Row {
        id: toolbarRow
        anchors.fill: parent
        anchors.leftMargin: toolSeparator.implicitWidth / 2

        Ui.IconToolButton {
            id: canvasSizeButton
            objectName: "canvasSizeButton"
            enabled: project.loaded
            hoverEnabled: true

            ToolTip.text: qsTr("Change the size of the canvas")
            ToolTip.visible: hovered && !canvasSizePopup.visible

            onClicked: canvasSizePopup.visible = !canvasSizePopup.visible

            Ui.IconRectangle {
                anchors.centerIn: parent
                color: "transparent"
                border.width: 2
                border.color: canvasSizeButton.contentItem.color
                width: 12
                height: 12

                Ui.IconRectangle {
                    y: -2 - height
                    width: parent.width
                    height: 1
                    color: canvasSizeButton.contentItem.color
                }

                Ui.IconRectangle {
                    x: -2 - width
                    width: 1
                    height: parent.height
                    color: canvasSizeButton.contentItem.color
                }
            }

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
                icon: "\uf0e2"
                enabled: project.undoStack.canUndo
                hoverEnabled: true

                ToolTip.text: qsTr("Undo the last canvas operation")
                ToolTip.visible: hovered

                onClicked: project.undoStack.undo()
            }

            Ui.IconToolButton {
                objectName: "redoButton"
                icon: "\uf01e"
                enabled: project.undoStack.canRedo
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
            icon: "\uf044"
            checked: canvas.mode === TileCanvas.TileMode
            checkable: true
            hoverEnabled: true

            ToolTip.text: qsTr("Operate on either pixels or whole tiles")
            ToolTip.visible: hovered

            onClicked: {
                canvas.ignoreToolChanges = true;
                canvas.mode = checked ? TileCanvas.TileMode : TileCanvas.PixelMode;
                canvas.ignoreToolChanges = false;
            }
        }

        ToolSeparator {}

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
                icon: "\uf040"
                checked: true
                hoverEnabled: true

                ToolTip.text: qsTr("Draw pixels or tiles on the canvas")
                ToolTip.visible: hovered

                onClicked: switchTool(TileCanvas.PenTool)
            }

            Ui.IconToolButton {
                id: eyeDropperToolButton
                objectName: "eyeDropperToolButton"
                icon: "\uf1fb"
                checkable: true
                hoverEnabled: true

                ToolTip.text: qsTr("Select colours or tiles from the canvas")
                ToolTip.visible: hovered

                onClicked: switchTool(TileCanvas.EyeDropperTool)
            }

            Ui.IconToolButton {
                id: eraserToolButton
                objectName: "eraserToolButton"
                icon: "\uf12d"
                checkable: true
                hoverEnabled: true

                ToolTip.text: qsTr("Erase pixels or tiles from the canvas")
                ToolTip.visible: hovered

                onClicked: switchTool(TileCanvas.EraserTool)
            }

            Ui.IconToolButton {
                id: fillToolButton
                objectName: "fillToolButton"
                icon: "\uf0c3"
                checkable: true
                hoverEnabled: true

                ToolTip.text: qsTr("Fill a contiguous area with pixels or tiles")
                ToolTip.visible: hovered

                onClicked: switchTool(TileCanvas.FillTool)
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

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                id: pointerIconLabel
                text: "\uf245"
                font.family: "FontAwesome"
                font.pixelSize: fontMetrics.font.pixelSize * 1.2
                horizontalAlignment: Label.AlignHCenter
                anchors.verticalCenter: parent.verticalCenter

                Layout.preferredWidth: Math.max(26, implicitWidth)
            }

            Label {
                objectName: "cursorTilePixelPosLabel"
                text: canvas.cursorTilePixelX + ", " + canvas.cursorTilePixelY
                width: Math.max(cursorMaxTextMetrics.width, implicitWidth)
                anchors.verticalCenter: parent.verticalCenter
            }
        }
    }
}
