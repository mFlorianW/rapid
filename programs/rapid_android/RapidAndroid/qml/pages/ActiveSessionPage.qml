// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound
import QtQuick
// import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material

// import Rapid.Android

Page {
    id: activeSessionPage
    anchors.fill: parent

    title: qsTr("Active Session")

    required property ActiveSessionPageModel viewModel

    Item {
        id: contentItem
        anchors.fill: parent
        Session {
            id: liveSession
            anchors.left: contentItem.left
            anchors.right: contentItem.right
            name: qsTrId("Track") + ": " + activeSessionPage.viewModel.activeSession.trackName
            firstEntry: qsTrId("Current") + ":"
            firstEntryValue: activeSessionPage.viewModel.activeSession.currentLapTime
            secondEntry: qsTrId("Current Sector") + ": "
            secondEntryValue: activeSessionPage.viewModel.activeSession.currentSectorTime
            thirdEntry: qsTrId("Lap count") + ": "
            thirdEntryValue: activeSessionPage.viewModel.activeSession.lapCount
        }
    }

    Timer {
        id: activeSessionUpdateTimer
        interval: 500
        repeat: true
        running: false
        onTriggered: {
            activeSessionPage.viewModel.activeSession.updateLapData();
            activeSessionPage.viewModel.activeSession.updateTrackData();
        }
    }

    Component.onCompleted: activeSessionUpdateTimer.running = true
    Component.onDestruction: activeSessionUpdateTimer.running = false
}
