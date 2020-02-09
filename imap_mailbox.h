#ifndef MAILSTATE_H
#define MAILSTATE_H

#include <QObject>
#include "imap_connection.h"
#include "imap_message.h"
#include "imap_structures.h"

#include <functional> //OH NONONONO

namespace imap
{
    /*Nie wiedzalem jak lepiej nazwać*/
    enum class Request
    {
        none,
        fetch_envelope,
        fetch_body
    };

    struct Context
    {
        Command cmd;
        Request req;
        Context(Command c,Request r=Request::none);
    };

    class MailBox : public QObject
    {
        Q_OBJECT
    public:
        explicit MailBox(QObject *parent = nullptr);
        void open(QString hostname);
        void login(QString username, QString password);
        void send(Command arglessCmd);
        void select(QString folderName);
        void fetchLatest(int num=10);
    signals:
        void log(QString);
        void error(QString);
        void loggedIn(Account);
        void syntaxError();
    private:
        void getResponse(QStringList responseBatch);
        Connection conn;
        Account logged_in;
        QList<Context> contextQueue;
        QList<std::function<void(void)>> callQueue; //this kills the cpplet xd
        bool safeState=true;
        int mailNum;

    };
}

#endif // MAILSTATE_H
