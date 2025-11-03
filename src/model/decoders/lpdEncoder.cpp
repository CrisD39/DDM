#include "lpdEncoder.h"
#include "commandContext.h"
#include <QByteArray>
#include <QtEndian>
#include <QtGlobal>

encoderLPD::encoderLPD() {}

QByteArray encoderLPD::buildFullMessage(const CommandContext &ctx) {
    QByteArray bigBuffer;

    const int words_tracks     = static_cast<int>(ctx.tracks.size()) * 6;
    const int words_center     = 2;
    const int words_obm        = 3;
    const int words_cursors    = static_cast<int>(ctx.cursors.size()) * 4;
    const int words_ownership  = static_cast<int>(sizeof(INVALID_OWNSHIP) / 3);
    const int words_padding    = static_cast<int>(sizeof(PADDING_BYTES) / 3);

    int palabras = words_center + words_ownership + words_obm
                   + words_tracks + words_cursors + words_padding;


    bigBuffer.append(static_cast<char>(HEADER_BYTES[0]));
    bigBuffer.append(static_cast<char>(HEADER_BYTES[1]));
    bigBuffer.append(static_cast<char>(palabras & 0xFF));


    //center

    bigBuffer.append(encodeCoordinate(ctx.centerX, AB1_ID_X)); // V=1 y ID=1001
    bigBuffer.append(encodeCoordinate(ctx.centerY, AB1_ID_Y)); // ID=1011


    bigBuffer.append(reinterpret_cast<const char*>(INVALID_OWNSHIP), sizeof(INVALID_OWNSHIP));
    bigBuffer.append(buildOBM());

    for (const auto &track : ctx.tracks) {
        bigBuffer.append(buildAB2Message(track));
    }

    //cursores
    for (const auto &cursors : ctx.cursors){
        bigBuffer.append(buildAB3Message(cursors));
    }


    bigBuffer.append(reinterpret_cast<const char*>(PADDING_BYTES), sizeof(PADDING_BYTES));

    QByteArray header = bigBuffer.left(3);
    QByteArray rest   = bigBuffer.mid(3);
    rest = negateData(rest);

    return header + rest;


}

QPair<uint8_t, uint8_t> encoderLPD::symbolFor(const Track& track) const
{
    const Type t = track.getType();
    const Identity id = track.getIdentity();

    static const QHash<Identity, QHash<Type, QPair<uint8_t,uint8_t>>> LUT = {
        {
            Identity::Pending, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1D, 0x60} },
                { Type::Air,         {0x01, 0x60} },
                { Type::Subsurface,  {0x1A, 0x60} },
            }
        },{
            Identity::PossFriend, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1D, 0x64} },
                { Type::Air,         {0x01, 0x64} },
                { Type::Subsurface,  {0x1A, 0x64} },
            }
        },{
            Identity::PossHostile, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1D, 0x7C} },
                { Type::Air,         {0x01, 0x7C} },
                { Type::Subsurface,  {0x1A, 0x7C} },
            }
        },{
            Identity::ConfFriend, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1E, 0x00} },
                { Type::Air,         {0x02, 0x00} },
                { Type::Subsurface,  {0x1A, 0x00} },
            }
        },{
            Identity::ConfHostile, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1F, 0x00} },
                { Type::Air,         {0x03, 0x00} },
                { Type::Subsurface,  {0x1B, 0x00} },
            }
        },{
            Identity::EvalUnknown, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1D, 0x00} },
                { Type::Air,         {0x01, 0x00} },
                { Type::Subsurface,  {0x19, 0x00} },
            }
        },{
            Identity::Heli, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x0A, 0x00} },
                { Type::Air,         {0x0A, 0x00} },
                { Type::Subsurface,  {0x0A, 0x00} },
                }
        },
    };

    // Lookup
    const auto itId = LUT.find(id);
    if (itId != LUT.end()) {
        const auto& byType = itId.value();
        const auto itT = byType.find(t);
        if (itT != byType.end()) {
            return itT.value();
        }
    }

    // Fallback: "Evaluated unknown – Surface" (era tu default {0x1D,0xE0})
    return {0x1D, 0xE0};
}

