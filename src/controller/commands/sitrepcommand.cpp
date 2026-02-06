#include "Commands/sitrepCommand.h"
#include "enums.h"

#include <QThread>
#include <QtMath>
#include <cmath>

// Limpieza de pantalla ANSI (Windows 10+ / Linux / la mayoría de terminales modernas)
static void clearScreen(QTextStream& out) {
    out << "\x1B[2J\x1B[H";
}

static double norm360(double deg) {
    while (deg < 0.0) deg += 360.0;
    while (deg >= 360.0) deg -= 360.0;
    return deg;
}

// Azimut desde "Norte" sentido horario asumiendo:
// x: derecha, y: arriba. Az = atan2(x,y).
static double computeAzDeg(float x, float y) {
    const double rad = std::atan2((double)x, (double)y);
    return norm360(rad * 180.0 / M_PI);
}

static double computeDistDM(float x, float y) {
    return std::sqrt((double)x*(double)x + (double)y*(double)y);
}

// Render de la tabla (snapshot para evitar iterar sobre contenedor vivo)
static void printSitrep(QTextStream& out, const CommandContext& ctx, const std::deque<Track>& tracksSnap) {
    out << "SITREP (refresh=2000ms)  Tracks=" << tracksSnap.size() << "\n";
    out << "Track  Ident       Az/Dt(DM)        X         Y        Info\n";
    out << "-----------------------------------------------------------------------\n";

    for (const Track& t : tracksSnap) {
        const int id = t.getId();
        const float x = t.getX();
        const float y = t.getY();

        const double az = computeAzDeg(x, y);
        const double dt = computeDistDM(x, y);

        const QString ident = TrackData::toQString(t.getIdentity());
        const QString info  = ctx.sitrepInfo(id);

        out
            << QString("%1").arg(id, 5) << "  "
            << QString("%1").arg(ident, 10) << "  "
            << QString("%1/%2").arg(az, 6, 'f', 1).arg(dt, 8, 'f', 2) << "  "
            << QString("%1").arg(x, 8, 'f', 3) << "  "
            << QString("%1").arg(y, 8, 'f', 3) << "  "
            << info
            << "\n";
    }

    out << "\nCTRL+C para salir.\n";
    out.flush();
}

CommandResult SitrepCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    const QStringList& args = inv.args;

    // subcomando: por cómo está AddCommand, args NO incluye el nombre del comando,
    // entonces para "sitrep watch" args[0] = "watch".
    const QString sub = args.isEmpty() ? "list" : args[0].toLower();

    if (sub == "list") {
        const std::deque<Track> snap = ctx.getTracks();

        // En list, yo NO limpiaría pantalla para no molestar,
        // pero si querés consistencia visual, podés limpiar también.
        // clearScreen(ctx.out);

        QString outMsg;
        QTextStream oss(&outMsg);

        // Reutilizamos la misma rutina, pero escribiendo en string para devolver por CommandResult
        printSitrep(oss, ctx, snap);

        return {true, outMsg};
    }

    if (sub == "watch") {
        static constexpr int REFRESH_MS = 2000;

        while (true) {
            const std::deque<Track> snap = ctx.getTracks();
            clearScreen(ctx.out);
            printSitrep(ctx.out, ctx, snap);
            QThread::msleep(REFRESH_MS);
        }

        // no retorna: se corta con CTRL+C
    }

    if (sub == "delete") {
        if (args.size() < 2) {
            return {false, "Uso: " + usage()};
        }

        bool ok = false;
        const int id = args[1].toInt(&ok);
        if (!ok) {
            return {false, "trackId invalido: " + args[1]};
        }

        const bool erased = ctx.eraseTrackById(id);
        if (!erased) {
            return {false, QString("No existe track con id=%1").arg(id)};
        }

        // limpieza opcional del extra SITREP
        ctx.sitrepExtra.erase(id);

        return {true, QString("OK delete → id=%1").arg(id)};
    }

    if (sub == "info") {
        if (args.size() < 3) {
            return {false, "Uso: sitrep info <trackId> <texto>"};
        }

        bool ok = false;
        const int id = args[1].toInt(&ok);
        if (!ok) {
            return {false, "trackId invalido: " + args[1]};
        }

        if (!ctx.findTrackById(id)) {
            return {false, QString("No existe track con id=%1").arg(id)};
        }

        // texto = resto desde args[2]
        QString text;
        for (int i = 2; i < args.size(); ++i) {
            if (i > 2) text += " ";
            text += args[i];
        }

        ctx.setSitrepInfo(id, text);
        return {true, QString("OK sitrep info → id=%1").arg(id)};
    }

    return {false, QString("Subcomando no soportado: %1\nUso: %2").arg(sub, usage())};
}
