import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtQuick.Window 2.0

import App 1.0

import "." as Ui

Pane {
    id: root
    implicitWidth: tilesetImage.implicitWidth
    background: Rectangle {
        color: Ui.CanvasColours.panelColour
    }

    padding: 0

    property TileCanvas tileCanvas
    property Project project
    property alias tilesetImage: tilesetImage

    readonly property int penTileId: tileCanvas && tileCanvas.penTile ? tileCanvas.penTile.id : -1

    Keys.onEscapePressed: contextMenu.cancelCurrentAction()

    Flickable {
        id: tilesetFlickable
        objectName: "tilesetSwatchFlickable"
        anchors.fill: parent
        contentWidth: tilesetImage.implicitWidth
        contentHeight: tilesetImage.implicitHeight
        boundsBehavior: Flickable.StopAtBounds
        visible: project && project.loaded
        clip: true

        ScrollBar.vertical: ScrollBar {
            id: verticalScrollBar
            Binding {
                target: verticalScrollBar
                property: "active"
                value: verticalScrollBar.hovered
            }
        }
        ScrollBar.horizontal: ScrollBar {
            id: horizontalScrollBar
            Binding {
                target: horizontalScrollBar
                property: "active"
                value: horizontalScrollBar.hovered
            }
        }

        Connections {
            target: tileCanvas
            onPenTileChanged: {
                // Don't try to change the flickable's content position if it doesn't have a size yet,
                // as this will cause tests to fail as the wrong tile swatch is selected.
                if (!tileCanvas.penTile || tilesetFlickable.width === 0 || tilesetFlickable.height === 0)
                    return;

                var tileRect = tileCanvas.penTile.sourceRect;
                // get the position of the tile relative to the top left of the view.
                var mappedPos = tilesetImage.mapToItem(tilesetFlickable, tileRect.x, tileRect.y);
                if (mappedPos.x < 0) {
                    // The selected tile is to the left of the view; move it back in.
                    tilesetFlickable.contentX -= Math.abs(mappedPos.x);
                } else if (mappedPos.x + tileRect.width >= tilesetFlickable.width) {
                    // The selected tile is to the right of the view; move it back in.
                    tilesetFlickable.contentX -= tilesetFlickable.width - (mappedPos.x + tileRect.width);
                }

                if (mappedPos.y < 0) {
                    // The selected tile is above the view; move it back in.
                    tilesetFlickable.contentY -= Math.abs(mappedPos.y);
                } else if (mappedPos.y + tileRect.height >= tilesetFlickable.height) {
                    // The selected tile is below the view; move it back in.
                    tilesetFlickable.contentY -= tilesetFlickable.height - (mappedPos.y + tileRect.height);
                }
            }
        }

        // The Image type only knows about URLs,
        // so we can't update it when the tileset's image changes.
        TilesetSwatchImage {
            id: tilesetImage
            tileset: project ? project.tileset : null

            TileGrid {
                highlightedIndex: penTileId - 1
                tileWidth: project ? project.tileWidth : 1
                tileHeight: project ? project.tileHeight : 1
                tilesWide: project && project.tileset ? project.tileset.tilesWide : 1
                tilesHigh: project && project.tileset ? project.tileset.tilesHigh : 1
                colour: tileCanvas ? tileCanvas.gridColour : "black"
                highlightColour: Ui.CanvasColours.focusColour
                anchors.fill: parent
            }
        }

        // http://lists.qt-project.org/pipermail/interest/2016-October/024763.html
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: contextMenu.currentAction
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            cursorShape: {
                if (!contextMenu.currentAction)
                    return Qt.ArrowCursor;

                if (!hoveredTile)
                    return Qt.ForbiddenCursor;

                return Qt.ArrowCursor;
            }

            property point cursorPosInImage: {
                // Force hovered state to update when scrolling the flickable without moving the mouse.
                tilesetFlickable.contentY;
                return mapToItem(tilesetImage, mouseX, mouseY);
            }
            property var hoveredTile: mouseArea.containsMouse ? project.tilesetTileAt(cursorPosInImage.x, cursorPosInImage.y) : null
            // We use hoveredTile.sourceRect instead of just using cursorPosInImage because we want grid-aligned positions.
            property point hoveredTilePos: hoveredTile ? mapFromItem(tilesetImage, hoveredTile.sourceRect.x, hoveredTile.sourceRect.y) : Qt.point(0, 0)

            onReleased: {
                if (mouse.button === Qt.RightButton) {
                    if (!contextMenu.rightClickedTile) {
                        contextMenu.rightClickedTile = project.tilesetTileAt(cursorPosInImage.x, cursorPosInImage.y);
                        contextMenu.open();
                    }
                } else {
                    if (contextMenu.rightClickedTile && mouseArea.hoveredTile) {
                        // The left button was released while duplicating; duplicate the tile here.
                        project.duplicateTile(contextMenu.rightClickedTile,
                            mouseArea.hoveredTile.sourceRect.x, mouseArea.hoveredTile.sourceRect.y);
                        // Now that it's been duplicated, clear the current action.
                        contextMenu.cancelCurrentAction();
                    } else {
                        // The left button was clicked without any current context menu action; select this tile.
                        tileCanvas.penTile = project.tilesetTileAt(cursorPosInImage.x, cursorPosInImage.y);
                    }
                }
            }
        }
    }

    TilesetSwatchImage {
        id: duplicatePreviewImage
        x: mouseArea.hoveredTile ? mouseArea.hoveredTilePos.x : 0
        y: mouseArea.hoveredTile ? mouseArea.hoveredTilePos.y : 0
        width: project ? project.tileWidth : 1
        height: project ? project.tileHeight : 1
        tileset: project ? project.tileset : null
        sourceRect: mouseArea.hoveredTile
            ? Qt.rect(mouseArea.hoveredTile.sourceRect.x, mouseArea.hoveredTile.sourceRect.y, width, height)
            : Qt.rect(0, 0, 0, 0)
        visible: contextMenu.currentAction === duplicateMenuItem.text && mouseArea.hoveredTile

        Text {
            id: cursorIcon
            x: -width
            y: parent.height / 2 - height / 2
            font.family: "FontAwesome"
            font.pixelSize: Qt.application.font.pixelSize * 1.5
            rotation: 180
        }
    }

    Menu {
        id: contextMenu
        objectName: "tilesetContextMenu"
        x: rightClickedTile ? rightClickedTileScenePos.x : 0
        y: rightClickedTile ? rightClickedTileScenePos.y : 0
        parent: root
        modal: true
        dim: false

        // TODO: add these back when they're re-added
//        allowHorizontalFlip: true
//        allowVerticalFlip: true

        property string currentAction
        property var rightClickedTile
        property point rightClickedTileScenePos: rightClickedTile ? tilesetImage.mapToItem
            (root, rightClickedTile.sourceRect.x, rightClickedTile.sourceRect.y + project.tileHeight) : Qt.point(0, 0)

        // There isn't a current context action, so either the user cancelled
        // whatever they were going to do by closing the menu, or the menu item
        // has an immediate effect.
        onClosed: if (!currentAction) rightClickedTile = null

        function cancelCurrentAction() {
            contextMenu.rightClickedTile = null;
            contextMenu.currentAction = "";
        }

        MenuItem {
            id: duplicateMenuItem
            objectName: "duplicateTileMenuButton"
            text: qsTr("Duplicate")
            onClicked: {
                contextMenu.currentAction = text;
                cursorIcon.text = "\uf112";
            }
        }

        MenuItem {
            objectName: "rotateTileLeftMenuButton"
            text: qsTr("Rotate 90° Left")
            onClicked: {
                project.rotateTileCounterClockwise(contextMenu.rightClickedTile);
                contextMenu.rightClickedTile = null;
            }
        }

        MenuItem {
            objectName: "rotateTileRightMenuButton"
            text: qsTr("Rotate 90° Right")
            onClicked: {
                project.rotateTileClockwise(contextMenu.rightClickedTile);
                contextMenu.rightClickedTile = null;
            }
        }
    }
}
