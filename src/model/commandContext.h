/*
  Estado global y I/O: streams `out/err`, contador de comandos;
    modelo de dominio (lista de `Track`, `nextTrackId`, `centerX/centerY`).
*/
#pragma once
#include <QTextStream>
#include <QStringConverter>   // Qt 6.x
#include <QList>
#include <QString>

// Dominio de tracks


struct Track {
    int id = 0;
    TrackType type = TrackType::Unknown;
    QString identity; // de flags -s|-a|-b (opcional)
    double x = 0.0;
    double y = 0.0;
};

struct CommandContext {
    CommandContext() : out(stdout), err(stderr) {
        out.setEncoding(QStringConverter::Utf8);
        err.setEncoding(QStringConverter::Utf8);
    }
    QTextStream out;
    QTextStream err;

    // Evita ejecutar dos veces el mismo comando consecutivo (anti-replay)
    QString lastCommandLine;
    quint64 lastCommandHash = 0;

    // Contador de líneas/comandos: "1 echo list", "2 echo add ..."
    int commandCounter = 1;

    // Estado del dominio
    QList<Track> tracks;
    int nextTrackId = 1;

    // "center" global
    double centerX = 0.0;
    double centerY = 0.0;
};
