#include "maillistmodel.h"
#include <QSize>
MailListModel::MailListModel(int size,QObject *parent)
    : QAbstractTableModel(parent)
{
    mailList.resize(size);
}

QVariant MailListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    const char* titles[]={
        "Data",
        "Nadawca",
        "Temat"
    };
    if(orientation == Qt::Horizontal)
        return QVariant(titles[section]);
    else
        return QVariant();
}

int MailListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return mailList.size();
}

int MailListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    //data, nadawca, temat
    return 3;
}

QVariant MailListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role == Qt::DisplayRole)
    {
        auto entry = mailList.at(index.row());
        QString out;
        switch(index.column())
        {
        case 0:
            out=entry.details.date().toString("hh:mm dd.MM.yyyy");
            break;
        case 1:
            out=entry.details.from.name;
            break;
        case 2:
            out=entry.details.subject;
            break;
        default:
            out="[INDEX ERROR]";
        }

        return QVariant(out);
    }
    else if(role == Qt::SizeHintRole)
    {
        switch (index.column())
        {
        case 0:
            return QSize(20,50);
        case 1:
            return QSize(20,500);
        case 2:
            return QSize(20,100);
        default:
            return QVariant();
        }
    }
    else if(role == Qt::UserRole)
    {
        return mailList.at(index.row()).uid;
    }
    else
    {
        return  QVariant();
    }
}

void MailListModel::setMails(const QVector<imap::MailEntry>& entries)
{
   beginResetModel();

   mailList = entries;

   endResetModel();
}

