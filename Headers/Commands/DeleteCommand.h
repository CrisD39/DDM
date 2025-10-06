/*
    Comando `delete`/`del`/`rm`: elimina un `Track` por `id` del `ctx.tracks` con validaciones de argumentos.
*/

#pragma once
#include "ICommand.h"
#include "CommandContext.h"

class DeleteCommand : public ICommand {
public:
    QString getName() const override { return "delete"; }
    QString getDescription() const override { return "Elimina un track por id"; }
    QString usage() const override { return "delete <id>"; }
    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;
};
