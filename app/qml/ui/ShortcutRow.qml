import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.1

import App 1.0

RowLayout {
    width: parent ? parent.width : 0

    property string shortcutName
    property string shortcutDisplayName
    property alias originalSequence: editor.originalSequence
    property alias hasChanged: editor.hasChanged
    property alias newSequence: editor.newSequence

    function reset() {
        editor.reset();
    }

    Label {
        text: shortcutDisplayName

        Layout.leftMargin: 10
        Layout.fillWidth: true
    }

    KeySequenceEditor {
        id: editor
        objectName: shortcutName + "Editor"
        enabled: shortcutName.length > 0
        implicitHeight: editButton.implicitHeight
        originalSequence: enabled ? settings[shortcutName] : ""

        Layout.minimumWidth: 200

        // The fix for QTBUG-57098 should have been implemented in C++ as well.
        Keys.onShortcutOverride: event.accepted = (event.key === Qt.Key_Escape)

        ItemDelegate {
            id: editButton
            objectName: shortcutName + "Button"
            text: editor.displaySequence
            font.bold: editor.hasChanged
            width: parent.width
            implicitWidth: 200

            onClicked: editor.forceActiveFocus()

            SequentialAnimation {
                id: flashAnimation
                running: editor.activeFocus

                loops: Animation.Infinite
                alwaysRunToEnd: true

                NumberAnimation {
                    target: editButton.contentItem
                    property: "opacity"
                    from: 1
                    to: 0.5
                    duration: 300
                }
                NumberAnimation {
                    target: editButton.contentItem
                    property: "opacity"
                    from: 0.5
                    to: 1
                    duration: 300
                }
            }
        }
    }
}
