#ifndef IDECODIFICATOR_H
#define IDECODIFICATOR_H
#include <QObject>
#include <QByteArray>

class IDecodificator: public QObject{
    Q_OBJECT

public:
    virtual void decodificate(QByteArray message) const = 0;

protected:
    QByteArray decodificatedMessage;

};

#endif // IDECODIFICATOR_H
