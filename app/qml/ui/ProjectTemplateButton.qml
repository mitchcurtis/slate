import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Templates as T

T.AbstractButton {
    id: root
    implicitWidth: 300
    implicitHeight: 160
    spacing: 20
    checkable: true
    focusPolicy: Qt.TabFocus

    property string titleText
    property string descriptionText
    property real radius
    property color iconBackgroundColour

    contentItem: RowLayout {
        spacing: root.spacing

        Rectangle {
            color: Qt.darker(root.iconBackgroundColour, root.down || root.checked || root.visualFocus ? 1.3 : 1.0)
            radius: root.radius

            Layout.preferredWidth: root.implicitHeight
            Layout.fillHeight: true

            Image {
                source: root.icon.source
                anchors.centerIn: parent
            }
        }

        ColumnLayout {
            Label {
                text: titleText
                font.pixelSize: Qt.application.font.pixelSize * 1.5
                Layout.topMargin: 20
            }

            Label {
                text: descriptionText
                wrapMode: Label.Wrap
                opacity: 0.65
                Layout.fillWidth: true
            }
        }
    }
}
