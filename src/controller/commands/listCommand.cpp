/*
    Comando `list`: muestra la lista de `Track` actuales con formato tabular.
    Incluye cabecera, separadores y columnas con ancho fijo definido por constantes.
*/

#include "commands/listCommand.h"
#include "enumsTrack.h"
#include "entities/track.h"
#include <QString>
#include <QTextStream>
#include <iomanip>

CommandResult ListCommand::execute(const CommandInvocation& inv, CommandContext& ctx) const {
    Q_UNUSED(inv);

    const auto& tracks = ctx.tracks;

    if (tracks.empty()) {
        return {true, "No hay tracks cargados."};
    }

    // ======= CONFIGURACIÓN DE COLUMNAS =======
    const int W_ID     = 4;
    const int W_TYPE   = 12;
    const int W_IDENT  = 10;
    const int W_COORD  = 12;
    const int W_RUMBO  = 10;
    const int W_VEL    = 10;

    // ======= IMPRESIÓN DE ENCABEZADO =======
    ctx.out << QString("+%1+%2+%3+%4+%5+%6+%7+\n")
                   .arg(QString(W_ID,    '-'))
                   .arg(QString(W_TYPE,  '-'))
                   .arg(QString(W_IDENT, '-'))
                   .arg(QString(W_COORD, '-'))
                   .arg(QString(W_COORD, '-'))
                   .arg(QString(W_RUMBO, '-'))
                   .arg(QString(W_VEL,   '-'));

    ctx.out << QString("|%1|%2|%3|%4|%5|%6|%7|\n")
                   .arg(" ID",     W_ID,    ' ')
                   .arg(" TYPE",   W_TYPE,  ' ')
                   .arg(" IDENT",  W_IDENT, ' ')
                   .arg(" X",      W_COORD, ' ')
                   .arg(" Y",      W_COORD, ' ')
                   .arg(" RUMBO",  W_RUMBO, ' ')
                   .arg(" VEL",    W_VEL,   ' ');

    ctx.out << QString("+%1+%2+%3+%4+%5+%6+%7+\n")
                   .arg(QString(W_ID,    '-'))
                   .arg(QString(W_TYPE,  '-'))
                   .arg(QString(W_IDENT, '-'))
                   .arg(QString(W_COORD, '-'))
                   .arg(QString(W_COORD, '-'))
                   .arg(QString(W_RUMBO, '-'))
                   .arg(QString(W_VEL,   '-'));

    // ======= IMPRESIÓN DE CADA TRACK =======
    for (const Track& t : tracks) {
        ctx.out << QString("|%1|%2|%3|%4|%5|%6|%7|\n")
        .arg(QString(" %1").arg(t.getId()),                          W_ID,    ' ')
            .arg(QString(" %1").arg(TrackData::toQString(t.getType())),  W_TYPE,  ' ')
            .arg(QString(" %1").arg(TrackData::toQString(t.getIdentity())), W_IDENT, ' ')
            .arg(QString(" %1").arg(t.getX(), 0, 'f', 3),                W_COORD, ' ')
            .arg(QString(" %1").arg(t.getY(), 0, 'f', 3),                W_COORD, ' ')
            .arg(QString(" %1").arg(t.getRumbo(), 0, 'f', 1),            W_RUMBO, ' ')
            .arg(QString(" %1").arg(t.getVelocidad(), 0, 'f', 1),        W_VEL,   ' ');
    }

    // ======= PIE DE TABLA =======
    ctx.out << QString("+%1+%2+%3+%4+%5+%6+%7+\n")
                   .arg(QString(W_ID,    '-'))
                   .arg(QString(W_TYPE,  '-'))
                   .arg(QString(W_IDENT, '-'))
                   .arg(QString(W_COORD, '-'))
                   .arg(QString(W_COORD, '-'))
                   .arg(QString(W_RUMBO, '-'))
                   .arg(QString(W_VEL,   '-'));

    return {true, ""};
}
