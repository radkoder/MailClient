#include "imap_parsers.h"
#include <QRegularExpression>
QString parseLiteral(const QChar* pptr[])
{
    //Parsowanie dosłownego stringa wg sekcji 4.3 dokumentacji IMAPv4
    auto ptr = *pptr;
    int num=0;
    QString buff;
    while(num==0)
    {
        //Odczytanie liczby oktetów stringa
        switch (ptr->unicode())
        {
        case '{':
            ptr++;
            break;
        case '0' ... '9':
            buff+=*ptr;
            ptr++;
            break;
        case '}':
            num = buff.toInt();
            ptr++;
            break;
        }
    }
    buff.clear();
    buff.reserve(num);
    ptr+=2; //właściwy tekst jest oddzielony od liczby oktetów [CR][LF]
    for(int i=0;i<num;i++)
    {
        buff+=*ptr;
        ptr++;
    }
    return buff;


}
QStringList imap::parseLine(const QString& string)
{
    const QChar *ptr = string.data();
    int paren = 0;  //licznik zagnieżdżenia nawiasów
    bool quotes = false; //czy jesteśmy w "stringu"

    QString buff;
    QStringList out; //tablica tokenów
    while(*ptr != '\0')
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
            if(paren == 0 && !quotes){ //jeżeli jesteśmy w nawiasach albo w "" to nie rozdzielamy
                out.push_back(buff);
                buff.clear();
            }
            else buff+=*ptr;
            break;
        case '\"':
            if(paren > 0)buff+=*ptr; //jeżeli jesteśmy w nawiasach i natrafimy na "" to nie pomijamy go tylko kopiujemy jak leci
            else quotes = !quotes; //jeżlei jesteśmy poza to pomijamy "" i kopiujemy tylko zawartość stringa
            break;
        case '{':
            buff+=parseLiteral(&ptr); //pomocnicza funkcja żeby już nie mieszać kodu
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
   QRegularExpression r("^[0-9a-f]{4} \\w");
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

QString imap::getFromList(const QStringList &list,const QString& name)
{
    //wg specyfikacji
    //wartość pola następuje bezpośrednio po nazwie pola
    return list.at( list.indexOf(name) + 1 );
}
void imap::Assembler::feed(const QByteArray &arr)
{
    const char* data = arr.data();
    auto literalSize = [&](int& i)->int{
            int num=0;
            QString buff;
            while(num==0)
            {
                //Odczytanie liczby oktetów stringa
                switch (data[i])
                {
                case '{':
                    break;
                case '0' ... '9':
                    buff+=data[i];
                    break;
                case '}':
                    num = buff.toInt();
                    break;
                }
                i++;
            }
            i+=2;
            //if(i>arr.size()) oops;
            return num;
    };

    for(int i=0;i<arr.size();i++)
    {
        if(state.literal)
        {
            state.literalSize--;
            if(state.literalSize == 0)
            {
                state.literal = false;
            }
        }

        switch(data[i])
        {
        case '(':
            state.paren_count++;
            buffer+=data[i];
            break;
        case ')':
            state.paren_count--;
            buffer+=data[i];
            break;
        case '\"':
            state.quotes = !state.quotes;
            buffer+=data[i];
            break;
        case '{':
            state.literalSize=literalSize(i);
            state.literal = true;
            break;
        case '\n':
            buffer+=data[i];
            if(isStateOk())
            {
                outputLines.push_back(buffer);
                buffer.clear();
            }
            break;
        default:
            buffer+=data[i];
            break;
        }
    }


}

void imap::Assembler::reset()
{
    state = {};
    outputLines.clear();
}

bool imap::Assembler::isFinished()
{
    if(outputLines.empty())
        return false;
    else
        return isTagged(outputLines.back().data());
}

bool imap::Assembler::isStateOk()
{
    return (!state.literal && !state.quotes && state.paren_count==0);
}
