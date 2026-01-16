#include "network_service.h"

#include "config.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QQmlEngine>
#include <QTimer>
#include <QUuid>
#include <QWebSocket>

namespace ShirohaChat
{

    NetworkService& NetworkService::instance()
    {
        static NetworkService instance{nullptr};
        return instance;
    }

    NetworkService* NetworkService::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
    {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine)

        auto* singleton = &NetworkService::instance();
        QQmlEngine::setObjectOwnership(singleton, QQmlEngine::CppOwnership);
        return singleton;
    }

    NetworkService::NetworkService(QObject* parent)
        : QObject(parent),
          m_webSocket(std::make_unique<QWebSocket>()),
          m_heartbeatTimer(new QTimer(this)),
          m_reconnectTimer(new QTimer(this))
    {
        connect(m_webSocket.get(), &QWebSocket::connected, this, [this]() {
            m_connected = true;
            m_reconnectAttempts = 0;
            m_reconnectTimer->stop();
            m_heartbeatTimer->start(Config::Network::HEARTBEAT_INTERVAL);
        });

        connect(m_webSocket.get(), &QWebSocket::disconnected, this, [this]() {
            m_connected = false;
            m_heartbeatTimer->stop();
            emit connectionLost();
            scheduleReconnect();
        });

        connect(m_webSocket.get(), &QWebSocket::textMessageReceived, this,
                [this](const QString& rawData) { decodePacket(rawData); });

        m_heartbeatTimer->setInterval(Config::Network::HEARTBEAT_INTERVAL);
        QObject::connect(m_heartbeatTimer, &QTimer::timeout, this, [this]() {
            if (m_connected) {
                const auto packet = encodePacket("heartbeat", QUuid::createUuid().toString(), QJsonObject{});
                m_webSocket->sendTextMessage(packet);
            }
        });

        m_reconnectTimer->setSingleShot(true);
        QObject::connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
            if (!m_connected && m_reconnectAttempts < Config::Network::MAX_RECONNECT_ATTEMPTS) {
                m_webSocket->open(m_serverUrl);
                m_reconnectAttempts++;
            }
        });
    }

    NetworkService::~NetworkService() = default;

    void NetworkService::connectToServer(const QUrl& serverUrl)
    {
        m_serverUrl = serverUrl;

        if (serverUrl.scheme() == "mock") {
            m_connected = true;
            return;
        }

        m_webSocket->open(serverUrl);
    }

    void NetworkService::disconnectFromServer()
    {
        m_webSocket->close();
    }

    bool NetworkService::isConnected() const
    {
        return m_connected;
    }

    void NetworkService::push(const Message& msg)
    {
        if (!m_connected)
            return;

        m_webSocket->sendTextMessage(msg.content());
    }

    bool NetworkService::sendText(const QString& msgId, const QString& from, const QString& to, const QString& content)
    {
        if (!m_connected)
            return false;

        const auto trimmedContent = content.trimmed();
        if (msgId.isEmpty() || from.isEmpty() || to.isEmpty() || trimmedContent.isEmpty())
            return false;

        if (m_serverUrl.scheme() == "mock") {
            QTimer::singleShot(Config::Network::MOCK_ACK_DELAY, this, [this, msgId]() { emit ackReceived(msgId); });
            return true;
        }

        QJsonObject payload{
            {"sessionId", to},
            {"content", trimmedContent},
            {"senderUserId", from},
            {"senderNickname", from},
        };

        const auto packet = encodePacket("send_message", msgId, payload);
        m_webSocket->sendTextMessage(packet);
        return true;
    }

    QString NetworkService::encodePacket(const QString& cmd, const QString& msgId, const QJsonObject& payload)
    {
        QJsonObject packet{
            {"cmd", cmd},
            {"msgId", msgId},
            {"timestamp", QDateTime::currentDateTimeUtc().toString(Qt::ISODate)},
            {"payload", payload},
        };

        return QString::fromUtf8(QJsonDocument(packet).toJson(QJsonDocument::Compact));
    }

    void NetworkService::decodePacket(const QString& rawData)
    {
        emit messageArrived(rawData);

        QJsonParseError error{};
        const auto doc = QJsonDocument::fromJson(rawData.toUtf8(), &error);
        if (error.error != QJsonParseError::NoError || !doc.isObject())
            return;

        const auto obj = doc.object();
        const auto cmd = obj.value("cmd").toString();

        if (cmd == "send_message_ack") {
            const auto msgId = obj.value("msgId").toString();
            const auto code = obj.value("code").toInt();

            if (!msgId.isEmpty() && code == 200) {
                emit ackReceived(msgId);
            }
        }
        else if (cmd == "receive_message") {
            const auto payload = obj.value("payload").toObject();
            const auto from = payload.value("senderUserId").toString();
            const auto sessionId = payload.value("sessionId").toString();
            const auto content = payload.value("content").toString();
            const auto ts = obj.value("timestamp").toString();

            const auto effectiveSessionId = sessionId.isEmpty() ? from : sessionId;
            if (!effectiveSessionId.isEmpty() && !from.isEmpty() && !content.isEmpty()) {
                emit textReceived(effectiveSessionId, from, content, ts);
            }
        }
    }

    void NetworkService::scheduleReconnect()
    {
        if (m_reconnectAttempts >= Config::Network::MAX_RECONNECT_ATTEMPTS) {
            // 达到最大重连次数，放弃重连
            return;
        }

        // 指数退避
        const int delay = Config::Network::RECONNECT_INITIAL_DELAY * (1 << m_reconnectAttempts);
        m_reconnectTimer->start(delay);
    }

} // namespace ShirohaChat
