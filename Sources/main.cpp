#include <QCoreApplication>
#include <QByteArray>
#include <QDebug>
#include "FCDecodificator.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    FCDecodificator decoder;

    QByteArray message;

    // --- Word 1: 0101 0010 0001 0000 0000 0000 ---
    message.append(static_cast<char>(0b01010010)); // 0x52
    message.append(static_cast<char>(0b00010000)); // 0x10
    message.append(static_cast<char>(0b00000000)); // 0x00

    // --- Word 2: 1000 1000 0000 0000 0000 0000 ---
    message.append(static_cast<char>(0b10001000)); // 0x88
    message.append(static_cast<char>(0b00000000)); // 0x00
    message.append(static_cast<char>(0b00000000)); // 0x00

    // --- Word 3: 0000 0000 0000 0000 0000 0000 ---
    message.append(static_cast<char>(0b00000000)); // 0x00
    message.append(static_cast<char>(0b00000000)); // 0x00
    message.append(static_cast<char>(0b00000000)); // 0x00


    // --- Word 4: 0010 0101 0010 0110 0000 0000 ---
    message.append(static_cast<char>(0b00100101)); // 0x25
    message.append(static_cast<char>(0b00100110)); // 0x26
    message.append(static_cast<char>(0b00000000)); // 0x00

    qDebug() << "Mensaje en hexadecimal:" << message.toHex(' ');

    qDebug() << "Mensaje en binario:";
    for (unsigned char c : message)
        qDebug().noquote() << QString("%1").arg(c, 8, 2, QLatin1Char('0'));

    decoder.decode(message);

    return 0;
}
