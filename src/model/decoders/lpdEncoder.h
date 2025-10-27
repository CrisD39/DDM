#ifndef LPDENCODER_H
#define LPDENCODER_H

#include "commandContext.h"
#include "obmHandler.h"
#include <QObject>
#include <QByteArray>
#include <QPair>
#include <QMap>

using Type      = TrackData::Type;
using Identity  = TrackData::Identity;
using TrackMode = TrackData::TrackMode;

class encoderLPD : public QObject
{
    Q_OBJECT
public:
    explicit encoderLPD();
    QByteArray buildFullMessage(const CommandContext &ctx);
    void setOBMHandler(OBMHandler* oh){this->obmHandler = oh;}

private:
    QByteArray buildAB2Message(const Track &track);
    QByteArray buildAB3Message(const CursorEntity &cursor);
    QByteArray encodeCoordinate(double value, uint8_t idBits, bool AP = true, bool PV = false, bool LS = false);
    QByteArray encodeAngle(double value, unint8_t idBits, bool Ap = true, bool PV = false, bool LS = false);
    QByteArray buildSymbolBytes(const Track &track) const;
    QPair<uint8_t, uint8_t> symbolFor(const Track &track) const;
    uint8_t trackModeFor(const Track &track) const;
    QByteArray negateData(const QByteArray &data);

    OBMHandler *obmHandler;
    QByteArray buildOBM();

};

constexpr uint8_t BIT_LS = 1 << 6;
constexpr uint8_t BIT_PV = 1 << 5;
constexpr uint8_t BIT_V  = 1 << 4;
constexpr uint8_t BIT_AP = 1 << 3;

constexpr quint8 AB3_ID_ANGLE = 0x0D;   // <- PONER EL REAL
constexpr quint8 AB3_ID_RHO   = 0x0E;   // <- PONER EL REAL
constexpr quint8 AB3_ID_X     = 0x05;   // <- PONER EL REAL
constexpr quint8 AB3_ID_Y     = 0x07;   // <- PONER EL REAL

constexpr uint8_t AB2_ID_X = 0x01;
constexpr uint8_t AB2_ID_Y = 0x03;

constexpr uint8_t AB1_ID_X = 0x09;
constexpr uint8_t AB1_ID_Y = 0x0B;


constexpr uint8_t EOMM = 0x17;

constexpr uint8_t HEADER_BYTES[2]       = { 0x00, 0x00 };
constexpr uint8_t DESCENTRADO_BYTES[6]  = { 0x00, 0x00, 0x19, 0x00, 0x00, 0x0B };
constexpr uint8_t PADDING_BYTES[9]      = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
constexpr uint8_t INVALID_OWNSHIP[9]   = { 0x00, 0x00, 0x61, 0x00, 0x00, 0x23, 0x1C, 0x00, 0x17 };

#endif // LPDENCODER_H
