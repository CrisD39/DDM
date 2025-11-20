#pragma once

#include <QByteArray>
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
     * @brief Construye una respuesta de éxito
     * @param command Nombre del comando ejecutado (ej: "create_line")
     * @param args Objeto JSON con los argumentos de respuesta (debe incluir "id")
     * @return QByteArray con el JSON compacto listo para enviar
     */
    static QByteArray buildSuccessResponse(
        const QString& command,
        const QJsonObject& args
    );
    
    /**
     * @brief Construye una respuesta de error
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
};
