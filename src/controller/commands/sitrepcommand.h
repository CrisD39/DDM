#pragma once
#include "iCommand.h"
#include "commandContext.h"

class SitrepCommand : public ICommand {
    Q_OBJECT
public:
    QString getName() const override { return "sitrep"; }
    QString getDescription() const override { return "SITREP: planilla de tracks en tiempo real (list/watch/delete/info)."; }
    QString usage() const override {
        return "sitrep [list]\n"
               "sitrep watch\n"
               "sitrep delete <trackId>\n"
               "sitrep info <trackId> <texto>";
    }

    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;
};
