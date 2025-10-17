/*
    Comando `list`/`ls`: imprime tabla con `id`, `type`, `ident`, `x`, `y` de los tracks existentes.
*/
#include "Commands/listCommand.h"

static inline QString typeToString(TrackType t) {
    switch (t) {
    case TrackType::Friendly: return "friendly";
    case TrackType::Enemy:    return "enemy";
    default:                  return "unknown";
    }
}

CommandResult ListCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    Q_UNUSED(inv);
    if (ctx.tracks.isEmpty()) return {true, "(sin tracks)"};

    QString out;
    out += "ID  | TYPE     | IDENT |      X   |      Y\n";
    out += "----+----------+-------+----------+----------\n";
    for (const auto& t : ctx.tracks) {
        out += QString("%1 | %2 | %3 | %4 | %5\n")
            .arg(t.id, 3)
            .arg(typeToString(t.type).left(8).leftJustified(8, ' '))
            .arg((t.identity.isEmpty() ? "-" : t.identity).left(5).leftJustified(5, ' '))
            .arg(QString::number(t.x, 'f', 3).rightJustified(8, ' '))
            .arg(QString::number(t.y, 'f', 3).rightJustified(8, ' '));
    }
    out.chop(1);
    return {true, out};
}
