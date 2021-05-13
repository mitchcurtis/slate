import QtQml.Models 2.2
import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
// TODO: remove in Qt 6
import QtQml 2.15

import App 1.0

Dialog {
    id: dialog
    objectName: "optionsDialog"
    modal: true
    focus: true
    implicitWidth: 500
    implicitHeight: 400

    standardButtons: Dialog.Ok | Dialog.Cancel

    onAccepted: applyChangesToSettings()
    // We used to use onRejected here, but if the settings changes outside of our control
    // (e.g. through being reset during testing) then some controls will contain outdated
    // values since clearChanges() won't be called in that case.
    onAboutToShow: revertToOldSettings()

    function applyChangesToSettings() {
        appearanceTab.applyChangesToSettings()
        behaviourTab.applyChangesToSettings()
    }

    function revertToOldSettings() {
        appearanceTab.revertToOldSettings()
        behaviourTab.revertToOldSettings()
    }

    header: TabBar {
        id: tabBar
        // For the Universal style.
        clip: true

        TabButton {
            objectName: "appearanceTabButton"
            text: qsTr("Appearance")
        }

        TabButton {
            objectName: "behaviourTabButton"
            text: qsTr("Behaviour")
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        AppearanceTab {
            id: appearanceTab
        }

        BehaviourTab {
            id: behaviourTab
        }
    }
}
