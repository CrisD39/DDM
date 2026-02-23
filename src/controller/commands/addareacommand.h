#ifndef ADDAREACOMMAND_H
#define ADDAREACOMMAND_H

#include "iCommand.h"
#include <QPointF>
#include <QString>

class AddAreaCommand : public ICommand {
public:
    QString getName() const override { return "addArea"; }
    QString getDescription() const override { return "Crea un área interactiva definiendo puntos A, B, C, D, tipo y color."; }
    QString usage() const override { return "addArea(ax,ay,bx,by,cx,cy,dx,dy,tipo,color)"; }

    // El método execute es const por interfaz, no se puede cambiar
    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;

private:
    // Este método debe ser const para ser llamado desde execute
    QPointF requestPoint(const std::string& label) const;
};

#endif
