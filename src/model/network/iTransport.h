// ITransport.h
#pragma once
#include <QObject>
#include <QByteArray>

class ITransport : public QObject {
    Q_OBJECT
public:
    using QObject::QObject;
    virtual ~ITransport() = default;

    virtual bool send(const QByteArray& data) = 0;
    virtual bool isConnected() const { return true; }

signals:
    void messageReceived(const QByteArray& data);
    void connected();
    void disconnected();
    void error(const QString& errorString);

public slots:
    virtual void start() {}
    virtual void stop() {}
};
