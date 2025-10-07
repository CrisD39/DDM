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
    QString identity;          // -s | -a | -b (opcional)
    bool hasType = false;      // -f | -e | -u (obligatorio)
    TrackType t = TrackType::Unknown;

    // ---------- PARSEO DE FLAGS ----------
    while (idx < args.size()) {
        const QString tok = args[idx];

        // número negativo (p.ej. "-50" o "-12.3") => NO es flag
        if (tok.startsWith('-') && tok.size() > 1 && (tok[1].isDigit() || tok[1] == '.')) {
            break;
        }
        // si no empieza con '-', terminó la sección de flags
        if (!tok.startsWith('-')) {
            break;
        }

        const QString f = tok.toLower();

        if (f == "-s" || f == "-a" || f == "-b") {
            identity = f.mid(1);   // "s" | "a" | "b"
            ++idx;
            continue;
        }

        if (f == "-f" || f == "-e" || f == "-u") {
            if (hasType) return {false, "Solo un flag de tipo permitido (-f|-e|-u)."};
            hasType = true;
            if (f == "-f")      t = TrackType::Friendly;
            else if (f == "-e") t = TrackType::Enemy;
            else                t = TrackType::Unknown;
            ++idx;
            continue;
        }

        // Flag desconocida
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

    // (si te quedan tokens extras, lo marcamos)
    if (idx < args.size()) {
        return {false, "Argumentos de más. Uso: " + usage()};
    }

    if (x < -255 || x > 255 || y < -255 || y > 255) {
        return {false, QString("Coordenadas fuera de rango. Deben estar entre -256 y 256.")};
    }

    // ---------- ALTA DEL TRACK ----------
    Track tr;
    tr.id        = ctx.nextTrackId++;
    tr.type      = t;
    tr.identity  = identity;    // puede ser vacío
    tr.x         = x;
    tr.y         = y;

    ctx.tracks.append(tr);

    const QString identStr = tr.identity.isEmpty() ? "-" : tr.identity;
    return { true,
            QString("OK add → id=%1 ident=%3 type=%2 x=%4 y=%5")
                .arg(tr.id)
                .arg(typeToString(tr.type))
                .arg(identStr)
                .arg(tr.x, 0, 'f', 3)
                .arg(tr.y, 0, 'f', 3) };
}
