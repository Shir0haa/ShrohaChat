#include "models/session_list_model.h"

namespace ShirohaChat
{

SessionListModel::SessionListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int SessionListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    
}

QVariant SessionListModel::data(const QModelIndex& index, int role) const
{
   // TODO: Implement
}

QHash<int, QByteArray> SessionListModel::roleNames() const
{
    // TODO: Implement    
}

} // namespace ShirohaChat
