#ifndef ECHOCOMMAND_H
#define ECHOCOMMAND_H

#include "BaseCommand.h"
#include "CommandContext.h"
#include <QString>

class EchoCommand : public BaseCommand {
    Q_OBJECT
public:
    EchoCommand(CommandContext& ctx) : BaseCommand(ctx) {}

    QString getName() const override { return "echo"; }
    QString getDescription() const override { return "Imprime en mayúsculas los argumentos."; }
    QString usage() const override { return "echo [texto]"; }

    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override {
        if (inv.args.isEmpty()) {
            return {false, "Uso: echo [texto]"};
        }

        QString result = inv.args.join(" ").toUpper();
        return {true, result};
    }
};

#endif // ECHOCOMMAND_H
