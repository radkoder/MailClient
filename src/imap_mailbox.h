#ifndef MAILSTATE_H
#define MAILSTATE_H

#include <QObject>
#include <QSet>
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
        void fetchInfo(int num,int skip = 0);
        QVector<MailEntry> getLatest(int num);
        template<typename Func>
        void onFetchReady(Func f);
    signals:
        void log(QString);
        void error(QString);
        void loggedIn(Account);
        void syntaxError();
        void fetchReady();
    private:
        void getResponse(QStringList responseBatch);
        void addMail(MailEntry newEntry);
        Connection conn;

        QList<Context> contextQueue;
        QList<std::function<void(void)>> callQueue; //this kills the cpplet xd

        bool safeState=true;
        int mailNum;
        Account logged_in;

        QVector<MailEntry> mails;
        QSet<int> uids;

    };

    template<typename Func>
    void MailBox::onFetchReady(Func f)
    {
        QMetaObject::Connection * const connection = new QMetaObject::Connection;
        *connection = connect(this, &MailBox::fetchReady, [this, f, connection](){
            f();
            QObject::disconnect(*connection);
            delete connection;
        });
    }
}

#endif // MAILSTATE_H
