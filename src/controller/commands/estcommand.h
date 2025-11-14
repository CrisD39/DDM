#pragma once
#include "iCommand.h"
#include "commandContext.h"

class EstCommand : public ICommand {
public:
    QString getName() const override { return "est"; }
    QString getDescription() const override {
        return "Calcula un rumbo/tiempo/velocidad para estacionar OwnShip respecto a un Track";
    }
    QString usage() const override {
        return "est <id> -v <velocidad> | -t <tiempo_horas>";
    }

    CommandResult execute(const CommandInvocation& inv,
                          CommandContext& ctx) const override;
};
