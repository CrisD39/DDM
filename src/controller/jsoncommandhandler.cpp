#include "jsoncommandhandler.h"
#include "jsonresponsebuilder.h"
#include "commandContext.h"  // Incluimos la definición completa
#include "entities/cursorEntity.h"
#include "network/iTransport.h"
#include "qjsonarray.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <qfloat16.h>
#include <QTimer>
#include <QElapsedTimer>
#include <qmath.h>

JsonCommandHandler::JsonCommandHandler(CommandContext* context, ITransport* transport, QObject *parent)
    : QObject(parent), m_context(context), m_transport(transport)
{
    Q_ASSERT(m_context);
    Q_ASSERT(m_transport);
}

void JsonCommandHandler::processJsonCommand(const QByteArray& jsonData)
{
    qDebug() << "[JsonCommandHandler] Comando recibido:" << jsonData;
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "[JsonCommandHandler] JSON inválido recibido:" << parseError.errorString();
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "unknown",
            "INVALID_JSON",
            QString("Error al parsear JSON: %1").arg(parseError.errorString())
        );
        sendResponse(errorResponse);
        return;
    }
    
    if (!doc.isObject()) {
        qWarning() << "[JsonCommandHandler] Se recibió un JSON que no es un objeto:" << jsonData;
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "unknown",
            "INVALID_JSON",
            "El JSON debe ser un objeto"
        );
        sendResponse(errorResponse);
        return;
    }

    QJsonObject obj = doc.object();
    QString command = obj.value("command").toString();
    QJsonObject args = obj.value("args").toObject();

    if(command == "start_cpa"){
        handleStartCPA(args);
    }

    if (command == "create_line") {
        handleCreateLine(args);
    } else if (command == "delete_line") {
        handleDeleteLine(args);
    } else {
        qWarning() << "[JsonCommandHandler] Comando JSON no reconocido:" << command;
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            command,
            "UNKNOWN_COMMAND",
            QString("Comando no reconocido: %1").arg(command)
        );
        sendResponse(errorResponse);
    }
}

