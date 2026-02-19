#ifndef ADDAREACOMMAND_H
#define ADDAREACOMMAND_H

#include "iCommand.h"
#include <QPointF>
#include <QString>
#include <vector>

class AddAreaCommand : public ICommand {
public:
    QString getName() const override { return "addArea"; }
    QString getDescription() const override { return "Crea un área interactiva definiendo puntos A, B, C, D, tipo y color."; }
    QString usage() const override { return "addArea"; }
    CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const override;

    static std::vector<CursorEntity> calculateArea(const QPointF& a, const QPointF& b, const QPointF& c, const QPointF& d, CommandContext& ctx);

private:
    QPointF requestPoint(const QString& pointName, CommandContext& ctx) const;
    QString requestString(const QString& prompt, CommandContext& ctx) const;
};

#endif // ADDAREACOMMAND_H
