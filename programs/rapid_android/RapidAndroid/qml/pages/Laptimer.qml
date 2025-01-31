// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls as Ctrl
import QtQuick.Controls.Material
import QtQuick.Layouts

Ctrl.Page {
    id: laptimerPage
    anchors.fill: parent

    title: qsTr("Laptimer (Devices)")

    Item {
        id: contentItem
        anchors.fill: parent

        Ctrl.RoundButton {
            id: laptimerPageFabButton
            height: 60
            width: laptimerPageFabButton.height
            text: "+"

            Material.roundedScale: Material.FullScale

            anchors.bottom: contentItem.bottom
            anchors.bottomMargin: 30
            anchors.right: contentItem.right
            anchors.rightMargin: 15

            onClicked: deviceInputPopup.open()
        }
    }

    Ctrl.Dialog {
        id: deviceInputPopup
        anchors.centerIn: parent
        title: "New Device"
        standardButtons: Dialog.Save | Dialog.Cancel
        width: 0.8 * laptimerPage.width

        ColumnLayout {
            id: deviceInputPopupLayout
            anchors.fill: parent

            Ctrl.TextField {
                id: deviceInputPopupDeviceName
                Layout.fillWidth: true
                placeholderText: "Name"
                text: "Rapid Laptimer"
            }

            Ctrl.TextField {
                id: deviceInputPopupDeviceIp
                Layout.fillWidth: true
                placeholderText: "IP Address"
                text: "192.168.1.115"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: RegularExpressionValidator {
                    id: ipAddressRegExValidation
                    regularExpression: /^(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|1?[0-9][0-9]?)$/
                }
            }

            Ctrl.TextField {
                id: deviceInputPopupDevicePort
                Layout.fillWidth: true
                placeholderText: "Device Port"
                text: "27015"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: IntValidator {
                    id: portValidator
                    bottom: 0
                    top: 65535
                }
            }
        }
    }
}
