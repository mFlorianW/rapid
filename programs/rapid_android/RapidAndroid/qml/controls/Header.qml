// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: columnLayout
    spacing: 0

    property alias text: headerText.text

    Text {
        id: headerText
    }

    Rectangle {
        id: separator
        Layout.fillWidth: true
        implicitHeight: 1
        color: "#1E000000"
    }
}
