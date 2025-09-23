// CommandContext.h
#pragma once
#include <QTextStream>
#include <QStringConverter>   // Qt 6.x

struct CommandContext {
    CommandContext() : out(stdout), err(stderr) {
        out.setEncoding(QStringConverter::Utf8);
        err.setEncoding(QStringConverter::Utf8);
    }
    QTextStream out;
    QTextStream err;

    int echoCounter = 1; // dejalo si lo usás; agregá aquí cualquier otro estado que ya tenías
};
