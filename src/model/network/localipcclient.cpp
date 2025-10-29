#include "LocalIpcClient.h"
#include <QDataStream>

LocalIpcClient::LocalIpcClient(QString name, QObject* parent)
    : ITransport(parent), name_(std::move(name)) {}

void LocalIpcClient::start() {
    if (sock_) return;
    sock_ = new QLocalSocket(this);
    //connect(sock_, &QLocalSocket::connected,    this, &LocalIpcClient::connected);
    //connect(sock_, &QLocalSocket::disconnected, this, &LocalIpcClient::disconnected);
    connect(sock_, &QLocalSocket::readyRead,    this, &LocalIpcClient::onReadyRead);

    sock_->connectToServer(name_);
}

void LocalIpcClient::stop() {
    if (!sock_) return;
    sock_->disconnectFromServer();
    sock_->deleteLater();
    sock_ = nullptr;
}

bool LocalIpcClient::send(const QByteArray& data) {
    if (!sock_ || sock_->state() != QLocalSocket::ConnectedState) return false;
    auto frame = pack(data);
    qint64 w = sock_->write(frame);
    sock_->flush();
    return w == frame.size();
}

void LocalIpcClient::onReadyRead() {
    buf_.append(sock_->readAll());

    // desempaquetado: [len(4 bytes LE)] + payload
    while (buf_.size() >= 4) {
        quint32 len;
        memcpy(&len, buf_.constData(), 4);
        if (buf_.size() < 4 + int(len)) break;
        QByteArray payload = buf_.mid(4, len);
        buf_.remove(0, 4 + int(len));
        emit messageReceived(payload);
    }
}

QByteArray LocalIpcClient::pack(const QByteArray& payload) {
    QByteArray out;
    out.resize(4 + payload.size());
    quint32 len = payload.size();
    memcpy(out.data(), &len, 4);
    memcpy(out.data()+4, payload.constData(), payload.size());
    return out;
}
