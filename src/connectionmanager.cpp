#include "connectionmanager.h"
#include <QDebug>
#include <QMediaDevices>
#include <QCameraDevice>

ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent)
    , m_isConnected(false)
    , m_connectionType(NetworkCamera)
    , m_connectionStatus("未连接")
{
    qDebug() << "ConnectionManager initialized";
    enumerateCameras();
}

ConnectionManager::~ConnectionManager()
{
    disconnectFromDevice();
}

QString ConnectionManager::connectionTime() const
{
    if (!m_isConnected || !m_connectionStartTime.isValid()) {
        return "--:--:--";
    }

    qint64 seconds = m_connectionStartTime.secsTo(QDateTime::currentDateTime());
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(secs, 2, 10, QChar('0'));
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
        emit errorOccurred("设备地址为空");
        return;
    }

    emit connectionStatusChanged(QString("正在连接到 %1...").arg(m_deviceAddress));

    // 根据连接类型验证和处理地址
    ConnectionType type = static_cast<ConnectionType>(m_connectionType);

    switch (type) {
        case NetworkCamera: {
            // 验证RTSP/RTMP URL格式
            QString addr = m_deviceAddress.toLower();
            if (!addr.startsWith("rtsp://") && !addr.startsWith("rtmp://") &&
                !addr.startsWith("rtp://") && !addr.startsWith("http://") &&
                !addr.startsWith("https://")) {
                emit errorOccurred("网络相机地址格式错误，应以 rtsp://, rtmp://, rtp:// 或 http:// 开头");
                return;
            }
            qDebug() << "Connecting to network camera:" << m_deviceAddress;
            break;
        }

        case NetworkVTX: {
            // VTX协议暂不支持
            emit errorOccurred("网络VTX协议暂不支持，请使用其他连接方式");
            emit connectionStatusChanged("连接失败：VTX协议暂不支持");
            return;
        }

        case LocalCamera:
        case UVCCamera: {
            // 验证相机是否在可用列表中
            if (m_availableCameras.isEmpty() || m_availableCameras.first() == "未检测到摄像头") {
                emit errorOccurred("未检测到可用的摄像头设备");
                return;
            }

            // 将Qt相机设备名称转换为FFmpeg格式
            // 在macOS上使用avfoundation，格式为 "avfoundation:0" 或设备名称
            const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
            int cameraIndex = -1;

            for (int i = 0; i < cameras.size(); ++i) {
                if (cameras[i].description() == m_deviceAddress) {
                    cameraIndex = i;
                    break;
                }
            }

            if (cameraIndex >= 0) {
                // 在macOS上，使用avfoundation格式
                #ifdef Q_OS_MACOS
                m_deviceAddress = QString("avfoundation:%1").arg(cameraIndex);
                #elif defined(Q_OS_LINUX)
                m_deviceAddress = QString("/dev/video%1").arg(cameraIndex);
                #elif defined(Q_OS_WIN)
                m_deviceAddress = QString("video=%1").arg(cameras[cameraIndex].description());
                #endif

                qDebug() << "Mapped camera to:" << m_deviceAddress;
            } else {
                emit errorOccurred("未找到指定的摄像头设备");
                return;
            }
            break;
        }

        case USBVTX: {
            // USB VTX协议暂不支持
            emit errorOccurred("USB VTX协议暂不支持，请使用其他连接方式");
            emit connectionStatusChanged("连接失败：USB VTX协议暂不支持");
            return;
        }

        default:
            emit errorOccurred("未知的连接类型");
            return;
    }

    // 设置为已连接状态
    // 实际的视频流连接由VideoHandler通过VideoDecoder处理
    m_isConnected = true;
    m_connectionStartTime = QDateTime::currentDateTime();
    m_connectionStatus = QString("已连接");

    emit isConnectedChanged();
    emit connectionTimeChanged();
    emit connectionStatusTextChanged();
    emit connectionStatusChanged(QString("已连接到 %1").arg(m_deviceAddress));
    qDebug() << "Connected to device:" << m_deviceAddress;
}

void ConnectionManager::disconnectFromDevice()
{
    if (!m_isConnected) {
        return;
    }

    emit connectionStatusChanged("正在断开连接...");

    // 实际的视频流断开由VideoHandler/VideoDecoder处理
    // ConnectionManager只负责管理连接状态

    QString deviceAddr = m_deviceAddress;
    m_isConnected = false;
    m_connectionStartTime = QDateTime();
    m_connectionStatus = "未连接";

    emit isConnectedChanged();
    emit connectionTimeChanged();
    emit connectionStatusTextChanged();
    emit connectionStatusChanged(QString("已从 %1 断开连接").arg(deviceAddr));
    qDebug() << "Disconnected from device:" << deviceAddr;
}

void ConnectionManager::sendCommand(const QString &command)
{
    if (!m_isConnected) {
        emit errorOccurred("未连接到设备，无法发送命令");
        return;
    }

    // 命令发送功能暂不实现
    // 主要用于未来扩展，例如发送控制命令到无人机/机器人
    qDebug() << "Sending command (not implemented):" << command;
    emit errorOccurred("命令发送功能暂不支持");
}

void ConnectionManager::processReceivedData(const QByteArray &data)
{
    // 数据接收处理功能暂不实现
    // 主要用于未来扩展，例如接收遥测数据

    if (data.isEmpty()) {
        return;
    }

    QString dataStr = QString::fromUtf8(data);
    qDebug() << "Received data (raw):" << dataStr;
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
