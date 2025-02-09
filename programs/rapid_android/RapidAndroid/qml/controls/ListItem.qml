// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick.Controls
import QtQuick.Layouts

ItemDelegate {
    id: itemDelegate

    property alias firstItem: firstItem.contentItem
    property alias secondItem: secondItem.contentItem

    RowLayout {
        id: itemDelegateRowLayout
        anchors.right: itemDelegate.right
        anchors.rightMargin: 5
        anchors.verticalCenter: itemDelegate.verticalCenter

        Control {
            id: firstItem
        }

        Control {
            id: secondItem
        }
    }

    //property Menu menu
}
