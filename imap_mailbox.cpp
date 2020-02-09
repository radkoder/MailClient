#include "imap_mailbox.h"
#include "imap_parsers.h"
imap::MailBox::MailBox(QObject *parent) : QObject(parent)
{
    connect(&conn,&Connection::log,[&](QString s){
        emit log(s);
    });
    connect(&conn,&Connection::error,[&](QString s){
        emit error(s);
    });
    connect(&conn,&Connection::responseReady,this,&MailBox::getResponse);
}

void imap::MailBox::open(QString hostname)
{
    conn.open(hostname);
    logged_in.server=hostname;
}

void imap::MailBox::login(QString username, QString password)
{
    logged_in.login = username;
    contextQueue.push_back(Command::login);
    conn.send(makeRequest(Command::login,username,password));
}

void imap::MailBox::send(imap::Command arglessCmd)
{
    /*noop capability logout*/
    contextQueue.push_back(arglessCmd);
    conn.send(makeRequest(arglessCmd));
}

void imap::MailBox::select(QString folderName)
{
    //musimy sprawidzić czy stan jest bezpieczny,
    //inaczej zajdzie hazard w stylu -
    //będziemy chccieli sfetchować maila z folderu zanim dojdzie do nas odpowiedz na selecta
    //wszystkie niebezpieczne requesty stoją w kolejce aż będzie bezpiecznie
    //stan niebezpieczny powstaje tylko po zapytaniach select... narazie
    if(!safeState)
    {
        emit log("select waiting");
        callQueue.push_back([folderName,this](){
           select(folderName);
        });

    }
    else
    {
        contextQueue.push_back(Command::select);
        conn.send(makeRequest(Command::select,folderName));
        safeState = false;
    }

}

void imap::MailBox::fetchLatest(int num)
{
    if(!safeState)
    {
        emit log("fetch waiting");
        callQueue.push_back([num,this](){
           fetchLatest(num);
        });
    }
    else
    {
        contextQueue.push_back({Command::fetch,Request::fetch_envelope});
        auto start = QString::number(mailNum);
        auto end = QString::number(mailNum - num);
        auto range = start+':'+end;
        conn.send(makeRequest(Command::fetch,range,"ENVELOPE"));
    }
}
void imap::MailBox::getResponse(QStringList responseBatch)
{
    //jeżeli wiadomość od serwera nie ma kontekstu wtedy ignorujemy
    //nie ma kontekstu tzn że nie jest odpowiedzią na nasze zapytanie
    if(contextQueue.empty()){
        emit error("No context, ignoring");
        return;
    }
    auto ctx = contextQueue.takeFirst();
    if(contextQueue.empty())
    emit log(QString("[[MailBox]]:Accepted response to ")+imapCmds[ctx.cmd].data());

    auto bad = responseBatch.filter("BAD",Qt::CaseInsensitive);
    if(!bad.empty())
    {
        emit syntaxError();
    }

    /*TODO jakoś to ulepszyć*/
    //obsługa różnych odpowiedzi na polecenia
    if(ctx.cmd == Command::login)
    {
        //sprawdamy czy logowanie sie powiodło i potwierdzamy zalogowanie
        auto ok = responseBatch.filter("OK");
        if(!ok.empty())
        {
            emit loggedIn(logged_in);
        }
    }
    else if(ctx.cmd == Command::select)
    {
        //wyciągamy informacje o folderze, updateujemy stan wewnętrzny
        //a potem przywracamy stan bezpieczny
        auto exists = responseBatch.filter("EXISTS");
        mailNum = parseLine(exists[0])[1].toInt();

        safeState = true;
    }
    else if(ctx.cmd == Command::fetch)
    {
        if(ctx.req == Request::fetch_envelope)
        {
            //parsujemy strukture "envelope"
            auto envs = responseBatch.filter("ENVELOPE");
            for(const auto& line : envs)
            {
                auto l = parseLine(line);
                auto lf = l.filter("ENVELOPE");
                auto pl = parseList(lf[0]);
                auto env = pl[1];
                //auto env = parseList(parseLine(line).filter("ENVELOPE")[0])[1];
                Envelope e(env);
                emit log("Got mail: "+e.from.handle+"@"+e.from.domain+" - "+e.subject);
            }
        }
        //BIG BOI INC parsowanie maila
    }

    //kiedy stan jest bezpieczny a są jeszcze zakolejkowane wywołania
    //wtedy wywołaj wszystkie w kolejności
    //((może lepiej to by było zrobić w sygnałach??))
    if(safeState && !callQueue.empty())
    {
        while(!callQueue.empty())
        {
            std::invoke(callQueue.takeFirst());
        }
    }

}


imap::Context::Context(imap::Command c, imap::Request r)
    :cmd(c),req(r)
{}
