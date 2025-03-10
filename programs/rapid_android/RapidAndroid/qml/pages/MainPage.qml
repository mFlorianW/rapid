// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
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
        anchors.bottom: parent.bottom

        Header {
            id: liveHeader
            Layout.fillWidth: true
            text: qsTr("Live")
        }

        Session {
            id: liveSession
            Layout.fillWidth: true
            name: GlobalState.activeSession.trackName
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

        ListView {
            id: localSessionView
            Layout.fillWidth: true
            Layout.fillHeight: true
            // Layout.topMargin: 100
            boundsBehavior: ListView.StopAtBounds
            spacing: 5

            model: GlobalState.localSessionManagement.listModel

            delegate: Session {
                required property string trackName
                required property string time
                required property string date

                width: parent.width

                name: trackName
                firstEntry: date
                secondEntry: time
            }
        }
    }

    Timer {
        id: activeSessionUpdateTimer
        interval: 500
        repeat: true
        onTriggered: {
            GlobalState.activeSession.updateLapData();
            GlobalState.activeSession.updateTrackData();
        }
    }

    Component.onCompleted: {
        activeSessionUpdateTimer.running = mainPage.visible;
    }

    Component.onDestruction: {
        activeSessionUpdateTimer.running = mainPage.visible;
    }
}
