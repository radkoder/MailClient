#ifndef IMAPCONNECTION_H
#define IMAPCONNECTION_H
#include <future>
#include <list>
#include <QObject>
#include <QList>
#include <QStringList>
#include <QSslSocket>
#include "imap_message.h"
#include "imap_parsers.h"
namespace imap {

    constexpr quint16 SIMAPport = 993;
    class Connection : public QObject
    {
        Q_OBJECT
        QSslSocket sock;
        QByteArray responseBuffer;
        QList<QByteArray> msgBuffer;
        std::list<std::promise<Message>> promises;
        std::list<Request> requestQueue;
        typeof(promises.begin()) currentPromise;
        Assembler assembler;

        bool reqInProgress=false;
    public:
        explicit Connection(QObject *parent = nullptr);
        Connection(const QString& hostname, QObject *parent = nullptr);
        void open(const QString& hostname);
        void close();
        void send(Request r);
        ~Connection();
    public slots:

    signals:
        void error(QString);
        void log(QString);
        void responseReady(int);
    private:
        void gotData();
        void sendNext();

    };
}
#endif // IMAPCONNECTION_H
