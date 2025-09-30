/*
    Implementa `IInputParser`: tokeniza la línea de entrada (soporta comillas),
    llena `CommandInvocation` con nombre y args.
*/
#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H

#include "IInputParser.h"   // ⬅️ imprescindible (antes NO estaba)
#include "ICommand.h"
#include <QRegularExpression>
#include <QStringList>

class CommandParser : public IInputParser {
public:
    bool parse(const QString& line,
               CommandInvocation& out,
               QString& err) const override {
        QString l = line.trimmed();
        if (l.isEmpty()) { err = "línea vacía"; return false; }

        // Tokenizar respetando comillas
        QStringList tokens;
        QRegularExpression rx(R"((\"[^\"]*\"|\S+))");
        auto it = rx.globalMatch(l);
        while (it.hasNext()) {
            auto m = it.next().captured(1);
            if (m.startsWith('"') && m.endsWith('"')) m = m.mid(1, m.size()-2);
            tokens << m;
        }
        if (tokens.isEmpty()) { err = "sin tokens"; return false; }

        // name + args simples (placeholder)
        out.name = tokens.takeFirst();
        out.args = tokens;
        out.path.clear();
        out.opts.clear();
        return true;
    }
};

#endif // COMMANDPARSER_H
