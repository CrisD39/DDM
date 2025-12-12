#include "addCommand.h"
#include "enums/enumsTrack.h"
#include <QtMath>

// Helper para parsear números (ya lo tenías parecido)
static bool takeNumber(const QString& s, double& out) {
    bool ok = false;
    double v = s.toDouble(&ok);
    if (ok) out = v;
    return ok;
}

CommandResult AddCommand::execute(const CommandInvocation& inv,
                                  CommandContext& ctx) const
{
    // Esperamos:
    //  add <idFlag> <typeFlag> <x> <y> [<rumbo> <vel>]
    const int n = inv.args.size();
    if (n != 4 && n != 6) {
        return {false, "Cantidad de argumentos invalida. Uso: " + usage()};
    }

    const QString medioFlag = inv.args[0].toLower();  // -s | -a | -b
    const QString identFlag = inv.args[1].toLower();  // -f | -e | -u

    // 1) Mapear medio (-s|-a|-b) -> TrackData::Type
    TrackData::Type type;

    if (medioFlag == "-s") {
        type = TrackData::Type::Surface;     // Superficie
    } else if (medioFlag == "-a") {
        type = TrackData::Type::Air;         // Aéreo
    } else if (medioFlag == "-b") {
        type = TrackData::Type::Subsurface;  // Subsuperficie
    } else {
        return {false, "Medio invalido. Use -s (superficie), -a (aéreo) o -b (subsuperficie)"};
    }

    // 2) Mapear tipo (-f|-e|-u) a tu enum
    TrackData::Identity identity;

    if (identFlag == "-f") {
        identity = TrackData::Identity::ConfFriend;   // amigo confirmado
    } else if (identFlag == "-e") {
        identity = TrackData::Identity::ConfHostile;  // enemigo confirmado
    } else if (identFlag == "-u") {
        identity = TrackData::Identity::EvalUnknown;  // desconocido / a evaluar
    } else {
        return {false, "Identidad invalida. Use -f (friend), -e (enemy) o -u (unknown)"};
    }

    // 3) TrackMode por defecto
    TrackData::TrackMode mode = TrackData::TrackMode::Auto;  // o FC1 si preferís

    // 4) Parsear posición
    double x = 0.0, y = 0.0;
    if (!takeNumber(inv.args[2], x)) {
        return {false, "x invalido (no es numero)"};
    }
    if (!takeNumber(inv.args[3], y)) {
        return {false, "y invalido (no es numero)"};
    }

    // 5) Parsear rumbo y velocidad (opcionales)
    double rumbo = 0.0;
    double velocidad = 0.0;

    if (n == 6) {
        if (!takeNumber(inv.args[4], rumbo)) {
            return {false, "Rumbo invalido (no es numero)"};
        }
        if (!takeNumber(inv.args[5], velocidad)) {
            return {false, "Velocidad invalida (no es numero)"};
        }

        // Normalizar rumbo a [0, 360)
        while (rumbo < 0.0)   rumbo += 360.0;
        while (rumbo >= 360.) rumbo -= 360.0;

        if (velocidad < 0.0) {
            return {false, "La velocidad no puede ser negativa"};
        }
    }

    // 6) Generar ID de track
    const int id = ctx.acquireId();

    // 7) Crear y guardar el track en el contexto
    // Usamos el ctor extendido: (id, type, identity, mode, x, y, rumbo, velocidad)
    Track& t = ctx.emplaceTrackFront(id, type, identity, mode, x, y, rumbo, velocidad);

    // 8) Mensaje de salida
    QString msg = QString("Track creado: id=%1, pos=(%2,%3), rumbo=%4, vel=%5")
                      .arg(t.getId())
                      .arg(t.getX(), 0, 'f', 2)
                      .arg(t.getY(), 0, 'f', 2)
                      .arg(t.getRumbo(), 0, 'f', 1)
                      .arg(t.getVelocidad(), 0, 'f', 1);

    ctx.out << msg << Qt::endl;

    return {true, msg};
}
