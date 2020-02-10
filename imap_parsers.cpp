#include "imap_parsers.h"
#include <QRegularExpression>
QStringList imap::parseLine(const QString& string)
{
    const QChar *ptr = string.data();
    int paren = 0;  //licznik zagnieżdżenia nawiasów
    bool str = false; //czy jesteśmy w "stringu"
    QString buff;
    QStringList out; //tablica tokenów
    while(*ptr != '\0' && *ptr != '\n')
    {
        switch(ptr->unicode())
        {
        case '(':
            paren++;
            buff+=*ptr;
            break;
        case ')':
            paren--;
            if(paren<0)break;
            buff+=*ptr;
            break;
        case ' ':
            if(paren == 0 && !str){ //jeżeli jesteśmy w nawiasach albo w "" to nie rozdzielamy
                out.push_back(buff);
                buff.clear();
            }
            else buff+=*ptr;
            break;
        case '\"':
            if(paren > 0)buff+=*ptr; //jeżeli jesteśmy w nawiasach i natrafimy na "" to nie pomijamy go tylko kopiujemy jak leci
            else str = !str; //jeżlei jesteśmy poza to pomijamy "" i kopiujemy tylko zawartość stringa
            break;
        default:
            buff+=*ptr;
            break;
        }
        ptr++;
    }

    out.push_back(buff);
    return out;
}

QStringList imap::parseList(const QString& str)
{
    if(str.trimmed()[0] == '(' && str.trimmed().endsWith(')'))
    {
        auto s = str.mid(1).chopped(1);
        return parseLine(s);
    }
    else
    {
        /*error*/
        return QStringList();
    }
}

//dla list w stylu ((A B C))
QStringList imap::parseDoubleList(const QString& str)
{
    return parseList(parseList(str)[0]);
}
bool imap::isTagged(const QByteArray& in)
{
   auto str = QString::fromUtf8(in);
   QRegularExpression r("^\\d{4} \\w");
   return r.match(str).hasMatch();
}
QString imap::decodeMimeWord(const QString& in)
{
    //Z wikipedii, hasło:MIME
    if(in[0] != '=')return in;
    //Gmail wysyła wieloczęściowe tematy oddzielone spacją
    if(in.contains(' '))
    {
        auto strList = in.split(' ');
        QString ret;
        for(auto& s: strList)
        {
            ret += decodeMimeWord(s);
        }
        return ret;
    }
    auto parts = in.split('?');

    auto format = parts[1];
    auto coding = parts[2];
    auto text = parts[3];
    QByteArray decoded;
    if(coding == "B")
    {
        //base64
        decoded = QByteArray::fromBase64(text.toLocal8Bit());
    }
    else
    {
        //Q - quoted printable
        text.replace('_',' ');
        decoded = QByteArray::fromPercentEncoding(text.toLocal8Bit(),'=');
    }
    if(format == "UTF-8")
    {
        auto out = QString::fromUtf8(decoded);
        return out;
    }
    else
    {
        return "[UNSUPPORTED FORMAT] - "+format;
    }


}
