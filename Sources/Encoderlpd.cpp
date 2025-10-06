#include "encoderlpd.h"
#include "CommandContext.h"
#include "qdebug.h"

#include <QByteArray>
#include <QtEndian>
#include <QtGlobal>


// =====================================================
// encoderLPD
// =====================================================

encoderLPD::encoderLPD() {}

// -----------------------------------------------------
// Mapea el tipo e identidad del Track al par de bytes del símbolo
// -----------------------------------------------------
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

    // Búsqueda segura: default Unknown-s si no se encuentra
    QPair<TT, QString> key(track.type, track.identity.toLower());
    if (map.contains(key))
        return map.value(key);
    return {0x1D, 0xE0}; // default Unknown-s
}

// -----------------------------------------------------
// Codifica coordenada (17 bits complemento a dos + ID bits)
// -----------------------------------------------------
QByteArray encoderLPD::encodeCoordinate(double value, uint8_t idBits) {
    QByteArray bytes;

    // 1. Escalar con resolución 1/256
    int32_t scaled = static_cast<int32_t>(qRound(value * 256.0));

    // 2. Limitar a 17 bits en complemento a dos
    constexpr int bitCount = 17;
    constexpr int maxVal = (1 << (bitCount - 1)) - 1;
    constexpr int minVal = -(1 << (bitCount - 1));
    scaled = qBound(minVal, scaled, maxVal);

    // 3. Convertir a complemento a dos
    uint32_t encoded = (scaled < 0)
                           ? ((1u << bitCount) + scaled)
                           : static_cast<uint32_t>(scaled);

    // 4. Desplazar 7 bits para dejar lugar al byte de control
    encoded <<= 7;

    // 5. Construir el byte de control según el ID
    uint8_t controlByte = 0;
    if (idBits == AB2_ID_X) {
        // Coordenada X → LS, PV y V en 1
        controlByte = BIT_LS | BIT_PV | BIT_V;   // 0x70
    } else if (idBits == AB2_ID_Y) {
        // Coordenada Y → solo AP en 1
        controlByte = BIT_AP;                    // 0x08
    }

    // 6. Agregar los 4 bits de ID (parte baja)
    controlByte |= (idBits & 0x0F);

    // 7. Insertar byte de control
    encoded |= controlByte;

    // 8. Empaquetar en 3 bytes (big-endian)
    bytes.append(static_cast<char>((encoded >> 16) & 0xFF));
    bytes.append(static_cast<char>((encoded >> 8) & 0xFF));
    bytes.append(static_cast<char>(encoded & 0xFF));

    return bytes;
}



// -----------------------------------------------------
// Construye los 12 bytes del símbolo AB2
// -----------------------------------------------------
QByteArray encoderLPD::buildSymbolBytes(const Track &track) const {
    QByteArray bytes;

    // 1. Dos primeros bytes: símbolo según type + identity
    auto [sym1, sym2] = symbolFor(track);
    bytes.append(static_cast<char>(sym1));
    bytes.append(static_cast<char>(sym2));

    // 2. Tres bytes fijos (por ahora desconocidos)
    bytes.append(static_cast<char>(0x2B));
    bytes.append(static_cast<char>(0x00));
    bytes.append(static_cast<char>(0x00));

    // 3. Cuatro bytes: ID del track en octal ASCII
    QString octalId = QString("%1").arg(track.id, 4, 8, QChar('0')); // ej. "0031"
    for (QChar c : octalId) {
        bytes.append(static_cast<char>(c.toLatin1()));
    }

    // 4. Dos bytes nulos (según estructura base)
    bytes.append(static_cast<char>(0x00));
    bytes.append(static_cast<char>(0x00));

    // 5. Byte final EOMM
    bytes.append(static_cast<char>(EOMM));

    return bytes;
}

// -----------------------------------------------------
// Codifica un Track en formato AB2
// -----------------------------------------------------
QByteArray encoderLPD::buildAB2Message(const Track &track) {
    QByteArray buffer;
    buffer.append(encodeCoordinate(track.x, AB2_ID_X));
    buffer.append(encodeCoordinate(track.y, AB2_ID_Y));
    buffer.append(buildSymbolBytes(track));
    return buffer;
}

// -----------------------------------------------------
// Construye el mensaje completo (Header + Descentrado + Tracks)
// -----------------------------------------------------
QByteArray encoderLPD::buildFullMessage(const CommandContext &ctx) {
    QByteArray bigBuffer;

    bigBuffer.append(static_cast<char>(HEADER_BYTES[0]));
    bigBuffer.append(static_cast<char>(HEADER_BYTES[1]));
    int palabrasTracks = ctx.tracks.size() * 6;
    bigBuffer.append(static_cast<char>(palabrasTracks & 0xFF));

    bigBuffer.append(reinterpret_cast<const char*>(DESCENTRADO_BYTES), sizeof(DESCENTRADO_BYTES));

    for (const auto &track : ctx.tracks) {
        bigBuffer.append(buildAB2Message(track));
    }

    QByteArray header = bigBuffer.left(3);
    QByteArray rest   = bigBuffer.mid(3);
    //qDebug() << rest.toHex(' ');
    rest = negateData(rest);


    return header + rest;
}

// -----------------------------------------------------
// Bitwise NOT
// -----------------------------------------------------
QByteArray encoderLPD::negateData(const QByteArray &data) {
    QByteArray invertedData = data;
    for (char &b : invertedData)
        b = ~b;
    return invertedData;
}
