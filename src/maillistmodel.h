#ifndef MAILLISTMODEL_H
#define MAILLISTMODEL_H

#include <QAbstractTableModel>
#include "imap_structures.h"
class MailListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit MailListModel(int size =10,QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setMails(const QVector<imap::MailEntry>& mails);


private:
    QVector<imap::MailEntry> mailList;
};

#endif // MAILLISTMODEL_H
