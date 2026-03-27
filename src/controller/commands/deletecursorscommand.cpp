#include "deletecursorscommand.h"

CommandResult DeleteCursorsCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    const QStringList& args = inv.args;
    if (args.size() != 1) {
        return {false, "Uso: " + usage()};
    }

    bool ok = false;
    const int id = args[0].toInt(&ok);
    if (!ok) {
        return {false, "ID inválido"};
    }

    if (ctx.eraseCursorById(id)) {
        return {true, QString("OK deletecursor → id=%1").arg(id)};
    }

    return {false, QString("No existe el cursor id=%1").arg(id)};
}
