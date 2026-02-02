#include "messagerouter.h"
#include "dclConcController.h"  // Definiciones completas
#include "json/jsoncommandhandler.h"
#include <QDebug>

MessageRouter::MessageRouter(DclConcController* dclController,
                             JsonCommandHandler* jsonHandler,
                             QObject *parent)
    : QObject(parent),
    m_dclController(dclController),
    m_jsonHandler(jsonHandler)
{
    Q_ASSERT(m_dclController);
    Q_ASSERT(m_jsonHandler);
}

void MessageRouter::onMessageReceived(const QByteArray& datagram)
{
    // Criterio de ruteo simple:
    // Si empieza con '{', asumimos que es JSON.
    // Si no, es binario para el DclConcController.
    if (datagram.startsWith('{') && datagram.endsWith('}')) {
        m_jsonHandler->processJsonCommand(datagram);
    } else {
        m_dclController->onDatagram(datagram);
    }
}
