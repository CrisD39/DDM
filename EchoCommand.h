#ifndef ECHOCOMMAND_H
#define ECHOCOMMAND_H

#include "BaseCommand.h"
#include "CommandContext.h"
#include <QString>
#include <QStringList>

static inline QString typeToString(TrackType t) {
    switch (t) {
    case TrackType::Friendly: return "friendly";
    case TrackType::Enemy:    return "enemy";
    default:                  return "unknown";
    }
}
static inline TrackType parseTrackType(const QString& s, bool& ok) {
    const QString k = s.trimmed().toLower();
    ok = true;
    if (k == "friendly") return TrackType::Friendly;
    if (k == "enemy")    return TrackType::Enemy;
    if (k == "unknown")  return TrackType::Unknown;
    ok = false; return TrackType::Unknown;
}

class EchoCommand : public BaseCommand {
    Q_OBJECT
public:
    EchoCommand(CommandContext& ctx) : BaseCommand(ctx) {}

    QString getName() const override { return "echo"; }
    QString getDescription() const override {
        return "Control de tracks: echo add|delete|center|list ...";
    }
    QString usage() const override {
        return "echo add <-s|-a|-b> <-f|-e|-u>  <x> <y>\n"
               "echo delete <id>\n"
               "echo center <x> <y>\n"
               "echo list";
    }

    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override {
        if (inv.args.isEmpty()) {
            return {false, usage()};
        }
        const QString sub = inv.args.first().toLower();
        if (sub == "add")    return doAdd(inv.args.mid(1), ctx);
        if (sub == "delete") return doDelete(inv.args.mid(1), ctx);
        if (sub == "center") return doCenter(inv.args.mid(1), ctx);
        if (sub == "list")   return doList(ctx);

        return {false, "Subcomando desconocido. Uso:\n" + usage()};
    }

private:
    static bool takeNumber(const QString& s, double& out) {
        bool ok=false; double v = s.toDouble(&ok);
        if (ok) out = v;
        return ok;
    }

    CommandResult doAdd(const QStringList& args, CommandContext& ctx) const {
        if (args.size() < 3) {
            return {false, "Uso: echo add <-s|-a|-b> <-f|-e|-u> <x> <y>"};
        }

        int idx = 0;

        // Flags
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
            return {false, "Falta flag de tipo (-f|-e|-u). Uso: echo add <-s|-a|-b> <-f|-e|-u> <x> <y>"};
        }
        if (idx + 2 != args.size()) {
            return {false, "Faltan coordenadas. Uso: echo add <-s|-a|-b> <-f|-e|-u> <x> <y>"};
        }

        double x=0, y=0;
        bool okx=false, oky=false;
        x = args[idx++].toDouble(&okx);
        y = args[idx++].toDouble(&oky);
        if (!okx || !oky) {
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

        return {true, QString("OK add → id=%1 ident=%3 type=%2 Fx=%4 y=%5")
                          .arg(tr.id)
                          .arg(typeToString(tr.type))
                          .arg(identStr)
                          .arg(tr.x, 0, 'f', 3)
                          .arg(tr.y, 0, 'f', 3)};
    }

    CommandResult doDelete(const QStringList& args, CommandContext& ctx) const {
        if (args.size() != 1) return {false, "Uso: echo delete <id>"};
        bool ok=false; int id = args[0].toInt(&ok);
        if (!ok) return {false, "ID inválido"};

        for (int i=0; i<ctx.tracks.size(); ++i) {
            if (ctx.tracks[i].id == id) {
                ctx.tracks.removeAt(i);
                return {true, QString("OK delete → id=%1").arg(id)};
            }
        }
        return {false, QString("No existe track con id=%1").arg(id)};
    }

    CommandResult doCenter(const QStringList& args, CommandContext& ctx) const {
        if (args.size() != 2) return {false, "Uso: echo center <x> <y>"};
        double x=0, y=0;
        if (!takeNumber(args[0], x) || !takeNumber(args[1], y)) {
            return {false, "Coordenadas inválidas. Deben ser números (x y)."};
        }
        ctx.centerX = x;
        ctx.centerY = y;
        return {true, QString("OK center → (%1, %2)")
                          .arg(x, 0, 'f', 3)
                          .arg(y, 0, 'f', 3)};
    }

    CommandResult doList(CommandContext& ctx) const {
        if (ctx.tracks.isEmpty()) return {true, "(sin tracks)"};

        QString out;
        out += "ID  | TYPE     | IDENT |      X   |      Y\n";
        out += "----+----------+-------+----------+----------\n";
        for (const auto& t : ctx.tracks) {
            out += QString("%1 | %2 | %3 | %4 | %5\n")
            .arg(t.id, 3)                                   // ancho 3
                .arg(typeToString(t.type).left(8).leftJustified(8, ' '))
                .arg((t.identity.isEmpty() ? "-" : t.identity).left(5).leftJustified(5, ' '))
                .arg(QString::number(t.x, 'f', 3).rightJustified(8, ' '))
                .arg(QString::number(t.y, 'f', 3).rightJustified(8, ' '));

            QString identStr = t.identity;
            if (identStr.isEmpty()) {
                identStr = "-";
            }
        }
        out.chop(1);
        return {true, out};
    }
};

#endif // ECHOCOMMAND_H
