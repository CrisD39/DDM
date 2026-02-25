#include "deleteAreaCommand.h"
#include "model/commandContext.h"

CommandResult DeleteAreaCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    if (inv.args.size() != 1) {
        return {false, "Error: Se requiere exactamente un argumento (el ID del área). Uso: deleteArea(id)"};
    }

    bool ok = false;
    int areaId = inv.args[0].toInt(&ok);
    if (!ok) {
        return {false, "Error: El ID del área debe ser un número entero."};
    }

    if (ctx.deleteArea(areaId)) {
        return {true, QString("Área %1 eliminada exitosamente.").arg(areaId)};
    } else {
        return {false, QString("Error: No se encontró un área con ID %1.").arg(areaId)};
    }
}
