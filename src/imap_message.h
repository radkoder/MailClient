#ifndef IMAPMESSAGE_H
#define IMAPMESSAGE_H

#include <QString>
#include <QVector>
#include <QMetaType>
#include <string_view>
#include <memory>
#include <future>
namespace imap{
    class MailBox;
    enum Command
    {
        login,
        capability,
        logout,
        ok,
        bad,
        fetch,
        select,
        unselect,
        uid_fetch
    };
    constexpr const std::string_view imapCmds[] = {
        [login] = "LOGIN",
        [capability] = "CAPABILITY",
        [logout] = "LOGOUT",
        [ok] = "OK",
        [bad] = "BAD",
        [fetch] = "FETCH",
        [select] = "SELECT",
        [unselect] = "UNSELECT",
        [uid_fetch] = "UID FETCH"

    };
    enum DataType
    {
        string,
        nil,
        list,
        number,
        line,
        deduce
    };
    enum class Context
    {
        none,
        fetch_envelope,
        fetch_body
    };

    class Message
    {
        QStringList lines;
    public:
        Message()=default;
        Message(QStringList rawMsg);
        const QStringList& Lines() const;
    };

    struct Request
    {
        //wymagane żeby ten obiekt mógł być przesyłany w sygnale
        Request()=default;
        Request(const Request& other)=default;
        ~Request()=default;

        QByteArray data;
        std::shared_ptr<std::promise<Message>> promise; //UGH TODO nie wiem jak ale trzeba to zmienić
        int futureIndex;
    };

    static uint16_t tagCount=0;
    template<typename... T>
    QByteArray makeReqStr(Command type,T ...args)
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
Q_DECLARE_METATYPE(imap::Request);
#endif // IMAPCOMMAND_H
