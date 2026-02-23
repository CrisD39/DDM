#include "addareacommand.h"
#include "model/commandContext.h"
#include <iostream>
#include <string>
#include <limits> // Necesario para numeric_limits

#include <sstream> // Necesario para std::stringstream

QPointF AddAreaCommand::requestPoint(const std::string& label) const {
    std::string input;
    double x = 0, y = 0;

    // Leer X
    std::cout << "\n[ENTRADA] Punto " << label << " -> x: ";
    std::getline(std::cin >> std::ws, input); // 'std::ws' elimina espacios/enters iniciales
    try {
        x = std::stod(input);
    } catch (...) { x = 0; }

    // Leer Y
    std::cout << "[ENTRADA] Punto " << label << " -> y: ";
    std::getline(std::cin, input);
    try {
        y = std::stod(input);
    } catch (...) { y = 0; }

    return QPointF(x, y);
}

#include "addareacommand.h"
#include "model/commandContext.h"
#include <QString>
#include <vector>

CommandResult AddAreaCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    // 1. Verificar que tengamos la cantidad correcta de argumentos
    // ax, ay, bx, by, cx, cy, dx, dy (8) + tipo (1) + color (1) = 10 argumentos
    if (inv.args.size() < 10) {
        return {false, "Error: Faltan argumentos. Uso: addArea(ax,ay,bx,by,cx,cy,dx,dy,tipo,color)"};
    }

    try {
        // Helper lambda to convert QString to double with validation
        auto validateAndConvert = [](const QString& value, const QString& fieldName) -> double {
            bool ok = false;
            double result = value.toDouble(&ok);
            if (!ok) {
                throw std::invalid_argument(("No se pudo convertir '" + fieldName + "' a un número: " + value).toStdString());
            }
            return result;
        };

        // 2. Extraer y convertir las coordenadas (los primeros 8 argumentos)
        double ax = validateAndConvert(inv.args[0], "ax");
        double ay = validateAndConvert(inv.args[1], "ay");
        double bx = validateAndConvert(inv.args[2], "bx");
        double by = validateAndConvert(inv.args[3], "by");
        double cx = validateAndConvert(inv.args[4], "cx");
        double cy = validateAndConvert(inv.args[5], "cy");
        double dx = validateAndConvert(inv.args[6], "dx");
        double dy = validateAndConvert(inv.args[7], "dy");

        // 3. Extraer tipo de área y color
        int areaType = 0;
        {
            bool ok = false;
            areaType = inv.args[8].toInt(&ok);
            if(!ok) throw std::invalid_argument("El tipo de área debe ser un entero.");
        }
        
        // El color ya es QString (según el error anterior inv.args es vector<QString>)
        QString areaColor = inv.args[9]; 

        // 4. Crear los puntos QPointF
        QPointF pa(ax, ay);
        QPointF pb(bx, by);
        QPointF pc(cx, cy);
        QPointF pd(dx, dy);

        // 5. Crear la entidad usando el contador del contexto
        // Esto mantiene la arquitectura de tu proyecto CLI
        AreaEntity area(ctx.commandCounter++, {pa, pb, pc, pd}, areaType, areaColor);

        // 6. Procesar y guardar
        area.calculateAndStoreCursors(ctx);

        return {true, QString("Área %1 creada exitosamente con los argumentos provistos.").arg(area.getId())};

    } catch (const std::exception& e) {
        return {false, QString("Error al procesar argumentos: %1. Asegúrese de usar números para coordenadas y tipo.").arg(e.what())};
    }
}
