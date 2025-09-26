/*
    Comando `add`: crea un `Track` con flags `-s|-a|-b` (identidad) y `-f|-e|-u` (tipo),
    y coordenadas `<x> <y>`; actualiza `ctx.tracks`.
*/
#include "Commands/AddCommand.h"


static inline QString typeToString(TrackType t) {
    switch (t) {
    case TrackType::Friendly: return "friendly";
    case TrackType::Enemy:    return "enemy";
    default:                  return "unknown";
    }
}


static bool takeNumber(const QString& s, double& out) {
    bool ok=false; double v = s.toDouble(&ok);
    if (ok) out = v;
    return ok;
}

CommandResult AddCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    const QStringList& args = inv.args;
    if (args.size() < 3) {
        return {false, "Uso: " + usage()};
    }

    int idx = 0;
    QString identity;          // opcional: -s | -a | -b
    bool hasType = false;      // obligatorio: uno de -f|-e|-u
    TrackType t = TrackType::Unknown;

    while (idx < args.size() && args[idx].startsWith("-")) {
        const QString f = args[idx].toLower();

        if (f == "-s" || f == "-a" || f == "-b") {
            identity = f.mid(1);  // "s" | "a" | "b"
            idx++;
            continue;
        }
        if (f == "-f" || f == "-e" || f == "-u") {
            if (hasType) {
                return {false, "Solo un flag de tipo permitido (-f|-e|-u)."};
            }
            hasType = true;
            if (f == "-f") t = TrackType::Friendly;
            else if (f == "-e") t = TrackType::Enemy;
            else t = TrackType::Unknown;
            idx++;
            continue;
        }

        return {false, QString("Flag no soportada: %1").arg(args[idx])};
    }

    if (!hasType) {
        return {false, "Falta flag de tipo (-f|-e|-u). Uso: " + usage()};
    }
    if (idx + 2 != args.size()) {
        return {false, "Faltan coordenadas. Uso: " + usage()};
    }

    double x=0, y=0;
    if (!takeNumber(args[idx++], x) || !takeNumber(args[idx++], y)) {
        return {false, "Coordenadas inválidas. Deben ser números (x y)."};
    }

    Track tr;
    tr.id = ctx.nextTrackId++;
    tr.type = t;
    tr.identity = identity; // puede ser vacío
    tr.x = x;
    tr.y = y;

    ctx.tracks.append(tr);

    QString identStr = tr.identity;
    if (identStr.isEmpty()) identStr = "-";

    return {true, QString("OK add → id=%1 ident=%3 type=%2 x=%4 y=%5")
                      .arg(tr.id)
                      .arg(typeToString(tr.type))
                      .arg(identStr)
                      .arg(tr.x, 0, 'f', 3)
                      .arg(tr.y, 0, 'f', 3)};
}
