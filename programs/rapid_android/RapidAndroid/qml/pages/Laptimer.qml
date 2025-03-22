// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Ctrl
import QtQuick.Controls.Material
import QtQuick.Layouts
import Rapid.Android
import "qrc:/qt/qml/Rapid/Android/qml/controls"

Ctrl.Page {
    id: laptimerPage
    anchors.fill: parent

    title: qsTr("Laptimer (Devices)")

    required property RapidAndroidModel appModel

    Item {
        id: contentItem
        anchors.fill: parent

        ListView {
            id: deviceListView
            anchors.fill: parent
            anchors.margins: 5
            spacing: 5
            boundsBehavior: Flickable.StopAtBounds
            model: laptimerPage.appModel.deviceManagement.model

            delegate: ListItem {
                id: delegate
                required property var laptimer
                required property var index
                icon.source: "qrc:/qt/qml/Rapid/Android/img/Stopwatch.svg"
                width: deviceListView.width
                text: laptimer.name
                autoExclusive: true

                firstItem: Ctrl.Switch {
                    id: enableSwitch
                    checked: delegate.laptimer.defaultDevice
                    onClicked: {
                        laptimerPage.appModel.deviceManagement.enable(delegate.laptimer);
                    }
                }

                onClicked: {
                    deviceInputPopup.laptimer = this.laptimer;
                    laptimerPage.state = "edit";
                    contextMenu.open();
                }
            }
        }

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

            onClicked: {
                laptimerPage.state = "new";
                deviceInputPopup.laptimer = null;
                deviceInputPopup.open();
            }
        }
    }

    ContextMenu {
        id: contextMenu
        model: ListModel {
            ListElement {
                entryText: qsTr("Edit Laptimer")
                iconSource: "qrc:/qt/qml/Rapid/Android/img/Edit.svg"
                clickedAction: function () {
                    contextMenu.close();
                    deviceInputPopup.open();
                }
            }

            ListElement {
                entryText: qsTr("Delete Laptimer")
                iconSource: "qrc:/qt/qml/Rapid/Android/img/Trash.svg"
                clickedAction: function () {
                    contextMenu.close();
                    laptimerPage.appModel.deviceManagement.remove(deviceInputPopup.laptimer);
                }
            }
        }
    }

    Ctrl.Dialog {
        id: deviceInputPopup
        anchors.centerIn: parent
        standardButtons: Dialog.Save | Dialog.Cancel
        width: 0.8 * laptimerPage.width

        property var laptimer: null

        ColumnLayout {
            id: deviceInputPopupLayout
            anchors.fill: parent
            spacing: 10

            Ctrl.TextField {
                id: deviceInputPopupDeviceName
                Layout.fillWidth: true
                placeholderText: "Name"
            }

            Ctrl.TextField {
                id: deviceInputPopupDeviceIp
                Layout.fillWidth: true
                placeholderText: "IP Address"
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
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                validator: IntValidator {
                    id: portValidator
                    bottom: 0
                    top: 65535
                }
            }
        }

        onAccepted: {
            var device = laptimerPage.appModel.create(deviceInputPopupDeviceName.text, deviceInputPopupDeviceIp.text, deviceInputPopupDevicePort.text, false);
            if (laptimerPage.state === "new") {
                laptimerPage.appModel.deviceManagement.store(device);
            } else if (laptimerPage.state === "edit") {
                laptimerPage.appModel.deviceManagement.update(deviceInputPopup.laptimer, device);
            }
        }
    }

    states: [
        State {
            name: "new"
            PropertyChanges {
                deviceInputPopup {
                    title: qsTr("New Laptimer")
                }
                deviceInputPopupDeviceName {
                    text: "Rapid"
                }
                deviceInputPopupDeviceIp {
                    text: "192.168.1.1"
                }
                deviceInputPopupDevicePort {
                    text: "27018"
                }
            }
        },
        State {
            name: "edit"
            PropertyChanges {
                deviceInputPopup {
                    title: qsTr("Edit Laptimer")
                }
                deviceInputPopupDeviceName {
                    text: deviceInputPopup.laptimer.name
                }
                deviceInputPopupDeviceIp {
                    text: deviceInputPopup.laptimer.ip
                }
                deviceInputPopupDevicePort {
                    text: deviceInputPopup.laptimer.port
                }
            }
        }
    ]
}
