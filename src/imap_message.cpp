#include "imap_message.h"
#include <algorithm>
imap::Message::Message(QStringList lines)
{
    this->lines = lines;
}

const QStringList& imap::Message::Lines() const
{
    return lines;
}
