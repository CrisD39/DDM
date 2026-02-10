#include "track.h"
// No hace falta incluir enums.h aquí porque ya está incluido en track.h

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
    // Usamos los helpers estáticos que definiste en enums.h
    // Nota: Como estamos dentro de la clase, podemos acceder a los getters o miembros.

    return QStringLiteral("Track{id=%1, type=%2, identity=%3, mode=%4, pos=(%5, %6)}")
        .arg(m_id)
        .arg(TrackData::toQString(getType()))       // Usamos tu helper de enums.h
        .arg(TrackData::toQString(getIdentity()))
        .arg(TrackData::toQString(getTrackMode()))
        .arg(double(m_x), 0, 'f', 2)                // float -> double para el print
        .arg(double(m_y), 0, 'f', 2);
}
