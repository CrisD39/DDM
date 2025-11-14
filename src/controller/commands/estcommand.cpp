#include "EstCommand.h"
#include <algorithm>
#include <cmath>

// Calcula azimut en grados [0, 360)
static double calcAzimut(double dx, double dy) {
    double ang = std::atan2(dx, dy) * 180.0 / M_PI;
    return (ang < 0) ? ang + 360.0 : ang;
}

static double calcDist(double dx, double dy) {
    return std::sqrt(dx*dx + dy*dy);
}

CommandResult EstCommand::execute(const CommandInvocation& inv,
                                  CommandContext& ctx) const
{
    // 1) Validación de argumentos mínimos: est <id> [-v vel] [-t tiempo]
    if (inv.args.size() < 2) {
        return { false, QString("Uso incorrecto. ") + usage() };
    }

    // 2) Parseo del id
    bool ok = false;
    const int id = inv.args[0].toInt(&ok);
    if (!ok) {
        return {false, "ID inválido"};
    }

    // 3) Buscar el track por id en ctx.tracks (std::deque<Track>)
    auto it = std::find_if(ctx.tracks.begin(), ctx.tracks.end(),
                           [id](const Track& tr) {
                               return tr.getId() == id;
                           });

    if (it == ctx.tracks.end()) {
        return { false, QString("No existe el track con id %1").arg(id) };
    }

    const Track& t = *it;

    // 4) Parseo de -v y -t
    double velocidad = -1.0;
    double tiempo    = -1.0;

    // AHORA EMPIEZA EN 1, NO EN 2
    for (int i = 1; i < inv.args.size(); ++i) {
        const QString& arg = inv.args[i];

        if (arg == "-v" && i + 1 < inv.args.size()) {
            double v = inv.args[i + 1].toDouble(&ok);
            if (!ok || v <= 0) {
                return { false, QStringLiteral("Velocidad inválida.") };
            }
            velocidad = v;
        } else if (arg == "-t" && i + 1 < inv.args.size()) {
            double t_h = inv.args[i + 1].toDouble(&ok);
            if (!ok || t_h <= 0) {
                return { false, QStringLiteral("Tiempo inválido.") };
            }
            tiempo = t_h;
        }
    }

    if (velocidad < 0 && tiempo < 0) {
        return { false,
                QStringLiteral("Debe especificar -v <velocidad> o -t <tiempo_horas>") };
    }

    // 5) Geometría: por ahora OwnShip en (0,0)
    double dx = t.getX();
    double dy = t.getY();

    double distancia = calcDist(dx, dy);
    double azimut    = calcAzimut(dx, dy);

    // 6) Completar datos: si viene v, saco t; si viene t, saco v.
    if (velocidad > 0 && tiempo < 0) {
        tiempo = distancia / velocidad;
    } else if (tiempo > 0 && velocidad < 0) {
        velocidad = distancia / tiempo;
    }

    double rumbo = azimut;

    // 7) Mensaje
    QString msg;
    msg += QString("EST para track id %1\n").arg(id);
    msg += QString("Distancia: %1\n").arg(distancia);
    msg += QString("Azimut: %1°\n").arg(QString::number(azimut, 'f', 1));
    msg += QString("Rumbo: %1°\n").arg(QString::number(rumbo, 'f', 1));
    msg += QString("Velocidad: %1 kn\n").arg(QString::number(velocidad, 'f', 2));
    msg += QString("Tiempo: %1 h").arg(QString::number(tiempo, 'f', 3));

    // 8) Éxito
    return { true, msg };
}
