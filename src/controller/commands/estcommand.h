#pragma once
#include "ICommand.h"

// Forward declarations para no acoplar de más
class CommandContext;
class Track;
struct StationEntity;

class EstCommand : public ICommand {
    Q_OBJECT
public:
    QString getName() const override { return "STA"; } // Nombre corto para invocar
    QString getDescription() const override { return "Calcula estacionamiento (VD/DU)"; }
    QString usage() const override {
        return "STA [Slot] [TrkA] [TrkB] [Az] [Dist] [Modo:VD/DU] [Valor]";
    }

    CommandResult execute(const CommandInvocation& inv,
                          CommandContext& ctx) const override;

private:
    // Helpers matemáticos privados
    void resolverCinematica(StationEntity& ent,
                            const Track& trkA,
                            const Track& trkB) const;
};
