// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Drawer {
    id: contextMenu
    edge: Qt.BottomEdge
    dragMargin: 0
    height: 1.2 * drawerColumnLayout.height
    width: parent.width

    property alias model: contextMenuItems.model

    ColumnLayout {
        id: drawerColumnLayout
        anchors.right: parent.right
        anchors.left: parent.left
        spacing: 0

        Rectangle {
            id: indicator
            radius: 10
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.bottomMargin: 10
            color: "black"
            Layout.preferredHeight: 3
            Layout.preferredWidth: 30
        }

        Repeater {
            id: contextMenuItems
            delegate: ListItem {
                required property string entryText
                required property string iconSource
                required property var clickedAction
                text: entryText
                icon.source: iconSource
                Layout.fillWidth: true
                onClicked: {
                    if (typeof clickedAction === 'function') {
                        clickedAction(); // qmllint disable use-proper-function
                    } else {
                        console.error("Item callback is not a function");
                    }
                }
            }
        }
    }
}
