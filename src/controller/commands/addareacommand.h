#ifndef ADDAREACOMMAND_H
#define ADDAREACOMMAND_H

#include "iCommand.h"

struct areaFigures{

};

class AddAreaCommand: public ICommand
{
public:
    QString getName() const override {return "addArea";}
    QString getDescription() const override {return "Crea un area (x1,y1,x2,y2,x3,y3,x4,y4,tipo,color)";}
    QString usage() const override{return "addArea <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>";
    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;
    };

#endif // ADDAREACOMMAND_H
