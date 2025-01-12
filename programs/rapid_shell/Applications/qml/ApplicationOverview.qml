// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import Rapid.Shell.Applications

Rectangle {
    id: root
    color: palette.window

    GridView {
        id: applicationsOverview
        anchors.fill: parent
        anchors.topMargin: 20
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        model: g_applicationModel

        cellHeight: 110
        cellWidth: 110

        highlight: Rectangle {
            color: palette.highlight
            radius: 5
        }

        delegate: ApplicationButton {
            appName: model.appName
            iconSource: iconUrl

            onClicked: {
                applicationsOverview.currentIndex = index;
            }

            onDoubleClicked: {
                g_control.startApplication(exePath);
            }
        }
    }
}
