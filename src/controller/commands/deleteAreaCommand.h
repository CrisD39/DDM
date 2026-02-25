#ifndef DELETEAREACOMMAND_H
#define DELETEAREACOMMAND_H

#include "iCommand.h"

class DeleteAreaCommand : public ICommand {
public:
    QString getName() const override { return "deleteArea"; }
    QString getDescription() const override { return "Elimina un área y sus cursores asociados dado su ID."; }
    QString usage() const override { return "deleteArea(id)"; }

    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;
};

#endif // DELETEAREACOMMAND_H
