#ifndef IMAPMESSAGE_H
#define IMAPMESSAGE_H

#include <QString>
#include <QVector>
#include <string_view>
namespace imap{
    enum Command
    {
        login,
        capability,
        logout,
        ok,
        bad,
        fetch,
        select,
        unselect
    };
    constexpr const std::string_view imapCmds[] = {
        [login] = "LOGIN",
        [capability] = "CAPABILITY",
        [logout] = "LOGOUT",
        [ok] = "OK",
        [bad] = "BAD",
        [fetch] = "FETCH",
        [select] = "SELECT"

    };
    class Message
    {
    public:
        Message()=default;
        Message(const QString& rawMsg);
        bool isCorrect() const;
        bool isTagged() const;
        bool isOk() const;

        QString tag;
        QString cmd;
        QString args;
    };
    class ResponsePack
    {
        Command response_to;
        QStringList lines;
    };
    static uint16_t tagCount=0;
    template<typename... T>
    QByteArray makeRequest(Command type,T ...args)
    {
        //tags will be hex numbers from 0000 to FFFF

        auto tag = QString::number(tagCount++,16).rightJustified(4,'0');

        auto cmd = imapCmds[type];
        QStringList s;
        s << tag << cmd.data();
        (s << ... <<args);
        return s.join(' ').toUtf8().trimmed()+"\r\n"; //tag command [args...]\r\n
    }
}
#endif // IMAPCOMMAND_H
