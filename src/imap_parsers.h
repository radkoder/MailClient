#ifndef IMAP_PARSERS_H
#define IMAP_PARSERS_H
#include <QByteArray>
#include <QString>
#include <QStringList>

namespace imap {

    QStringList parseLine(const QString& string);
    QStringList parseList(const QString& str);
    QStringList parseDoubleList(const QString& str);
    bool isTagged(const QByteArray& in);
    QString decodeMimeWord(const QString& str);
    QString getFromList(const QStringList& list,const QString& name);
}
#endif // IMAP_PARSERS_H
