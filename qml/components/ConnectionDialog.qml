import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Window {
    id: root

    width: 550
    height: 350
    minimumWidth: 500
    minimumHeight: 300
    modality: Qt.ApplicationModal
    flags: Qt.Dialog | Qt.FramelessWindowHint
    title: "连接到设备"

    // 从外部传入的管理器对象
    property var connMgr: null
    property var cfgMgr: null

    signal accepted()
    signal rejected()

    // 自定义标题栏（可拖动）
    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        color: "#2d2d2d"
        border.color: "#1a1a1a"
        border.width: 1

        MouseArea {
            id: titleBarMouseArea
            anchors.fill: parent
            property point clickPos: Qt.point(0, 0)

            onPressed: {
                clickPos = Qt.point(mouse.x, mouse.y)
            }

            onPositionChanged: {
                if (pressed) {
                    var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                    root.x += delta.x
                    root.y += delta.y
                }
            }
        }

        Label {
            anchors.centerIn: parent
            text: root.title
            font.pixelSize: 14
            font.bold: true
            color: "#ffffff"
        }

        // 关闭按钮
        Button {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 8
            width: 30
            height: 30
            text: "×"
            font.pixelSize: 18

            background: Rectangle {
                color: parent.hovered ? "#ff4444" : "transparent"
                radius: 4
            }

            contentItem: Label {
                text: parent.text
                color: parent.hovered ? "#ffffff" : "#cccccc"
                font.pixelSize: 18
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: {
                root.rejected()
                root.close()
            }
        }
    }

    // 主内容区域
    ColumnLayout {
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: buttonRow.top
        anchors.margins: 15
        spacing: 15

        // 连接类型选择
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Label {
                text: "连接类型:"
                Layout.preferredWidth: 80
            }

            ComboBox {
                id: connectionTypeCombo
                Layout.fillWidth: true
                model: ["网络相机", "网络VTX", "本地相机", "UVC相机", "USB VTX"]
                currentIndex: connMgr ? connMgr.connectionType : 0

                onCurrentIndexChanged: {
                    // 本地相机、UVC相机需要刷新摄像头列表
                    if (connMgr && (currentIndex === 2 || currentIndex === 3)) {
                        connMgr.refreshCameraList()
                    }
                }
            }
        }

        // 网络相机配置区域
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: connectionTypeCombo.currentIndex === 0

            // 网络协议选择
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Label {
                    text: "网络协议:"
                    Layout.preferredWidth: 80
                }

                ComboBox {
                    id: networkProtocolCombo
                    Layout.fillWidth: true
                    model: ["RTP/RTSP", "RTMP"]
                }
            }

            // 设备地址输入
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Label {
                    text: "设备地址:"
                    Layout.preferredWidth: 80
                }

                ComboBox {
                    id: deviceAddressCombo
                    Layout.fillWidth: true
                    editable: true
                    model: cfgMgr ? cfgMgr.networkAddressHistory : []
                    editText: cfgMgr ? (cfgMgr.lastDeviceAddress || "rtsp://192.168.1.100:8554/stream") : ""

                    property string placeholderText: networkProtocolCombo.currentIndex === 0 ?
                        "例如: rtsp://192.168.1.100:8554/stream" :
                        "例如: rtmp://192.168.1.100/live/stream"
                }
            }

            Label {
                text: deviceAddressCombo.placeholderText
                font.pixelSize: 11
                color: "#888"
                Layout.leftMargin: 90
            }
        }

        // 网络VTX配置区域
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: connectionTypeCombo.currentIndex === 1

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Label {
                    text: "VTX地址:"
                    Layout.preferredWidth: 80
                }

                ComboBox {
                    id: vtxAddressCombo
                    Layout.fillWidth: true
                    editable: true
                    model: cfgMgr ? cfgMgr.networkAddressHistory : []
                    editText: "vtx://192.168.1.100:5600"
                }
            }

            Label {
                text: "例如: vtx://192.168.1.100:5600"
                font.pixelSize: 11
                color: "#888"
                Layout.leftMargin: 90
            }
        }

        // 本地相机配置区域
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: connectionTypeCombo.currentIndex === 2

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Label {
                    text: "选择相机:"
                    Layout.preferredWidth: 80
                }

                ComboBox {
                    id: localCameraCombo
                    Layout.fillWidth: true
                    model: connMgr ? connMgr.availableCameras : []
                }

                Button {
                    text: "刷新"
                    onClicked: {
                        if (connMgr) {
                            connMgr.refreshCameraList()
                        }
                    }
                }
            }
        }

        // UVC 摄像头配置区域
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: connectionTypeCombo.currentIndex === 3

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Label {
                    text: "UVC设备:"
                    Layout.preferredWidth: 80
                }

                ComboBox {
                    id: uvcCameraCombo
                    Layout.fillWidth: true
                    model: connMgr ? connMgr.availableCameras : []
                }

                Button {
                    text: "刷新"
                    onClicked: {
                        if (connMgr) {
                            connMgr.refreshCameraList()
                        }
                    }
                }
            }
        }

        // USB VTX配置区域
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 10
            visible: connectionTypeCombo.currentIndex === 4

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Label {
                    text: "设备地址:"
                    Layout.preferredWidth: 80
                }

                TextField {
                    id: usbVtxAddressField
                    Layout.fillWidth: true
                    placeholderText: "例如: /dev/ttyUSB0 或设备路径"
                }
            }

            Label {
                text: "输入 USB VTX 设备路径或标识符"
                font.pixelSize: 11
                color: "#888"
                Layout.leftMargin: 90
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }

    // 底部按钮区域
    RowLayout {
        id: buttonRow
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 15
        spacing: 10

        Button {
            text: "确定"
            Layout.preferredWidth: 80

            onClicked: {
                if (!connMgr || !cfgMgr) {
                    return
                }

                if (connectionTypeCombo.currentIndex === 0) {
                    // 网络相机
                    var address = deviceAddressCombo.editText
                    if (address) {
                        connMgr.deviceAddress = address
                        connMgr.connectionType = 0
                        cfgMgr.lastDeviceAddress = address
                        cfgMgr.addNetworkAddress(address)
                        connMgr.connectToDevice()
                    }
                } else if (connectionTypeCombo.currentIndex === 1) {
                    // 网络VTX
                    var vtxAddress = vtxAddressCombo.editText
                    if (vtxAddress) {
                        connMgr.deviceAddress = vtxAddress
                        connMgr.connectionType = 1
                        cfgMgr.addNetworkAddress(vtxAddress)
                        connMgr.connectToDevice()
                    }
                } else if (connectionTypeCombo.currentIndex === 2) {
                    // 本地相机
                    if (localCameraCombo.currentIndex >= 0) {
                        connMgr.deviceAddress = localCameraCombo.currentText
                        connMgr.connectionType = 2
                        connMgr.connectToDevice()
                    }
                } else if (connectionTypeCombo.currentIndex === 3) {
                    // UVC 相机
                    if (uvcCameraCombo.currentIndex >= 0) {
                        connMgr.deviceAddress = uvcCameraCombo.currentText
                        connMgr.connectionType = 3
                        connMgr.connectToDevice()
                    }
                } else if (connectionTypeCombo.currentIndex === 4) {
                    // USB VTX
                    var usbAddress = usbVtxAddressField.text
                    if (usbAddress) {
                        connMgr.deviceAddress = usbAddress
                        connMgr.connectionType = 4
                        connMgr.connectToDevice()
                    }
                }

                root.accepted()
                root.close()
            }
        }

        Button {
            text: "取消"
            Layout.preferredWidth: 80

            onClicked: {
                root.rejected()
                root.close()
            }
        }
    }

    onVisibleChanged: {
        if (visible && connMgr) {
            // 对话框打开时刷新摄像头列表
            if (connectionTypeCombo.currentIndex === 2 || connectionTypeCombo.currentIndex === 3) {
                connMgr.refreshCameraList()
            }
        }
    }
}
