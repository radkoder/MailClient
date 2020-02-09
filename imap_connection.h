#ifndef IMAPCONNECTION_H
#define IMAPCONNECTION_H

#include <QObject>
#include <QStringList>
#include <QSslSocket>
namespace imap {

    constexpr quint16 SIMAPport = 993;
    class Connection : public QObject
    {
        Q_OBJECT
        QSslSocket sock;
        QByteArray responseBuffer;
        QList<QByteArray> msgBuffer;
        bool reqInProgress=false;
    public:
        explicit Connection(QObject *parent = nullptr);
        Connection(QString hostname, QObject *parent = nullptr);
        void open(QString hostname);
        void close();
        void send(const QByteArray& data);
        ~Connection();
    signals:
        void error(QString);
        void log(QString);
        void responseReady(QStringList);
    private:
        void gotData();
        void sendNext();

    };
}
#endif // IMAPCONNECTION_H
