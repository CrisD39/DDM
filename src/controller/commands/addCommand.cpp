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
    QString identity;          // -s | -a | -b (opcional)
    bool hasType = false;      // -f | -e | -u (obligatorio)
    Track* track = new Track();


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

        if (f == "-s"){
            track->setType(Type::Surface);
            ++idx;
            continue;
        } else if(f == "-a") {
            track->setType(Type::Air);
            ++idx;
            continue;
        } else if(f == "-b") {
            track->setType(Type::Subsurface);
            ++idx;
            continue;
        }
        if (f == "-f" || f == "-e" || f == "-u") {
            if (hasType) return {false, "Solo un flag de tipo permitido (-f|-e|-u)."};
            hasType = true;
            if (f == "-f")      track->setIdentity(Identity::ConfFriend);
            else if (f == "-e") track->setIdentity(Identity::ConfHostile);
            else                track->setIdentity(Identity::EvalUnknown);
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

    int newId;
    if (!ctx.freeIds.isEmpty()) {
        // política: reutilizar el MENOR ID libre primero (predecible)
        std::sort(ctx.freeIds.begin(), ctx.freeIds.end());
        newId = ctx.freeIds.takeFirst();
    } else {
        newId = ctx.nextTrackId++;
    }


    // ---------- ALTA DEL TRACK ----------
    track->setId(ctx.nextTrackId++);;
    track->setX(x);
    track->setY(y);

    ctx.tracks.append(*track);

    return { true,
            QString("OK add → id=%1 ident=%3 type=%2 x=%4 y=%5")
                .arg(track->getId())
                .arg(TrackData::toQString(track->getType()))
                .arg(TrackData::toQString(track->getIdentity()))
                .arg(track->getX(), 0, 'f', 3)
                .arg(track->getY(), 0, 'f', 3) };
}
