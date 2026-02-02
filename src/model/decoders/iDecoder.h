#ifndef IDECODER_H
#define IDECODER_H
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

#endif // IDECODER_H
