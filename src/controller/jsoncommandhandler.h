#pragma once

#include <QObject>
#include <QByteArray>
#include <QJsonObject>

class CommandContext; // Forward declaration
class ITransport;     // Forward declaration

class JsonCommandHandler : public QObject
{
    Q_OBJECT
public:
    // Inyectamos el CommandContext y el ITransport para enviar respuestas
    explicit JsonCommandHandler(CommandContext* context, ITransport* transport, QObject *parent = nullptr);

public slots:
    // Este slot será llamado por nuestro MessageRouter
    void processJsonCommand(const QByteArray& jsonData);

private:
    CommandContext* m_context;   // El handler ahora opera sobre el contexto
    ITransport* m_transport;     // Para enviar respuestas al frontend
    
    // Métodos privados para manejo de comandos específicos
    void handleCreateLine(const QJsonObject& args);
    void handleDeleteLine(const QJsonObject& args);
    
    // Helper para enviar respuestas
    void sendResponse(const QByteArray& responseData);
    void handleStartCPA(QJsonObject);
};
