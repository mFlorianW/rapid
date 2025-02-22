// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick.Controls
import QtQuick.Layouts
import Rapid.Android
import QtQml
import "qrc:/qt/qml/Rapid/Android/qml/controls"

Page {
    id: mainPage
    anchors.fill: parent

    title: qsTr("Sessions")

    ColumnLayout {
        id: columnLayout
        spacing: 5
        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10

        Header {
            id: liveHeader
            Layout.fillWidth: true
            text: qsTr("Live")
        }

        Session {
            id: liveSession
            Layout.fillWidth: true
            trackName: GlobalState.activeSession.trackName
            firstEntry: qsTrId("Current") + ":"
            firstEntryValue: GlobalState.activeSession.currentLapTime
            secondEntry: qsTrId("Current Sector") + ":"
            secondEntryValue: GlobalState.activeSession.currentSectorTime
            thirdEntry: qsTrId("Lap count") + ":"
            thirdEntryValue: GlobalState.activeSession.lapCount
        }

        Header {
            id: sessionHeader
            Layout.fillWidth: true
            text: qsTr("Session")
        }
    }

    Timer {
        id: activeSessionUpdateTimer
        interval: 500
        running: true
        repeat: true
        onTriggered: {
            GlobalState.activeSession.updateLapData();
            GlobalState.activeSession.updateTrackData();
        }
    }

    onVisibleChanged: {
        activeSessionUpdateTimer.running = mainPage.visible;
    }
}
