#include "imap_structures.h"
#include "imap_parsers.h"
imap::Address::Address(const QString &list)
    : Address(parseList(list))
{}

imap::Address::Address(const QStringList &list):
    name(list[0]),surname(list[1]),handle(list[2]),domain(list[3])
{}


QString imap::Address::mailAddress() const
{
    return handle+'@'+domain;
}

QString imap::Address::sender() const
{
    auto n = name=="NIL"?"":name;
    auto s = surname=="NIL"?"":surname;
    return (n+' '+s).trimmed();
}

imap::Envelope::Envelope(const QString &list)
    :Envelope(parseList(list))
{}

imap::Envelope::Envelope(const QStringList &list)
{
     date_str = list[0];
     subject = decodeMimeWord(list[1]);

     if(list[2]!="NIL")from = parseDoubleList(list[2]);
     if(list[3]!="NIL")sender = parseDoubleList(list[3]);
     if(list[4]!="NIL")reply_to = parseDoubleList(list[4]);
     if(list[5]!="NIL")to = parseDoubleList(list[5]);

     if(list[6]!="NIL")cc = parseDoubleList(list[6]);
     if(list[7]!="NIL")bcc = parseDoubleList(list[7]);
     in_reply_to = list[8];
     msgid = list[9];
}

QDateTime imap::Envelope::date() const
{
    return QDateTime::fromString(date_str,Qt::RFC2822Date);
}
