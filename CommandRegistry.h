#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

class CommandRegistry : public QObject {
    Q_OBJECT
public:
    explicit CommandRegistry(QObject* p=nullptr) : QObject(p) {}

    void registerCommand(QSharedPointer<ICommand> cmd) {
        cmds_[cmd->name()] = cmd;
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