void JsonCommandHandler::handleCreateLine(const QJsonObject& args)
{
    // 1. Extraer argumentos requeridos
    QJsonValue azimut_val = args.value("azimut");
    QJsonValue length_val = args.value("length");
    
    if (!azimut_val.isDouble() || !length_val.isDouble()) {
        qWarning() << "[JsonCommandHandler] 'create_line' requiere 'length' y 'azimut' como números";
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "create_line",
            "MISSING_REQUIRED_FIELDS",
            "Los campos 'azimut' y 'length' son requeridos y deben ser números"
        );
        sendResponse(errorResponse);
        return;
    }
    
    double azimut = azimut_val.toDouble();
    double length = length_val.toDouble();
    
    // 2. Validar rango de azimut [0.0, 359.9]
    if ((azimut < 0.0) || (azimut > 359.9)) {
        qWarning() << "[JsonCommandHandler] Error de validación: Azimut" << azimut << "fuera de rango [0.0, 359.9]";
        
        QJsonObject details;
        details["field"] = "azimut";
        details["value"] = azimut;
        details["expected"] = "0.0 - 359.9";
        
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "create_line",
            "INVALID_AZIMUT",
            QString("Azimut %1 fuera de rango [0.0, 359.9]").arg(azimut),
            details
        );
        sendResponse(errorResponse);
        return;
    }
    
    // 3. Validar rango de length [0.1, 256.0]
    if ((length < 0.1) || (length > 256.0)) {
        qWarning() << "[JsonCommandHandler] Error de validación: Length" << length << "fuera de rango [0.1, 256.0]";
        
        QJsonObject details;
        details["field"] = "length";
        details["value"] = length;
        details["expected"] = "0.1 - 256.0";
        
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "create_line",
            "INVALID_LENGTH",
            QString("Length %1 fuera de rango [0.1, 256.0]").arg(length),
            details
        );
        sendResponse(errorResponse);
        return;
    }
    
    // 4. Extraer argumentos opcionales con defaults
    int type = args.value("type").toInt(0);
    double x = args.value("x").toDouble(0.0);
    double y = args.value("y").toDouble(0.0);
    
    // 5. Validar type [0, 7]
    if ((type < 0) || (type > 7)) {
        qWarning() << "[JsonCommandHandler] Error de validación: Type" << type << "fuera de rango [0, 7]";
        
        QJsonObject details;
        details["field"] = "type";
        details["value"] = type;
        details["expected"] = "0 - 7";
        
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "create_line",
            "INVALID_TYPE",
            QString("Type %1 fuera de rango [0, 7]").arg(type),
            details
        );
        sendResponse(errorResponse);
        return;
    }
    
    qDebug() << "[JsonCommandHandler] Procesando create_line con azimut:" << azimut
             << "length:" << length << "x:" << x << "y:" << y << "type:" << type;
    
    // 6. Generar ID único (LINE_<nextCursorId>)
    int cursorId = m_context->nextCursorId++;
    QString lineId = QString("LINE_%1").arg(cursorId);
    
    qDebug() << "[JsonCommandHandler] Línea creada con ID:" << lineId;
    
    // 7. Preparar datos para crear CursorEntity
    QPair<qfloat16, qfloat16> coords((qfloat16(x)), qfloat16(y));
    qfloat16 angle = qfloat16(azimut);
    qfloat16 lengthFloat = qfloat16(length);
    
    // 8. Crear la entidad en el contexto
    try {
        CursorEntity& newCursor = m_context->emplaceCursorFront(
            coords,       // QPair<qfloat16, qfloat16>
            angle,        // qfloat16 cursorAngle
            lengthFloat,  // qfloat16 cursorLength
            type,         // int lineType
            cursorId,     // int cursorId
            true          // state
        );
        
        qInfo() << "[JsonCommandHandler] Entidad creada vía JSON:" << newCursor.toString();
        
        // 9. Construir respuesta exitosa con la lista completa de líneas
        QJsonArray linesArray;
        for (const auto& cursor : m_context->cursors) {
            QJsonObject lineObj;
            lineObj["id"] = QString("LINE_%1").arg(cursor.getCursorId());
            lineObj["x"] = static_cast<double>(cursor.getCoordinates().first);
            lineObj["y"] = static_cast<double>(cursor.getCoordinates().second);
            lineObj["azimut"] = static_cast<double>(cursor.getCursorAngle());
            lineObj["length"] = static_cast<double>(cursor.getCursorLength());
            lineObj["type"] = cursor.getLineType();
            lineObj["active"] = cursor.isActive();
            linesArray.append(lineObj);
        }
        
        QJsonObject responseArgs;
        responseArgs["created_id"] = lineId;
        responseArgs["lines"] = linesArray;
        
        QByteArray successResponse = JsonResponseBuilder::buildSuccessResponse(
            "create_line",
            responseArgs
        );
        
        qDebug() << "[JsonCommandHandler] Enviando respuesta exitosa con" << linesArray.size() << "líneas";
        sendResponse(successResponse);
        
    } catch (const std::exception& e) {
        qWarning() << "[JsonCommandHandler] Error al crear línea:" << e.what();
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "create_line",
            "BACKEND_ERROR",
            QString("Error interno al crear línea: %1").arg(e.what())
        );
        sendResponse(errorResponse);
    }
}

