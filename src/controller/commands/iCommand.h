/*
    Interfaz común de comandos (`getName()`, `getDescription()`, `usage()`,
    `execute(...)`), más tipos `CommandResult` y `CommandInvocation`.
*/

#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QObject>

#include "commandContext.h"

using Type      = TrackData::Type;
using Identity  = TrackData::Identity;
using TrackMode = TrackData::TrackMode;

struct CommandResult {
    bool ok = true;
    QString message;
};


struct CommandInvocation {
    QString name;                     // comando principal
    QStringList path;                 // subcomandos
    QMap<QString, QString> opts;      // --opt=val / -f
    QStringList args;                 // posicionales
};

class ICommand : public QObject
{
    Q_OBJECT
public:
    virtual QString getName() const = 0;
    virtual QString getDescription() const = 0;
    virtual QString usage() const = 0;
    virtual CommandResult execute(const CommandInvocation& inv, CommandContext& ctx) const = 0;

};

#endif // ICOMMAND_H
