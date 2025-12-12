#include "estCommand.h"
#include "../../model/commandContext.h"
#include "../../model/entities/stationEntity.h"
#include "../../model/entities/track.h"

#include <QtMath>
#include <QVector2D>

// ======================= HELPERS ===================================

static bool parseDouble(const QString& s, double& out)
{
    bool ok = false;
    const double v = s.toDouble(&ok);
    if (ok) out = v;
    return ok;
}

static QString formatTimeHMS(double seconds)
{
    if (seconds < 0.0) seconds = 0.0;

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
//
// STA <slot> <trkA> <trkB> <az_rel> <dist_DM> <VD|DU> <valor>
//

CommandResult EstCommand::execute(const CommandInvocation& inv,
                                  CommandContext& ctx) const
{
    if (inv.args.size() != 7) {
        return {false, "Cantidad de argumentos invalida. Uso: " + usage()};
    }

    bool ok = false;

    // 1) Slot
    const int slotId = inv.args[0].toInt(&ok);
    if (!ok || slotId < 1 || slotId > 10) {
        return {false, "Slot invalido (1..10)."};
    }

    // 2) Tracks
    const int trkAId = inv.args[1].toInt(&ok);
    if (!ok) return {false, "Track-A invalido."};

    const int trkBId = inv.args[2].toInt(&ok);
    if (!ok) return {false, "Track-B invalido."};

    const Track* trkA = ctx.findTrackById(trkAId);
    if (!trkA) return {false, "Track-A no encontrado."};

    const Track* trkB = ctx.findTrackById(trkBId);
    if (!trkB) return {false, "Track-B no encontrado."};

    // 3) AZ relativo / Distancia en DM
    double azRel = 0.0;
    double distDm = 0.0;

    if (!parseDouble(inv.args[3], azRel)) {
        return {false, "Azimut relativo invalido."};
    }
    if (!parseDouble(inv.args[4], distDm) || distDm < 0.0) {
        return {false, "Distancia (DM) invalida."};
    }

    while (azRel < 0.0)   azRel += 360.0;
    while (azRel >= 360.) azRel -= 360.0;

    // 4) Modo VD / DU
    const QString modoStr = inv.args[5].trimmed().toUpper();

    double valor = 0.0;
    if (!parseDouble(inv.args[6], valor) || valor <= 0.0) {
        return {false, "Valor VD/DU invalido."};
    }

    StationEntity st(slotId, trkAId, trkBId, azRel, distDm);

    if (modoStr == "VD") {
        st.esModoVD = true;
        st.velocidadRequerida = valor;   // DM/h
        st.tiempoResultado = 0.0;
    }
    else if (modoStr == "DU") {
        st.esModoVD = false;
        st.tiempoResultado = valor * 60.0; // minutos -> segundos
        st.velocidadRequerida = 0.0;
    }
    else {
        return {false, "Modo invalido. Use VD o DU."};
    }

    // 5) Resolver cinemática
    resolverCinematica(st, *trkA, *trkB);

    // 6) Guardar slot
    ctx.setStationingSlot(st);

    // 7) Salida CLI
    QString msg;
    if (st.esModoVD) {
        msg = QString(
                  "STA[%1] A=%2 B=%3 AZrel=%4° DT=%5DM -> Rumbo %6°, Tiempo %7")
                  .arg(st.slotId)
                  .arg(st.trackA)
                  .arg(st.trackB)
                  .arg(st.azimut, 0, 'f', 1)
                  .arg(st.distancia, 0, 'f', 2)
                  .arg(st.rumboResultado, 0, 'f', 1)
                  .arg(formatTimeHMS(st.tiempoResultado));
    }
    else {
        msg = QString(
                  "STA[%1] A=%2 B=%3 AZrel=%4° DT=%5DM -> Rumbo %6°, Vel %7 DM/h, Tiempo %8")
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
// Estaciona Track-A en una posición fija respecto de Track-B.
// AZ es relativo al rumbo del Track-B.
//

void EstCommand::resolverCinematica(StationEntity& ent,
                                    const Track& trkA,
                                    const Track& trkB) const
{
    // Todo en DM / DM-h / segundos

    const QVector2D posA(trkA.getX(), trkA.getY());
    const QVector2D posB(trkB.getX(), trkB.getY());

    // AZ absoluto = rumbo del guía + AZ relativo
    double azAbs = trkB.getRumbo() + ent.azimut;
    while (azAbs < 0.0)   azAbs += 360.0;
    while (azAbs >= 360.) azAbs -= 360.0;

    const double azRad = qDegreesToRadians(azAbs);

    const QVector2D offset(
        qSin(azRad) * ent.distancia,
        qCos(azRad) * ent.distancia
        );

    const QVector2D posDest = posB + offset;
    const QVector2D delta   = posDest - posA;
    const double distDm     = delta.length();

    if (distDm < 1e-6) {
        ent.rumboResultado = trkA.getRumbo();
        ent.tiempoResultado = 0.0;
        ent.velocidadRequerida = 0.0;
        return;
    }

    // Rumbo absoluto A -> destino
    double angMath = std::atan2(delta.y(), delta.x());
    double rumbo = 90.0 - qRadiansToDegrees(angMath);
    while (rumbo < 0.0)   rumbo += 360.0;
    while (rumbo >= 360.) rumbo -= 360.0;

    ent.rumboResultado = rumbo;

    if (ent.esModoVD) {
        const double velDmH = ent.velocidadRequerida;
        ent.tiempoResultado = (velDmH > 0.0)
                                  ? (distDm / velDmH) * 3600.0
                                  : 0.0;
    }
    else {
        const double horas = ent.tiempoResultado / 3600.0;
        ent.velocidadRequerida = (horas > 0.0)
                                     ? distDm / horas
                                     : 0.0;
    }
}
