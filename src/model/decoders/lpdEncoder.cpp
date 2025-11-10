#include "lpdEncoder.h"
#include "commandContext.h"
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


// ID de palabra para "longitud de cursor" (según planilla: 0b0111)
static constexpr uint32_t AB3_ID_CURSOR_LONG = 0x7;

QByteArray encoderLPD::encodeCursorLong(double rho, int type)
{
    QByteArray bytes;

    if (rho < 0.0) rho = 0.0;

    // Escala que venías usando (planilla): 512*4 = 2048
    const uint32_t q = static_cast<uint32_t>(qRound(rho * 256));

    // ρ ocupa 16 bits en [22..7]
    const uint32_t len16 = (q & 0xFFFFu);

    // type en [6..4]
    const uint32_t type3 = static_cast<uint32_t>(qBound(0, type, 7)) & 0x7u;

    // [23]=0 | [22..7]=len16 | [6..4]=type3 | [3..0]=ID(0111)
    const uint32_t word24 = (len16 << 7) | (type3 << 4) | AB3_ID_CURSOR_LONG;

    bytes.append(static_cast<char>((word24 >> 16) & 0xFF)); // [23..16]
    bytes.append(static_cast<char>((word24 >>  8) & 0xFF)); // [15..8]
    bytes.append(static_cast<char>( word24        & 0xFF)); // [7..0]

    // // ---- DEBUG opcional: bits por byte + campos decodificados ----
    // auto bin8 = [](quint8 v){ return QString("%1").arg(v, 8, 2, QChar('0')); };
    // const quint8 b0 = static_cast<quint8>(bytes[0]);
    // const quint8 b1 = static_cast<quint8>(bytes[1]);
    // const quint8 b2 = static_cast<quint8>(bytes[2]);

    // const uint32_t dec_len16 = (word24 >> 7) & 0xFFFFu;
    // const uint32_t dec_type3 = (word24 >> 4) & 0x7u;   // <-- [6..4]
    // const uint32_t dec_id4   =  word24       & 0xFu;   // <-- [3..0]

    // qDebug().noquote()
    //     << "\nbytes (bits MSB→LSB):"
    //     << "\n  [23..16] " << bin8(b0)
    //     << "\n  [15..8 ] " << bin8(b1)
    //     << "\n  [ 7..0 ] " << bin8(b2)
    //     << "\n  join     " << bin8(b0) << " " << bin8(b1) << " " << bin8(b2)
    //     << "\n  len16=" << dec_len16 << " type3=" << dec_type3 << " id4=" << dec_id4;

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
    buffer.append(encodeCoordinate(track.getX(), AB2_ID_X));
    buffer.append(encodeCoordinate(track.getY(), AB2_ID_Y));
    buffer.append(buildSymbolBytes(track));
    return buffer;
}

QByteArray encoderLPD::buildAB3Message(const CursorEntity &cursor)
{
    QByteArray buffer;
    buffer.append(encodeAngle(cursor.getCursorAngle(), true, cursor.isActive()  ));
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


