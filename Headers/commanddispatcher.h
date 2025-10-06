// CommandDispatcher.h
#pragma once
#include <QObject>
#include <QString>

class CommandRegistry;
/*
    Interfaz/implementación del despachador: recibe líneas, invoca el parser
    y ejecuta el comando correspondiente; maneja `help` y `exit`.
*/

class IInputParser;
class CommandContext;

class CommandDispatcher : public QObject {
    Q_OBJECT
public:
    CommandDispatcher(CommandRegistry* reg,
                      IInputParser* parser,
                      CommandContext& ctx,
                      QObject* parent = nullptr);

public slots:
    void onLine(const QString& line);

signals:
    void quitRequested();

private:
    CommandRegistry* reg_;
    IInputParser*    parser_;
    CommandContext&  ctx_;
};
