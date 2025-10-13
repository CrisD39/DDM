#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include "qudpsocket.h"
#include <QUdpSocket>

class clientSocket: public QObject
{
    Q_OBJECT
public:
    explicit clientSocket(QObject* parent);
private:
    QUdpSocket* socket;
    void readPendingDatagrams();

signals:
    void receivedDatagram(QByteArray datagram);

public slots:
    void sendMessage(QByteArray message);
};

#endif // CLIENTSOCKET_H
