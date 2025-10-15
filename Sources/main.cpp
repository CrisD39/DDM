#include "Encoderlpd.h"
#include "Sources/dclconccontroller.h"
#include "clientsocket.h"
#include "fcdecodificator.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
#include <QObject>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <stdinreader.h>

#include "CommandDispatcher.h"
#include "CommandRegistry.h"
#include "CommandParser.h"
#include "Commands/AddCommand.h"
#include "Commands/DeleteCommand.h"
#include "Commands/CenterCommand.h"
#include "Commands/ListCommand.h"
#include "CommandContext.h"
#include "QTimer"
#ifdef Q_OS_WIN
#endif
#include <windows.h>


#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_RESET   "\x1b[0m"

static void enableAnsiColorsOnWindows() {
    DWORD mode = 0;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &mode)) {
        mode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(hOut, mode);
    }
    HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
    if (hErr != INVALID_HANDLE_VALUE && GetConsoleMode(hErr, &mode)) {
        mode |= 0x0004;
        SetConsoleMode(hErr, mode);
    }
}

int main(int argc, char* argv[]) {

#ifdef Q_OS_WIN
    enableAnsiColorsOnWindows();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    QCoreApplication app(argc, argv);

    CommandContext ctx;
    CommandRegistry registry;
    CommandParser parser;

    // registrar comandos
    registry.registerCommand(QSharedPointer<ICommand>(new AddCommand()));
    registry.registerCommand(QSharedPointer<ICommand>(new DeleteCommand()));
    registry.registerCommand(QSharedPointer<ICommand>(new CenterCommand()));
    registry.registerCommand(QSharedPointer<ICommand>(new ListCommand()));
    CommandDispatcher dispatcher(&registry, &parser, ctx);


    QThread ioThread;
    StdinReader reader;
    reader.moveToThread(&ioThread);

    QObject::connect(&ioThread, &QThread::started, &reader, &StdinReader::readLoop);
    QObject::connect(&reader, &StdinReader::lineRead, &dispatcher, &CommandDispatcher::onLine);
    QObject::connect(&dispatcher, &CommandDispatcher::quitRequested, &app, &QCoreApplication::quit);
    QObject::connect(&reader, &StdinReader::finished, &ioThread, &QThread::quit);

    QTextStream out(stdout);
    ctx.out << ANSI_YELLOW << "Consola lista (help | exit)" << ANSI_RESET << "\n";
    ctx.out.flush();

    encoderLPD *encoder = new encoderLPD();

    clientSocket *socket = new clientSocket(nullptr);
    auto* decoder = new FCDecodificator();
    auto* controller = new DclConcController(socket, decoder, &app);

    QTimer timer;

    QObject::connect(&timer, &QTimer::timeout, &timer, [&ctx, encoder, socket]() {
        QByteArray message = encoder->buildFullMessage(ctx);
        socket->sendMessage(message);
    });



    timer.start(1000);

    ioThread.start();
    const int code = app.exec();
    ioThread.wait();
    return code;
}
