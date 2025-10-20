#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>

/**
 * @brief 配置管理类
 * 负责读取和保存应用程序配置
 */
class ConfigManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int maxLogLines READ maxLogLines WRITE setMaxLogLines NOTIFY maxLogLinesChanged)
    Q_PROPERTY(QString lastDeviceAddress READ lastDeviceAddress WRITE setLastDeviceAddress NOTIFY lastDeviceAddressChanged)
    Q_PROPERTY(int videoAspectRatio READ videoAspectRatio WRITE setVideoAspectRatio NOTIFY videoAspectRatioChanged)
    Q_PROPERTY(QStringList networkAddressHistory READ networkAddressHistory NOTIFY networkAddressHistoryChanged)

public:
    enum AspectRatio {
        Ratio_16_9 = 0,
        Ratio_4_3 = 1
    };
    Q_ENUM(AspectRatio)

    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    int maxLogLines() const { return m_maxLogLines; }
    QString lastDeviceAddress() const { return m_lastDeviceAddress; }
    int videoAspectRatio() const { return m_videoAspectRatio; }
    QStringList networkAddressHistory() const { return m_networkAddressHistory; }

    void setMaxLogLines(int lines);
    void setLastDeviceAddress(const QString &address);
    void setVideoAspectRatio(int ratio);

public slots:
    void loadConfig();
    void saveConfig();
    QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant());
    void setValue(const QString &key, const QVariant &value);
    void addNetworkAddress(const QString &address);
    void removeNetworkAddress(const QString &address);

signals:
    void maxLogLinesChanged();
    void lastDeviceAddressChanged();
    void videoAspectRatioChanged();
    void networkAddressHistoryChanged();
    void configLoaded();
    void configSaved();

private:
    int m_maxLogLines;
    QString m_lastDeviceAddress;
    int m_videoAspectRatio;
    QStringList m_networkAddressHistory;

    QString getConfigFilePath() const;
};

#endif // CONFIGMANAGER_H
