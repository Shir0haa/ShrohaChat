#pragma once

#include "domain/message.h"

#include <QJSEngine>
#include <QObject>
#include <QQmlEngine>
#include <QtQml/qqml.h>
#include <QUrl>
#include <QWebSocket>

#include <memory>

class QTimer;

namespace ShirohaChat
{

    /**
     * @brief WebSocket 网络服务单例
     */
    class NetworkService : public QObject
    {
        Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

  public:
    static NetworkService& instance();
    static NetworkService* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

    NetworkService(const NetworkService&) = delete;
    NetworkService& operator=(const NetworkService&) = delete;

    Q_INVOKABLE void connectToServer(const QUrl& serverUrl);
    Q_INVOKABLE void disconnectFromServer();
    Q_INVOKABLE [[nodiscard]] bool isConnected() const;

    void push(const Message& msg);
    bool sendText(const QString& msgId, const QString& from, const QString& to, const QString& content);

  signals:
    void ackReceived(const QString& msgId);
    void messageArrived(const QString& rawData);
    void textReceived(const QString& sessionId, const QString& from, const QString& content, const QString& ts);
    void connectionLost();

  private:
    explicit NetworkService(QObject* parent);
    ~NetworkService() override;

    QString encodePacket(const QString& cmd, const QString& msgId, const QJsonObject& payload);
    void decodePacket(const QString& rawData);
    void scheduleReconnect();

    QUrl m_serverUrl;
    bool m_connected{false};
    std::unique_ptr<QWebSocket> m_webSocket;
    QTimer* m_heartbeatTimer{nullptr};
    QTimer* m_reconnectTimer{nullptr};
    int m_reconnectAttempts{0};
};

} // namespace ShirohaChat
