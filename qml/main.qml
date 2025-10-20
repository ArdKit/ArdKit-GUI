import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import "components"

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 600
    height: 800
    minimumWidth: 500
    minimumHeight: 600
    title: "ArdKit-GUI - 机器人与无人机控制平台"

    // 计算视频区域的尺寸（根据宽高比）
    property real videoAspectRatio: configManager.videoAspectRatio === 0 ? 16/9 : 4/3
    property bool isConnected: connectionManager.isConnected
    property bool isRecording: videoHandler.isRecording

    // 菜单栏
    menuBar: MenuBar {
        Menu {
            title: "文件(&F)"
            MenuItem {
                text: "保存日志..."
                onTriggered: saveLogDialog.open()
            }
            MenuSeparator {}
            MenuItem {
                text: "退出(&Q)"
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: "连接(&C)"
            MenuItem {
                text: isConnected ? "断开连接" : "连接设备"
                onTriggered: {
                    if (isConnected) {
                        connectionManager.disconnectFromDevice()
                    } else {
                        connectionDialog.show()
                    }
                }
            }
        }

        Menu {
            title: "视频(&V)"
            MenuItem {
                text: "16:9"
                checkable: true
                checked: configManager.videoAspectRatio === 0
                onTriggered: configManager.videoAspectRatio = 0
            }
            MenuItem {
                text: "4:3"
                checkable: true
                checked: configManager.videoAspectRatio === 1
                onTriggered: configManager.videoAspectRatio = 1
            }
        }

        Menu {
            title: "帮助(&H)"
            MenuItem {
                text: "关于"
                onTriggered: aboutDialog.open()
            }
        }
    }

    // 主布局
    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 视频显示区域（可拖动调整大小）
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Vertical

            VideoDisplay {
                id: videoDisplay
                SplitView.fillWidth: true
                SplitView.preferredHeight: parent.width / videoAspectRatio
                aspectRatio: videoAspectRatio
                isPlaying: videoHandler.isPlaying
            }

            // 下半部分：工具栏 + 信息区（固定布局，不可拖动）
            ColumnLayout {
                SplitView.fillWidth: true
                SplitView.minimumHeight: 200
                spacing: 0

                // 工具栏（固定高度）
                CustomToolBar {
                    id: toolBar
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40

                    isConnected: mainWindow.isConnected
                    isRecording: mainWindow.isRecording

                    onConnectClicked: {
                        if (isConnected) {
                            connectionManager.disconnectFromDevice()
                        } else {
                            connectionDialog.show()
                        }
                    }

                    onRecordClicked: {
                        if (isRecording) {
                            videoHandler.stopRecording()
                        } else {
                            recordDialog.open()
                        }
                    }

                    onScreenshotClicked: {
                        screenshotDialog.open()
                    }

                    onConfigClicked: {
                        configDialog.open()
                    }
                }

                // 信息显示区域（自动填充剩余空间）
                MessageConsole {
                    id: messageConsole
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    messages: messageLogger.messages
                }
            }
        }
    }

    // 连接对话框
    ConnectionDialog {
        id: connectionDialog
        connectionManager: mainWindow.connectionManager
        configManager: mainWindow.configManager
    }

    // 录像对话框
    Dialog {
        id: recordDialog
        title: "开始录像"
        modal: true
        anchors.centerIn: parent
        width: 400

        ColumnLayout {
            Label {
                text: "录像将保存到视频目录"
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            var timestamp = Qt.formatDateTime(new Date(), "yyyyMMdd_HHmmss")
            var filePath = "recording_" + timestamp + ".mp4"
            videoHandler.startRecording(filePath)
        }
    }

    // 截图对话框
    Dialog {
        id: screenshotDialog
        title: "截图"
        modal: true
        anchors.centerIn: parent
        width: 400

        ColumnLayout {
            Label {
                text: "截图将保存到图片目录"
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            var timestamp = Qt.formatDateTime(new Date(), "yyyyMMdd_HHmmss")
            var filePath = "screenshot_" + timestamp + ".png"
            videoHandler.takeScreenshot(filePath)
        }
    }

    // 配置对话框
    Dialog {
        id: configDialog
        title: "配置"
        modal: true
        anchors.centerIn: parent
        width: 400

        ColumnLayout {
            spacing: 10

            Label {
                text: "最大日志行数:"
            }

            SpinBox {
                id: maxLogLinesSpinBox
                Layout.fillWidth: true
                from: 100
                to: 10000
                stepSize: 100
                value: configManager.maxLogLines
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            configManager.maxLogLines = maxLogLinesSpinBox.value
            messageLogger.maxLines = maxLogLinesSpinBox.value
        }
    }

    // 保存日志对话框
    Dialog {
        id: saveLogDialog
        title: "保存日志"
        modal: true
        anchors.centerIn: parent
        width: 400

        ColumnLayout {
            Label {
                text: "日志将保存到文档目录"
            }
        }

        standardButtons: Dialog.Ok | Dialog.Cancel

        onAccepted: {
            var timestamp = Qt.formatDateTime(new Date(), "yyyyMMdd_HHmmss")
            var filePath = "log_" + timestamp + ".txt"
            messageLogger.saveToFile(filePath)
        }
    }

    // 关于对话框
    Dialog {
        id: aboutDialog
        title: "关于 ArdKit-GUI"
        modal: true
        anchors.centerIn: parent
        width: 400

        ColumnLayout {
            spacing: 10

            Label {
                text: "ArdKit-GUI v1.0.0"
                font.pixelSize: 18
                font.bold: true
            }

            Label {
                text: "基于 Qt Quick 的机器人与无人机控制平台"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }

            Label {
                text: "功能特性:"
                font.bold: true
            }

            Label {
                text: "• 实时图传显示\n• 设备连接管理\n• 录像与截图\n• 信息日志查看"
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }

        standardButtons: Dialog.Close
    }
}
