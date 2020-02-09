#include "imap_connection.h"
#include "imap_message.h"
#include "imap_parsers.h"
imap::Connection::Connection(QObject *parent) : QObject(parent)
{
    connect(&sock,&QSslSocket::readyRead,this,&Connection::gotData);
    connect(&sock, QOverload<const QList<QSslError> &>::of(&QSslSocket::sslErrors),
        [=](const QList<QSslError> &errors){
        foreach(auto err,errors)
        {
          emit error(err.errorString());
        }
    });

}
imap::Connection::Connection(QString hostname, QObject *parent)
    :Connection(parent)
{
    open(hostname);
}

void imap::Connection::open(QString hostname)
{
    sock.connectToHostEncrypted(hostname,SIMAPport);
}

void imap::Connection::close()
{
   send(imap::makeRequest(imap::Command::logout));
}

imap::Connection::~Connection()
{
    close();
}

void imap::Connection::gotData()
{
    auto responsePart = sock.readAll();
    emit log("S:"+responsePart);
    responseBuffer += responsePart;

    bool isTagged=false;
    //odpowiedz otagowana zazwyczaj jest ostatnia
    if(responsePart.contains('\n'))
    {
        auto ress = responsePart.split('\n');
        ress.removeLast(); //po .split zostaje jedna pusta część na końcu
        isTagged = imap::isTagged(ress.last());
    }


    if(isTagged)
    {
        //emit log("Sending response to Mailbox");
        auto bytelist = responseBuffer.split('\n');
        QStringList strlist;
        strlist.reserve(bytelist.size());
        for(auto& ba : bytelist)
        {
            strlist.push_back(QString::fromUtf8(ba));
        }
        responseBuffer.clear();
        emit responseReady(strlist);
        sendNext();
    }


}

void imap::Connection::send(const QByteArray &data)
{
    if(reqInProgress)msgBuffer.push_back(data);
    else
    {
        sock.write(data);
        reqInProgress = true;
        emit log("C:"+data);
    }

}
void imap::Connection::sendNext()
{
    reqInProgress = false;
    if(!msgBuffer.empty())
    {
        send(msgBuffer.takeFirst());
    }
}
