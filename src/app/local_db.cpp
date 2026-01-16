#include "local_db.h"

#include <QQmlEngine>

namespace ShirohaChat
{

LocalDB& LocalDB::instance()
{
    static LocalDB instance{nullptr};
    return instance;
}

LocalDB* LocalDB::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    auto singleton = &LocalDB::instance();
    QQmlEngine::setObjectOwnership(singleton, QQmlEngine::CppOwnership);
    return singleton;
}

LocalDB::LocalDB(QObject* parent)
    : QObject(parent)
{
}

LocalDB::~LocalDB() = default;

} // namespace ShirohaChat

