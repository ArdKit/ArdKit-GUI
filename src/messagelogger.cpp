#include "messagelogger.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <QDebug>

MessageLogger::MessageLogger(QObject *parent)
    : QObject(parent)
    , m_maxLines(1000)
{
    qDebug() << "MessageLogger initialized";
}

MessageLogger::~MessageLogger()
{
}

void MessageLogger::setMaxLines(int lines)
{
    if (m_maxLines != lines && lines > 0) {
        m_maxLines = lines;
        emit maxLinesChanged();
        trimMessages();
        qDebug() << "Max log lines set to:" << lines;
    }
}

void MessageLogger::addMessage(const QString &message)
{
    QMutexLocker locker(&m_mutex);

    QString timestampedMessage = QString("[%1] %2")
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
        .arg(message);

    m_messages.append(timestampedMessage);
    trimMessages();

    emit messagesChanged();
    emit messageCountChanged();
    emit newMessageAdded(timestampedMessage);
}

void MessageLogger::addInfoMessage(const QString &message)
{
    addMessage(formatMessage("INFO", message));
}

void MessageLogger::addWarningMessage(const QString &message)
{
    addMessage(formatMessage("WARN", message));
}

void MessageLogger::addErrorMessage(const QString &message)
{
    addMessage(formatMessage("ERROR", message));
}

void MessageLogger::clearMessages()
{
    QMutexLocker locker(&m_mutex);

    m_messages.clear();
    emit messagesChanged();
    emit messageCountChanged();

    qDebug() << "Messages cleared";
}

void MessageLogger::saveToFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return;
    }

    QTextStream out(&file);
    QMutexLocker locker(&m_mutex);

    for (const QString &message : m_messages) {
        out << message << "\n";
    }

    file.close();
    qDebug() << "Messages saved to:" << filePath;
}

void MessageLogger::trimMessages()
{
    // 在 mutex 已经锁定的情况下调用此函数
    while (m_messages.size() > m_maxLines) {
        m_messages.removeFirst();
    }
}

QString MessageLogger::formatMessage(const QString &prefix, const QString &message)
{
    return QString("[%1] %2").arg(prefix, message);
}
