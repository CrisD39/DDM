/*
    Registro de comandos (`registerCommand`, `find`, `names`, `all`) usando `QSharedPointer<ICommand>`.
*/

#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H
#include <QObject>
#include "ICommand.h"

#include <QSharedPointer>
#include <QMap>
#include <QStringList>
#include <QList>


class CommandRegistry : public QObject {
    Q_OBJECT
public:
    explicit CommandRegistry(QObject* p=nullptr) : QObject(p) {}

    void registerCommand(QSharedPointer<ICommand> cmd) {
        cmds_[cmd->getName() ] = cmd;
    }
    QSharedPointer<ICommand> find(const QString& name) const {
        return cmds_.value(name);
    }
    QStringList names() const { return cmds_.keys(); }
    QList<QSharedPointer<ICommand>> all() const { return cmds_.values(); }

private:
    QMap<QString, QSharedPointer<ICommand>> cmds_;
};

#endif // COMMANDREGISTRY_H
