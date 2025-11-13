#include "jsoncommandhandler.h"
#include "commandContext.h"  // Incluimos la definición completa
#include "entities/cursorEntity.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <qfloat16.h>

JsonCommandHandler::JsonCommandHandler(CommandContext* context, QObject *parent)
    : QObject(parent), m_context(context)
{
    Q_ASSERT(m_context);
}

void JsonCommandHandler::processJsonCommand(const QByteArray& jsonData)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "[JsonCommandHandler] Se recibió un JSON inválido:" << jsonData;
        return;
    }

    QJsonObject obj = doc.object();
    QString command = obj.value("command").toString();
    QJsonObject args = obj.value("args").toObject();

    if (command == "create_line") {
        // 1. Extraer argumentos con los defaults especificados
        int type = args.value("type").toInt(0);
        double x_d = args.value("x").toDouble(0.0);
        double y_d = args.value("y").toDouble(0.0);

        // 2. Extraer argumentos requeridos (length, azimut)
        QJsonValue length_val = args.value("length");
        QJsonValue azimut_val = args.value("azimut");

        if (!length_val.isDouble() || !azimut_val.isDouble()) {
            qWarning() << "[JsonCommandHandler] 'create_line' requiere 'length' y 'azimut' como números:" << jsonData;
            return;
        }

        double length_d = length_val.toDouble();
        double azimut_d = azimut_val.toDouble();

        // 3. Preparar los datos para el constructor de CursorEntity
        QPair<qfloat16, qfloat16> coords((qfloat16(x_d)), qfloat16(y_d));
        qfloat16 angle = qfloat16(azimut_d);
        qfloat16 length = qfloat16(length_d);
        int lineType = type;

        // 4. Obtener el próximo ID desde el contexto
        int newId = m_context->nextCursorId++;

        // 5. Llamar a emplaceCursorFront en el contexto
        CursorEntity& newCursor = m_context->emplaceCursorFront( //
            coords,     // QPair<qfloat16, qfloat16>
            angle,      // qfloat16 cursorAngle
            length,     // qfloat16 cursorLength
            lineType,   // int lineType
            newId,      // int cursorId
            true        //state
            );

        qInfo() << "[JsonCommandHandler] Entidad creada vía JSON:" << newCursor.toString();

    } else {
        qWarning() << "[JsonCommandHandler] Comando JSON no reconocido:" << command;
    }
}
