#include <QCoreApplication>
#include <QTextStream>
#include <QThread>
#include <QObject>

class StdinReader : public QObject {
    Q_OBJECT
public slots:
    void readLoop() {
        QTextStream in(stdin);
        // Lee bloqueando, pero en un hilo separado (no frena el event loop)
        while (true) {
            QString line = in.readLine();        // null en EOF (Ctrl+D/Linux-Mac, Ctrl+Z+Enter/Windows)
            if (line.isNull()) break;

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

    QThread ioThread;
    StdinReader reader;
    reader.moveToThread(&ioThread);

    QObject::connect(&ioThread, &QThread::started, &reader, &StdinReader::readLoop);

    QObject::connect(&reader, &StdinReader::lineRead, [&app](const QString& line){
        static int contador = 1;
        QTextStream out(stdout);

        if (line.trimmed().compare("salir", Qt::CaseInsensitive) == 0) {
            out << "Adiós!\n";
            out.flush();
            QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
            return;
        }

        out << line << " " << contador++ << "\n";
        out.flush();
    });

    QObject::connect(&reader, &StdinReader::finished, &ioThread, &QThread::quit);

    QTextStream out(stdout);
    out << "Escribe algo (\"salir\" para terminar):\n";
    out.flush();

    ioThread.start();
    int code = app.exec();
    ioThread.wait();              // Asegura que el hilo termine antes de salir
    return code;
}

#include "main.moc"
