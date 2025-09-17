#ifndef ICOMMAND_H
#define ICOMMAND_H


struct CommandResult {
    bool ok = true;
    QString message;
};

struct CommandInvocation {
    QString name;                 // comando principal (p.ej. "echo")
    QStringList path;             // subcomandos (p.ej. ["user","add"])
    QMap<QString, QString> opts;  // --opt=val  /  -f
    QStringList args;             // posicionales
};

public class ICommand : public QObject
{
    Q_OBJECT
public:
    using QObject:QObject;
    virtual QString getName() const = 0;
    virtual QString getDescription() const = 0;
    virtual QString usage() const = 0;
    virtual Command Result execute() const = 0;
};

#endif // ICOMMAND_H
