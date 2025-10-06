#include "encoderlpd.h"
#include "CommandContext.h"

#include <QByteArray>
#include <QtEndian> // para qToBigEndian / qToLittleEndian si querés

encoderLPD::encoderLPD() {}

QByteArray encoderLPD::buildAB2Message(const Track &track) {
    QByteArray buffer;

    // 1) Coordenada X en 3 bytes
    int32_t xInt = static_cast<int32_t>(track.x);
    buffer.append(static_cast<char>((xInt >> 16) & 0xFF));
    buffer.append(static_cast<char>((xInt >> 8) & 0xFF));
    buffer.append(static_cast<char>(xInt & 0xFF));

    // 2) Coordenada Y en 3 bytes
    int32_t yInt = static_cast<int32_t>(track.y);
    buffer.append(static_cast<char>((yInt >> 16) & 0xFF));
    buffer.append(static_cast<char>((yInt >> 8) & 0xFF));
    buffer.append(static_cast<char>(yInt & 0xFF));

    // 3) Secuencia fija de 12 bytes (símbolo)
    static const uint8_t symbolBytes[12] = {
        0x1D, 0xE0, 0x2B, 0x00, 0x00,
        0x31, 0x31, 0x30, 0x30,
        0x00, 0x00, 0x17
    };

    buffer.append(reinterpret_cast<const char*>(symbolBytes), sizeof(symbolBytes));

    return buffer;
}

QByteArray encoderLPD::buildFullMessage(const CommandContext &ctx) {
    QByteArray bigBuffer;

    // Cantidad de palabras: cada track son 6 palabras (18/3)
    int palabrasTracks = ctx.tracks.size() * 6;

    // Encabezado de 3 bytes (00 00 <cantidad>)
    bigBuffer.append(static_cast<char>(0x00));
    bigBuffer.append(static_cast<char>(0x00));
    bigBuffer.append(static_cast<char>(palabrasTracks & 0xFF));

    // Concatenar todos los tracks
    for (const auto &track : ctx.tracks) {
        bigBuffer.append(buildAB2Message(track)); // <- este ya es miembro
    }

    // Append final fijo (6 bytes)
    static const uint8_t footer[6] = { 0x00, 0x00, 0x19, 0x00, 0x00, 0x0B };
    bigBuffer.append(reinterpret_cast<const char*>(footer), sizeof(footer));

    return bigBuffer;
}

QByteArray encoderLPD::negateData(const QByteArray &data) {
    QByteArray invertedData = data;
    for (int i = 0; i < invertedData.size(); ++i) {
        invertedData[i] = ~invertedData[i];
    }
    return invertedData;
}
