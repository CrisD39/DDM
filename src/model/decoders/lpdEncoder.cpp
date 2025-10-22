#include "lpdEncoder.h"
#include "commandContext.h"
#include "qdebug.h"

#include <QByteArray>
#include <QtEndian>
#include <QtGlobal>

encoderLPD::encoderLPD() {}

QByteArray encoderLPD::buildFullMessage(const CommandContext &ctx) {
    QByteArray bigBuffer;

    bigBuffer.append(static_cast<char>(HEADER_BYTES[0]));
    bigBuffer.append(static_cast<char>(HEADER_BYTES[1]));
    int palabrasTracks = (ctx.tracks.size() * 6) + 2 + 9;
    bigBuffer.append(static_cast<char>(palabrasTracks & 0xFF));

    //center

    bigBuffer.append(encodeCoordinate(ctx.centerX, AB1_ID_X)); // V=1 y ID=1001
    bigBuffer.append(encodeCoordinate(ctx.centerY, AB1_ID_Y)); // ID=1011


    bigBuffer.append(reinterpret_cast<const char*>(INVALID_OWNSHIP), sizeof(INVALID_OWNSHIP));
    bigBuffer.append(buildOBM());

    for (const auto &track : ctx.tracks) {
        bigBuffer.append(buildAB2Message(track));
    }

    bigBuffer.append(reinterpret_cast<const char*>(PADDING_BYTES), sizeof(PADDING_BYTES));

    QByteArray header = bigBuffer.left(3);
    QByteArray rest   = bigBuffer.mid(3);
    rest = negateData(rest);

    return header + rest;
}

QPair<uint8_t, uint8_t> encoderLPD::symbolFor(const Track &track) const {

    // Definimos todas las combinaciones conocidas
    static const QMap<QPair<Identity, QString>, QPair<uint8_t, uint8_t>> map = {
      {{Identity::ConfFriend,   "s"}, {0x1E, 0x00}},
      {{Identity::ConfFriend,   "a"}, {0x02, 0x00}},
      {{Identity::ConfFriend,   "b"}, {0x1A, 0x00}},
      {{Identity::ConfHostile,  "s"}, {0x1F, 0x00}},
      {{Identity::ConfHostile,  "a"}, {0x03, 0x00}},
      {{Identity::ConfHostile,  "b"}, {0x1B, 0x00}},
      {{Identity::EvalUnknown,  "s"}, {0x1D, 0xE0}},
      {{Identity::EvalUnknown,  "a"}, {0x01, 0xE0}},
      {{Identity::EvalUnknown,  "b"}, {0x19, 0xE0}},
      };

    QPair<Identity, QString> key(track.getIdentity(), TrackData::toQString(track.getIdentity()).toLower());

    if (map.contains(key))
        return map.value(key);
    return {0x1D, 0xE0}; // default Unknown-s
}

QByteArray encoderLPD::encodeCoordinate(double value, uint8_t idBits, bool AP, bool PV, bool LS) {
    QByteArray bytes;

    int32_t scaled = static_cast<int32_t>(qRound(value * 256.0));

    constexpr int bitCount = 17;
    constexpr int maxVal = (1 << (bitCount - 1)) - 1;
    constexpr int minVal = -(1 << (bitCount - 1));
    scaled = qBound(minVal, scaled, maxVal);

    uint32_t encoded = (scaled < 0)
                           ? ((1u << bitCount) + scaled)
                           : static_cast<uint32_t>(scaled);

    encoded <<= 7;
    uint8_t controlByte = 0;
    switch (idBits & 0x0F) {
    case AB2_ID_X:
        controlByte = BIT_V;
        if(PV)
            controlByte |= BIT_PV;
        if(LS)
            controlByte |= BIT_LS;
        break;
    case AB1_ID_X:
        controlByte = BIT_V;
        break;
    case AB2_ID_Y:
        controlByte = BIT_AP;
        break;
    default:
        controlByte = 0x00;
        break;
    }
    controlByte |= (idBits & 0x0F);
    encoded |= controlByte;

    // Empaquetar en 3 bytes
    bytes.append(static_cast<char>((encoded >> 16) & 0xFF));
    bytes.append(static_cast<char>((encoded >> 8) & 0xFF));
    bytes.append(static_cast<char>(encoded & 0xFF));

    return bytes;
}

QByteArray encoderLPD::buildSymbolBytes(const Track &track) const {
    QByteArray bytes;

    auto [sym1, sym2] = symbolFor(track);
    bytes.append(static_cast<char>(sym1));
    bytes.append(static_cast<char>(sym2));

    bytes.append(static_cast<char>(0x2B));
    bytes.append(static_cast<char>(0x00));
    bytes.append(static_cast<char>(0x00));

    QString octalId = QString("%1").arg(track.getId(), 4, 8, QChar('0'));
    for (QChar c : octalId) {
        bytes.append(static_cast<char>(c.toLatin1()));
    }

    bytes.append(static_cast<char>(0x00));
    bytes.append(static_cast<char>(0x00));
    bytes.append(static_cast<char>(EOMM));

    return bytes;
}

QByteArray encoderLPD::buildAB2Message(const Track &track) {
    QByteArray buffer;
    buffer.append(encodeCoordinate(track.getX(), AB2_ID_X));
    buffer.append(encodeCoordinate(track.getY(), AB2_ID_Y));
    buffer.append(buildSymbolBytes(track));
    return buffer;
}

QByteArray encoderLPD::buildOBM()
{
    QByteArray buffer;
    buffer.append(encodeCoordinate(obmHandler->getPosition().first, AB2_ID_X, true, true, true));
    buffer.append(encodeCoordinate(obmHandler->getPosition().second, AB2_ID_Y, true, true, true));
    buffer.append(static_cast<char>(0x0F));
    buffer.append(static_cast<char>(0x00));
    buffer.append(static_cast<char>(EOMM));
    return buffer;
}

QByteArray encoderLPD::negateData(const QByteArray &data) {
    QByteArray invertedData = data;
    for (char &b : invertedData)
        b = ~b;
    return invertedData;
}


