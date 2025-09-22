#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
#include <QObject>

#include "CommandDispatcher.cpp"
#include "CommandRegistry.h"
#include "CommandParser.h"
#include "EchoCommand.h"
#include "CommandContext.h"

class StdinReader : public QObject {
    Q_OBJECT
public slots:
    void readLoop() {
        QTextStream in(stdin);
        while (true) {
            QString line = in.readLine();
            if (line.isNull()) break;  // EOF (Ctrl+D / Ctrl+Z+Enter)

            emit lineRead(line);

            if (line.trimmed().compare("salir", Qt::CaseInsensitive) == 0)
                break;
        }
        emit finished();
    }

signals:
    void lineRead(const QString &line);
    void finished();
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // ====== Setup de comandos ======
    CommandContext ctx;
    CommandRegistry registry;
    CommandParser parser;
    CommandDispatcher dispatcher(&registry, &parser);

    // Registrar comandos
    registry.registerCommand(QSharedPointer<ICommand>(new EchoCommand(ctx)));

    // ====== Hilo de entrada ======
    QThread ioThread;
    StdinReader reader;
    reader.moveToThread(&ioThread);

    QObject::connect(&ioThread, &QThread::started, &reader, &StdinReader::readLoop);

    // Cada línea recibida → pasa al dispatcher
    QObject::connect(&reader, &StdinReader::lineRead, &dispatcher, &CommandDispatcher::onLine);

    // Si un comando pide salir → cerrar app
    QObject::connect(&dispatcher, &CommandDispatcher::quitRequested, &app, &QCoreApplication::quit);

    QObject::connect(&reader, &StdinReader::finished, &ioThread, &QThread::quit);

    QTextStream out(stdout);
    out << "Consola lista (escribe 'help' o 'exit' para salir):\n";
    out.flush();

    ioThread.start();
    int code = app.exec();
    ioThread.wait();
    return code;
}

#include "main.moc"
