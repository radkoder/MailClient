#ifndef IMAP_STRUCTURES_H
#define IMAP_STRUCTURES_H
#include <QString>
#include <QDateTime>
namespace imap
{
    struct Account
    {
        QString server,login,pass;
    };

    struct Address
    {
        QString name;
        QString surname;
        QString handle;
        QString domain;
        Address(const QString& list);
        Address(const QStringList& list);
        Address()=default;
        QString mailAddress() const;
        QString sender() const;
    };
    struct Envelope
    {
        QString date_str;
        QString subject;

        Address from;
        Address sender;
        Address reply_to;
        Address to;

        Address cc;     //???
        Address bcc;    //???

        QString in_reply_to;
        QString msgid;

        Envelope()=default;
        Envelope(const QString& list);
        Envelope(const QStringList& list);
        QDateTime date() const;

    };
    struct MailEntry
    {
        QString uid;
        Envelope details;
    };
}



#endif // IMAP_STRUCTURES_H
