#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <memory>
#include "../services/cursorservice.h"

class CommandContext;
class ITransport;

class CursorCommandHandler
{
public:
    CursorCommandHandler(CommandContext* context, ITransport* transport);

    QByteArray createLine(const QJsonObject& args);
    QByteArray deleteLine(const QJsonObject& args);

private:
    CommandContext* m_context;
    ITransport* m_transport;
    std::unique_ptr<CursorService> m_cursorService;

    QByteArray buildLineCreatedSuccessResponse(const QString& lineId);
    QByteArray buildLineDeletedSuccessResponse(const QString& lineId);
};
