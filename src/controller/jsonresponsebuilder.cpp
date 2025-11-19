#include "jsonresponsebuilder.h"
#include <QJsonDocument>
#include <QDebug>

QByteArray JsonResponseBuilder::buildSuccessResponse(
    const QString& command,
    const QJsonObject& args)
{
    QJsonObject response;
    response["status"] = "success";
    response["command"] = command;
    response["args"] = args;
    
    QJsonDocument doc(response);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    
    qDebug() << "[JsonResponseBuilder] Respuesta exitosa construida:" << jsonData;
    
    return jsonData;
}

QByteArray JsonResponseBuilder::buildErrorResponse(
    const QString& command,
    const QString& errorCode,
    const QString& errorMessage,
    const QJsonObject& details)
{
    QJsonObject response;
    response["status"] = "error";
    response["command"] = command;
    
    QJsonObject argsObj;
    argsObj["code"] = errorCode;
    argsObj["message"] = errorMessage;
    
    if (!details.isEmpty()) {
        argsObj["details"] = details;
    }
    
    response["args"] = argsObj;
    
    QJsonDocument doc(response);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    
    qWarning() << "[JsonResponseBuilder] Respuesta de error construida:" << jsonData;
    
    return jsonData;
}
