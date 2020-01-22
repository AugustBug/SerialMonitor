#include "message.h"

Message::Message(QDateTime time, QString msg, bool isSent)
{
    this->data = msg.toLocal8Bit();
    this->isSent = isSent;
    this->time = time;
}

bool Message::isMessageSent() {
    return isSent;
}

QByteArray Message::getData() {
    return data;
}

QDateTime Message::getMessageTime() {
    return time;
}
