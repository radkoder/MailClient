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
    emit log("[[Connection]]:opening connection to"+hostname);
    sock.connectToHostEncrypted(hostname,SIMAPport);
    sock.waitForReadyRead(3000);
}

void imap::Connection::close()
{
   sock.write(imap::makeReqStr(imap::Command::logout));
}

imap::Connection::~Connection()
{
    close();
}

void imap::Connection::gotData()
{
    auto data = sock.readAll();
    emit log("[[imap::Connection]]: Got:"+data);
    assembler.feed(data);

    if(assembler.isFinished())
    {
        emit log("[[imap::Connection]]: Accepted data");
        auto bytelist = assembler.outputLines;
        assembler.reset();
        //TODO sanitize/defer QString conversion
        QStringList strlist;
        strlist.reserve(bytelist.size());
        for(auto& ba : bytelist)
        {
            strlist.push_back(QString::fromUtf8(ba));
        }

        requestQueue.front().promise->set_value(strlist);
        emit responseReady(requestQueue.front().futureIndex);
        reqInProgress = false;
        requestQueue.pop_front();
        sendNext();
    }
    else
    {
        emit log("[[imap::Connection]]: Waiting for more data...");
    }





    /* Chyba już nie potrzebne ale trzamam dla referencji jakby coś nie działało
     *
    auto responsePart = sock.readAll();
    emit log("S:"+responsePart);
    responseBuffer += responsePart;
    bool isTagged=false;
    //odpowiedz otagowana zazwyczaj jest ostatnia
    if(responsePart.endsWith('\n'))
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
    }*/


}

void imap::Connection::send(Request r)
{
    emit log("[[imap::Connection]]: Accepted send request "+r.data);
    requestQueue.push_back(std::move(r));
    if(!reqInProgress)sendNext();
}
void imap::Connection::sendNext()
{
    if(!requestQueue.empty() && !reqInProgress)
    {
        emit log("[[imap::Connection]]: Sending \""+requestQueue.front().data+"\"");
        sock.write(requestQueue.front().data);
        reqInProgress=true;
    }
}
