#include "dclconccontroller.h"
#include "clientsocket.h"
#include "fcdecodificator.h"
#include <QtEndian>
#include <QDebug>

DclConcController::DclConcController(clientSocket* socket,
                                     FCDecodificator* decodificator,
                                     QObject* parent)
    : QObject(parent),
    m_socket(socket),
    m_decoder(decodificator)
{
    Q_ASSERT(m_socket);
    Q_ASSERT(m_decoder);

    connect(m_socket, &clientSocket::receivedDatagram,
            this, &DclConcController::onDatagram);

    // Timer a 100 ms para enviar PEDIDO_DCL_CONC
    m_timer.setInterval(1000);
    connect(&m_timer, &QTimer::timeout, this, &DclConcController::askForConcentrator);
    m_timer.start();
}

void DclConcController::askForConcentrator()
{
    m_socket->sendMessage(buildPedidoDclConc());
}

void DclConcController::onDatagram(const QByteArray& datagram)
{
    // Necesitamos al menos la primera palabra (3 bytes) para extraer secuencia
    if (datagram.size() < 3) {
        qWarning() << "[DCL_CONC] Datagram demasiado corto";
        return;
    }

    // Primera palabra = 3 bytes (24 bits). Secuencia = últimos 15 bits (LSBs).
    // w1 = B0 B1 B2 (big-endian a 24 bits)
    const uchar b0 = static_cast<uchar>(datagram[0]);
    const uchar b1 = static_cast<uchar>(datagram[1]);
    const uchar b2 = static_cast<uchar>(datagram[2]);
    const quint32 w1 = (static_cast<quint32>(b0) << 16) |
                       (static_cast<quint32>(b1) << 8)  |
                       (static_cast<quint32>(b2));

    const quint16 seq = static_cast<quint16>(w1 & 0x7FFF); // últimos 15 bits

    const QByteArray ack = buildAckFromSeq(seq);
    m_socket->sendMessage(ack);

    if (datagram.size() > 3) {
        QByteArray payload = datagram.mid(3);
        payload = negateData(payload);
        qDebug() << payload.toHex(' ');
        m_decoder->decode(payload);
    }
}

QByteArray DclConcController::buildPedidoDclConc()
{
    return QByteArray::fromHex("010000012E2F");
}

QByteArray DclConcController::buildAckFromSeq(quint16 seq)
{
    // Campo de 16 bits: MSB=1 (ACK) | 15 bits de secuencia
    quint16 ackField = static_cast<quint16>(0x8000 | (seq & 0x7FFF));

    QByteArray out;
    out.reserve(3);
    out.append(char(0x04));                        // CONC_ID
    out.append(char((ackField >> 8) & 0xFF));      // MSB del campo ACK|SEQ
    out.append(char(ackField & 0xFF));             // LSB del campo ACK|SEQ
    return out;
}

QByteArray DclConcController::negateData(const QByteArray &data) {
    QByteArray invertedData = data;
    for (char &b : invertedData)
        b = ~b;
    return invertedData;
}
