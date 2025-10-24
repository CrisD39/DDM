/*
    Comando `add`: crea un `Track` con flags `-s|-a|-b` (identidad) y `-f|-e|-u` (tipo),
    y coordenadas `<x> <y>`; actualiza `ctx.tracks`.
*/
#include "commands/addCommand.h"
#include "enums.h"
#include <algorithm> // por las dudas, aunque no lo usemos acá

static bool takeNumber(const QString& s, double& out) {
    bool ok = false; double v = s.toDouble(&ok);
    if (ok) out = v;
    return ok;
}

CommandResult AddCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    const QStringList& args = inv.args;
    if (args.isEmpty()) {
        return {false, "Uso: " + usage()};
    }

    int idx = 0;
    bool hasType = false;       // -f | -e | -u (obligatorio, TIPO)
    bool hasIdent = false;      // -s | -a | -b (opcional, IDENTIDAD)
    Type type = Type::Surface;          // default
    Identity ident = Identity::Pending; // default

    auto isNumericToken = [](const QString& tok) {
        // Token numérico permite "-12", "-12.3", "12", "12.3", ".5", "-.5"
        bool digitLike = (!tok.isEmpty() && (tok[0].isDigit() || tok[0] == '-' || tok[0] == '.'));
        if (!digitLike) return false;
        bool okX=false; tok.toDouble(&okX);
        return okX;
    };

    // ---------- PARSEO DE FLAGS ----------
    while (idx < args.size()) {
        const QString tok = args[idx];
        if (isNumericToken(tok) || !tok.startsWith('-')) break;

        const QString f = tok.toLower();

        // Identidad: -s | -a | -b
        if (f == "-s" || f == "-a" || f == "-b") {
            hasIdent = true;
            if      (f == "-s") type = Type::Surface;
            else if (f == "-a") type = Type::Air;
            else                type = Type::Subsurface;
            ++idx;
            continue;
        }

        // Tipo: -f | -e | -u
        if (f == "-f" || f == "-e" || f == "-u") {
            if (hasType) return {false, "Solo un flag de tipo permitido (-f|-e|-u)."};
            hasType = true;
            if      (f == "-f") ident = Identity::ConfFriend;
            else if (f == "-e") ident = Identity::ConfHostile;
            else                ident = Identity::EvalUnknown;
            ++idx;
            continue;
        }

        return {false, QString("Flag no soportada: %1").arg(tok)};
    }

    // ---------- COORDENADAS ----------
    if (!hasType) {
        return {false, "Falta tipo (-f|-e|-u). Uso: " + usage()};
    }
    if (idx + 1 >= args.size()) {
        return {false, "Faltan coordenadas <x> <y>. Uso: " + usage()};
    }

    double x = 0, y = 0;
    if (!takeNumber(args[idx], x) || !takeNumber(args[idx + 1], y)) {
        return {false, "Coordenadas inválidas. Deben ser números (x y)."};
    }
    idx += 2;
    if (idx < args.size()) {
        return {false, "Argumentos de más. Uso: " + usage()};
    }
    if (x < -256 || x > 256 || y < -256 || y > 256) {
        return {false, "Coordenadas fuera de rango. Deben estar entre -256 y 256."};
    }

    // ---------- ALTA DEL TRACK ----------
    const int id = ctx.acquireId();   // toma menor ID libre o nextTrackId

    Track& t = ctx.emplaceTrackFront(
        id,
        type,
        ident,            // si no vino flag, queda el default
        TrackMode::Auto,  // ajustá si tu diseño lo define distinto
        x,
        y
        );

    return {
        true,
        QString("OK add → id=%1 ident=%3 type=%2 x=%4 y=%5")
            .arg(t.getId())
            .arg(TrackData::toQString(t.getType()))
            .arg(TrackData::toQString(t.getIdentity()))
            .arg(t.getX(), 0, 'f', 3)
            .arg(t.getY(), 0, 'f', 3)
    };
}
