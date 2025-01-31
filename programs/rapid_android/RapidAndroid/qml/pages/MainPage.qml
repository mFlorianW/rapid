// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls

Page {
    id: mainPage
    anchors.fill: parent

    title: qsTr("Sessions & Tracks")

    TabBar {
        id: mainPageTabBar
        width: parent.width
        currentIndex: mainPageViews.currentIndex

        TabButton {
            text: qsTr("Sessions")
        }

        TabButton {
            text: qsTr("Tracks")
        }
    }

    SwipeView {
        id: mainPageViews
        currentIndex: mainPageTabBar.currentIndex

        Item {
            Text {
                id: text1
                text: "Sessions"
                anchors.centerIn: parent
            }
        }

        Item {
            Text {
                id: text2
                text: "Tracks"
                anchors.centerIn: parent
            }
        }
    }
}
