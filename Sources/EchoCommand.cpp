#include "BaseCommand.h"
#include "ICommand.h"
#include "CommandContext.h"
#include <QStringList>

class EchoCommand : public BaseCommand {
public:
    using BaseCommand::BaseCommand;

    QString getName() const override        { return "echo"; }
    QString getDescription() const override { return "Imprime argumentos"; }
    QString usage() const override          { return "echo [texto]"; }

    CommandResult execute(const CommandInvocation& inv,
                          CommandContext& ctx) const override
    {
        ctx.out << inv.args.join(" ") << "\n";
        ctx.out.flush();
        return { true, "" };
    }
};
