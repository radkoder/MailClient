#ifndef MAILSTATE_H
#define MAILSTATE_H

#include <QObject>
#include <QSet>
#include <QMap>
#include "imap_connection.h"
#include "imap_message.h"
#include "imap_structures.h"

#include <future>
#include <functional> //OH NONONONO
#include <QThread>
namespace imap
{
    struct ResponseEntry
    {
        Command cmd;
        Context ctx;
        std::future<Message> future;
    };
    struct ResponseHandle
    {
        Message get();
        ResponseHandle &onReady(std::function<void(Message)> func);
        ResponseHandle(int index,MailBox* owner);
    private:
        int myIndex;
        MailBox* myBox;
    };

    class MailBox : public QObject
    {
        Q_OBJECT
    public:
        explicit MailBox(QObject *parent = nullptr);
        void open(const QString& hostname);
        ResponseHandle login(QString username, QString password);
        ResponseHandle send(Command arglessCmd);
        ResponseHandle select(QString folderName);
        ResponseHandle fetchInfo(int num,int skip = 0);
        ResponseHandle fetchBody(QString uid);
        QVector<MailEntry> getLatest(int num,int skip=0);
        QString getBody(QString uid);

        Message get(int index);
        void putCallback(int index, const std::function<(Message)> &func);
        template<typename Func>
        void onFetchReady(Func f);
        ~MailBox();
    signals:
        void log(QString);
        void error(QString);
        void loggedIn(imap::Account);
        void syntaxError();
        void fetchReady();
        void sendRequest(imap::Request);
        void openConn(QString hostname);
    private:
        void getResponse(int index);
        void addMail(const MailEntry &newEntry);
        Connection* conn;

        QList<Context> contextQueue;
        QList<std::function<void(void)>> callQueue; //this kills the cpplet xd

        bool safeState=true;
        int mailNum;
        Account logged_in;

        QVector<MailEntry> mails;
        QMap<int,QString> mailBodies;
        QSet<int> uids;

        /*newstuff*/
        std::unordered_map<int, ResponseEntry> futures;
        QMap<int, Message> responses;
        QMap<int,std::function<void(Message)>> callbacks;
        QThread netThread;

        int keycount=0;

    };

    template<typename Func>
    void MailBox::onFetchReady(Func f)
    {
        /*TODO*/
        //trzeba będzie zastąpić kolejką wywołań
        //bo buguje się kiedy jest więcej niż 1 f()
        QMetaObject::Connection * const connection = new QMetaObject::Connection;
        *connection = connect(this, &MailBox::fetchReady, [this, f, connection](){
            f();
            QObject::disconnect(*connection);
            delete connection;
        });
    }
}

#endif // MAILSTATE_H
