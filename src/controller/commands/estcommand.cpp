#include "estCommand.h"
#include "../../model/commandContext.h"
#include "../../model/entities/stationEntity.h"
#include "../../model/entities/track.h"
#include <QtMath>
#include <QVector2D>
#include <QDebug>

// Helper numérico
static bool parseDouble(const QString& s, double& out) {
    bool ok = false;
    const double v = s.toDouble(&ok);
    if (ok) out = v;
    return ok;
}

// Helper para formatear tiempo (segundos) a HH:MM:SS
static QString formatTimeHMS(double seconds) {
    if (seconds < 0) seconds = 0;
    qint64 total = qRound64(seconds);
    const qint64 h = total / 3600;
    total %= 3600;
    const qint64 m = total / 60;
    const qint64 s = total % 60;
    return QString("%1:%2:%3")
        .arg(h, 2, 10, QLatin1Char('0'))
        .arg(m, 2, 10, QLatin1Char('0'))
        .arg(s, 2, 10, QLatin1Char('0'));
}

// =========================== EXECUTE ===============================

CommandResult EstCommand::execute(const CommandInvocation& inv,
                                  CommandContext& ctx) const
{
    // Esperamos:
    // STA <slot> <trkA> <trkB> <az> <dist> <VD|DU> <valor>
    if (inv.args.size() != 7) {
        return {false, "Cantidad de argumentos invalida. Uso: " + usage()};
    }

    bool ok = false;

    // 1) Slot
    int slotId = inv.args[0].toInt(&ok);
    if (!ok || slotId < 1 || slotId > 10) {
        return {false, "Slot invalido (debe estar entre 1 y 10)."};
    }

    // 2) Tracks
    int trkAId = inv.args[1].toInt(&ok);
    if (!ok) return {false, "Track-A invalido (no es un entero)."};

    int trkBId = inv.args[2].toInt(&ok);
    if (!ok) return {false, "Track-B invalido (no es un entero)."};

    const Track* trkA = ctx.findTrackById(trkAId);
    if (!trkA) return {false, "Track-A no encontrado en el contexto."};

    const Track* trkB = ctx.findTrackById(trkBId);
    if (!trkB) return {false, "Track-B no encontrado en el contexto."};

    // 3) AZ / DT
    double az = 0.0;
    double distMn = 0.0;

    if (!parseDouble(inv.args[3], az)) {
        return {false, "Azimut invalido (no es numero)."};
    }
    if (!parseDouble(inv.args[4], distMn)) {
        return {false, "Distancia invalida (no es numero)."};
    }
    if (distMn < 0.0) {
        return {false, "La distancia no puede ser negativa."};
    }

    // Normalizar AZ a [0, 360)
    while (az < 0.0)   az += 360.0;
    while (az >= 360.) az -= 360.0;

    // 4) Modo VD / DU
    const QString modoStr = inv.args[5].trimmed().toUpper();

    double valor = 0.0;
    if (!parseDouble(inv.args[6], valor)) {
        return {false, "Valor numérico invalido (VD/DU)."};
    }

    StationEntity st(slotId, trkAId, trkBId, az, distMn);

    if (modoStr == "VD") {
        if (valor <= 0.0) {
            return {false, "Para modo VD la velocidad debe ser > 0."};
        }
        st.esModoVD = true;
        st.velocidadRequerida = valor;  // nudos

        // el tiempo lo vamos a calcular más abajo
        st.tiempoResultado = 0.0;
    }
    else if (modoStr == "DU") {
        if (valor <= 0.0) {
            return {false, "Para modo DU el tiempo debe ser > 0."};
        }
        st.esModoVD = false;
        // El usuario ingresa DURACIÓN en minutos por CLI
        // la almacenamos inicialmente en segundos
        st.tiempoResultado = valor * 60.0;
        st.velocidadRequerida = 0.0;    // la calculamos más abajo
    }
    else {
        return {false, "Modo invalido. Use VD (velocidad) o DU (duracion)."};
    }

    // 5) Resolver cinemática con datos reales
    resolverCinematica(st, *trkA, *trkB);

    // 6) Guardar slot en el contexto para que la GUI lo use
    ctx.setStationingSlot(st);

    // 7) Mensaje para CLI
    QString msg;
    if (st.esModoVD) {
        msg = QString("STA[Slot %1] A=%2 B=%3 AZ=%4 DT=%5mn  -> Rumbo %6°, Tiempo %7")
                  .arg(st.slotId)
                  .arg(st.trackA)
                  .arg(st.trackB)
                  .arg(st.azimut, 0, 'f', 1)
                  .arg(st.distancia, 0, 'f', 2)
                  .arg(st.rumboResultado, 0, 'f', 1)
                  .arg(formatTimeHMS(st.tiempoResultado));
    } else {
        msg = QString("STA[Slot %1] A=%2 B=%3 AZ=%4 DT=%5mn  -> Rumbo %6°, Velocidad %7 kn, Tiempo %8")
                  .arg(st.slotId)
                  .arg(st.trackA)
                  .arg(st.trackB)
                  .arg(st.azimut, 0, 'f', 1)
                  .arg(st.distancia, 0, 'f', 2)
                  .arg(st.rumboResultado, 0, 'f', 1)
                  .arg(st.velocidadRequerida, 0, 'f', 1)
                  .arg(formatTimeHMS(st.tiempoResultado));
    }

    ctx.out << msg << Qt::endl;
    return {true, msg};
}

