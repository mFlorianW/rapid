// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound
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

    required property SessionPageModel viewModel

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

            property var clickedIndex: 0

            model: mainPage.viewModel.sessionListModel

            delegate: Session {
                required property string trackName
                required property string time
                required property string date
                required property var model

                width: parent.width

                name: trackName
                firstEntry: date
                secondEntry: time

                onClicked: {
                    localSessionView.clickedIndex = model.index;
                    contextMenu.open();
                }
            }
        }
    }

    Drawer {
        id: contextMenu
        edge: Qt.BottomEdge
        dragMargin: 0
        height: 1.2 * drawerColumnLayout.height
        width: parent.width

        ColumnLayout {
            id: drawerColumnLayout
            anchors.right: parent.right
            anchors.left: parent.left
            spacing: 0

            Rectangle {
                id: indicator
                radius: 10
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.bottomMargin: 10
                color: "black"
                Layout.preferredHeight: 3
                Layout.preferredWidth: 30
            }

            ListItem {
                id: showLap
                Layout.fillWidth: true
                text: qsTr("Show Laps")
                icon.source: "qrc:/qt/qml/Rapid/Android/img/Stopwatch.svg"
                onClicked: {
                    mainPage.viewModel.analyzeSession(localSessionView.clickedIndex);
                    contextMenu.close();
                    lapDialog.open();
                }
            }
        }
    }

    Dialog {
        id: lapDialog
        anchors.centerIn: parent
        standardButtons: Dialog.Close
        width: 0.8 * mainPage.width
        height: 0.7 * mainPage.height

        title: qsTr("Laps")

        ListView {
            id: lapListView
            anchors.fill: parent

            model: mainPage.viewModel.lapListModel

            delegate: ListItem {
                id: lapDelegate
                required property var laptime
                required property var model
                width: lapListView.width
                text: model.index + ": " + laptime
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
