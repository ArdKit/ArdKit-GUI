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
    signal playbackClicked()

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

        // 回放按钮
        Item {
            Layout.preferredWidth: 90
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: root.playbackClicked()

                ToolTip.visible: containsMouse
                ToolTip.text: "播放本地录像文件"
                ToolTip.delay: 500

                Rectangle {
                    anchors.fill: parent
                    color: parent.containsMouse ? "#e0e0e0" : "transparent"
                    radius: 4

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 6

                        Image {
                            source: "qrc:/resources/icons/playback.png"
                            sourceSize.width: 32
                            sourceSize.height: 32
                            opacity: 1.0
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            text: "回放"
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

        // 分隔符
        Rectangle {
            width: 1
            Layout.fillHeight: true
            Layout.topMargin: 8
            Layout.bottomMargin: 8
            color: "#d0d0d0"
        }

        // 进度条（LIVE模式，显示流动动画）
        Item {
            Layout.preferredWidth: 250
            Layout.fillHeight: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8

            Rectangle {
                anchors.centerIn: parent
                width: parent.width
                height: 4
                color: "#e0e0e0"
                radius: 2
                clip: true

                // 流动指示器 - 移动的渐变条
                Rectangle {
                    id: flowIndicator
                    height: parent.height
                    width: parent.width * 0.3  // 30% 宽度的流动条
                    radius: 2
                    visible: root.isConnected

                    gradient: Gradient {
                        orientation: Gradient.Horizontal
                        GradientStop { position: 0.0; color: "transparent" }
                        GradientStop { position: 0.3; color: "#2196F3" }
                        GradientStop { position: 0.5; color: "#64B5F6" }
                        GradientStop { position: 0.7; color: "#2196F3" }
                        GradientStop { position: 1.0; color: "transparent" }
                    }

                    // 从左到右移动
                    SequentialAnimation on x {
                        running: root.isConnected
                        loops: Animation.Infinite
                        NumberAnimation {
                            from: -flowIndicator.width
                            to: parent.width
                            duration: 2000
                            easing.type: Easing.Linear
                        }
                    }
                }

                // 未连接时显示静态灰色条
                Rectangle {
                    anchors.fill: parent
                    radius: 2
                    visible: !root.isConnected
                    color: "#cccccc"
                }
            }
        }

        // 播放/暂停按钮
        Item {
            Layout.preferredWidth: 70
            Layout.fillHeight: true

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                enabled: root.isConnected
                cursorShape: enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                onClicked: {
                    if (videoHandler.isPaused) {
                        videoHandler.resumeVideo()
                    } else {
                        videoHandler.pauseVideo()
                    }
                }

                ToolTip.visible: containsMouse
                ToolTip.text: videoHandler.isPaused ? "继续播放" : "暂停播放"
                ToolTip.delay: 500

                Rectangle {
                    anchors.fill: parent
                    color: parent.containsMouse && parent.enabled ? "#e0e0e0" : "transparent"
                    radius: 4

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 6

                        Text {
                            text: videoHandler.isPaused ? "▶" : "⏸"
                            color: root.isConnected ? "#333" : "#999"
                            font.pixelSize: 16
                            Layout.alignment: Qt.AlignVCenter
                        }

                        Text {
                            text: videoHandler.isPaused ? "继续" : "暂停"
                            color: root.isConnected ? "#333" : "#999"
                            font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }
                    }
                }
            }
        }
    }
}
