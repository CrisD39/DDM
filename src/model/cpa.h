#ifndef CPA_H
#define CPA_H

#include <QObject>
#include "src/model/entities/track.h"
#include "commandContext.h"

struct CPAResult {
    double tcpa;  // segundos
    double dcpa;  // Data Miles
    double azimuth;
    bool valid;
};
class CPA : public QObject
{
    Q_OBJECT
public:
    explicit CPA(QObject *parent = nullptr);
    CPAResult computeCPA(const Track& a, const Track& b);
    CPAResult fromCLI(int idTrack1, int idTrack2, CommandContext &ctx);
    CPAResult fromJSON(int idTrack1, int idTrack2, CommandContext &ctx);
    static CPA& instance(int index);

private:
    static std::array<CPA, 10> m_instances;

signals:
};

#endif // CPA_H
