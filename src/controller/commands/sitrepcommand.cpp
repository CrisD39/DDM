#include "Commands/sitrepCommand.h"
#include "model/utils/maths.h"  // ajustá ruta si está en utils/...

#include <QThread>
#include <QTextStream>
#include <QStringList>
#include <deque>

// Limpieza de pantalla ANSI (Windows 10+ / Linux / la mayoría de terminales modernas)
static void clearScreen(QTextStream& out) {
    out << "\x1B[2J\x1B[H";
}

// Render de la tabla (snapshot para evitar iterar sobre contenedor vivo)
static void printSitrep(QTextStream& out, const CommandContext& ctx, const std::deque<Track>& tracksSnap) {
    out << "SITREP (refresh=2000ms)  Tracks=" << tracksSnap.size() << "\n";
    out << "Track  Identidad        Az/Dt(DM)        Info Ampliatoria\n";
    out << "---------------------------------------------------------------\n";

    for (const Track& t : tracksSnap) {
        const int id = t.getId();

        // Coordenadas en Data Miles (DM)
        const double xDm = static_cast<double>(t.getX());
        const double yDm = static_cast<double>(t.getY());

        // Cálculos 100% en DM
        const double azDeg  = RadarMath::azimuthDeg(xDm, yDm);
        const double distDm = RadarMath::distanceDm(xDm, yDm);

        const QString ident = TrackData::toQString(t.getIdentity());
        const QString info  = ctx.sitrepInfo(id);

        out
            << QString("%1").arg(id, 5) << "  "
            << QString("%1").arg(ident, 14) << "  "
            << QString("%1/%2").arg(azDeg, 6, 'f', 1).arg(distDm, 8, 'f', 2) << "  "
            << info
            << "\n";
    }

    out << "\nCTRL+C para salir.\n";
    out.flush();
}

CommandResult SitrepCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    const QStringList& args = inv.args;

    // args NO incluye el nombre del comando: "sitrep watch" => args[0]="watch"
    const QString sub = args.isEmpty() ? "list" : args[0].toLower();

    if (sub == "list") {
        const std::deque<Track> snap = ctx.getTracks();

        QString outMsg;
        QTextStream oss(&outMsg);

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

        // No retorna: se corta con CTRL+C
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
