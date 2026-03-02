#pragma once

#include <QJsonObject>
#include <QByteArray>
#include <QString>
#include "cpa.h"

class CpaCommandHandler
{
public:
    CpaCommandHandler(CommandContext* c);
    QByteArray startCPA(const QJsonObject& args);

private:
    QByteArray buildStartResponse(int index, CPAResult _cpa);
    CommandContext* ctx;
};
