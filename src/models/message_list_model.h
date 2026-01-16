#pragma once

#include <QAbstractListModel>

namespace ShirohaChat
{

class MessageListModel : public QAbstractListModel
{
    Q_OBJECT

  public:
    explicit MessageListModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

} // namespace ShirohaChat
