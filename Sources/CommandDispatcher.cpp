/*
    Interfaz/implementación del despachador: recibe líneas, invoca el parser y
    ejecuta el comando correspondiente; maneja `help` y `exit`.
*/
#include "CommandDispatcher.h"
#include "CommandRegistry.h"
#include "IInputParser.h"
#include "ICommand.h"
#include "CommandContext.h"
#include <QStringList>
#include "ConsoleUtils.h"
#include "ansi.h"
#ifdef Q_OS_WIN
#include <windows.h>
#include <QRegularExpression>
#endif


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
    // ========= Intérprete QEK (overlay SPC) =========
    const QString T = trimmed.toUpper();

    // dividir por espacios, guiones, dos puntos o underscore
    static const QRegularExpression sep(QStringLiteral("[\\s:_-]+"));
    QStringList tok = T.split(sep, Qt::SkipEmptyParts);

    // 1) Dominio: SURFACE (o SURF) o (provisorio) AUTO -> arma -s
    if (tok.contains(QStringLiteral("SURFACE")) ||
        tok.contains(QStringLiteral("SURF"))    ||
        tok.contains(QStringLiteral("AUTO")))       // ← si tu QEK manda "AUTO" en ese primer botón
    {
        qekPendingDomainOpt_ = QStringLiteral("-s");
        printPrefijo(true);
        ctx_.out << "SPC: dominio SURFACE preparado. Seleccione identidad (FRIEND/HOSTILE/UNKNOWN).\n";
        ctx_.out.flush();
        return;
    }

    // 2) Identidad: FRIEND / HOSTILE / UNKNOWN (acepta "POSS HOSTILE", "CONF_HOSTILE", etc.)
    auto has = [&](const char* k){ return tok.contains(QString::fromLatin1(k)); };

    if (!qekPendingDomainOpt_.isEmpty() &&
        (has("FRIEND") || has("HOSTILE") || has("UNKNOWN")))
    {
        QString idOpt;
        if (has("FRIEND"))      idOpt = QStringLiteral("-f");
        else if (has("HOSTILE")) idOpt = QStringLiteral("-h"); // vale para POSS/CONF HOSTILE
        else                     idOpt = QStringLiteral("-u");

        const QString synthetic = QStringLiteral("add %1 %2 0 0")
                                      .arg(qekPendingDomainOpt_, idOpt);

        qekPendingDomainOpt_.clear();

        CommandInvocation inv2;
        QString perr2;
        if (!parser_->parse(synthetic, inv2, perr2)) {
            printPrefijo(false);
            ctx_.err << Ansi::red
                     << "Error de parseo (synthetic): " << perr2 << "\n"
                     << "Comando: " << synthetic << Ansi::reset << "\n";
            ctx_.err.flush();
            return;
        }

        auto cmd2 = reg_->find(inv2.name);
        if (cmd2.isNull()) {
            printPrefijo(false);
            ctx_.err << Ansi::red << "Comando desconocido: " << inv2.name << Ansi::reset << "\n";
            ctx_.err.flush();
            return;
        }

        auto res2 = cmd2->execute(inv2, ctx_);
        printPrefijo(res2.ok);
        if (!res2.message.isEmpty()) {
            QTextStream& ts = res2.ok ? ctx_.out : ctx_.err;
            const char* color = res2.ok ? Ansi::green : Ansi::red;
            ts << color << res2.message << Ansi::reset << "\n";
            ts.flush();
        }
        return;
    }


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
        ctx_.out << Ansi::cyan << "Comandos:\n";

        // Admin fijos
        ctx_.out << "  help, exit/salir\n";

        for (const auto& sp : reg_->all()) {
            const ICommand* cmd = sp.data();
            ctx_.out << "  "
                     << cmd->usage()
                     << "  - " << cmd->getDescription()
                     << "\n";
        }

        ctx_.out << Ansi::reset;
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
