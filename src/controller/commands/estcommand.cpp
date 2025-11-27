#include "estCommand.h"
#include "../../model/commandContext.h"
#include "../../model/entities/stationEntity.h"
#include <QtMath>      // Para senos, cosenos y radianes
#include <QVector2D>   // <--- ¡AGREGA ESTA LÍNEA!
#include <QDebug>

// FIX: La función execute es 'const', así que resolverCinematica también debe serlo.
CommandResult EstCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {

    // FIX: Acceder a 'args' directamente si es un miembro público de la struct
    // Si inv.args es un QVector/QList, usamos .size()
    if (inv.args.size() < 7) {
        // FIX: Uso de llaves {} para inicializar el struct CommandResult
        return CommandResult{false, "Faltan argumentos. Uso: " + usage()};
    }

    bool ok;
    // 2. Parsing
    // FIX: Cambiado getArgs() por args
    int slotId = inv.args[0].toInt(&ok);

    // FIX: Inicialización con llaves
    if (!ok || slotId < 1 || slotId > 10) return CommandResult{false, "Slot invalido (1-10)"};

    int trkA = inv.args[1].toInt(&ok);
    int trkB = inv.args[2].toInt(&ok);
    double az = inv.args[3].toDouble(&ok);
    double dt = inv.args[4].toDouble(&ok);
    QString modoStr = inv.args[5].toUpper();
    double valor = inv.args[6].toDouble(&ok);

    // 3. Entidad
    StationEntity st(slotId, trkA, trkB, az, dt);

    if (modoStr == "VD") {
        st.esModoVD = true;
        st.velocidadRequerida = valor;
    } else if (modoStr == "DU") {
        st.esModoVD = false;
        st.tiempoResultado = valor;
    } else {
        return CommandResult{false, "Modo desconocido. Use VD o DU."};
    }

    // 4. Lógica
    // Ahora esto funcionará porque resolverCinematica es 'const'
    resolverCinematica(st, ctx);

    // 5. Insertar
    ctx.setStationingSlot(st);

    // 6. Retorno
    QString msg = QString("STA Calculado Slot %1 -> Rumbo: %2").arg(slotId).arg(st.rumboResultado);
    return CommandResult{true, msg};
}

// FIX: Agregado 'const' al final de la firma para coincidir con execute
void EstCommand::resolverCinematica(StationEntity& ent, CommandContext& ctx) const   {
    // NOTA: Recuerda implementar el getter real de tracks en commandContext.h
    // Simulamos datos si no existen los getters aún
    double rumboGuia = 90.0;
    double velGuia = 10.0;

    // Ahora QVector2D ya no dará error de incomplete type
    QVector2D posA(0, 0);
    QVector2D posB(10, 10);

    double radGuia = qDegreesToRadians(rumboGuia);
    QVector2D vecVelGuia(sin(radGuia) * velGuia, cos(radGuia) * velGuia);

    double azEstacionVerdadero = rumboGuia + ent.azimut;
    double radEst = qDegreesToRadians(azEstacionVerdadero);

    QVector2D offsetEstacion(sin(radEst) * ent.distancia, cos(radEst) * ent.distancia);
    QVector2D posEstacion = posB + offsetEstacion;

    QVector2D deltaP = posEstacion - posA;
    double distTotal = deltaP.length();

    if (ent.esModoVD) {
        double miVel = ent.velocidadRequerida;
        double tiempoHoras = distTotal / (qAbs(miVel - velGuia) + 0.0001); // Evitar div 0

        ent.tiempoResultado = tiempoHoras * 3600;

        QVector2D puntoFuturo = posEstacion + (vecVelGuia * tiempoHoras);
        QVector2D vectorRumbo = puntoFuturo - posA;

        double anguloMath = std::atan2(vectorRumbo.y(), vectorRumbo.x());
        double azGeo = 90.0 - qRadiansToDegrees(anguloMath);
        while (azGeo < 0) azGeo += 360.0;

        ent.rumboResultado = azGeo;
    } else {
        double tiempoHoras = ent.tiempoResultado / 3600.0;
        if(tiempoHoras <= 0) tiempoHoras = 0.001;

        QVector2D puntoFuturo = posEstacion + (vecVelGuia * tiempoHoras);
        QVector2D vecVelNec = (puntoFuturo - posA) / tiempoHoras;

        ent.velocidadRequerida = vecVelNec.length();

        double anguloMath = std::atan2(vecVelNec.y(), vecVelNec.x());
        double azGeo = 90.0 - qRadiansToDegrees(anguloMath);
        while (azGeo < 0) azGeo += 360.0;

        ent.rumboResultado = azGeo;
    }
}
