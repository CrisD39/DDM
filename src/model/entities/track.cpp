#include "track.h"
// No hace falta incluir enums.h aquí porque ya está incluido en track.h

// Solo implementamos la lógica "lenta" de formateo de texto
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
