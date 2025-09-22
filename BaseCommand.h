#ifndef BASECOMMAND_H
#define BASECOMMAND_H
#include "ICommand.h"
#include "CommandContext.h"

class BaseCommand : public ICommand {
public:
    BaseCommand(CommandContext& ctx) : ctx_(ctx) {}
protected:
    CommandContext& ctx_;
};


#endif // BASECOMMAND_H
