#pragma once

#include <QByteArray>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

/**
 * @brief Builder para construir respuestas JSON estandarizadas al frontend
 * 
 * Esta clase sigue el patrón Factory para crear respuestas JSON consistentes
 * según la especificación del documento DDM_BACKEND_INTERFACE.md
 */
class JsonResponseBuilder
{
public:
    /**
     * @brief Construye una respuesta de éxito genérica
     * @param command Nombre del comando ejecutado (ej: "create_line")
     * @param args Objeto JSON con los argumentos de respuesta
     * @return QByteArray con el JSON compacto listo para enviar
     */
    static QByteArray buildSuccessResponse(
        const QString& command,
        const QJsonObject& args
    );
    
    /**
     * @brief Construye una respuesta de éxito para comandos que crean líneas
     * @param command Nombre del comando (ej: "create_line")
     * @param createdId ID de la línea creada (ej: "LINE_001")
     * @param allLines Array JSON con todas las líneas actuales
     * @return QByteArray con el JSON compacto listo para enviar
     */
    static QByteArray buildLineCreatedResponse(
        const QString& command,
        const QString& createdId,
        const QJsonArray& allLines
    );
    
    /**
     * @brief Construye una respuesta de éxito para comandos que eliminan líneas
     * @param command Nombre del comando (ej: "delete_line")
     * @param deletedId ID de la línea eliminada (ej: "LINE_001")
     * @param remainingLines Array JSON con todas las líneas restantes
     * @return QByteArray con el JSON compacto listo para enviar
     */
    static QByteArray buildLineDeletedResponse(
        const QString& command,
        const QString& deletedId,
        const QJsonArray& remainingLines
    );
    
    /**
     * @brief Construye una respuesta de éxito para comandos que listan líneas
     * @param command Nombre del comando (ej: "list_lines")
     * @param allLines Array JSON con todas las líneas
     * @return QByteArray con el JSON compacto listo para enviar
     */
    static QByteArray buildLineListResponse(
        const QString& command,
        const QJsonArray& allLines
    );
    
    /**
     * @brief Construye una respuesta de error genérica
     * @param command Nombre del comando que falló
     * @param errorCode Código de error estructurado (ej: "INVALID_AZIMUT")
     * @param errorMessage Descripción legible del error para logging
     * @param details Información adicional opcional para debugging
     * @return QByteArray con el JSON compacto listo para enviar
     */
    static QByteArray buildErrorResponse(
        const QString& command,
        const QString& errorCode,
        const QString& errorMessage,
        const QJsonObject& details = QJsonObject()
    );
    
    /**
     * @brief Construye una respuesta de error de validación
     * @param command Nombre del comando que falló
     * @param fieldName Nombre del campo inválido
     * @param fieldValue Valor recibido (convertido a string)
     * @param expectedRange Rango o formato esperado (ej: "0.0 - 359.9")
     * @return QByteArray con el JSON compacto listo para enviar
     */
    static QByteArray buildValidationErrorResponse(
        const QString& command,
        const QString& fieldName,
        const QString& fieldValue,
        const QString& expectedRange
    );
};
