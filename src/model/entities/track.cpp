#include "Track.h"
#include <QMetaEnum>

// Constructor extendido
Track::Track(int id,
             Type type,
             Identity identity,
             TrackMode mode,
             double x,
             double y,
             double rumbo,
             double velocidad)
    : m_id(id),
    m_type(type),
    m_mode(mode),
    m_identity(identity),
    m_x(x),
    m_y(y),
    m_rumbo(rumbo),
    m_velocidad(velocidad)
{}

// Getters
int Track::getId() const { return m_id; }
TrackData::Type Track::getType() const { return m_type; }
TrackData::Identity Track::getIdentity() const { return m_identity; }
TrackData::TrackMode Track::getTrackMode() const { return m_mode; }
double Track::getX() const { return m_x; }
double Track::getY() const { return m_y; }
double Track::getRumbo() const { return m_rumbo; }             // NUEVO
double Track::getVelocidad() const { return m_velocidad; }     // NUEVO

// Setters
void Track::setId(int id) { m_id = id; }
void Track::setType(Type type) { m_type = type; }
void Track::setIdentity(Identity identity) { m_identity = identity; }
void Track::setTrackMode(TrackMode mode) { m_mode = mode; }
void Track::setX(double x) { m_x = x; }
void Track::setY(double y) { m_y = y; }
void Track::setRumbo(double r) { m_rumbo = r; }                // NUEVO
void Track::setVelocidad(double v) { m_velocidad = v; }        // NUEVO

// toString
template <typename Enum>
static inline QString enumToQString(Enum value) {
    const QMetaEnum me = QMetaEnum::fromType<Enum>();
    const char* key = me.valueToKey(static_cast<int>(value));
    return key ? QString::fromLatin1(key)
               : QString::number(static_cast<int>(value));
}

QString Track::toString() const
{
    using TD = TrackData;

    return QStringLiteral("Track{id=%1, type=%2, identity=%3, mode=%4, x=%5, y=%6, rumbo=%7, vel=%8}")
        .arg(m_id)
        .arg(enumToQString<TD::Type>(m_type))
        .arg(enumToQString<TD::Identity>(m_identity))
        .arg(enumToQString<TD::TrackMode>(m_mode))
        .arg(m_x, 0, 'f', 3)
        .arg(m_y, 0, 'f', 3)
        .arg(m_rumbo, 0, 'f', 1)
        .arg(m_velocidad, 0, 'f', 1);
}
