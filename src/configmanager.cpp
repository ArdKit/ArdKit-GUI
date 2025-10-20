#include "configmanager.h"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_maxLogLines(1000)
    , m_videoAspectRatio(Ratio_16_9)
{
    qDebug() << "ConfigManager initialized";
    loadConfig();
}

ConfigManager::~ConfigManager()
{
    saveConfig();
}

void ConfigManager::setMaxLogLines(int lines)
{
    if (m_maxLogLines != lines && lines > 0) {
        m_maxLogLines = lines;
        emit maxLogLinesChanged();
        qDebug() << "Max log lines set to:" << lines;
    }
}

void ConfigManager::setLastDeviceAddress(const QString &address)
{
    if (m_lastDeviceAddress != address) {
        m_lastDeviceAddress = address;
        emit lastDeviceAddressChanged();
    }
}

void ConfigManager::setVideoAspectRatio(int ratio)
{
    if (m_videoAspectRatio != ratio) {
        m_videoAspectRatio = ratio;
        emit videoAspectRatioChanged();

        QString ratioName = (ratio == Ratio_16_9) ? "16:9" : "4:3";
        qDebug() << "Video aspect ratio set to:" << ratioName;
    }
}

void ConfigManager::loadConfig()
{
    QSettings settings("ArdKit", "ArdKit-GUI");

    m_maxLogLines = settings.value("maxLogLines", 1000).toInt();
    m_lastDeviceAddress = settings.value("lastDeviceAddress", "").toString();
    m_videoAspectRatio = settings.value("videoAspectRatio", Ratio_16_9).toInt();
    m_networkAddressHistory = settings.value("networkAddressHistory", QStringList()).toStringList();

    emit maxLogLinesChanged();
    emit lastDeviceAddressChanged();
    emit videoAspectRatioChanged();
    emit networkAddressHistoryChanged();
    emit configLoaded();

    qDebug() << "Configuration loaded";
}

void ConfigManager::saveConfig()
{
    QSettings settings("ArdKit", "ArdKit-GUI");

    settings.setValue("maxLogLines", m_maxLogLines);
    settings.setValue("lastDeviceAddress", m_lastDeviceAddress);
    settings.setValue("videoAspectRatio", m_videoAspectRatio);
    settings.setValue("networkAddressHistory", m_networkAddressHistory);

    settings.sync();
    emit configSaved();

    qDebug() << "Configuration saved";
}

QVariant ConfigManager::getValue(const QString &key, const QVariant &defaultValue)
{
    QSettings settings("ArdKit", "ArdKit-GUI");
    return settings.value(key, defaultValue);
}

void ConfigManager::setValue(const QString &key, const QVariant &value)
{
    QSettings settings("ArdKit", "ArdKit-GUI");
    settings.setValue(key, value);
}

QString ConfigManager::getConfigFilePath() const
{
    QSettings settings("ArdKit", "ArdKit-GUI");
    return settings.fileName();
}

void ConfigManager::addNetworkAddress(const QString &address)
{
    if (address.isEmpty()) {
        return;
    }

    // 移除已存在的相同地址（避免重复）
    m_networkAddressHistory.removeAll(address);

    // 将新地址插入到列表开头
    m_networkAddressHistory.prepend(address);

    // 限制历史记录数量为20条
    while (m_networkAddressHistory.size() > 20) {
        m_networkAddressHistory.removeLast();
    }

    emit networkAddressHistoryChanged();
    saveConfig();

    qDebug() << "Network address added to history:" << address;
}

void ConfigManager::removeNetworkAddress(const QString &address)
{
    if (m_networkAddressHistory.removeAll(address) > 0) {
        emit networkAddressHistoryChanged();
        saveConfig();
        qDebug() << "Network address removed from history:" << address;
    }
}
