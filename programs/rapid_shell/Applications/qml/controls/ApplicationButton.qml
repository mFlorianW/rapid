// SPDX-FileCopyrightText: 2024 - 2025 All contributors
//
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick

Item {
    id: root
    implicitHeight: 100
    implicitWidth: 100

    property alias iconSource: applicationIcon.source
    property alias appName: applicationName.text

    signal clicked
    signal doubleClicked

    Rectangle {
        id: hoverColor
        anchors.fill: parent
        visible: hoverHandler.hovered
        color: palette.highlight
        radius: 5
        opacity: 0.5
    }

    Item {
        id: applicationIconContainer
        implicitHeight: 35
        implicitWidth: root.implicitWidth

        Image {
            id: applicationIcon
            height: 40
            width: height
            // anchors.centerIn: parent
            anchors.top: applicationIconContainer.top
            anchors.topMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Item {
        id: applicationNameContainer
        anchors.top: applicationIconContainer.bottom
        anchors.topMargin: 10
        implicitHeight: applicationName.contentHeight + anchors.topMargin
        implicitWidth: root.implicitWidth

        Text {
            id: applicationName
            width: applicationNameContainer.width
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            maximumLineCount: 2
            wrapMode: Text.WordWrap
            elide: Text.ElideRight
        }
    }

    HoverHandler {
        id: hoverHandler
    }

    MouseArea {
        id: applicationButtonMouseArea
        anchors.fill: parent

        onClicked: root.clicked()
        onDoubleClicked: root.doubleClicked()
    }
}
