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
        bigBuffer.aapend(buildAB3Message(cursors));
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
                { Type::Air,         {0x02, 0x60} },
                { Type::Subsurface,  {0x1A, 0x60} },
            }
        },{
            Identity::PossFriend, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1D, 0x64} },
                { Type::Air,         {0x02, 0x64} },
                { Type::Subsurface,  {0x1A, 0x64} },
            }
        },{
            Identity::PossHostile, QHash<Type, QPair<uint8_t,uint8_t>>{
                { Type::Surface,     {0x1D, 0x7C} },
                { Type::Air,         {0x02, 0x7C} },
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

QByteArray encoderLPD::encodeAngle(double value, unint8_t idBits, bool Ap, bool PV, bool LS)
{

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

    buffer.append(encodeCoordinate(cursor.getCoordinates().first, AB3_ID_X));
    buffer.append(encodeCoordinate(cursor.getCoordinates().second, AB3_ID_Y));
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

QByteArray encoderLPD::buildOwnCurs()
{
    QByteArray buffer;
    buffer.append(enc)

}

QByteArray encoderLPD::negateData(const QByteArray &data) {
    QByteArray invertedData = data;
    for (char &b : invertedData)
        b = ~b;
    return invertedData;
}


