// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Ctrl
import QtQuick.Controls.Material
import "qrc:/Rapid/Android/qml/elements"

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
                text: pageLoader.item.title
                font.pointSize: 18
                color: "#ffffff"
            }
        }
    }

    Loader {
        id: pageLoader
        anchors.fill: parent
        source: "pages/MainPage.qml"
    }

    Ctrl.Drawer {
        id: menuDrawer
        width: 0.8 * window.width
        height: window.height
        topPadding: 0

        ColumnLayout {
            width: parent.width
            height: parent.height

            Item {
                width: parent.width
                height: 80

                RowLayout {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: 25
                    spacing: 10

                    Image {
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        source: "qrc:/Rapid/Android/img/Stopwatch.svg"
                    }

                    Text {
                        text: "Rapid"
                        font.pixelSize: 26
                    }
                }
            }

            MenuList {
                id: menuList
                Layout.fillWidth: true
                Layout.fillHeight: true

                onOpenPage: page => {
                    pageLoader.source = page;
                    menuDrawer.close();
                }
            }
        }
    }
}
