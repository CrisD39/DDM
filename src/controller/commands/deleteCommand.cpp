/*
    Comando `delete`/`del`/`rm`: elimina un `Track` por `id` del `ctx.tracks`.
    Al eliminar, libera el ID para que el próximo `add` tome el menor disponible.
*/
#include "commands/deleteCommand.h"
#include <algorithm> // std::find_if

CommandResult DeleteCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    const QStringList& args = inv.args;
    if (args.size() != 1) {
        return {false, "Uso: " + usage()};
    }

    bool ok = false;
    const int id = args[0].toInt(&ok);
    if (!ok) {
        return {false, "ID inválido"};
    }

    auto it = std::find_if(ctx.tracks.begin(), ctx.tracks.end(),
                           [id](const Track& t){ return t.getId() == id; });

    if (it == ctx.tracks.end()) {
        return {false, QString("No existe track con id=%1").arg(id)};
    }

    ctx.tracks.erase(it);  // std::deque: erase(it)
    ctx.releaseId(id);     // libera y compacta si corresponde

    return {true, QString("OK delete → id=%1").arg(id)};
}
