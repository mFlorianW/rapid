// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Rapid.Android

Page {
    id: laptimerSessionPage
    anchors.fill: parent

    title: qsTr("Laptimer Sessions")

    required property LaptimerSessionPageModel viewModel

    Item {
        id: contentItem
        anchors.fill: parent

        ListView {
            id: laptimerSessionView
            anchors.fill: parent
            boundsBehavior: ListView.StopAtBounds
            spacing: 5

            property var clickedIndex: 0

            model: laptimerSessionPage.viewModel.sessionMetadataListModel

            delegate: Session {
                required property string trackName
                required property string time
                required property string date
                required property var model

                width: laptimerSessionView.width

                name: trackName
                firstEntry: date
                secondEntry: time

                onClicked: {}
            }
        }

        RoundButton {
            id: laptimerPageFabButton
            height: 60
            width: laptimerPageFabButton.height
            icon.source: "qrc:/qt/qml/Rapid/Android/img/Refresh.svg"

            Material.roundedScale: Material.FullScale

            anchors.bottom: contentItem.bottom
            anchors.bottomMargin: 30
            anchors.right: contentItem.right
            anchors.rightMargin: 15

            onClicked: {
                laptimerSessionPage.viewModel.updateSessionMetadata();
            }
        }
    }
}
