#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
#include <QObject>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "CommandDispatcher.h"
#include "CommandRegistry.h"
#include "CommandParser.h"
#include "EchoCommand.h"
#include "CommandContext.h"

class StdinReader : public QObject {
    Q_OBJECT
signals:
    void lineRead(const QString& line);
    void finished();
public slots:
    void readLoop() {
        QTextStream in(stdin);
        while (true) {
            QString l = in.readLine();
            if (l.isNull()) break;
            emit lineRead(l);
        }
        emit finished();
    }
};

int main(int argc, char* argv[]) {
    #ifdef Q_OS_WIN
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif

    QCoreApplication app(argc, argv);

    CommandContext ctx;
    CommandRegistry registry;
    CommandParser parser;

    registry.registerCommand(QSharedPointer<ICommand>(new EchoCommand(ctx)));

    CommandDispatcher dispatcher(&registry, &parser, ctx);

    QThread ioThread;
    StdinReader reader;
    reader.moveToThread(&ioThread);

    QObject::connect(&ioThread, &QThread::started, &reader, &StdinReader::readLoop);
    QObject::connect(&reader, &StdinReader::lineRead, &dispatcher, &CommandDispatcher::onLine);
    QObject::connect(&dispatcher, &CommandDispatcher::quitRequested, &app, &QCoreApplication::quit);
    QObject::connect(&reader, &StdinReader::finished, &ioThread, &QThread::quit);

    QTextStream out(stdout);
    out << "Consola lista (help | exit)\n"; out.flush();

    ioThread.start();
    const int code = app.exec();
    ioThread.wait();
    return code;
}

#include "main.moc"
