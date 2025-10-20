import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property var messages: []
    property alias agentMessages: agentListModel

    color: "#1e1e1e"
    border.color: "#333333"
    border.width: 1

    // Agent消息模型
    ListModel {
        id: agentListModel
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Tab栏和清除按钮
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 35
            color: "#2d2d2d"

            RowLayout {
                anchors.fill: parent
                spacing: 0

                // Tab标签栏（左对齐，梯形样式）
                RowLayout {
                    spacing: -8
                    Layout.preferredHeight: 35

                    // 信息输出标签
                    Item {
                        Layout.preferredWidth: 120
                        Layout.preferredHeight: 35

                        // 梯形背景
                        Canvas {
                            id: tab1Canvas
                            anchors.fill: parent
                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.reset()

                                var isActive = tabBar.currentIndex === 0
                                ctx.fillStyle = isActive ? "#3d3d3d" : "#2d2d2d"

                                // 绘制梯形（下宽上窄）
                                ctx.beginPath()
                                ctx.moveTo(0, 35)           // 左下
                                ctx.lineTo(8, 8)            // 左上
                                ctx.lineTo(width - 8, 8)    // 右上
                                ctx.lineTo(width, 35)       // 右下
                                ctx.closePath()
                                ctx.fill()
                            }
                        }

                        RowLayout {
                            anchors.centerIn: parent
                            anchors.verticalCenterOffset: 4
                            spacing: 5

                            Label {
                                text: "信息输出"
                                color: tabBar.currentIndex === 0 ? "#ffffff" : "#888888"
                                font.pixelSize: 12
                            }
                            Label {
                                text: "(" + root.messages.length + ")"
                                color: tabBar.currentIndex === 0 ? "#cccccc" : "#666666"
                                font.pixelSize: 10
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: tabBar.currentIndex = 0
                        }

                        Connections {
                            target: tabBar
                            function onCurrentIndexChanged() {
                                tab1Canvas.requestPaint()
                            }
                        }
                    }

                    // Agent标签
                    Item {
                        Layout.preferredWidth: 90
                        Layout.preferredHeight: 35
                        enabled: false  // 暂时禁用，待实现后启用

                        // 梯形背景
                        Canvas {
                            id: tab2Canvas
                            anchors.fill: parent
                            opacity: parent.enabled ? 1.0 : 0.5
                            onPaint: {
                                var ctx = getContext("2d")
                                ctx.reset()

                                var isActive = tabBar.currentIndex === 1 && parent.enabled
                                ctx.fillStyle = isActive ? "#3d3d3d" : "#2d2d2d"

                                // 绘制梯形（下宽上窄）
                                ctx.beginPath()
                                ctx.moveTo(0, 35)           // 左下
                                ctx.lineTo(8, 8)            // 左上
                                ctx.lineTo(width - 8, 8)    // 右上
                                ctx.lineTo(width, 35)       // 右下
                                ctx.closePath()
                                ctx.fill()
                            }
                        }

                        Label {
                            anchors.centerIn: parent
                            anchors.verticalCenterOffset: 4
                            text: "Agent"
                            color: parent.enabled ? (tabBar.currentIndex === 1 ? "#ffffff" : "#888888") : "#555555"
                            font.pixelSize: 12
                        }

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: parent.enabled ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                            enabled: parent.enabled
                            onClicked: tabBar.currentIndex = 1
                        }

                        Connections {
                            target: tabBar
                            function onCurrentIndexChanged() {
                                tab2Canvas.requestPaint()
                            }
                        }
                    }
                }

                // 隐藏的TabBar（用于管理状态）
                Item {
                    id: tabBar
                    property int currentIndex: 0
                }

                // 填充空间
                Item {
                    Layout.fillWidth: true
                }
            }
        }

        // StackLayout 切换内容
        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // 第一页：信息输出
            ColumnLayout {
                spacing: 0

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    ListView {
                        id: messageListView
                        model: root.messages
                        spacing: 2

                        // 自动滚动到底部
                        onCountChanged: {
                            Qt.callLater(function() {
                                messageListView.positionViewAtEnd()
                            })
                        }

                        delegate: Rectangle {
                            width: messageListView.width
                            height: messageText.implicitHeight + 10
                            color: index % 2 === 0 ? "#252525" : "#2a2a2a"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                spacing: 10

                                Label {
                                    text: (index + 1).toString()
                                    color: "#666666"
                                    font.pixelSize: 11
                                    font.family: "Monospace"
                                    Layout.preferredWidth: 40
                                    horizontalAlignment: Text.AlignRight
                                }

                                Label {
                                    id: messageText
                                    text: modelData
                                    color: {
                                        // 根据消息类型设置颜色
                                        if (modelData.includes("[ERROR]")) {
                                            return "#f44336"
                                        } else if (modelData.includes("[WARN]")) {
                                            return "#ff9800"
                                        } else if (modelData.includes("[INFO]")) {
                                            return "#4CAF50"
                                        } else {
                                            return "#e0e0e0"
                                        }
                                    }
                                    font.pixelSize: 12
                                    font.family: "Monospace"
                                    wrapMode: Text.Wrap
                                    Layout.fillWidth: true
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    messageText.selectAll()
                                }
                            }
                        }

                        // 占位符（当没有消息时显示）
                        Label {
                            anchors.centerIn: parent
                            text: "暂无消息"
                            color: "#666666"
                            font.pixelSize: 14
                            visible: messageListView.count === 0
                        }
                    }
                }

                // 底部信息栏
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 25
                    color: "#2d2d2d"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        spacing: 10

                        Label {
                            text: "最大行数: " + messageLogger.maxLines
                            color: "#888888"
                            font.pixelSize: 10
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        Label {
                            id: timeLabel
                            text: Qt.formatDateTime(new Date(), "yyyy-MM-dd HH:mm:ss")
                            color: "#888888"
                            font.pixelSize: 10
                        }
                    }

                    // 定时器更新时间
                    Timer {
                        interval: 1000
                        running: true
                        repeat: true
                        onTriggered: timeLabel.text = Qt.formatDateTime(new Date(), "yyyy-MM-dd HH:mm:ss")
                    }
                }
            }

            // 第二页：Agent聊天
            ColumnLayout {
                spacing: 0

                // Agent消息显示区
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    ListView {
                        id: agentListView
                        model: agentListModel
                        spacing: 8

                        // 自动滚动到底部
                        onCountChanged: {
                            Qt.callLater(function() {
                                agentListView.positionViewAtEnd()
                            })
                        }

                        delegate: Rectangle {
                            width: agentListView.width
                            height: agentMessageText.implicitHeight + 20
                            color: "transparent"

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 20
                                anchors.rightMargin: 10
                                spacing: 10

                                // 消息内容
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: agentMessageText.implicitHeight + 10
                                    color: model.role === "user" ? "#2a2a3d" : "#2a3d2a"
                                    radius: 4

                                    Label {
                                        id: agentMessageText
                                        anchors.fill: parent
                                        anchors.margins: 5
                                        text: model.message
                                        color: "#e0e0e0"
                                        font.pixelSize: 14
                                        wrapMode: Text.Wrap
                                    }
                                }
                            }
                        }

                        // 占位符
                        Label {
                            anchors.centerIn: parent
                            text: "开始与Agent对话..."
                            color: "#666666"
                            font.pixelSize: 14
                            visible: agentListView.count === 0
                        }
                    }
                }

                // 输入区域
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: "#2d2d2d"

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 24
                        anchors.rightMargin: 24
                        anchors.topMargin: 5
                        anchors.bottomMargin: 5
                        spacing: 10

                        // "告诉Agent:" 提示标签
                        Label {
                            text: "告诉Agent:"
                            color: "#cccccc"
                            font.pixelSize: 14
                            Layout.alignment: Qt.AlignVCenter
                        }

                        TextField {
                            id: agentInputField
                            Layout.fillWidth: true
                            Layout.preferredHeight: 32
                            placeholderText: "输入消息..."
                            font.pixelSize: 14
                            color: "#e0e0e0"
                            verticalAlignment: TextInput.AlignVCenter

                            background: Rectangle {
                                color: "#3d3d3d"
                                border.color: agentInputField.activeFocus ? "#4CAF50" : "#555555"
                                border.width: 1
                                radius: 4
                            }

                            Keys.onReturnPressed: {
                                sendAgentMessage()
                            }
                        }

                        Button {
                            text: "发送"
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 32

                            onClicked: sendAgentMessage()

                            background: Rectangle {
                                color: parent.pressed ? "#388E3C" : (parent.hovered ? "#4CAF50" : "#2E7D32")
                                radius: 4
                            }

                            contentItem: Label {
                                text: parent.text
                                color: "#ffffff"
                                font.pixelSize: 14
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
            }
        }
    }

    // 发送Agent消息函数
    function sendAgentMessage() {
        var message = agentInputField.text.trim()
        if (message.length > 0) {
            // 添加用户消息
            agentListModel.append({
                role: "user",
                message: message
            })

            // 清空输入框
            agentInputField.text = ""

            // TODO: 这里应该调用后端Agent处理
            // 现在先添加一个模拟的Agent回复
            Qt.callLater(function() {
                agentListModel.append({
                    role: "agent",
                    message: "收到消息: " + message + "\n(Agent功能待实现)"
                })
            })
        }
    }
}
