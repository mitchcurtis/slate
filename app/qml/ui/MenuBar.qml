import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.1

import App 1.0

RowLayout {
    property TileCanvas canvas
    property Project project: canvas.project

    function doIfChangesDiscarded(actionFunction, discardChangesBeforeAction) {
        if (!project.unsavedChanges) {
            if (!!discardChangesBeforeAction)
                project.close();
            actionFunction();
            return;
        }

        function disconnectSignals() {
            saveChangesDialog.accepted.disconnect(discardChanges);
            saveChangesDialog.rejected.disconnect(dontDiscardChanges);
        }

        function discardChanges() {
            if (!!discardChangesBeforeAction)
                project.close()
            actionFunction();
            disconnectSignals();
        }

        function dontDiscardChanges() {
            disconnectSignals();
        }

        saveChangesDialog.accepted.connect(discardChanges);
        saveChangesDialog.rejected.connect(dontDiscardChanges);
        saveChangesDialog.open();
    }

    ToolButton {
        id: fileToolButton
        objectName: "fileToolButton"
        text: qsTr("File")
        hoverEnabled: true
        focusPolicy: Qt.TabFocus

        // QTBUG-54916
        Layout.preferredWidth: implicitWidth == implicitHeight ? implicitHeight + 1 : implicitWidth

        onClicked: fileMenu.open()

        Menu {
            id: fileMenu
            y: fileToolButton.height

            MenuItem {
                objectName: "newMenuButton"
                text: qsTr("New")
                hoverEnabled: true
                onClicked: doIfChangesDiscarded(function() { newProjectPopup.open() })
            }

            MenuItem {
                objectName: "openMenuButton"
                text: qsTr("Open")
                hoverEnabled: true
                onClicked: doIfChangesDiscarded(function() { openProjectDialog.open() }, true)
            }

            MenuItem {
                objectName: "saveMenuButton"
                text: qsTr("Save")
                enabled: project.canSave
                hoverEnabled: true
                onClicked: project.saveOrSaveAs()
            }

            MenuItem {
                objectName: "saveAsMenuButton"
                text: qsTr("Save As")
                enabled: project.loaded
                hoverEnabled: true
                onClicked: saveAsDialog.open()
            }

            MenuItem {
                objectName: "closeMenuButton"
                text: qsTr("Close")
                enabled: project.loaded
                hoverEnabled: true
                onClicked: doIfChangesDiscarded(function() { project.close() })
            }

            MenuItem {
                objectName: "revertMenuButton"
                text: qsTr("Revert")
                enabled: project.loaded && project.unsavedChanges
                hoverEnabled: true
                onClicked: project.revert()
            }
        }
    }

    ToolButton {
        id: editToolButton
        objectName: "editToolButton"
        text: qsTr("Edit")
        hoverEnabled: true
        focusPolicy: Qt.TabFocus

        // QTBUG-54916
        Layout.preferredWidth: implicitWidth == implicitHeight ? implicitHeight + 1 : implicitWidth

        onClicked: editMenu.open()

        Menu {
            id: editMenu
            y: editToolButton.height

            MenuItem {
                objectName: "undoMenuButton"
                text: qsTr("Undo")
                onClicked: project.undoStack.undo()
                enabled: project.undoStack.canUndo
            }

            MenuItem {
                objectName: "redoMenuButton"
                text: qsTr("Redo")
                onClicked: project.undoStack.redo()
                enabled: project.undoStack.canRedo
            }
        }
    }

    ToolButton {
        id: viewToolButton
        objectName: "viewToolButton"
        text: qsTr("View")
        hoverEnabled: true
        focusPolicy: Qt.TabFocus
        Layout.preferredWidth: implicitWidth <= implicitHeight ? implicitWidth + 1 : implicitWidth
        onClicked: viewMenu.open()

        Menu {
            id: viewMenu
            y: viewToolButton.height

            MenuItem {
                objectName: "centreMenuButton"
                text: qsTr("Centre")
                onClicked: canvas.centreView()
            }

            MenuSeparator {}

            MenuItem {
                objectName: "showGridMenuButton"
                text: qsTr("Show Grid")
                checkable: true
                checked: settings.gridVisible
                onClicked: settings.gridVisible = checked
            }

            MenuItem {
                objectName: "splitScreenMenuButton"
                text: qsTr("Split Screen")
                checkable: true
                checked: settings.splitScreen
                onClicked: settings.splitScreen = checked
            }

            MenuItem {
                objectName: "splitterLockedMenuButton"
                text: qsTr("Lock Splitter")
                checkable: true
                checked: settings.splitterLocked
                enabled: settings.splitScreen
                onClicked: settings.splitterLocked = checked
            }
        }
    }

    ToolButton {
        id: toolsToolButton
        objectName: "optionsToolButton"
        text: qsTr("Tools")
        hoverEnabled: true
        focusPolicy: Qt.TabFocus
        onClicked: settingsMenu.open()

        Menu {
            id: settingsMenu
            y: toolsToolButton.height

            MenuItem {
                objectName: "settingsMenuButton"
                text: qsTr("Options")
                onClicked: optionsDialog.open()
            }
        }
    }

    Label {
        text: "Slate"
        color: "#ffffff"
        opacity: 0.1
        style: Text.Sunken
        font.family: "arial"
        font.pixelSize: Qt.application.font.pixelSize * 1.5
        Layout.leftMargin: 8
    }
}
