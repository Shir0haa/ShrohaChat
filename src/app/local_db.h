#pragma once

#include "domain/message.h"

#include <QDateTime>
#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QString>
#include <QtQml/qqml.h>

namespace ShirohaChat
{

/**
 * @brief 本地 SQLite 数据库访问单例
 */
class LocalDB : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

  public:
    static LocalDB& instance();
    static LocalDB* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

    LocalDB(const LocalDB&) = delete;
    LocalDB& operator=(const LocalDB&) = delete;

    Q_INVOKABLE bool open(const QString& databasePath);
    Q_INVOKABLE void close();
    Q_INVOKABLE [[nodiscard]] bool isOpen() const;

    bool insertMessage(const Message& msg);
    bool updateMessageStatus(const QString& msgId, MessageStatus status);
    bool upsertPendingAck(const QString& msgId);
    bool removePendingAck(const QString& msgId);
    QList<Message> fetchPendingMessages();

    struct SessionLastMessage
    {
        QString sessionId;
        QString lastMessage;
        QDateTime lastMessageAt;
    };

    QList<SessionLastMessage> queryLastMessagePerSession(int limit = 100, int offset = 0);

  private:
    explicit LocalDB(QObject* parent);
    ~LocalDB() override;

    bool ensureSchema();

    QString m_connectionName;
    QSqlDatabase m_db;
};
} // namespace ShirohaChat
