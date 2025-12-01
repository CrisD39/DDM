#include "jsoncommandhandler.h"
#include "../handlers/linecommandhandler.h"
#include "jsonresponsebuilder.h"
#include "commandContext.h"
#include "network/iTransport.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

JsonCommandHandler::JsonCommandHandler(CommandContext* context, ITransport* transport, QObject *parent)
    : QObject(parent), m_transport(transport){
    Q_ASSERT(context);
    Q_ASSERT(m_transport);
    
    // Crea un puntero único a LineCommandHandler y se asegura ownership
    m_lineHandler = std::make_unique<LineCommandHandler>(context, transport);
    
    // Inicializar el mapa de comandos
    initializeCommandMap();
}

JsonCommandHandler::~JsonCommandHandler() = default;

void JsonCommandHandler::processJsonCommand(const QByteArray& jsonData){   
    QJsonObject obj;
    // En obj obtenemos el resultado del parseo
    if (!parseJson(jsonData, obj))
        return;
    
    QString command = obj.value("command").toString();
    QJsonObject args = obj.value("args").toObject();
    routeCommand(command, args);
}

bool JsonCommandHandler::parseJson(const QByteArray& jsonData, QJsonObject& outObject){
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        sendParseError(parseError.errorString());
        return false;
    }
    
    if (!doc.isObject()) {
        sendParseError("El JSON debe ser un objeto");
        return false;
    }
    
    outObject = doc.object();
    return true;
}

void JsonCommandHandler::initializeCommandMap()
{
    // Registrar comandos de línea
    m_commandMap["create_line"] = [this](const QJsonObject& args) {
        return m_lineHandler->createLine(args);
    };
    
    m_commandMap["delete_line"] = [this](const QJsonObject& args) {
        return m_lineHandler->deleteLine(args);
    };

    // Agregar nuevos comandos ACA.
}

void JsonCommandHandler::routeCommand(const QString& command, const QJsonObject& args)
{
    auto it = m_commandMap.find(command);
    
    if (it != m_commandMap.end()) {
        QByteArray response = it.value()(args);
        sendResponse(response);
    } else {
        sendUnknownCommandError(command);
    }
}

void JsonCommandHandler::sendResponse(const QByteArray& responseData)
{
    if (!m_transport) {
        qWarning() << "[JsonCommandHandler] Transport no disponible";
        return;
    }
    
    if (!m_transport->send(responseData)) {
        qWarning() << "[JsonCommandHandler] Fallo al enviar respuesta";
    }
}

void JsonCommandHandler::sendParseError(const QString& errorDetail)
{
    qWarning() << "[JsonCommandHandler] Error de parseo JSON:" << errorDetail;
    QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
        "unknown",
        "INVALID_JSON",
        QString("Error al parsear JSON: %1").arg(errorDetail)
    );
    sendResponse(errorResponse);
}

void JsonCommandHandler::sendUnknownCommandError(const QString& command)
{
    qWarning() << "[JsonCommandHandler] Comando no reconocido:" << command;
    QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
        command,
        "UNKNOWN_COMMAND",
        QString("Comando no reconocido: %1").arg(command)
    );
    sendResponse(errorResponse);
}
