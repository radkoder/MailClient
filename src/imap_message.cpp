#include "imap_message.h"
#include <algorithm>
imap::Message::Message(const QString& rawMsg)
{
    auto parts = rawMsg.split(' ',QString::SkipEmptyParts);
    if(parts.size() >= 2)
    {
        tag = parts.takeFirst();
        cmd = parts.takeFirst();
    }
    args = parts.join(' ');
}

bool imap::Message::isCorrect() const
{
    //check if cmd is in the known commands
    //TODO real check
    auto r = std::find(std::begin(imapCmds),std::end(imapCmds),cmd.toUtf8().data());
    return r != std::end(imapCmds);
}
bool imap::Message::isTagged() const
{
    return tag != "*" || tag.length() != 0;
}
bool imap::Message::isOk() const
{
    return cmd == imapCmds[imap::Command::ok].data();
}



