#include "models/message_list_model.h"

namespace ShirohaChat
{

MessageListModel::MessageListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int MessageListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

}

QVariant MessageListModel::data(const QModelIndex& index, int role) const
{
    // TODO: Implement
}

QHash<int, QByteArray> MessageListModel::roleNames() const
{
   // TODO: Implement
}

} // namespace ShirohaChat
