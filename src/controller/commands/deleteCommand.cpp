/*
    Comando `delete`/`del`/`rm`: elimina un `Track` por `id` del `ctx.tracks` con validaciones de argumentos.
*/
#include "commands/deleteCommand.h"

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

    for (int i=0; i<ctx.tracks.size(); ++i) {
        if (ctx.tracks[i].getId() == id) {
            ctx.tracks.removeAt(i);

            // Si borro el último ID asignado y no hay huecos, achico el contador
            if (id == ctx.nextTrackId - 1 && ctx.freeIds.isEmpty()) {
                ctx.nextTrackId--;
            } else {
                ctx.freeIds.append(id); // guardo para reutilizar
            }

            return {true, QString("OK delete → id=%1").arg(id)};
        }
    }
    return {false, QString("No existe el track id=%1").arg(id)};
}

