#pragma once

#include <QJsonObject>
#include <QByteArray>
#include <QString>

class CpaCommandHandler
{
public:
    CpaCommandHandler();
    QString startCPA(const QJsonObject& args);
};

#endif // CPACOMMANDHANDLER_H
