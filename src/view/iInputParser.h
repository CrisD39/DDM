/*
    Interfaz de parser de línea (`parse(line, CommandInvocation&, err)`);
    desacopla el tokenizer del dispatcher.
*/
#ifndef IINPUTPARSER_H
#define IINPUTPARSER_H

#include <QString>
class CommandInvocation;   // viene de ICommand.h (fwd ok)

class IInputParser {
public:
    virtual ~IInputParser() = default;
    virtual bool parse(const QString& line,
                       CommandInvocation& out,
                       QString& err) const = 0;
};

#endif // IINPUTPARSER_H
