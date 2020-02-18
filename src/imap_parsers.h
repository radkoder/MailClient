#ifndef IMAP_PARSERS_H
#define IMAP_PARSERS_H
#include <QByteArray>
#include <QString>
#include <QStringList>
#include "imap_message.h"
#include <vector>
namespace imap {

    QStringList parseLine(const QString& string);
    QStringList parseList(const QString& str);
    QStringList parseDoubleList(const QString& str);
    bool isTagged(const QByteArray& in);
    QString decodeMimeWord(const QString& str);
    QString getFromList(const QStringList& list,const QString& name);

    //znowu nie wiedzialem jak lepiej nazwać xd
    class Assembler
    {
    public:
        void feed(const QByteArray& arr);
        void reset();
        QList<QByteArray> outputLines;
        bool isFinished();
    private:
        QString decode(const std::string raw);

        QByteArray buffer;
        bool isStateOk();
    struct{
        bool quotes=false;
        int paren_count=0;
        bool literal=false;
        bool mime=false;
        bool freshToken=true;
        int literalSize=0;
    }state;

    };
}
#endif // IMAP_PARSERS_H
