#include "encoderlpd.h"
#include "CommandContext.h"
#include "qdebug.h"

#include <QByteArray>
#include <QtEndian>
#include <QtGlobal>

encoderLPD::encoderLPD() {}


QPair<uint8_t, uint8_t> encoderLPD::symbolFor(const Track &track) const {
    using TT = TrackType;

    // Definimos todas las combinaciones conocidas
    static const QMap<QPair<TT, QString>, QPair<uint8_t, uint8_t>> map = {
                                                                          {{TT::Friendly, "s"}, {0x1E, 0x00}},
                                                                          {{TT::Friendly, "a"}, {0x02, 0x00}},
                                                                          {{TT::Friendly, "b"}, {0x1A, 0x00}},
                                                                          {{TT::Enemy,    "s"}, {0x1F, 0x00}},
                                                                          {{TT::Enemy,    "a"}, {0x03, 0x00}},
                                                                          {{TT::Enemy,    "b"}, {0x1B, 0x00}},
                                                                          {{TT::Unknown,  "s"}, {0x1D, 0xE0}},
                                                                          {{TT::Unknown,  "a"}, {0x01, 0xE0}},
                                                                          {{TT::Unknown,  "b"}, {0x19, 0xE0}},
                                                                          };

    QPair<TT, QString> key(track.type, track.identity.toLower());
    if (map.contains(key))
        return map.value(key);
    return {0x1D, 0xE0}; // default Unknown-s
}

QByteArray encoderLPD::encodeCoordinate(double value, uint8_t idBits) {
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

    QString octalId = QString("%1").arg(track.id, 4, 8, QChar('0'));
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
    buffer.append(encodeCoordinate(track.x, AB2_ID_X));
    buffer.append(encodeCoordinate(track.y, AB2_ID_Y));
    buffer.append(buildSymbolBytes(track));
    return buffer;
}

QByteArray encoderLPD::buildFullMessage(const CommandContext &ctx) {
    QByteArray bigBuffer;

    bigBuffer.append(static_cast<char>(HEADER_BYTES[0]));
    bigBuffer.append(static_cast<char>(HEADER_BYTES[1]));
    int palabrasTracks = (ctx.tracks.size() * 6) + 2 + 9;
    bigBuffer.append(static_cast<char>(palabrasTracks & 0xFF));

    bigBuffer.append(encodeCoordinate(ctx.centerX, AB1_ID_X)); // V=1 y ID=1001
    bigBuffer.append(encodeCoordinate(ctx.centerY, AB1_ID_Y)); // ID=1011

    for (const auto &track : ctx.tracks) {
        bigBuffer.append(buildAB2Message(track));
    }

    bigBuffer.append(reinterpret_cast<const char*>(PADDING_BYTES), sizeof(PADDING_BYTES));

    QByteArray header = bigBuffer.left(3);
    QByteArray rest   = bigBuffer.mid(3);
    rest = negateData(rest);

    return header + rest;
}

QByteArray encoderLPD::negateData(const QByteArray &data) {
    QByteArray invertedData = data;
    for (char &b : invertedData)
        b = ~b;
    return invertedData;
}
