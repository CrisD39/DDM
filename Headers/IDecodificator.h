#ifndef IDECODIFICATOR_H
#define IDECODIFICATOR_H
#include <QObject>
#include <QByteArray>

class IDecodificator: public QObject{
    Q_OBJECT

public:
    // IDecodificator.h
    virtual void decode(const QByteArray &message) = 0;


protected:
    QByteArray decodificatedMessage;

};

#endif // IDECODIFICATOR_H
