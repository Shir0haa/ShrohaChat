#pragma once

#include <QObject>
#include <QtQml/qqml.h>

namespace ShirohaChat
{

class ChatController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

  public:
    static ChatController& instance();
    static ChatController* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

  private:
    explicit ChatController(QObject* parent);
    ChatController(const ChatController&) = delete;
    ChatController& operator=(const ChatController&) = delete;
};

} // namespace ShirohaChat
