#pragma once
#include <memory>
#include <QString>

class ITransport;

enum class TransportKind { Udp, LocalIpc };

struct TransportOpts {
    // Para UDP: bind y peer en formato "udp://IP:PORT"
    QString endpointBind;
    QString endpointPeer;
    // Para Local IPC: name en "local://NAME" o solo "NAME"
    QString localName;
};

std::unique_ptr<ITransport> makeTransport(TransportKind kind, const TransportOpts& o, QObject* parent=nullptr);
