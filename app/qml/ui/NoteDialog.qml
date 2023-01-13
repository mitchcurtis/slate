/*
    Copyright 2023, Mitch Curtis

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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Slate

import "." as Ui

Dialog {
    id: root
    objectName: "noteDialog"
    title: currentAction === NoteDialog.NoteAction.Create ? qsTr("Create note") : qsTr("Edit note")
    modal: true
    dim: false
    focus: true
    width: Math.max(implicitWidth, 300)

    enum NoteAction {
        Create,
        Modify
    }

    property Project project
    property ImageCanvas canvas
    property int currentAction
    property int newNoteX
    property int newNoteY
    property int modifyingNoteIndex: -1

    readonly property int editedNoteX: parseInt(xTextField.text, 10)
    readonly property int editedNoteY: parseInt(yTextField.text, 10)

    onAboutToShow: {
        if (currentAction === NoteDialog.NoteAction.Create) {
            noteTextField.text = ""
            xTextField.text = newNoteX
            yTextField.text = newNoteY
        } else {
            noteTextField.text = project.noteTextAtIndex(modifyingNoteIndex)
            let position = project.notePositionAtIndex(modifyingNoteIndex)
            xTextField.text = position.x
            yTextField.text = position.y
        }

        noteTextField.forceActiveFocus()
    }

    onAccepted: {
        if (currentAction === NoteDialog.NoteAction.Create)
            canvas.addNote(Qt.point(editedNoteX, editedNoteY), noteTextField.text)
        else
            canvas.modifyNote(modifyingNoteIndex, Qt.point(editedNoteX, editedNoteY), noteTextField.text)
    }

    onClosed: canvas.forceActiveFocus()

    contentItem: GridLayout {
        columns: 2
        rowSpacing: 4

        Label {
            text: qsTr("Text")

            Layout.column: 0
            Layout.fillWidth: true
        }
        Ui.TextField {
            id: noteTextField
            objectName: root.objectName + "TextField"
            maximumLength: 256

            Layout.fillWidth: true

            onAccepted: root.accept()
        }

        Label {
            text: qsTr("X")

            Layout.fillWidth: true
        }
        Ui.TextField {
            id: xTextField
            objectName: root.objectName + "XTextField"
            selectByMouse: true
            validator: IntValidator {
                bottom: 0
                top: project ? project.size.width - 100 : 1 // TODO
            }

            Layout.fillWidth: true

            onAccepted: root.accept()
        }

        Label {
            text: qsTr("Y")

            Layout.fillWidth: true
        }
        Ui.TextField {
            id: yTextField
            objectName: root.objectName + "YTextField"
            selectByMouse: true
            validator: IntValidator {
                bottom: 0
                top: project ? project.size.height - 20 : 1 // TODO
            }

            Layout.fillWidth: true

            onAccepted: root.accept()
        }
    }

    footer: DialogButtonBox {
        DialogButton {
            objectName: "noteDialogOkButton"
            text: currentAction === NoteDialog.NoteAction.Create ? qsTr("Create") : qsTr("Modify")

            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        DialogButton {
            objectName: "noteDialogCancelButton"
            text: qsTr("Cancel")

            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
    }
}
