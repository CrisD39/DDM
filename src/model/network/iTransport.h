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

signals:
    void messageReceived(const QByteArray& data);

public slots:
    virtual void start() {}
    virtual void stop() {}
};
