// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Control {
    id: menu

    signal openPage(string page)

    ListView {
        id: menuListView
        anchors.fill: menu

        boundsBehavior: Flickable.StopAtBounds

        delegate: ItemDelegate {
            required property string title
            required property string page
            required property var model
            Material.theme: Material.System

            width: menuListView.width
            text: model.title
            icon.source: "qrc:/Rapid/Android/img/Stopwatch.svg"

            onClicked: menu.openPage(page)
        }

        model: ListModel {
            ListElement {
                title: qsTr("Sessions")
                page: "pages/MainPage.qml"
            }
            ListElement {
                title: qsTr("Laptimer (Devices)")
                page: "pages/Laptimer.qml"
            }
        }
    }
}
