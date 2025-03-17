// SPDX-FileCopyrightText: 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Pane {
    id: session

    property alias name: sessionTrackName.text
    property alias firstEntry: sessionLast.text
    property alias firstEntryValue: sessionLastTime.text
    property alias secondEntry: sessionCurrent.text
    property alias secondEntryValue: sessionCurrentTime.text
    property alias thirdEntry: sessionLap.text
    property alias thirdEntryValue: sessionLapCount.text

    signal clicked

    ColumnLayout {
        id: sessionLayout
        spacing: 0
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        Text {
            id: sessionTrackName
            text: qsTr("Unknown Track")
        }

        Row {
            Layout.fillWidth: true
            spacing: 2

            Text {
                id: sessionLast
            }

            Text {
                id: sessionLastTime
            }
        }

        Row {
            Layout.fillWidth: true
            spacing: 2

            Text {
                id: sessionCurrent
            }

            Text {
                id: sessionCurrentTime
            }
        }

        Row {
            Layout.fillWidth: true
            spacing: 2

            Text {
                id: sessionLap
            }

            Text {
                id: sessionLapCount
            }
        }
        TapHandler {
            id: sessionTapHandler
            gesturePolicy: TapHandler.WithinBounds
            acceptedButtons: Qt.LeftButton
            onTapped: session.clicked()
        }
    }
}
