import QtQuick
import QtQuick.Controls

import App

Dialog {
    id: root
    objectName: "discardChangesDialog"
    title: qsTr("Unsaved changes")
    modal: true

    property Project project
    property var saveAsDialog

    function doIfChangesSavedOrDiscarded(actionFunction, skipChangesConfirmationIfNoProject) {
        if ((skipChangesConfirmationIfNoProject === undefined || skipChangesConfirmationIfNoProject === true) && !project) {
            // If there's no project open, some features should be able to
            // be performed immediately, such as Open.
            actionFunction()
            return
        }

        if (!project) {
            // Auto tests can skip this function.
            return
        }

        if (!project.unsavedChanges) {
            actionFunction()
            return
        }

        // There are unsaved changes, so we need to prompt.

        function disconnectSaveChangesSignals() {
            root.accepted.disconnect(saveChanges)
            root.discarded.disconnect(discardChanges)
            root.rejected.disconnect(cancel)
        }

        function saveChanges() {
            if (project.url.toString().length > 0) {
                // Existing project; can save without a dialog.
                if (project.save()) {
                    // Saved successfully, so now we can perform the action.
                    performAction()
                } else {
                    // Failed to save; cancel.
                    cancel()
                }
            } else {
                // New project; need to save as.
                function disconnectSaveAsSignals() {
                    project.errorOccurred.disconnect(saveAsFailed)
                    project.postProjectSaved.disconnect(saveAsSucceeded)
                    saveAsDialog.rejected.disconnect(saveAsDialogRejected)
                }

                function saveAsSucceeded() {
                    disconnectSaveAsSignals()
                    performAction()
                }

                function saveAsFailed() {
                    disconnectSaveAsSignals()
                    disconnectSaveChangesSignals()
                }

                function saveAsDialogRejected() {
                    disconnectSaveAsSignals()
                    cancel()
                }

                // The save as dialog can be accepted, but the project
                // still needs to save successfully. If it fails, the error
                // signal will be emitted after the dialog's accepted signal
                // is emitted, so connecting to the error signal in response
                // to the save as dialog being accepted is too late,
                // and that's why we do it here.
                project.errorOccurred.connect(saveAsFailed)

                // Performing the action in response to the accepted signal is
                // too early, as that is emitted before the actual saving is
                // done. That's why we need to listen to the postProjectSaved()
                // signal.
                project.postProjectSaved.connect(saveAsSucceeded)

                saveAsDialog.rejected.connect(saveAsDialogRejected)

                saveAsDialog.open()
            }
        }

        function discardChanges() {
            performAction()
            // TODO: temporary until https://bugreports.qt.io/browse/QTBUG-67168 is fixed.
            root.close()
        }

        function performAction() {
            disconnectSaveChangesSignals()
            actionFunction()
        }

        function cancel() {
            disconnectSaveChangesSignals()
        }

        root.accepted.connect(saveChanges)
        root.discarded.connect(discardChanges)
        root.rejected.connect(cancel)
        root.open()
    }

    onAccepted: console.log(loggingCategory, "accepted (saved) SaveChangesDialog")
    onRejected: console.log(loggingCategory, "rejected (cancelled) SaveChangesDialog")
    onDiscarded: console.log(loggingCategory, "discarded SaveChangesDialog")

    LoggingCategory {
        id: loggingCategory
        name: "ui.saveChangesDialog"
    }
    
    Label {
        text: qsTr("Save changes to the project before closing?")
    }
    
    // Using a DialogButtonBox allows us to assign objectNames to the buttons,
    // which makes it possible to test them.
    footer: DialogButtonBox {
        Button {
            objectName: "cancelDialogButton"
            text: qsTr("Cancel")
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
        Button {
            objectName: "saveChangesDialogButton"
            text: qsTr("Save")
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
        Button {
            objectName: "discardChangesDialogButton"
            text: qsTr("Don't save")
            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
        }
    }
}
