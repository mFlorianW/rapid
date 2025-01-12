// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs

ApplicationWindow {
    id: applicationWindow
    title: "Lappy Rest GPS Source"

    minimumWidth: 640
    minimumHeight: 480

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")

            Action {
                text: qsTr("Open GPS file")
                shortcut: "Ctrl+o"
                icon.name: "document-open"
                onTriggered: fileDialog.open()
            }

            Action {
                id: actionGpsSource
                text: qsTr("Start GPS source")
            }

            MenuSeparator {}

            Action {
                text: qsTr("Quit")
                icon.name: "application-exit"
            }
        }

        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
                icon.name: "help-about"
            }
        }
    }

    header: ToolBar {
        RowLayout {
            spacing: 0

            ToolButton {
                icon.name: "document-open"
                onClicked: fileDialog.open()

                ToolTip {
                    text: qsTr("Open GPS position file")
                }
            }

            ToolButton {
                id: toolButtonGpsSource
                onClicked: !g_MainWindowViewModel.gpsSourceActive ? g_MainWindowViewModel.startGpsSource() : g_MainWindowViewModel.stopGpsSource()

                ToolTip {
                    text: qsTr("Start GPS source")
                }
            }

            Label {
                wrapMode: Label.Wrap
                text: "Host:"
                font.pixelSize: 18
            }

            TextField {
                id: serverIpInput
                implicitWidth: 150
                text: g_MainWindowViewModel.hostAddress
                Layout.leftMargin: 5

                onEditingFinished: {
                    g_MainWindowViewModel.hostAddress = serverIpInput.text;
                }
            }

            Label {
                wrapMode: Label.Wrap
                text: "Port:"
                font.pixelSize: 18
                Layout.leftMargin: 5
            }

            TextField {
                id: serverPortInput
                implicitWidth: 50
                text: g_MainWindowViewModel.hostPort
                Layout.leftMargin: 5

                onEditingFinished: {
                    g_MainWindowViewModel.hostPort = serverPortInput.text;
                }
            }
        }
    }

    SplitView {
        id: splitView
        anchors.fill: parent
        orientation: Qt.Vertical

        ListView {
            id: gpsPositionsView
            implicitHeight: 400
            width: parent.width
            boundsBehavior: ListView.StopAtBounds
            model: g_MainWindowViewModel.positionModel

            ScrollBar.vertical: ScrollBar {
                active: true
            }

            header: Rectangle {
                height: 40
                anchors.left: parent.left
                anchors.right: parent.right
                color: "black"
                //                    radius: 0.03 * root.width
                RowLayout {
                    anchors.fill: parent

                    Text {
                        id: indexText
                        Layout.alignment: Qt.AlignHCenter
                        //                            paintedWidth: parent.width / 10
                        text: qsTr("Pos Number")
                        color: "white"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        //                            width: parent.width / 4.5
                        text: qsTr("Latitude")
                        color: "white"
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        //                            width: parent.width / 4.5
                        text: qsTr("Longitude")
                        color: "white"
                    }
                }
            }

            delegate: Rectangle {
                height: 40
                width: gpsPositionsView.width
                color: model.index % 2 ? "#d3d3d3" : "white"
                RowLayout {
                    anchors.fill: parent

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: index
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: latitude
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: longitude
                    }
                }
            }
        }

        Map {
            id: map
            plugin: Plugin {
                id: mapPlugin
                name: "esri"
            }
            center: QtPositioning.coordinate(52.0270889, 11.2803483)
            zoomLevel: 16
            activeMapType: supportedMapTypes[1]
            implicitHeight: 300

            MapCircle {
                id: currentPostion
                radius: 5.0
                color: "red"
                border.width: 1
            }
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            Label {
                id: statusFooterLabel
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a GPS file"
        currentFolder: shortcuts.home

        onAccepted: {
            g_MainWindowViewModel.loadGpsFile(fileDialog.fileUrl);
        }
    }

    Connections {
        target: g_MainWindowViewModel
        function onCurrentPositionChanged() {
            currentPostion.center = g_MainWindowViewModel.currentPosition;
        }
    }

    Item {
        id: stateItem
        states: [
            State {
                name: "GpsSourceActive"
                when: g_MainWindowViewModel.gpsSourceActive === true
                PropertyChanges {
                    target: actionGpsSource
                    icon.name: "media-playback-stop"
                }
                PropertyChanges {
                    target: toolButtonGpsSource
                    icon.name: "media-playback-stop"
                }
                PropertyChanges {
                    target: statusFooterLabel
                    text: "Running"
                }
            },
            State {
                name: "GpsSourceInactive"
                when: g_MainWindowViewModel.gpsSourceActive === false
                PropertyChanges {
                    target: actionGpsSource
                    icon.name: "media-playback-start"
                }
                PropertyChanges {
                    target: toolButtonGpsSource
                    icon.name: "media-playback-start"
                }
                PropertyChanges {
                    target: statusFooterLabel
                    text: "Not Running"
                }
            }
        ]
    }
}
