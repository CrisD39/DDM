#ifndef COMMANDCONTEXT_H
#define COMMANDCONTEXT_H

class CommandContext {
public:
    QTextStream out{stdout};
    QTextStream err{stderr};
    // Agregá aquí servicios compartidos (settings, db, logger, etc.)
};

#endif // COMMANDCONTEXT_H
