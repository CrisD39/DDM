#include "clientsocket.h"
#include "configuration.h"

clientSocket::clientSocket(QObject *parent): QObject(parent) {
    socket = new QUdpSocket(this);
    socket->bind(Configuration::instance().ipFpga, PORT_FPGA);
    connect(socket, &QUdpSocket::readyRead, this, &clientSocket::readPendingDatagrams);
}

void clientSocket::sendMessage(QByteArray message)
{
    socket->writeDatagram(message, Configuration::instance().masterIpDhcNetwork, PORT_NOTEBOOK);
}

void clientSocket::readPendingDatagrams(){
    while (socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        socket->readDatagram(datagram.data(), datagram.size());
        emit receivedDatagram(datagram);
    }
}
