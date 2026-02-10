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
