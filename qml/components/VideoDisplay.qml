import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root

    property real aspectRatio: 16/9
    property bool isPlaying: false

    color: "#000000"

    // 视频显示区域
    Rectangle {
        id: videoContainer
        anchors.centerIn: parent
        width: {
            var targetWidth = parent.width
            var targetHeight = parent.height
            var containerRatio = targetWidth / targetHeight

            if (containerRatio > root.aspectRatio) {
                // 容器更宽，按高度计算
                return targetHeight * root.aspectRatio
            } else {
                // 容器更高，按宽度计算
                return targetWidth
            }
        }
        height: width / root.aspectRatio

        color: "#1a1a1a"
        border.color: "#333333"
        border.width: 2

        // 占位符内容
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20
            visible: !root.isPlaying

            Image {
                source: "qrc:/resources/icons/video-placeholder-large.png"
                Layout.alignment: Qt.AlignHCenter
                sourceSize.width: 384
                sourceSize.height: 384
                smooth: true
            }

            Label {
                text: "视频显示区域"
                font.pixelSize: 24
                color: "#666666"
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                text: root.aspectRatio === 16/9 ? "16:9" : "4:3"
                font.pixelSize: 16
                color: "#888888"
                Layout.alignment: Qt.AlignHCenter
            }

            Label {
                text: "请连接设备以查看视频流"
                font.pixelSize: 14
                color: "#888888"
                Layout.alignment: Qt.AlignHCenter
            }
        }

        // TODO: 实际的视频渲染区域
        // 可以使用 VideoOutput (Qt Multimedia) 或自定义 QQuickItem
        // VideoOutput {
        //     id: videoOutput
        //     anchors.fill: parent
        //     visible: root.isPlaying
        // }

        // 视频信息叠加层
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 10
            width: infoColumn.width + 20
            height: infoColumn.height + 20
            color: "#80000000"
            radius: 5
            visible: root.isPlaying

            Column {
                id: infoColumn
                anchors.centerIn: parent
                spacing: 5

                Label {
                    text: "分辨率: " + videoContainer.width + "x" + videoContainer.height
                    color: "#ffffff"
                    font.pixelSize: 12
                }

                Label {
                    text: "比例: " + (root.aspectRatio === 16/9 ? "16:9" : "4:3")
                    color: "#ffffff"
                    font.pixelSize: 12
                }
            }
        }
    }

    // 尺寸信息（调试用）
    Label {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        text: "容器: " + Math.round(root.width) + "x" + Math.round(root.height)
        color: "#666666"
        font.pixelSize: 10
    }
}
