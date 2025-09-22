#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "ICommand.h"
#include <QRegularExpression>

class CommandParser : public IInputParser {
public:
    bool parse(const QString& line, CommandInvocation& out, QString& err) const override {
        QString l = line.trimmed();
        if (l.isEmpty()) { err = "línea vacía"; return false; }

        // Tokenizar respetando comillas
        QStringList tokens;
        {
            QRegularExpression rx(R"((\"[^\"]*\"|\S+))");
            auto it = rx.globalMatch(l);
            while (it.hasNext()) {
                auto m = it.next();
                QString t = m.captured(1);
                if (t.startsWith('"') && t.endsWith('"')) t = t.mid(1, t.size()-2);
                tokens << t;
            }
        }
        if (tokens.isEmpty()) { err = "sin tokens"; return false; }

        out = {};
        out.name = tokens.first();
        tokens.pop_front();

        for (const QString& t : tokens) {
            if (t.startsWith("--")) {
                auto kv = t.mid(2).split('=', Qt::KeepEmptyParts);
                out.opts[kv[0]] = kv.size()>1 ? kv[1] : "true";
            } else if (t.startsWith('-') && t.size()>1) {
                for (int i=1;i<t.size();++i) out.opts[QString(t[i])] = "true";
            } else {
                out.args << t;
            }
        }

        // Soporte de subcomandos estilo "user add ..." => name="user", path=["add"]
        // (si querés jerarquía más profunda, podés decidir por convención)
        if (!out.args.isEmpty() && out.args.first() == "help") {
            // Convención: "<cmd> help" redirige a HelpCommand
        }

        return true;
    }
};


#endif // COMMANDPARSER_H
