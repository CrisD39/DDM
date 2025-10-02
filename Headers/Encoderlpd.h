#ifndef ENCODERLPD_H
#define ENCODERLPD_H

#include "CommandContext.h"
#include <QObject>

class encoderLPD: public QObject
{
    Q_OBJECT
public:
    encoderLPD();
    QByteArray buildAB2Message(const Track &track);
    QByteArray buildFullMessage(const CommandContext &ctx);
    QByteArray negateData(const QByteArray &data);
};

#endif // ENCODERLPD_H
