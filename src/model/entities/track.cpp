#include "track.h"
#include "RadarMath.h"   // ajustá la ruta si está en utils/...

double Track::getAzimuthDeg() const
{
    // Todo se trabaja en Data Miles (DM)
    return RadarMath::azimuthDeg(static_cast<double>(m_x),
                                 static_cast<double>(m_y));
}

double Track::getDistanceDm() const
{
    // Todo se trabaja en Data Miles (DM)
    return RadarMath::distanceDm(static_cast<double>(m_x),
                                 static_cast<double>(m_y));
}

// Lógica "lenta" de formateo de texto
// Solo implementamos la lógica "lenta" de formateo de texto
void Track::updatePosition(double deltaTime)
{
    /*
    m_x += m_velocityX * (deltaTime / 3600.0);
    m_y += m_velocityY * (deltaTime / 3600.0);
    */
    if (m_speedKnots<= 0.0)
        return;

    // Distancia recorrida en Data Miles
    double distanceDM = m_speedKnots * (deltaTime / 3600.0);

    double courseRad = qDegreesToRadians(m_course);

    double dx = distanceDM * std::sin(courseRad);
    double dy = distanceDM * std::cos(courseRad);

    m_x += dx;
    m_y += dy;

    //qDebug()<< "[track.cpp] Posicion Actiulizada: ["<< m_x << ", " << m_y <<"]";
}

QString Track::toString() const
{
    return QStringLiteral("Track{id=%1, type=%2, identity=%3, mode=%4, pos=(%5, %6)}")
    .arg(m_id)
        .arg(TrackData::toQString(getType()))
        .arg(TrackData::toQString(getIdentity()))
        .arg(TrackData::toQString(getTrackMode()))
        .arg(double(m_x), 0, 'f', 2)
        .arg(double(m_y), 0, 'f', 2);
}
