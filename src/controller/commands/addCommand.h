/*
    Comando `add`: crea un `Track` con flags `-s|-a|-b` (identidad) y `-f|-e|-u` (tipo),
    y coordenadas `<x> <y>`; actualiza `ctx.tracks`.
*/

#pragma once
#include "iCommand.h"
#include "commandContext.h"

class AddCommand : public ICommand {
public:
    QString getName() const override { return "add"; }
    QString getDescription() const override { return "Crea un track"; }
    QString usage() const override { return "add <-s|-a|-b> <-f|-e|-u> <x> <y>"; }
    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;
};