// ====================== CINEMÁTICA GEOMÉTRICA ======================
//
// Esta versión estaciona Track-A en una posición fija respecto de Track-B,
// definida por AZ/DT. No modela todavía el movimiento futuro del guía.
//

void EstCommand::resolverCinematica(StationEntity& ent,
                                    const Track& trkA,
                                    const Track& trkB) const
{
    // 1) Posiciones actuales en XY
    const QVector2D posA(trkA.getX(), trkA.getY());
    const QVector2D posB(trkB.getX(), trkB.getY());

    // 2) Posicion destino del estacionamiento:
    //    P_dest = P_B + rot(AZ) * Dist
    //
    //    Convención: 0° = norte(+Y), 90° = este(+X)
    //
    const double azRad = qDegreesToRadians(ent.azimut);
    const QVector2D offset(qSin(azRad) * ent.distancia,
                           qCos(azRad) * ent.distancia);
    const QVector2D posDest = posB + offset;

    // 3) Vector desde A hacia la posición destino
    const QVector2D delta = posDest - posA;
    const double dist = delta.length();

    if (dist < 1e-6) {
        // Ya está prácticamente estacionado; usamos rumbo actual de A
        ent.rumboResultado = trkA.getRumbo();
        if (ent.esModoVD) {
            // si VD, con dist ~0 el tiempo es 0
            ent.tiempoResultado = 0.0;
        } else {
            // si DU, ya tenemos tiempo, la vel requerida es 0
            ent.velocidadRequerida = 0.0;
        }
        return;
    }

    // 4) Rumbo necesario para ir de A a la posición destino
    //
    //    Usamos la misma convención que antes:
    //    - atan2(y, x) da el ángulo desde el eje +X (este)
    //    - convertimos a rumbo 0°=norte, 90°=este:
    //
    double angMath = std::atan2(delta.y(), delta.x());
    double rumbo = 90.0 - qRadiansToDegrees(angMath);
    while (rumbo < 0.0)   rumbo += 360.0;
    while (rumbo >= 360.) rumbo -= 360.0;

    ent.rumboResultado = rumbo;

    // 5) Modo VD: tenemos velocidad, calculamos tiempo.
    //    Distancia en millas náuticas, velocidad en nudos (mn/h).
    //
    if (ent.esModoVD) {
        const double vel = ent.velocidadRequerida;  // nudos
        if (vel <= 0.0) {
            ent.tiempoResultado = 0.0;
            return;
        }
        const double tiempoHoras = dist / vel;       // h = mn / (mn/h)
        ent.tiempoResultado = tiempoHoras * 3600.0; // segundos
    }
    // 6) Modo DU: tenemos tiempo, calculamos velocidad requerida.
    else {
        const double tiempoSeg = ent.tiempoResultado;
        const double tiempoHoras = tiempoSeg / 3600.0;
        if (tiempoHoras <= 0.0) {
            ent.velocidadRequerida = 0.0;
            return;
        }
        const double vel = dist / tiempoHoras;    // nudos
        ent.velocidadRequerida = vel;
    }
}
