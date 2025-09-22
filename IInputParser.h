#ifndef IINPUTPARSER_H
#define IINPUTPARSER_H

class IInputParser {
public:
    // virtual ~IInputParser() const = 0;
    virtual  bool parse(const QString& line, CommandInvocation& out, QString& err) const = 0;
};

#endif // IINPUTPARSER_H