uint8_t encoderLPD::trackModeFor(const Track &track) const {

    // Definimos todas las combinaciones conocidas
    static const QMap<TrackMode,uint8_t> map = {
        {TrackMode::FC1, 0x31},
        {TrackMode::FC2, 0x32},
        {TrackMode::FC3, 0x33},
        {TrackMode::FC4, 0x34},
        {TrackMode::FC5, 0x35},
        {TrackMode::Auto, 0x2E},
        {TrackMode::TentativeAuto, 0x08},
        {TrackMode::AutomaticLost, 0x11},
        {TrackMode::RAM, 0x2B},
        {TrackMode::DR, 0x2D},
        {TrackMode::Lost, 0x1E},
        {TrackMode::Blank, 0x00},
    };

    TrackMode tm = track.getTrackMode();

    if (map.contains(tm))
        return map.value(tm);
    return 0x01;
}

//tengo que guardar todos los booleanos en el Cursor entity porque son muchos parametros
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
    case AB3_ID_X:
        controlByte = 0x00;
    case AB3_ID_Y:
        controlByte = 0x00;
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


QByteArray encoderLPD::encodeAngle(double value, bool e, bool v)
{
    QByteArray bytes;

    // Normalizar a [0,360)
    double a = std::fmod(value, 360.0);
    if (a < 0.0) a += 360.0;

    // 12 bits para el ángulo: LSB = 360/4096
    // Nota: el & 0x0FFF ya envuelve 4096 -> 0 si redondea justo 360°
    uint32_t code12 = static_cast<uint32_t>(qRound(a * (4096.0 / 360.0))) & 0x0FFF;

    // Byte/bits de control: ID en el nibble bajo + flags E/V arriba
    uint32_t control = AB3_ID_ANGLE;          // .... .... .... 0101
    if (e) control |= BIT_E;                  // ej: 0x40 → bit 6
    if (v) control |= BIT_V;                  // ej: 0x20 → bit 5

    // Empaquetar: ángulo en [23..12], control en [11..0] (nos interesa [7..0])
    uint32_t encoded = (code12 << 12) | (control & 0x0FFF);

    // MSB primero
    bytes.append(static_cast<char>((encoded >> 16) & 0xFF));
    bytes.append(static_cast<char>((encoded >> 8)  & 0xFF));
    bytes.append(static_cast<char>( encoded        & 0xFF));

    return bytes;
}


QByteArray encoderLPD::encodeCursorLong(double dm, int type)
{
    QByteArray bytes;

    // ρ ≥ 0, Q8.8 (LSB = 1/256)
    if (dm < 0.0) dm = 0.0;
    uint32_t code16 = static_cast<uint32_t>(qRound(dm * 256.0));
    if (code16 > 0xFFFFu) code16 = 0xFFFFu;

    // Usamos solo 12 bits para ubicar ρ en [19..8]
    const uint32_t code12 = (code16 & 0x0FFFu);

    // tipo 0..7  → 000..111 (saturado y enmascarado a 3 bits)
    const uint32_t type3 = static_cast<uint32_t>(qBound(0, type, 7)) & 0x7u;

    // Construcción de la palabra de 24 bits
    uint32_t encoded = 0;
    encoded |= (0x7u << 20);   // [23..20] = 0111  (garantiza bit 23 = 0)
    encoded |= (code12 << 8);  // [19..8]  = ρ(12 bits)
    // [7..4] = 0000
    // [3]    = 0
    encoded |= type3;          // [2..0]   = tipo

    // Empaquetado MSB-first
    bytes.append(static_cast<char>((encoded >> 16) & 0xFF));
    bytes.append(static_cast<char>((encoded >> 8)  & 0xFF));
    bytes.append(static_cast<char>( encoded        & 0xFF));
    return bytes;
}



QByteArray encoderLPD::buildSymbolBytes(const Track &track) const {
    QByteArray bytes;

    auto [sym1, sym2] = symbolFor(track);
    bytes.append(static_cast<char>(sym1));
    bytes.append(static_cast<char>(sym2));

    bytes.append(trackModeFor(track));

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
    buffer.append(buildSymbolBytes(track));
    buffer.append(encodeCoordinate(track.getX(), AB2_ID_X));
    buffer.append(encodeCoordinate(track.getY(), AB2_ID_Y));
    return buffer;
}

QByteArray encoderLPD::buildAB3Message(const CursorEntity &cursor)
{
    QByteArray buffer;
    buffer.append(encodeAngle(cursor.getCursorAngle(), true, true));
    buffer.append(encodeCursorLong(cursor.getCursorLength(), cursor.getLineType()));
    buffer.append(encodeCoordinate(cursor.getCoordinates().first,  AB3_ID_X));
    buffer.append(encodeCoordinate(cursor.getCoordinates().second, AB3_ID_Y));
    return buffer; // ← FALTABA ESTO
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


