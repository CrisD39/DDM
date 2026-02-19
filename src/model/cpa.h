#ifndef CPA_H
#define CPA_H

#include <QObject>
#include "src/model/entities/track.h"

struct CPAResult {
    double tcpa;  // segundos
    double dcpa;  // Data Miles
    bool valid;
};
class CPA : public QObject
{
    Q_OBJECT
public:
    explicit CPA(QObject *parent = nullptr);
    CPAResult computeCPA(const Track& a, const Track& b);
signals:
};

#endif // CPA_H
