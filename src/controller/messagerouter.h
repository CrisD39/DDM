#pragma once

#include <QObject>
#include <QByteArray>

class DclConcController; // Forward declaration
class JsonCommandHandler; // Forward declaration

class MessageRouter : public QObject
{
    Q_OBJECT
public:
    explicit MessageRouter(DclConcController* dclController,
                           JsonCommandHandler* jsonHandler,
                           QObject *parent = nullptr);

public slots:
    // El único slot conectado al ITransport
    void onMessageReceived(const QByteArray& datagram);

private:
    DclConcController* m_dclController;
    JsonCommandHandler* m_jsonHandler;
};
