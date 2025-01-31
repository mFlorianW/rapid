// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Ctrl
import QtQuick.Controls.Material

Ctrl.ApplicationWindow {
    id: window
    visible: true
    title: qsTr("Rapid Android")
    width: 360
    height: 720

    header: Ctrl.ToolBar {
        id: topNavbar
        height: 64

        Material.elevation: 6

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter
            Ctrl.Button {
                id: navBarBurgerButton
                implicitHeight: 50
                implicitWidth: navBarBurgerButton.implicitHeight
                flat: true
                icon.source: "qrc:/Rapid/Android/img/BurgerButton.svg"
                onClicked: menuDrawer.open()
            }

            Text {
                id: navBarText
                text: "Rapid"
                font.pointSize: 18
                color: "#ffffff"
            }
        }
    }

    Ctrl.Drawer {
        id: menuDrawer
        width: 290
        height: window.height
        topPadding: 0

        Column {
            width: parent.width

            Item {
                width: parent.width
                height: 60

                RowLayout {

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 10

                    Image {
                        Layout.preferredWidth: 24
                        Layout.preferredHeight: 24
                        source: "qrc:/Rapid/Android/img/Stopwatch.svg"
                    }

                    Text {
                        text: "Rapid"
                    }
                }
            }
        }
    }
}
