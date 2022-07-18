import QtQml.Models
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import Slate

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
