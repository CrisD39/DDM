// CommandDispatcher.cpp
#include "CommandDispatcher.h"
#include "CommandRegistry.h"
#include "IInputParser.h"
#include "ICommand.h"
#include "CommandContext.h"
#include <QStringList>
#include "ConsoleUtils.h"
#include "ansi.h"

CommandDispatcher::CommandDispatcher(CommandRegistry* reg,
                                     IInputParser* parser,
                                     CommandContext& ctx,
                                     QObject* parent)
    : QObject(parent), reg_(reg), parser_(parser), ctx_(ctx) {}



void CommandDispatcher::onLine(const QString& line) {
    const QString trimmed = line.trimmed();
    if (trimmed.isEmpty()) return;

    Console::clear(ctx_.out);

    auto printPrefijo = [&](bool ok) {
        const char* color = ok ? Ansi::green : Ansi::red;
        ctx_.out << color << ctx_.commandCounter << " " << trimmed << Ansi::reset << "\n";
        ctx_.out.flush();
        ctx_.commandCounter++;
    };

    // Intento parsear
    CommandInvocation inv;
    QString perr;
    if (!parser_->parse(trimmed, inv, perr)) {
        printPrefijo(false);
        ctx_.err << Ansi::red
                 << "Error de parseo: " << perr << "\n"
                 << "Use 'help' para ver la lista de comandos."
                 << Ansi::reset << "\n";
        ctx_.err.flush();
        return;
    }

    const QString first = inv.name;

    // Admin: exit/salir
    if (first.compare("salir", Qt::CaseInsensitive) == 0
        || first.compare("exit", Qt::CaseInsensitive) == 0) {
        printPrefijo(true);
        ctx_.out << "Adiós!\n"; ctx_.out.flush();
        emit quitRequested();
        return;
    }

    // Admin: help
    if (first.compare("help", Qt::CaseInsensitive) == 0) {
        printPrefijo(true);
        ctx_.out << Ansi::cyan
                 << "Comandos:\n"
                 << "  help, exit/salir\n"
                 << "  echo add <-s|-a|-b> <-f|-e|-u> <x> <y>\n"
                 << "  echo delete <id>\n"
                 << "  echo center <x> <y>\n"
                 << "  echo list\n"
                 << Ansi::reset;
        ctx_.out.flush();
        return;
    }

    // Lookup de comando real
    auto cmd = reg_->find(inv.name);
    if (cmd.isNull()) {
        printPrefijo(false);
        ctx_.err << Ansi::red
                 << "Comando desconocido: " << inv.name << "\n"
                 << "Use 'help' para ver la lista de comandos."
                 << Ansi::reset << "\n";
        ctx_.err.flush();
        return;
    }

    // Ejecutar
    auto res = cmd->execute(inv, ctx_);
    printPrefijo(res.ok);

    if (!res.message.isEmpty()) {
        QTextStream& ts = res.ok ? ctx_.out : ctx_.err;
        const char* color = res.ok ? Ansi::green : Ansi::red;
        ts << color << res.message << Ansi::reset << "\n";
        ts.flush();
        if (!res.ok) {
            ctx_.err << "Use 'help' para ver la lista de comandos.\n";
            ctx_.err.flush();
        }
    }
}
