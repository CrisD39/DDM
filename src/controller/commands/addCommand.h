#pragma once
#include "ICommand.h"
#include "commandContext.h"

class AddCommand : public ICommand {
public:
    QString getName() const override { return "add"; }

    QString getDescription() const override {
        return "Crea un Track con identidad, tipo, posicion (x,y) y opcionalmente rumbo y velocidad";
    }

    // x, y obligatorios — rumbo y velocidad opcionales
    QString usage() const override {
        return "add <-s|-a|-b> <-f|-e|-u> <x> <y> [<rumbo> <velocidad>]";
    }

    CommandResult execute(const CommandInvocation& inv,
                          CommandContext& ctx) const override;
};
