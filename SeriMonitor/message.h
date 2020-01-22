#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDateTime>

class Message
{
public:
    Message(QDateTime time, QString msg, bool isSent);
    bool isMessageSent();
    QByteArray getData();
    QDateTime getMessageTime();
private:
    QDateTime time;
    QByteArray data;
    bool isSent;
};

#endif // MESSAGE_H
