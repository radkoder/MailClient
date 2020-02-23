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

        //operacje możliwe do wykonywania na skrzynce pocztowej
        ResponseHandle login(QString username, QString password);
        ResponseHandle send(Command arglessCmd);
        ResponseHandle select(QString folderName);
        ResponseHandle fetchInfo(int num,int skip = 0);
        ResponseHandle fetchBody(QString uid);

        //operacje możliwe do wykonania na zapisanym lokalnym stanie skrzynki
        QVector<MailEntry> getLatest(int num,int skip=0);
        QString getBody(QString uid);

        Message get(int index);
        void putCallback(int index, const std::function<void(Message)> &func);
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
        void getResponse(int index); //odbiera informacje z połączenia z future o danym indeksie
        void addMail(const MailEntry &newEntry); //dodaje informacje o mailu do lokalnego bufora
        Request newRequest(Command cmd,Context ctx = Context::none); //zwraca poprawny Request i trzeba tylko ustawić pole .data
        template<typename Func>
        void putSafeRequest(Func f); //dodaje zapytanie ze sprawdzeniem stanu
        Connection* conn;

        QList<Context> contextQueue;
        QList<std::function<void()>> callQueue; //this kills the cpplet xd
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
    void MailBox::putSafeRequest(Func f)
    {
        if(!safeState)
        {
            emit log(QString{"[[Mailbox]]:defering [] request"});
            callQueue.push_back(f);
        }
        else
        {
            f();
        }
    }

}

#endif // MAILSTATE_H
