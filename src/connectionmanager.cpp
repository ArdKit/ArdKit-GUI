#include "connectionmanager.h"
#include <QDebug>
#include <QMediaDevices>
#include <QCameraDevice>

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
    , m_connectionType(NetworkCamera)
{
    qDebug() << "ConnectionManager initialized";
    enumerateCameras();
}

ConnectionManager::~ConnectionManager()
{
    disconnectFromDevice();
}

void ConnectionManager::setDeviceAddress(const QString &address)
{
    if (m_deviceAddress != address) {
        m_deviceAddress = address;
        emit deviceAddressChanged();
        qDebug() << "Device address set to:" << address;
    }
}

void ConnectionManager::setConnectionType(int type)
{
    if (m_connectionType != type) {
        m_connectionType = type;
        emit connectionTypeChanged();

        QString typeName;
        switch (static_cast<ConnectionType>(type)) {
            case NetworkCamera: typeName = "NetworkCamera"; break;
            case NetworkVTX: typeName = "NetworkVTX"; break;
            case LocalCamera: typeName = "LocalCamera"; break;
            case UVCCamera: typeName = "UVCCamera"; break;
            case USBVTX: typeName = "USBVTX"; break;
        }
        qDebug() << "Connection type set to:" << typeName;
    }
}

void ConnectionManager::connectToDevice()
{
    if (m_isConnected) {
        qDebug() << "Already connected";
        return;
    }

    if (m_deviceAddress.isEmpty()) {
        emit errorOccurred("Device address is empty");
        return;
    }

    // TODO: 实现连接逻辑
    // 根据 connectionType 选择不同的连接方式
    // NetworkCamera: 建立网络相机连接 (RTP/RTSP, RTMP)
    // NetworkVTX: 建立网络VTX连接 (VTX协议)
    // LocalCamera: 打开本地系统摄像头
    // UVCCamera: 打开标准 UVC USB 摄像头
    // USBVTX: 使用 USB VTX 协议连接设备

    emit connectionStatusChanged(QString("正在连接到 %1...").arg(m_deviceAddress));

    // 模拟连接
    m_isConnected = true;
    emit isConnectedChanged();
    emit connectionStatusChanged(QString("已连接到 %1").arg(m_deviceAddress));
    qDebug() << "Connected to device:" << m_deviceAddress;
}

void ConnectionManager::disconnectFromDevice()
{
    if (!m_isConnected) {
        return;
    }

    // TODO: 实现断开连接逻辑
    // 关闭网络连接或串口

    m_isConnected = false;
    emit isConnectedChanged();
    emit connectionStatusChanged("已断开连接");
    qDebug() << "Disconnected from device";
}

void ConnectionManager::sendCommand(const QString &command)
{
    if (!m_isConnected) {
        emit errorOccurred("Not connected to device");
        return;
    }

    // TODO: 实现命令发送逻辑
    // 将命令转换为字节流并发送

    qDebug() << "Sending command:" << command;
}

void ConnectionManager::processReceivedData(const QByteArray &data)
{
    // TODO: 解析接收到的数据
    // 发送 dataReceived 信号

    QString dataStr = QString::fromUtf8(data);
    emit dataReceived(dataStr);
}

void ConnectionManager::refreshCameraList()
{
    enumerateCameras();
}

void ConnectionManager::enumerateCameras()
{
    m_availableCameras.clear();

    // 获取所有可用的摄像头设备
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();

    for (const QCameraDevice &cameraDevice : cameras) {
        QString cameraInfo = cameraDevice.description();
        if (!cameraInfo.isEmpty()) {
            m_availableCameras.append(cameraInfo);
            qDebug() << "Found camera:" << cameraInfo;
        }
    }

    if (m_availableCameras.isEmpty()) {
        m_availableCameras.append("未检测到摄像头");
        qDebug() << "No cameras found";
    }

    emit availableCamerasChanged();
}
