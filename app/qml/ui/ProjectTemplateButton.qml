import QtQuick 2.6
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.1
import QtQuick.Templates 2.1 as T

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
    default property alias contents: contentRowLayout.children
    property alias contentRowLayoutSpacing: contentRowLayout.spacing

    contentItem: RowLayout {
        spacing: root.spacing

        Rectangle {
            color: Qt.darker(root.iconBackgroundColour, root.down || root.checked || root.visualFocus ? 1.3 : 1.0)
            radius: root.radius

            Layout.preferredWidth: root.implicitHeight
            Layout.fillHeight: true

            RowLayout {
                id: contentRowLayout
                anchors.fill: parent
                anchors.margins: 20
                spacing: 30
            }
        }

        ColumnLayout {
            Label {
                text: titleText
                font.pixelSize: fontMetrics.font.pixelSize * 1.5
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
