#ifndef ENCODERLPD_H
#define ENCODERLPD_H

#include "CommandContext.h"
#include <QObject>
#include <QByteArray>
#include <QPair>
#include <QMap>

class encoderLPD : public QObject
{
    Q_OBJECT
public:
    explicit encoderLPD();
    QByteArray buildFullMessage(const CommandContext &ctx);

private:
    QByteArray buildAB2Message(const Track &track);
    QByteArray encodeCoordinate(double value, uint8_t idBits);
    QByteArray buildSymbolBytes(const Track &track) const;
    QPair<uint8_t, uint8_t> symbolFor(const Track &track) const;
    QByteArray negateData(const QByteArray &data);
};

constexpr uint8_t BIT_LS = 1 << 6;
constexpr uint8_t BIT_PV = 1 << 5;
constexpr uint8_t BIT_V  = 1 << 4;
constexpr uint8_t BIT_AP = 1 << 3;

constexpr uint8_t AB2_ID_X = 0x01;
constexpr uint8_t AB2_ID_Y = 0x03;
constexpr uint8_t AB1_ID_X = 0x09;
constexpr uint8_t AB1_ID_Y = 0x0B;

constexpr uint8_t EOMM = 0x17;

constexpr uint8_t HEADER_BYTES[2]       = { 0x00, 0x00 };
constexpr uint8_t DESCENTRADO_BYTES[6]  = { 0x00, 0x00, 0x19, 0x00, 0x00, 0x0B };
constexpr uint8_t PADDING_BYTES[9]      = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#endif // ENCODERLPD_H
