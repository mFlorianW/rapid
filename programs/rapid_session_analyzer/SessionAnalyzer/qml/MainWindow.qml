// SPDX-FileCopyrightText: 2024 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: applicationWindow
    title: "Rapid Session Analyzer"

    minimumWidth: 480
    minimumHeight: 320

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")

            Action {
                text: qsTr("Quit")
                icon.name: "application-exit"
                shortcut: "Ctrl+q"
                onTriggered: Qt.quit()
            }
        }
    }
}
