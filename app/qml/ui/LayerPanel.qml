import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.0

import App 1.0

import "." as Ui

Page {
    id: root
    implicitWidth: 200
    background: Rectangle {
        color: Ui.CanvasColours.panelColour
    }

    padding: 0

    property LayeredImageCanvas layeredImageCanvas
    property LayeredImageProject project

    ButtonGroup {
        buttons: listView.contentItem.children
    }

    ListView {
        id: listView
        objectName: "layerListView"
        anchors.fill: parent
        boundsBehavior: ListView.StopAtBounds
        visible: project && project.loaded
        clip: true

        ScrollBar.vertical: ScrollBar {
            id: verticalScrollBar
        }
        ScrollBar.horizontal: ScrollBar {
            id: horizontalScrollBar
        }

        model: LayerModel {
            layeredImageProject: project
        }

        delegate: ItemDelegate {
            objectName: model.layer.name
            checkable: true
            checked: project.currentLayerIndex === index
            width: listView.width
            leftPadding: visibilityCheckBox.width + 18
            focusPolicy: Qt.NoFocus

            onClicked: project.currentLayerIndex = index
            onDoubleClicked: layerNameTextField.forceActiveFocus()

            CheckBox {
                id: visibilityCheckBox
                objectName: "layerVisibilityCheckBox"
                x: 14
                text: model.layer.visible ? "\uf06e" : "\uf070"
                focusPolicy: Qt.NoFocus
                indicator: null
                anchors.verticalCenter: parent.verticalCenter

                onClicked: project.setLayerVisible(index, !model.layer.visible)
            }

            TextField {
                id: layerNameTextField
                objectName: "layerNameTextField"
                x: parent.leftPadding
                text: model.layer.name
                activeFocusOnPress: false
                anchors.verticalCenter: parent.verticalCenter
                anchors.verticalCenterOffset: 6
                background.visible: false
                font.pixelSize: 12
                visible: false

                Keys.onEscapePressed: {
                    focus = false;
                    text = model.layer.name;
                }
                onAccepted: {
                    focus = false;
                    project.setLayerName(index, text);
                }
            }

            // We don't want TextField's editable cursor to be visible,
            // so we set visible: false to disable the cursor, and instead
            // render it via this.
            ShaderEffectSource {
                sourceItem: layerNameTextField
                anchors.fill: layerNameTextField
            }

            // Apparently the one above only works for the top level control item,
            // so we also need one for the background.
            ShaderEffectSource {
                sourceItem: layerNameTextField.background
                x: layerNameTextField.x + layerNameTextField.background.x
                y: layerNameTextField.y + layerNameTextField.background.y
                width: layerNameTextField.background.width
                height: layerNameTextField.background.height
                visible: layerNameTextField.activeFocus
            }

            Rectangle {
                id: focusRect
                width: 2
                height: parent.height
                color: Ui.CanvasColours.focusColour
                visible: parent.checked
            }
        }
    }

    MenuSeparator {
        width: parent.width
        y: footer.y - height
        padding: 6
        topPadding: 0
        bottomPadding: 0
    }

    footer: RowLayout {
        Button {
            objectName: "newLayerButton"
            text: "+"
            flat: true
            focusPolicy: Qt.NoFocus
            hoverEnabled: true

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true
            Layout.leftMargin: 6

            onClicked: project.addNewLayer()
        }

        Button {
            objectName: "moveLayerDownButton"
            text: "\uf107"
            flat: true
            focusPolicy: Qt.NoFocus
            hoverEnabled: true
            enabled: project && project.currentLayerIndex < project.layerCount - 1

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: project.moveCurrentLayerDown()
        }

        Button {
            objectName: "moveLayerUpButton"
            text: "\uf106"
            flat: true
            focusPolicy: Qt.NoFocus
            hoverEnabled: true
            enabled: project && project.currentLayerIndex > 0

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true

            onClicked: project.moveCurrentLayerUp()
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            objectName: "deleteLayerButton"
            text: "\uf1f8"
            flat: true
            focusPolicy: Qt.NoFocus
            enabled: project && project.currentLayer && project.layerCount > 1
            hoverEnabled: true

            Layout.maximumWidth: implicitHeight
            Layout.fillWidth: true
            Layout.rightMargin: 6

            onClicked: project.deleteCurrentLayer()
        }
    }

//    Menu {
//        id: contextMenu
//        objectName: "layerPanelContextMenu"
//        x: rightClickedTile ? rightClickedTileScenePos.x : 0
//        y: rightClickedTile ? rightClickedTileScenePos.y : 0
//        parent: root
//        modal: true
//        dim: false

//        // TODO: add these back when they're re-added
////        allowHorizontalFlip: true
////        allowVerticalFlip: true

//        property string currentAction
//        property var rightClickedTile
//        property point rightClickedTileScenePos: rightClickedTile ? tilesetImage.mapToItem
//            (root, rightClickedTile.sourceRect.x, rightClickedTile.sourceRect.y + project.tileHeight) : Qt.point(0, 0)

//        // There isn't a current context action, so either the user cancelled
//        // whatever they were going to do by closing the menu, or the menu item
//        // has an immediate effect.
//        onClosed: if (!currentAction) rightClickedTile = null

//        function cancelCurrentAction() {
//            contextMenu.rightClickedTile = null;
//            contextMenu.currentAction = "";
//        }

//        MenuItem {
//            id: duplicateMenuItem
//            objectName: "duplicateTileMenuButton"
//            text: qsTr("Duplicate")
//            onClicked: {
//                contextMenu.currentAction = text;
//                cursorIcon.text = "\uf112";
//            }
//        }

//        MenuItem {
//            objectName: "rotateTileLeftMenuButton"
//            text: qsTr("Rotate 90° Left")
//            onClicked: {
//                project.rotateTileCounterClockwise(contextMenu.rightClickedTile);
//                contextMenu.rightClickedTile = null;
//            }
//        }

//        MenuItem {
//            objectName: "rotateTileRightMenuButton"
//            text: qsTr("Rotate 90° Right")
//            onClicked: {
//                project.rotateTileClockwise(contextMenu.rightClickedTile);
//                contextMenu.rightClickedTile = null;
//            }
//        }
//    }
}
