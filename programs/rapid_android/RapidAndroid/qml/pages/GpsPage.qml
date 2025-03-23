// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import Rapid.Android

Page {
    id: gpsPage
    anchors.fill: parent

    title: qsTr("GPS Informations")

    required property GpsPageModel viewModel

    Item {
        id: contentItem
        anchors.fill: parent

        ColumnLayout {
            id: contentColumn
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem {
                id: gpsStatus
                text: qsTr("GPS Status")
                Layout.fillWidth: true

                firstItem: Image {
                    sourceSize.height: 20
                    sourceSize.width: 20
                    source: gpsPage.viewModel.gpsStatus
                }
            }

            ListItem {
                id: sattellites
                text: qsTr("Satellites")
                Layout.fillWidth: true

                firstItem: Text {
                    text: gpsPage.viewModel.satellites
                }
            }
        }

        RoundButton {
            id: laptimerPageFabButton
            height: 60
            width: laptimerPageFabButton.height
            icon.source: gpsPage.viewModel.activeIcon

            Material.roundedScale: Material.FullScale

            anchors.bottom: contentItem.bottom
            anchors.bottomMargin: 30
            anchors.right: contentItem.right
            anchors.rightMargin: 15

            onClicked: gpsPage.viewModel.isRunning ? gpsPage.viewModel.stop() : gpsPage.viewModel.start()
        }
    }

    Component.onCompleted: viewModel.start()
}
