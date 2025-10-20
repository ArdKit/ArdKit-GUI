#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMutex>

/**
 * @brief 消息日志类
 * 负责管理和显示机器人/无人机的输出信息
 */
class MessageLogger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList messages READ messages NOTIFY messagesChanged)
    Q_PROPERTY(int maxLines READ maxLines WRITE setMaxLines NOTIFY maxLinesChanged)
    Q_PROPERTY(int messageCount READ messageCount NOTIFY messageCountChanged)

public:
    explicit MessageLogger(QObject *parent = nullptr);
    ~MessageLogger();

    QStringList messages() const { return m_messages; }
    int maxLines() const { return m_maxLines; }
    int messageCount() const { return m_messages.size(); }

    void setMaxLines(int lines);

public slots:
    void addMessage(const QString &message);
    void addInfoMessage(const QString &message);
    void addWarningMessage(const QString &message);
    void addErrorMessage(const QString &message);
    void clearMessages();
    void saveToFile(const QString &filePath);

signals:
    void messagesChanged();
    void maxLinesChanged();
    void messageCountChanged();
    void newMessageAdded(const QString &message);

private:
    QStringList m_messages;
    int m_maxLines;
    QMutex m_mutex;

    void trimMessages();
    QString formatMessage(const QString &prefix, const QString &message);
};

#endif // MESSAGELOGGER_H