void JsonCommandHandler::handleDeleteLine(const QJsonObject& args)
{
    // 1. Extraer el ID de la línea a eliminar
    QJsonValue id_val = args.value("id");
    
    if (!id_val.isString() || id_val.toString().isEmpty()) {
        qWarning() << "[JsonCommandHandler] 'delete_line' requiere un 'id' válido como string";
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "delete_line",
            "MISSING_REQUIRED_FIELDS",
            "El campo 'id' es requerido y debe ser un string no vacío"
        );
        sendResponse(errorResponse);
        return;
    }
    
    QString lineId = id_val.toString();
    
    // 2. Extraer el número del ID (formato LINE_<número>)
    if (!lineId.startsWith("LINE_")) {
        qWarning() << "[JsonCommandHandler] Formato de ID inválido:" << lineId;
        
        QJsonObject details;
        details["id"] = lineId;
        details["expected_format"] = "LINE_<número>";
        
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "delete_line",
            "INVALID_ID_FORMAT",
            QString("Formato de ID inválido: %1. Esperado: LINE_<número>").arg(lineId),
            details
        );
        sendResponse(errorResponse);
        return;
    }
    
    bool ok;
    int cursorId = lineId.mid(5).toInt(&ok);  // Extrae el número después de "LINE_"
    
    if (!ok) {
        qWarning() << "[JsonCommandHandler] No se pudo extraer número del ID:" << lineId;
        
        QJsonObject details;
        details["id"] = lineId;
        
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "delete_line",
            "INVALID_ID_FORMAT",
            QString("No se pudo extraer número del ID: %1").arg(lineId),
            details
        );
        sendResponse(errorResponse);
        return;
    }
    
    qDebug() << "[JsonCommandHandler] Intentando eliminar línea con ID:" << lineId << "(cursor ID:" << cursorId << ")";
    
    // 3. Intentar eliminar el cursor del contexto
    bool deleted = m_context->eraseCursorById(cursorId);
    
    if (deleted) {
        qInfo() << "[JsonCommandHandler] Línea eliminada exitosamente:" << lineId;
        
        // 4. Construir respuesta exitosa con la lista completa actualizada
        QJsonArray linesArray;
        for (const auto& cursor : m_context->cursors) {
            QJsonObject lineObj;
            lineObj["id"] = QString("LINE_%1").arg(cursor.getCursorId());
            lineObj["x"] = static_cast<double>(cursor.getCoordinates().first);
            lineObj["y"] = static_cast<double>(cursor.getCoordinates().second);
            lineObj["azimut"] = static_cast<double>(cursor.getCursorAngle());
            lineObj["length"] = static_cast<double>(cursor.getCursorLength());
            lineObj["type"] = cursor.getLineType();
            lineObj["active"] = cursor.isActive();
            linesArray.append(lineObj);
        }
        
        QJsonObject responseArgs;
        responseArgs["deleted_id"] = lineId;
        responseArgs["lines"] = linesArray;
        
        QByteArray successResponse = JsonResponseBuilder::buildSuccessResponse(
            "delete_line",
            responseArgs
        );
        
        qDebug() << "[JsonCommandHandler] Enviando respuesta exitosa con" << linesArray.size() << "líneas restantes";
        sendResponse(successResponse);
        
    } else {
        qWarning() << "[JsonCommandHandler] Línea no encontrada:" << lineId;
        
        QJsonObject details;
        details["id"] = lineId;
        
        QByteArray errorResponse = JsonResponseBuilder::buildErrorResponse(
            "delete_line",
            "LINE_NOT_FOUND",
            QString("Línea no encontrada: %1").arg(lineId),
            details
        );
        sendResponse(errorResponse);
    }
}

void JsonCommandHandler::sendResponse(const QByteArray& responseData)
{
    if (m_transport) {
        m_transport->send(responseData);
    } else {
        qWarning() << "[JsonCommandHandler] No se puede enviar respuesta: transport es nullptr";
    }
}

void JsonCommandHandler::handleStartCPA(QJsonObject){
    // 1. Extraer argumentos requeridos
    QJsonValue cpa_id_val = args.value("id");
    QJsonValue TN_Source_val = args.value("track-a");
    QJsonValue TN_Target_val = args.value("track-b");

    int cpa_id = cpa_id_val.toInt();
    int TN_Source = TN_Source_val.toInt();
    int TN_Target = TN_Target_val.toInt();
    Track *trackA = m_context->findTrackById(TN_Source);
    Track *trackB = m_context->findTrackById(TN_Target);

    QPair<double, double> start_posA = {trackA->getX(), trackA->getY()};
    QPair<double, double> start_posB = {trackB->getX(), trackB->getY()};

    QElapsedTimer elapsedTime;

    QTimer *start_timer = new QTimerEvent(this);
    connect(start_timer, qOverload<>(&QTimer::start),
            this, [this]() {

        //QPair<double, double> start_posA = {trackA->getX(), trackA->getY()};
        //QPair<double, double> start_posB = {trackB->getX(), trackB->getY()};
        double timeA;
        double timeB;

        //tiene que pasar un tiempo
        QPair<double, double> final_posA = {trackA->getX(), trackA->getY()};
        QPair<double, double> final_posB = {trackB->getX(), trackB->getY()};

        QPair<double, double> deltaA = (final_posA - start_posA);
        QPair<double, double> deltaB = (final_posB - start_posB);

        QPair<double, double> velocityA =
            deltaA / timeInterval;

        QPair<double, double> velocityB =
            deltaB / timeInterval;

        QPair<QPair<double,double>, double> rPol_a = {deltaA, timeA};
        //         |   (x , y)    | time |

        QPair<QPair<double,double>, double> rPol_b = {deltaB, timeB};
        //         |   (x , y)    | time |


        double timeCPA = (deltaB - deltaA)/ (velocityA - velocityB);

        int timeInterval = start_timer->interval()


        QPair<double, double> posInTimeA = deltaA + velocityA * timeInterval;
        QPair<double, double> posInTimeB = deltaB + velocityB * timeInterval;

        QPair<double, double> cpaInCurrentTime =
            (final_posA - final_posB) / (velocityA-velocityB);


    });

    start_timer->start(5000);
    elapsedTime.start();

}


