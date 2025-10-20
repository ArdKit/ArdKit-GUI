import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property bool isConnected: false
    property bool isRecording: false

    signal connectClicked()
    signal recordClicked()
    signal screenshotClicked()
    signal configClicked()

    color: "#f5f5f5"
    border.color: "#d0d0d0"
    border.width: 1
    height: 40

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 8

        // 连接按钮
        Item {
            Layout.preferredWidth: 90
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.connectClicked()

                ToolTip.visible: containsMouse
                ToolTip.text: root.isConnected ? "断开设备连接" : "连接到设备"
                ToolTip.delay: 500

                Rectangle {
                    anchors.fill: parent
                    color: parent.containsMouse ? "#e0e0e0" : "transparent"
                    radius: 4

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 6

                        Image {
                            source: root.isConnected ? "qrc:/resources/icons/disconnect.png" : "qrc:/resources/icons/connect.png"
                            sourceSize.width: 32
                            sourceSize.height: 32
                            opacity: 1.0
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            text: root.isConnected ? "断开" : "连接"
                            color: "#333"
                            font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }
                }
            }
        }

        Rectangle {
            width: 1
            Layout.fillHeight: true
            Layout.topMargin: 8
            Layout.bottomMargin: 8
            color: "#d0d0d0"
        }

        // 录像按钮
        Item {
            Layout.preferredWidth: 90
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                enabled: root.isConnected
                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                onClicked: root.recordClicked()

                ToolTip.visible: containsMouse
                ToolTip.text: root.isRecording ? "停止录像" : "开始录像"
                ToolTip.delay: 500

                Rectangle {
                    anchors.fill: parent
                    color: parent.containsMouse && parent.enabled ? "#e0e0e0" : "transparent"
                    radius: 4

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 6

                        Image {
                            source: root.isRecording ? "qrc:/resources/icons/stop.png" : "qrc:/resources/icons/record.png"
                            sourceSize.width: 32
                            sourceSize.height: 32
                            opacity: root.isConnected ? 1.0 : 0.3
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            text: root.isRecording ? "停止" : "录像"
                            color: root.isConnected ? "#333" : "#999"
                            font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }
                }
            }
        }

        // 截图按钮
        Item {
            Layout.preferredWidth: 90
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                enabled: root.isConnected
                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                onClicked: root.screenshotClicked()

                ToolTip.visible: containsMouse
                ToolTip.text: "保存当前画面截图"
                ToolTip.delay: 500

                Rectangle {
                    anchors.fill: parent
                    color: parent.containsMouse && parent.enabled ? "#e0e0e0" : "transparent"
                    radius: 4

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 6

                        Image {
                            source: "qrc:/resources/icons/screenshot.png"
                            sourceSize.width: 32
                            sourceSize.height: 32
                            opacity: root.isConnected ? 1.0 : 0.3
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            text: "截图"
                            color: root.isConnected ? "#333" : "#999"
                            font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }
                }
            }
        }

        Rectangle {
            width: 1
            Layout.fillHeight: true
            Layout.topMargin: 8
            Layout.bottomMargin: 8
            color: "#d0d0d0"
        }

        // 配置按钮
        Item {
            Layout.preferredWidth: 90
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.configClicked()

                ToolTip.visible: containsMouse
                ToolTip.text: "打开配置对话框"
                ToolTip.delay: 500

                Rectangle {
                    anchors.fill: parent
                    color: parent.containsMouse ? "#e0e0e0" : "transparent"
                    radius: 4

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 6

                        Image {
                            source: "qrc:/resources/icons/settings.png"
                            sourceSize.width: 32
                            sourceSize.height: 32
                            opacity: 1.0
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            text: "配置"
                            color: "#333"
                            font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
        }

        // 状态信息
        Label {
            text: {
                if (root.isRecording) {
                    return "正在录像..."
                } else if (root.isConnected) {
                    return "已连接"
                } else {
                    return "未连接"
                }
            }
            color: {
                if (root.isRecording) {
                    return "#f44336"
                } else if (root.isConnected) {
                    return "#4CAF50"
                } else {
                    return "#999"
                }
            }
            font.pixelSize: 13
            font.bold: true
        }
    }
}
