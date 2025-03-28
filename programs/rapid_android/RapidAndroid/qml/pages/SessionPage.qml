// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import Rapid.Android
import QtQml
import "qrc:/qt/qml/Rapid/Android/qml/controls"

Page {
    id: sessionPage
    anchors.fill: parent

    title: qsTr("Stored Sessions")

    required property SessionPageModel viewModel

    Item {
        id: contentItem
        anchors.fill: parent
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

            ListView {
                id: localSessionView
                Layout.fillWidth: true
                Layout.fillHeight: true
                boundsBehavior: ListView.StopAtBounds
                spacing: 5

                property var clickedIndex: 0

                model: sessionPage.viewModel.sessionListModel

                delegate: Session {
                    required property string trackName
                    required property string time
                    required property string date
                    required property var model

                    name: trackName
                    firstEntry: date
                    secondEntry: time

                    onClicked: {
                        localSessionView.clickedIndex = model.index;
                        contextMenu.model = sessionContextMenuModel;
                        contextMenu.open();
                    }
                }
            }
        }
    }

    ContextMenu {
        id: contextMenu
    }

    ListModel {
        id: sessionContextMenuModel
        ListElement {
            entryText: qsTr("Show Laps")
            iconSource: "qrc:/qt/qml/Rapid/Android/img/Stopwatch.svg"
            clickedAction: function () {
                sessionPage.viewModel.analyzeSession(localSessionView.clickedIndex);
                contextMenu.close();
                lapDialog.open();
            }
        }
    }

    ListModel {
        id: pageContextMenuModel

        ListElement {
            entryText: qsTrId("Activate Live timing")
            iconSource: "qrc:/qt/qml/Rapid/Android/img/Stopwatch.svg"
            clickedAction: function () {
                activeSessionUpdateTimer.running = !activeSessionUpdateTimer.running;
            }
        }
    }

    Dialog {
        id: lapDialog
        anchors.centerIn: parent
        standardButtons: Dialog.Close
        width: 0.8 * sessionPage.width
        height: 0.7 * sessionPage.height

        title: qsTr("Laps")

        ListView {
            id: lapListView
            anchors.fill: parent

            model: sessionPage.viewModel.lapListModel

            delegate: ListItem {
                id: lapDelegate
                required property var laptime
                required property var model
                width: lapListView.width
                text: model.index + ": " + laptime
            }
        }
    }
}
