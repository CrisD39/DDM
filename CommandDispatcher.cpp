#include <QObject>
#include <QString>
#include "ICommand.h"
#include <CommandRegistry.h>
#include <CommandContext.h>
#include "IInputParser.h"

class CommandDispatcher : public QObject {
    Q_OBJECT
public:
    CommandDispatcher(CommandRegistry* reg, IInputParser* parser, QObject* p=nullptr)
        : QObject(p), reg_(reg), parser_(parser) {}

signals:
    void quitRequested();

private:
    void printHelp() {
        ctx_.out << "Comandos disponibles:\n";
        for (auto& c : reg_->all())
            ctx_.out << "  " << c->getName() << " - " << c->getDescription() << "\n";
        ctx_.out << "  help, salir\n";
        ctx_.out.flush();
    }

    CommandRegistry* reg_;
    IInputParser*    parser_;
    CommandContext   ctx_;   // stdout, stderr y servicios compartidos

public slots:
    void onLine(const QString& line) {
        CommandInvocation inv;
        QString perr;

        // 1) Parseo (Strategy)
        if (!parser_->parse(line, inv, perr)) {
            ctx_.err << "Parse error: " << perr << "\n"; ctx_.err.flush();
            return;
        }

        // 2) Comandos internos básicos
        if (inv.name.compare("exit", Qt::CaseInsensitive)==0) {
            // emit quitRequested();
            return;
        }
        if (inv.name == "help") { printHelp(); return; }

        // 3) Resolución (Registry/Factory)
        auto cmd = reg_->find(inv.name);
        if (!cmd) {
            ctx_.err << "Comando desconocido: " << inv.name << "\n";
            ctx_.err << "Escribe 'help' para ver la lista.\n";
            ctx_.err.flush();
            return;
        }

        // 4) Ejecución (Command)
        CommandResult res = cmd->execute(inv, ctx_);

        // 5) Salida
        if (!res.message.isEmpty()) {
            (res.ok ? ctx_.out : ctx_.err) << res.message << "\n";
            (res.ok ? ctx_.out : ctx_.err).flush();
        }
    }

};
