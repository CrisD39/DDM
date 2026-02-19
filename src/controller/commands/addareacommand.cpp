#include "addareacommand.h"
#include "model/commandContext.h"
#include "model/entities/areaEntity.h"
#include "model/entities/cursorEntity.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <stdexcept>
#include <QtMath>

CommandResult AddAreaCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    try {
        // Solicitar puntos interactivamente
        QPointF a = requestPoint("A", ctx);
        QPointF b = requestPoint("B", ctx);
        QPointF c = requestPoint("C", ctx);
        QPointF d = requestPoint("D", ctx);

        // Solicitar tipo y color
        QString type = requestString("Tipo de línea", ctx);
        QString color = requestString("Color", ctx);

        // Crear el área y asociar los puntos
        AreaEntity area(ctx.nextAreaId++, {a, b, c, d}, type, color);

        // Calcular y agregar cursores al contexto
        area.calculateAndStoreCursors(ctx);

        // Agregar el área al contexto
        ctx.addArea(area);

        // Respuesta de éxito
        return {true, QString("Área creada con ID %1")
                        .arg(area.getId())};
    } catch (const std::exception& e) {
        return {false, QString("Error al crear el área: %1").arg(e.what())};
    }
}

QPointF AddAreaCommand::requestPoint(const QString& pointName, CommandContext& ctx) const {
    double x, y;

    ctx.out << "Por favor, ingrese las coordenadas para el punto " << pointName << ":\n";
    ctx.out << pointName << " x: ";
    ctx.out.flush();

    ctx.out << pointName << " y: ";
    ctx.out.flush();

    return QPointF(x, y);
}
