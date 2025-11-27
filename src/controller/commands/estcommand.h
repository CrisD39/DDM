#pragma once
#include "ICommand.h"
#include <QtMath>
#include "commandContext.h"


class EstCommand : public ICommand {
    Q_OBJECT
public:

    QString getName() const override { return "STA"; } // Nombre corto para invocar
    QString getDescription() const override { return "Calcula estacionamiento (VD/DU)"; }
    QString usage() const override { return "STA [Slot] [TrkA] [TrkB] [Az] [Dist] [Modo:VD/DU] [Valor]"; }
    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;

private:
    // Helpers matemáticos privados (Triángulo de velocidades)
    void resolverCinematica(StationEntity& ent, CommandContext& ctx) const;
};
