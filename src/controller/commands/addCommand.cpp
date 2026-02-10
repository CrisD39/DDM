/*
    Comando `add`: crea un `Track` con flags `-s|-a|-b` (identidad) y `-f|-e|-u` (tipo),
    y coordenadas `<x> <y>`; actualiza `ctx.tracks`.
*/
#include "Commands/addCommand.h"
#include "enums.h"


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
    Type type = Type::Surface;  // default si querés otro, cámbialo
    Identity ident = Identity::Pending;

    auto isNumericToken = [](const QString& tok) {
        return tok.startsWith('-') && tok.size() > 1 && (tok[1].isDigit() || tok[1] == '.');
    };

    // ---------- PARSEO DE FLAGS ----------
    while (idx < args.size()) {
        const QString tok = args[idx];
        if (isNumericToken(tok) || !tok.startsWith('-')) break;

        const QString f = tok.toLower();

        // Identidad: -s | -a | -b
        if (f == "-s" || f == "-a" || f == "-b") {
            hasIdent = true;
            if      (f == "-s") ident = Identity::ConfFriend;
            else if (f == "-a") ident = Identity::ConfHostile;
            else                ident = Identity::EvalUnknown;
            ++idx;
            continue;
        }

        // Tipo: -f | -e | -u
        if (f == "-f" || f == "-e" || f == "-u") {
            if (hasType) return {false, "Solo un flag de tipo permitido (-f|-e|-u)."};
            hasType = true;
            if      (f == "-f") type = Type::Surface;
            else if (f == "-e") type = Type::Air;
            else                type = Type::Subsurface;
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

    double vel = 0, cour = 0;
    takeNumber(args[idx + 2],vel);
    takeNumber(args[idx + 3],cour);

    idx += 4;
    if (idx < args.size()) {
        return {false, "Argumentos de más. Uso: " + usage()};
    }
    if (x < -255 || x > 255 || y < -255 || y > 255) {
        return {false, "Coordenadas fuera de rango. Deben estar entre -256 y 256."};
    }


    // ---------- ALTA DEL TRACK (in-place al frente, O(1)) ----------
    const int id = ctx.nextTrackId++;
    Track& t = ctx.emplaceTrackFront(
        id,
        type,
        ident,          // Identity (si no vino flag, queda lo que seteaste como default)
        TrackMode::Auto, // si en tu diseño viene de otro lado, reemplazalo
        x,
        y,
        vel,
        cour
        );

    // (si necesitás tocar algo más del track, podés hacerlo ahora con 't')

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

