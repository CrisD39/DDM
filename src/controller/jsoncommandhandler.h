#pragma once

#include <QObject>
#include <QByteArray>

class CommandContext; // Forward declaration

class JsonCommandHandler : public QObject
{
    Q_OBJECT
public:
    // Inyectamos el CommandContext en lugar del Dispatcher
    explicit JsonCommandHandler(CommandContext* context, QObject *parent = nullptr);

public slots:
    // Este slot será llamado por nuestro MessageRouter
    void processJsonCommand(const QByteArray& jsonData);

private:
    CommandContext* m_context; // El handler ahora opera sobre el contexto
};
