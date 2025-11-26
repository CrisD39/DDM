// Track.cpp
#include "Track.h"

// Ctor completo
Track::Track(
    int id,
    Type type,
    Identity identity,
    TrackMode mode,
    double x,
    double y
    ): m_id(id), m_type(type), m_mode(mode), m_identity(identity), m_x(x), m_y(y)
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Track::actualizarPos);
}

// Getters
int Track::getId() const { return m_id; }
TrackData::Type Track::getType() const { return m_type;}
TrackData::Identity Track::getIdentity() const { return m_identity; }
TrackData::TrackMode Track::getTrackMode() const { return m_mode; }
double Track::getX() const { return m_x; }
double Track::getY() const { return m_y; }

// Setters
void Track::setId(int id) { m_id = id; }
void Track::setType(Type type) { m_type = type; }
void Track::setIdentity(Identity identity) { m_identity = identity; }
void Track::setTrackMode(TrackMode mode) { m_mode = mode; }
void Track::setX(double x) { m_x = x; }
void Track::setY(double y) { m_y = y; }

double Track::setCourse(double c)
{
    m_course = c;
}

double Track::setVelocity(double v)
{
    if(!m_timer->isActive()){
        m_timer->stop();
    }
    m_timer->start(INTERVAL);

}

void Track::actualizarPos()
{
    QPair<double, double> = {getX(), getY()};

    if(m_course > 0.0 && m_course < 360.0 && m_velocity >=0){
        double dtSec = INTERVAL / 1000.0;  // milisegundos → segundos
        double theta = qDegreesToRadians(90.0 - rumbo);
        double new_x = getX() + m_velocity * dtSec * std::cos(theta);
        double new_y = getY() + m_velocity * dtSec * std::sin(theta);
        setX(new_x);
        setY(new_y);
    }
}

template <typename Enum>
static inline QString enumToQString(Enum value) {
    const QMetaEnum me = QMetaEnum::fromType<Enum>();   // requiere Q_ENUM / Q_ENUM_NS
    const char* key = me.valueToKey(static_cast<int>(value));
    return key ? QString::fromLatin1(key)
               : QString::number(static_cast<int>(value));
}

QString Track::toString() const
{
    using TD = TrackData; // atajo

    return QStringLiteral("Track{id=%1, type=%2, identity=%3, mode=%4, x=%5, y=%6}")
        .arg(m_id)
        .arg(enumToQString<TD::Type>(m_type))
        .arg(enumToQString<TD::Identity>(m_identity))
        .arg(enumToQString<TD::TrackMode>(m_mode))
        .arg(m_x, 0, 'f', 3)
        .arg(m_y, 0, 'f', 3);
}
