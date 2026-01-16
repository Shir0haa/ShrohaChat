#include "chat_controller.h"

#include "config.h"
#include "models/message_list_model.h"
#include "models/session_list_model.h"

#include <QDateTime>

namespace ShirohaChat
{

    namespace
    {
        QString formatSessionTimeString(const QDateTime& timestampUtc)
        {
            if (!timestampUtc.isValid())
                return {};

            const auto local = timestampUtc.toLocalTime();
            const auto now = QDateTime::currentDateTime();

            if (local.date() == now.date())
                return local.time().toString(QStringLiteral("HH:mm"));

            return local.date().toString(QStringLiteral("MM-dd"));
        }
    } // namespace

    ChatController& ChatController::instance()
    {
        static ChatController instance{nullptr};
        return instance;
    }

    ChatController* ChatController::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
    {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine)

        auto* singleton = &ChatController::instance();
        QQmlEngine::setObjectOwnership(singleton, QQmlEngine::CppOwnership);
        return singleton;
    }

    ChatController::ChatController(QObject* parent)
        : QObject(parent),
          m_networkService(NetworkService::instance()),
          m_localDB(LocalDB::instance()),
          m_sessionModel(new SessionListModel(this)),
          m_currentMessageModel(new MessageListModel(this))
    {
        QObject::connect(&m_networkService, &NetworkService::ackReceived, this, &ChatController::onAckReceived);

        QObject::connect(
            &m_networkService, &NetworkService::textReceived, this,
            [this](const QString& sessionId, const QString& from, const QString& content, const QString& ts) {
                const auto effectiveSessionId = sessionId.isEmpty() ? from : sessionId;
                if (effectiveSessionId.isEmpty() || from.isEmpty() || content.isEmpty())
                    return;

                const bool isMine = (!m_selfUserId.isEmpty() && from == m_selfUserId);

                QDateTime timestamp = QDateTime::fromString(ts, Qt::ISODate);
                if (!timestamp.isValid())
                    timestamp = QDateTime::currentDateTimeUtc();

                Message msg(content, from, effectiveSessionId);
                msg.setTimestamp(timestamp);
                msg.setStatus(MessageStatus::Delivered);
                m_localDB.insertMessage(msg);

                auto* sessionModel = qobject_cast<SessionListModel*>(m_sessionModel);
                if (sessionModel != nullptr) {
                    SessionListModel::SessionItem item;
                    item.sessionId = effectiveSessionId;
                    item.displayName = effectiveSessionId;
                    item.lastMessage = content;
                    item.timeString = formatSessionTimeString(timestamp);
                    sessionModel->upsertSession(item, !isMine && effectiveSessionId != m_currentSessionId);
                }

                if (effectiveSessionId != m_currentSessionId)
                    return;

                auto* model = qobject_cast<MessageListModel*>(m_currentMessageModel);
                if (model == nullptr)
                    return;

                model->appendMessage(from, content, isMine, ts);
            });
    }

    QString ChatController::currentSessionId() const
    {
        return m_currentSessionId;
    }

    QString ChatController::currentSessionName() const
    {
        return m_currentSessionName;
    }

    QAbstractListModel* ChatController::sessionModel() const
    {
        return m_sessionModel;
    }

    QAbstractListModel* ChatController::currentMessageModel() const
    {
        return m_currentMessageModel;
    }

    void ChatController::connect(const QString& serverUrl, const QString& userId)
    {
        const auto trimmedUserId = userId.trimmed();
        const auto url = QUrl::fromUserInput(serverUrl);

        if (trimmedUserId.isEmpty() || !url.isValid())
            return;

        m_selfUserId = trimmedUserId;
        m_networkService.connectToServer(serverUrl);
        loadSessions();
    }

    void ChatController::loadSessions()
    {
        auto* model = qobject_cast<SessionListModel*>(m_sessionModel);
        if (model == nullptr)
            return;

        const auto rows = m_localDB.queryLastMessagePerSession();
        QList<SessionListModel::SessionItem> sessions;
        sessions.reserve(rows.size());

        for (const auto& row : rows) {
            if (row.sessionId.isEmpty())
                continue;

            SessionListModel::SessionItem item;
            item.sessionId = row.sessionId;
            item.displayName = row.sessionId;
            item.lastMessage = row.lastMessage;
            item.timeString = formatSessionTimeString(row.lastMessageAt);
            sessions.append(item);
        }

        model->refresh(sessions);
    }

    void ChatController::sendMessage(const QString& content)
    {
        const auto trimmed = content.trimmed();
        if (trimmed.isEmpty())
            return;

        const auto from = m_selfUserId.isEmpty() ? QStringLiteral("me") : m_selfUserId;
        const auto to = m_currentSessionId.isEmpty() ? QStringLiteral("default") : m_currentSessionId;

        Message msg(trimmed, from, to);
        const QString msgId = msg.msgId();

        if (!m_localDB.insertMessage(msg)) {
            return;
        }

        m_localDB.upsertPendingAck(msgId);

        auto* model = qobject_cast<MessageListModel*>(m_currentMessageModel);
        if (model != nullptr)
            model->appendMessage(msg, true);

        auto* sessionModel = qobject_cast<SessionListModel*>(m_sessionModel);
        if (sessionModel != nullptr) {
            SessionListModel::SessionItem item;
            item.sessionId = to;
            item.displayName = to;
            item.lastMessage = trimmed;
            item.timeString = formatSessionTimeString(msg.timestamp());
            sessionModel->upsertSession(item, false);
        }

        auto* timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(Config::Network::ACK_TIMEOUT);
        QObject::connect(timer, &QTimer::timeout, this, [this, msgId]() { onSendTimeout(msgId); });
        timer->start();

        m_pendingMessages[msgId] = PendingContext{timer};

        m_networkService.sendText(msgId, from, to, trimmed);
    }

    void ChatController::onAckReceived(const QString& msgId)
    {
        auto iter = m_pendingMessages.find(msgId);
        if (iter == m_pendingMessages.end())
            return;

        if (iter->ackTimer) {
            iter->ackTimer->stop();
            iter->ackTimer->deleteLater();
        }
        m_pendingMessages.erase(iter);

        m_localDB.updateMessageStatus(msgId, MessageStatus::Delivered);
        m_localDB.removePendingAck(msgId);

        auto* model = qobject_cast<MessageListModel*>(m_currentMessageModel);
        if (model != nullptr)
            model->updateStatus(msgId, MessageStatus::Delivered);

        emit messageStatusChanged(msgId, MessageStatus::Delivered);
    }

    void ChatController::onSendTimeout(const QString& msgId)
    {
        m_pendingMessages.remove(msgId);

        m_localDB.updateMessageStatus(msgId, MessageStatus::Failed);

        auto* model = qobject_cast<MessageListModel*>(m_currentMessageModel);
        if (model != nullptr)
            model->updateStatus(msgId, MessageStatus::Failed);

        emit messageStatusChanged(msgId, MessageStatus::Failed);
    }

    void ChatController::switchToSession(const QString& sessionId)
    {
        if (m_currentSessionId == sessionId)
            return;

        m_currentSessionId = sessionId;
        emit currentSessionIdChanged();

        auto* sessionModel = qobject_cast<SessionListModel*>(m_sessionModel);
        if (sessionModel != nullptr) {
            sessionModel->clearUnread(sessionId);
            const auto displayName = sessionModel->displayNameForSession(sessionId);
            m_currentSessionName = displayName.isEmpty() ? sessionId : displayName;
        }
        else {
            m_currentSessionName = sessionId;
        }
        emit currentSessionNameChanged();

        emit currentMessageModelChanged();
    }

    void ChatController::recoverPendingMessages()
    {
        const auto pendingMessages = m_localDB.fetchPendingMessages();
        for (const auto& msg : pendingMessages) {
            const QString msgId = msg.msgId();

            auto* timer = new QTimer(this);
            timer->setSingleShot(true);
            timer->setInterval(Config::Network::ACK_TIMEOUT);
            QObject::connect(timer, &QTimer::timeout, this, [this, msgId]() { onSendTimeout(msgId); });
            timer->start();

            m_pendingMessages[msgId] = PendingContext{timer};

            m_networkService.sendText(msgId, msg.senderId(), msg.sessionId(), msg.content());
        }
    }

} // namespace ShirohaChat
