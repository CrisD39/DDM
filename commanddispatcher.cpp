// CommandDispatcher.cpp
#include "CommandDispatcher.h"
#include "CommandRegistry.h"
#include "IInputParser.h"
#include "ICommand.h"
#include "CommandContext.h"
#include <QStringList>

CommandDispatcher::CommandDispatcher(CommandRegistry* reg,
                                     IInputParser* parser,
                                     CommandContext& ctx,
                                     QObject* parent)
    : QObject(parent), reg_(reg), parser_(parser), ctx_(ctx) {}

void CommandDispatcher::onLine(const QString& line) {
    const QString trimmed = line.trimmed();
    if (trimmed.isEmpty()) return;

    // Admin: salir/exit
    const QString first = trimmed.split(' ', Qt::SkipEmptyParts).value(0);
    if (first.compare("salir", Qt::CaseInsensitive) == 0 ||
        first.compare("exit",  Qt::CaseInsensitive) == 0) {
        ctx_.out << "Adiós!\n"; ctx_.out.flush();
        emit quitRequested();
        return;
    }

    if (first.compare("help", Qt::CaseInsensitive) == 0) {
        ctx_.out << "Comandos: help, exit/salir\n"; ctx_.out.flush();
        return;
    }

    // Parseo general
    CommandInvocation inv;
    QString perr;
    if (!parser_->parse(trimmed, inv, perr)) {
        ctx_.err << "Parse error: " << perr << "\n"; ctx_.err.flush();
        return;
    }

    auto cmd = reg_->find(inv.name);
    if (cmd.isNull()) {
        ctx_.err << "Comando desconocido: " << inv.name << "\n"; ctx_.err.flush();
        return;
    }

    auto res = cmd->execute(inv, ctx_);
    if (!res.message.isEmpty()) {
        (res.ok ? ctx_.out : ctx_.err) << res.message << "\n";
        (res.ok ? ctx_.out : ctx_.err).flush();
    }
}
