import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ArdKitGUI 1.0

Rectangle {
    id: root

    property real aspectRatio: 16/9
    property bool isPlaying: false
    property bool showSizeInfo: false
    property bool isPaused: false

    color: "#000000"

    // 组件完成后将渲染器设置给 VideoHandler
    Component.onCompleted: {
        if (videoHandler) {
            videoHandler.setRenderer(videoRenderer)
        }
    }

    // 监听尺寸变化
    onWidthChanged: {
        showSizeInfo = true
        sizeInfoTimer.restart()
    }

    onHeightChanged: {
        showSizeInfo = true
        sizeInfoTimer.restart()
    }

    // 定时器：尺寸变化后2秒隐藏尺寸信息
    Timer {
        id: sizeInfoTimer
        interval: 2000
        running: false
        repeat: false
        onTriggered: {
            root.showSizeInfo = false
        }
    }

    // 背景图片（视频未播放时显示）
    Image {
        anchors.fill: parent
        source: "qrc:/resources/images/bk.png"
        fillMode: Image.PreserveAspectFit
        visible: !root.isPlaying
        smooth: true
        opacity: 0.4
    }

    // 视频渲染器（实际显示视频）
    VideoRenderer {
        id: videoRenderer
        anchors.fill: parent
        visible: root.isPlaying
        z: 1
    }

    // 占位符文字（视频未播放时显示）
    Label {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        text: "请连接到设备查看视频"
        font.pixelSize: 14
        color: "#888888"
        visible: !root.isPlaying
    }

    // 视频信息叠加层（左上角）
    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        width: infoColumn.width + 20
        height: infoColumn.height + 20
        color: "#80000000"
        radius: 5
        visible: root.isPlaying && videoHandler

        Column {
            id: infoColumn
            anchors.centerIn: parent
            spacing: 5

            Label {
                text: videoHandler ? ("分辨率: " + videoHandler.videoSize.width + "x" + videoHandler.videoSize.height) : ""
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


    // 尺寸信息（窗口大小调整时显示，右下角临时显示）
    Rectangle {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: root.isPlaying ? 60 : 10
        width: sizeLabel.width + 20
        height: sizeLabel.height + 10
        color: "#80000000"
        radius: 5
        visible: root.showSizeInfo

        Label {
            id: sizeLabel
            anchors.centerIn: parent
            text: "容器: " + Math.round(root.width) + "x" + Math.round(root.height)
            color: "#ffffff"
            font.pixelSize: 12
        }
    }

}
