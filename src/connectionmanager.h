#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @brief 连接管理类
 * 负责与机器人/无人机建立和维护通信连接
 */
class ConnectionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString deviceAddress READ deviceAddress WRITE setDeviceAddress NOTIFY deviceAddressChanged)
    Q_PROPERTY(int connectionType READ connectionType WRITE setConnectionType NOTIFY connectionTypeChanged)
    Q_PROPERTY(QStringList availableCameras READ availableCameras NOTIFY availableCamerasChanged)

public:
    enum ConnectionType {
        NetworkCamera = 0,
        NetworkVTX = 1,
        LocalCamera = 2,
        UVCCamera = 3,
        USBVTX = 4
    };
    Q_ENUM(ConnectionType)

    enum NetworkProtocol {
        RTP_RTSP = 0,
        RTMP = 1
    };
    Q_ENUM(NetworkProtocol)

    explicit ConnectionManager(QObject *parent = nullptr);
    ~ConnectionManager();

    bool isConnected() const { return m_isConnected; }
    QString deviceAddress() const { return m_deviceAddress; }
    int connectionType() const { return m_connectionType; }
    QStringList availableCameras() const { return m_availableCameras; }

    void setDeviceAddress(const QString &address);
    void setConnectionType(int type);

public slots:
    void connectToDevice();
    void disconnectFromDevice();
    void sendCommand(const QString &command);
    void refreshCameraList();

signals:
    void isConnectedChanged();
    void deviceAddressChanged();
    void connectionTypeChanged();
    void availableCamerasChanged();
    void dataReceived(const QString &data);
    void errorOccurred(const QString &error);
    void connectionStatusChanged(const QString &status);

private:
    bool m_isConnected;
    QString m_deviceAddress;
    int m_connectionType;
    QStringList m_availableCameras;

    void processReceivedData(const QByteArray &data);
    void enumerateCameras();
};

#endif // CONNECTIONMANAGER_H
