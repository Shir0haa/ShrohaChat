#pragma once

#include <QObject>
#include <QtQml/qqml.h>

namespace ShirohaChat
{

class LocalDB : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

  public:
    static LocalDB& instance();
    static LocalDB* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

  private:
    explicit LocalDB(QObject* parent);
    ~LocalDB() override;
    LocalDB(const LocalDB&) = delete;
    LocalDB& operator=(const LocalDB&) = delete;
};

} // namespace ShirohaChat
